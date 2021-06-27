#include "errordialog.h"

namespace pboman3 {
    ErrorDialog::ErrorDialog(const DiskAccessException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(ex.message() + "<br><b>" + ex.file() + "</b>");
    }

    ErrorDialog::ErrorDialog(const AppException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(ex.message());
    }

    ErrorDialog::~ErrorDialog() {
        delete ui_;
    }
}
