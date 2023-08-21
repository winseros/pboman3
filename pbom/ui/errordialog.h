#pragma once

#include "ui_errordialog.h"
#include "io/diskaccessexception.h"
#include "io/pbofileformatexception.h"
#include "exception.h"

#ifdef WIN32
#include "win32/win32fileviewer.h"
#endif

namespace Ui {
    class ErrorDialog;
}

namespace pboman3::ui {
    using namespace io;

    class ErrorDialog : public QDialog {
    public:
        ErrorDialog(const DiskAccessException& ex, QWidget* parent = nullptr);

        ErrorDialog(const PboFileFormatException& ex, QWidget* parent = nullptr);

        ErrorDialog(const AppException& ex, QWidget* parent = nullptr);

#ifdef WIN32
        ErrorDialog(const Win32FileViewerException& ex, QWidget* parent = nullptr);
#endif

        ErrorDialog(const QString& text, QWidget* parent = nullptr);

        ~ErrorDialog() override;

    private:
        Ui::ErrorDialog* ui_;
    };
}

#define UI_HANDLE_ERROR(EX) ErrorDialog(ex, QApplication::activeWindow()).exec();
#define UI_HANDLE_ERROR_RET(EX) ErrorDialog(ex, QApplication::activeWindow()).exec();\
    return;
