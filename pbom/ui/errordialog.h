#pragma once

#include "ui_errordialog.h"

#include "util/appexception.h"

namespace Ui {
    class ErrorDialog;
}

namespace pboman3 {
    class ErrorDialog : public QDialog {
    public:
        ErrorDialog(const QString& message, QWidget* parent = nullptr);

        ErrorDialog(const AppException& ex, QWidget* parent = nullptr);

        ~ErrorDialog();

    private:
        Ui::ErrorDialog* ui_;
    };
}
