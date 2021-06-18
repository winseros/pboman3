#include "renamedialog.h"
#include <QPushButton>
#include "ui_renamedialog.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/RenameDialog", __VA_ARGS__)

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
            LOG(info, "Title was set to the initial value")
            disableAccept(setErrorState(""));
        } else {
            LOG(info, "Title was set to the value:", title)
            if (isDirty_) {
                LOG(info, "The input is dirty - validating")
                disableAccept(setErrorState(node_->verifyTitle(title)));
            }
        }
    }

    void RenameDialog::accept() {
        const QString title = ui_->input->text();
        if (title == node_->title()) {
            LOG(info, "The user clicked Accept having not changed the title")
            QDialog::reject();
        } else {
            if (isDirty_) {
                LOG(info, "Updating the node title to:", title)
                node_->setTitle(title);
                QDialog::accept();
            } else {
                LOG(info, "The user clicked Accept the 1st time - running validations")
                isDirty_ = true;
                if (setErrorState(node_->verifyTitle(title))) {
                    LOG(info, "The title contained errors:", title)
                    disableAccept(true);
                } else {
                    LOG(info, "The title contained no errors:", title)
                    node_->setTitle(title);
                    QDialog::accept();
                }
            }
        }
    }

    bool RenameDialog::setErrorState(const TitleError& err) const {
        LOG(info, "Set validation error to:", err)
        ui_->error->setText(err);
        return !err.isEmpty();
    }

    void RenameDialog::disableAccept(bool disable) const {
        LOG(debug, "Disable buttons:", disable)
        ui_->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(disable);
    }
}
