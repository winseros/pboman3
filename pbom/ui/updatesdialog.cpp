#include "updatesdialog.h"
#include "ui_updatesdialog.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include "util/log.h"

#define LOG(...) LOGGER("ui/UpdatesDialog", __VA_ARGS__)

namespace pboman3::ui {
    SemanticVersion::SemanticVersion(QString rawVersion)
        : raw_(std::move(rawVersion)) {
        const QRegularExpression reg("v?(\\d+)\\.(\\d+)\\.(\\d+)");
        const QRegularExpressionMatch match = reg.match(raw_);
        if (match.hasMatch()) {
            major_ = match.captured(1).toShort();
            minor_ = match.captured(2).toShort();
            patch_ = match.captured(3).toShort();
        } else {
            major_ = -1;
            minor_ = -1;
            patch_ = -1;
        }
    }

    SemanticVersion::SemanticVersion()
        : major_(-1),
          minor_(-1),
          patch_(-1) {
    }

    bool SemanticVersion::isValid() const {
        return major_ > 0 || minor_ > 0 || patch_ > 0;
    }

    bool operator>(const SemanticVersion& v1, const SemanticVersion& v2) {
        return v2 < v1;
    }

    bool operator<(const SemanticVersion& v1, const SemanticVersion& v2) {
        if (v1.major_ == v2.major_) {
            if (v1.minor_ == v2.minor_) {
                if (v1.patch_ == v2.patch_) {
                    return false;
                }
                return v1.patch_ < v2.patch_;
            }
            return v1.minor_ < v2.minor_;
        }
        return v1.major_ < v2.major_;
    }

    const QString& SemanticVersion::raw() const {
        return raw_;
    }


    void GithubLatestVersion::check() {
        QNetworkRequest request(QUrl(PBOM_API_SITE"/releases?per-page=1"));
        request.setRawHeader("Accept", "application/vnd.github.v3+json");
        reply_.reset(network_.get(request));
        connect(reply_.get(), &QNetworkReply::finished, this, &GithubLatestVersion::replyReceived);
    }

    void GithubLatestVersion::abort() const {
        reply_->abort();
    }

#define MSG_UNEXPECTED_RESPONSE "The server returned unexpected response."
#define F_TAG_NAME "tag_name"

    void GithubLatestVersion::replyReceived() {
        if (reply_->error() == QNetworkReply::NoError) {
            const QByteArray response = reply_->readAll();
            QJsonParseError jsonParseErr;
            const QJsonDocument json = QJsonDocument::fromJson(response, &jsonParseErr);
            if (json.isNull()) {
                LOG(warning, "Not a JSON response. rror:", jsonParseErr.error, ". Offset:", jsonParseErr.offset,
                    ". Message:", jsonParseErr.errorString())
                emit error(MSG_UNEXPECTED_RESPONSE);
            } else {
                if (json.isArray() && json[0].isObject() && json[0][F_TAG_NAME].isString()) {
                    const QString rawVersion = json[0][F_TAG_NAME].toString();
                    const SemanticVersion version(rawVersion);
                    if (version.isValid()) {
                        emit success(version);
                    } else {
                        LOG(warning, "The JSON version information malformed:", rawVersion)
                        emit error(MSG_UNEXPECTED_RESPONSE);
                    }
                } else {
                    LOG(warning, "Could not get the version from the JSON:", json)
                    emit error(MSG_UNEXPECTED_RESPONSE);
                }
            }
        } else {
            LOG(warning, "Network failure. Code:", reply_->error(), ". Message:", reply_->errorString())
            emit error(reply_->errorString());
        }
    }


    UpdatesDialog::UpdatesDialog(QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::UpdatesDialog) {
        ui_->setupUi(this);

        uiSetLoading();

        connect(&github_, &GithubLatestVersion::success, this, &UpdatesDialog::versionReceiveSuccess);
        connect(&github_, &GithubLatestVersion::error, this, &UpdatesDialog::versionReceiveError);
        github_.check();
    }

    UpdatesDialog::~UpdatesDialog() {
        delete ui_;
    }

    void UpdatesDialog::closeEvent(QCloseEvent* evt) {
        QDialog::closeEvent(evt);
        github_.abort();
    }

    void UpdatesDialog::versionReceiveSuccess(const SemanticVersion& version) const {
        const SemanticVersion currentVersion(PBOM_VERSION);
        assert(currentVersion.isValid());

        if (version > currentVersion) {
            ui_->label1->setTextFormat(Qt::MarkdownText);
            ui_->label1->setText("New version available: **" + version.raw() + "**");
            ui_->label2->setTextFormat(Qt::MarkdownText);
            ui_->label2->setText("[Download](" PBOM_PROJECT_SITE"/releases) from GitHub");
            ui_->label2->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui_->label2->setOpenExternalLinks(true);
            ui_->label2->show();
        } else {
            ui_->label1->setText("No updates available.");
        }

        ui_->progressBar->hide();
    }

    void UpdatesDialog::versionReceiveError(const QString& error) const {
        ui_->progressBar->hide();

        ui_->label1->setText("Could not check for updates:");

        ui_->label2->show();
        ui_->label2->setText(error);
    }

    void UpdatesDialog::uiSetLoading() const {
        ui_->label2->hide();
    }
}
