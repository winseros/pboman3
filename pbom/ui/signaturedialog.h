#pragma once

#include <QDialog>
#include "ui_signaturedialog.h"
#include "model/signaturemodel.h"

namespace Ui {
    class SignatureDialog;
}

namespace pboman3 {
    class SignatureDialog : public QDialog {
    Q_OBJECT
    public:
        SignatureDialog(const SignatureModel* model, QWidget* parent);

        ~SignatureDialog();

    private:
        Ui::SignatureDialog* ui_;
    };
}
