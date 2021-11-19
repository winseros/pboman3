#include "closedialog.h"

namespace pboman3::ui {
    CloseDialog::CloseDialog(const QFileInfo& file, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::CloseDialog) {
        ui_->setupUi(this);
        ui_->label->setTextFormat(Qt::RichText);
        ui_->label->setText(
            "The file <b>" + file.fileName() +
            "</b> has unsaved changes. If you close the file, those changes will be lost. Close the file?");
    }

    CloseDialog::~CloseDialog() {
        delete ui_;
    }
}
