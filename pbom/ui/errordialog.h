#pragma once

#include "ui_errordialog.h"
#include "model/diskaccessexception.h"
#include "util/exception.h"

namespace Ui {
    class ErrorDialog;
}

namespace pboman3 {
    class ErrorDialog : public QDialog {
    public:
        ErrorDialog(const DiskAccessException& ex, QWidget* parent = nullptr);

        ErrorDialog(const AppException& ex, QWidget* parent = nullptr);

        ErrorDialog(const QString& text, QWidget* parent = nullptr);

        ~ErrorDialog() override;

    private:
        Ui::ErrorDialog* ui_;
    };
}

#define UI_HANDLE_ERROR(EX) ErrorDialog(ex, QApplication::activeWindow()).exec();
#define UI_HANDLE_ERROR_RET(EX) ErrorDialog(ex, QApplication::activeWindow()).exec();\
    return;
