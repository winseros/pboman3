#include "errordialog.h"

namespace pboman3 {
    ErrorDialog::ErrorDialog(const QString& message, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(message);
    }

    ErrorDialog::ErrorDialog(const AppException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
    }

    ErrorDialog::~ErrorDialog() {
        delete ui_;
    }
}
