#pragma once

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
    class UpdatesDialog;
}

namespace pboman3 {
    class SemanticVersion {
    public:
        explicit SemanticVersion(QString rawVersion);

        SemanticVersion();

        bool isValid() const;

        friend bool operator >(const SemanticVersion& v1, const SemanticVersion& v2);

        friend bool operator <(const SemanticVersion& v1, const SemanticVersion& v2);

        const QString& raw() const;

    private:
        qint16 major_;
        qint16 minor_;
        qint16 patch_;

        QString raw_;
    };

    class GithubLatestVersion: public QObject {
        Q_OBJECT

    public:
        GithubLatestVersion() = default;

        void check();

        void abort() const;

    signals:
        void success(const SemanticVersion& version);

        void error(const QString& message);

    private:
        QNetworkAccessManager network_;
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply_;

        void replyReceived();
    };

    class UpdatesDialog: public QDialog {
    public:
        UpdatesDialog(QWidget* parent);

        ~UpdatesDialog();

    protected:
        void closeEvent(QCloseEvent*) override;

    private:
        Ui::UpdatesDialog* ui_;
        GithubLatestVersion github_;

        void versionReceiveSuccess(const SemanticVersion& version) const;

        void versionReceiveError(const QString& error) const;

        void uiSetLoading() const;
    };
}
