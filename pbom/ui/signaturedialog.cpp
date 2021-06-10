#include "signaturedialog.h"

namespace pboman3 {
    SignatureDialog::SignatureDialog(const SignatureModel* model, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::SignatureDialog) {
        ui_->setupUi(this);

        QString sig = model->signatureString();
        if (sig.isNull()) {
            sig = "/*The file has no signature*/";
        }
        ui_->textEdit->setText(sig);
    }

    SignatureDialog::~SignatureDialog() {
        delete ui_;
    }
}
