#include "renamedialog.h"
#include <QPushButton>
#include "domain/pbonodetransaction.h"
#include "domain/validationexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/RenameDialog", __VA_ARGS__)

namespace pboman3::ui {
    RenameDialog::RenameDialog(QWidget* parent, PboNode* node)
        : QDialog(parent),
          ui_(new Ui::RenameDialog),
          initialTitle_(node->title()),
          isDirty_(false) {
        ui_->setupUi(this);

        transaction_ = node->beginTransaction();
        setErrorState("");
        setTextAndSelect(node->title());
    }

    RenameDialog::~RenameDialog() {
        delete ui_;
    }

    void RenameDialog::onTextEdited(const QString& title) const {
        if (title == initialTitle_) {
            LOG(info, "Title was set to the initial value")
            setErrorState("");
        } else {
            LOG(info, "Title was set to the value:", title)
            try {
                transaction_->setTitle(title);
                setErrorState("");
            } catch (const ValidationException& ex) {
                if (isDirty_) {
                    LOG(info, "The input is dirty - validating")
                    setErrorState(ex.message());
                }
            }
        }
    }

    void RenameDialog::accept() {
        const QString title = ui_->input->text();
        if (title == initialTitle_) {
            LOG(info, "The user clicked Accept having not changed the title")
            QDialog::reject();
        } else {
            try {
                LOG(info, "Updating the node title to:", title)
                isDirty_ = true;
                transaction_->setTitle(title);
                transaction_->commit();
                QDialog::accept();
            } catch (const ValidationException& ex) {
                LOG(info, "The title contained errors:", ex.message())
                setErrorState(ex.message());
            }
        }
    }

    void RenameDialog::setErrorState(const QString& err) const {
        LOG(info, "Set validation error to:", err)
        ui_->error->setText(err);
        disableAccept(!err.isEmpty());
    }

    void RenameDialog::disableAccept(bool disable) const {
        LOG(debug, "Disable buttons:", disable)
        ui_->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(disable);
    }

    void RenameDialog::setTextAndSelect(const QString& title) const {
        qsizetype index = title.lastIndexOf(".");
        if (index < 1)
            index = title.length();
        ui_->input->setText(title);
        ui_->input->setSelection(0, static_cast<int>(index));

        //"file.txt"   ->  "file"
        //"file.tar.gz ->  "file.tar"
        //"file"       ->  ""
        //".config"    ->  ""
    }
}
