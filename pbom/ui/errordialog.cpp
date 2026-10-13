#include "errordialog.h"

#ifdef Q_OS_WIN
#include "win32/win32fileviewer.h"
#endif

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

    ErrorDialog::ErrorDialog(const FileViewerException& ex, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::ErrorDialog) {
        ui_->setupUi(this);
        ui_->label->setTextFormat(Qt::PlainText);
        QString message = ex.message();
#ifdef Q_OS_WIN
        if (const auto* windowsError = dynamic_cast<const Win32FileViewerException*>(&ex)) {
            message += "\n\nError code: " + QString::number(windowsError->systemErrorCode())
                + "\nError text: " + windowsError->systemErrorDescription();
        }
#endif
        ui_->label->setText(message + "\n\n" + ex.filePath());
    }


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
