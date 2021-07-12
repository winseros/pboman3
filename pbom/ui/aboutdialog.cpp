#include "aboutdialog.h"
#include "ui_aboutdialog.h"

namespace pboman3 {
    AboutDialog::AboutDialog(QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::AboutDialog) {
        ui_->setupUi(this);

        ui_->projectNameValue->setText("<h3>" + QString(PBOM_PROJECT_NAME) + "</h3>");
        ui_->buildNumberValue->setText(PBOM_BUILD_NUMBER);
        ui_->versionValue->setText(PBOM_VERSION);
        ui_->projectSiteValue->setText("<a href=\"" + QString(PBOM_PROJECT_SITE) + "\">" + QString(PBOM_PROJECT_SITE) + "</a>");
    }

    AboutDialog::~AboutDialog() {
        delete ui_;
    }
}
