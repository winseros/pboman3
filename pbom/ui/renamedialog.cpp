#include "renamedialog.h"
#include <QPushButton>
#include "ui_renamedialog.h"

namespace pboman3 {
    RenameDialog::RenameDialog(QWidget* parent,
                               QString title,
                               std::function<QString(const QString&)> validate)
        : QDialog(parent),
          ui_(new Ui::RenameDialog),
          title_(std::move(title)),
          validate_(std::move(validate)),
          isDirty_(false) {
        ui_->setupUi(this);

        disableAccept(setErrorState(""));

        ui_->input->setText(title_);
        ui_->input->selectAll();
    }

    RenameDialog::~RenameDialog() {
        delete ui_;
    }

    QString RenameDialog::title() const {
        return ui_->input->text();
    }

    void RenameDialog::onTextEdited(const QString& title) const {
        if (title == title_) {
            disableAccept(setErrorState(""));
        } else {
            if (isDirty_) {
                disableAccept(setErrorState(validate_(title)));
            }
        }
    }

    void RenameDialog::onAcceptClick() {
        const QString title = ui_->input->text();
        if (title == title_) {
            reject();
        } else {
            if (isDirty_) {
                accept();
            } else {
                isDirty_ = true;
                if (setErrorState(validate_(title))) {
                    disableAccept(true);
                } else {
                    accept();
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
