#include "renamedialog.h"
#include <QPushButton>
#include "ui_renamedialog.h"

namespace pboman3 {
    RenameDialog::RenameDialog(QWidget* parent,
                               PboNode* node)
        : QDialog(parent),
          ui_(new Ui::RenameDialog),
          node_(node),
          isDirty_(false) {
        ui_->setupUi(this);

        disableAccept(setErrorState(""));

        ui_->input->setText(node->title());
        ui_->input->selectAll();
    }

    RenameDialog::~RenameDialog() {
        delete ui_;
    }

    void RenameDialog::onTextEdited(const QString& title) const {
        if (title == node_->title()) {
            disableAccept(setErrorState(""));
        } else {
            if (isDirty_) {
                disableAccept(setErrorState(node_->verifyTitle(title)));
            }
        }
    }

    void RenameDialog::accept() {
        const QString title = ui_->input->text();
        if (title == node_->title()) {
            QDialog::reject();
        } else {
            if (isDirty_) {
                node_->setTitle(title);
                QDialog::accept();
            } else {
                isDirty_ = true;
                if (setErrorState(node_->verifyTitle(title))) {
                    disableAccept(true);
                } else {
                    node_->setTitle(title);
                    QDialog::accept();
                }
            }
        }
    }

    bool RenameDialog::setErrorState(const TitleError& err) const {
        ui_->error->setText(err);
        return !err.isEmpty();
    }

    void RenameDialog::disableAccept(bool disable) const {
        ui_->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(disable);
    }
}
