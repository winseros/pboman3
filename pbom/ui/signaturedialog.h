#pragma once

#include <QDialog>
#include "ui_signaturedialog.h"

namespace Ui {
    class SignatureDialog;
}

namespace pboman3::ui {
    class SignatureDialog : public QDialog {
    Q_OBJECT
    public:
        SignatureDialog(const QByteArray* signature, QWidget* parent);

        ~SignatureDialog() override;

    private:
        Ui::SignatureDialog* ui_;
    };
}
