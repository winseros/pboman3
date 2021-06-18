#include "signaturedialog.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/SignatureDialog", __VA_ARGS__)

namespace pboman3 {
    SignatureDialog::SignatureDialog(const SignatureModel* model, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::SignatureDialog) {
        ui_->setupUi(this);

        QString sig = model->signatureString();
        if (sig.isNull()) {
            sig = "/*The file has no signature*/";
        }
        LOG(info, "Show signature as:", sig)
        ui_->textEdit->setText(sig);
    }

    SignatureDialog::~SignatureDialog() {
        delete ui_;
    }
}
