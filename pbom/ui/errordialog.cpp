#include "errordialog.h"

namespace pboman3::ui {
    ErrorDialog::ErrorDialog(const DiskAccessException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(ex.message() + "<br><b>" + ex.file() + "</b>");
    }

    ErrorDialog::ErrorDialog(const PboFileFormatException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText("Can not open the file. It is not a valid PBO.");
    }

    ErrorDialog::ErrorDialog(const AppException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(ex.message());
    }

#ifdef WIN32
    ErrorDialog::ErrorDialog(const Win32FileViewerException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setTextFormat(Qt::RichText);
        ui_->label->setText(ex.message()
            + "<br><br>Error code: " + QString::number(ex.systemErrorCode())
            + "<br>Error text: " + ex.systemErrorDescription()
            + "<br><br>" + ex.filePath());
    }
#endif


    ErrorDialog::ErrorDialog(const QString& text, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setText(text);
    }

    ErrorDialog::~ErrorDialog() {
        delete ui_;
    }
}
