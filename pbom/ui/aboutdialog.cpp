#include "aboutdialog.h"
#include <QPushButton>
#include <QClipboard>
#include "ui_aboutdialog.h"

namespace pboman3::ui {
    AboutDialog::AboutDialog(QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::AboutDialog) {
        ui_->setupUi(this);

        ui_->projectNameValue->setText("<h3>" + QString(PBOM_PROJECT_NAME) + "</h3>");
        ui_->buildNumberValue->setText(PBOM_BUILD_NUMBER);
        ui_->versionValue->setText(PBOM_VERSION);
        ui_->projectSiteValue->setText("<a href=\"" + QString(PBOM_PROJECT_SITE) + "\">" + QString(PBOM_PROJECT_SITE) + "</a>");

        const QPushButton* btnCopy = ui_->buttonBox->addButton("Copy", QDialogButtonBox::ActionRole);
        connect(btnCopy, &QPushButton::clicked, this, &AboutDialog::onCopyClicked);
    }

    AboutDialog::~AboutDialog() {
        delete ui_;
    }

    void AboutDialog::onCopyClicked() const {
        const QString text = QString(PBOM_PROJECT_NAME) + "\r\nVersion: " + PBOM_VERSION + "\r\nBuild: " + PBOM_BUILD_NUMBER;
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(text, QClipboard::Clipboard);
    }
}
