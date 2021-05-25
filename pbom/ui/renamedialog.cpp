#include "renamedialog.h"
#include <QPushButton>
#include "ui_renamedialog.h"

namespace pboman3 {
    RenameDialog::RenameDialog(QWidget* parent,
                               const TreeWidgetItem* item,
                               std::function<QString(const PboPath&)> validate)
        : QDialog(parent),
          ui_(new Ui::RenameDialog),
          item_(item),
          validate_(std::move(validate)),
          isDirty_(false) {
        ui_->setupUi(this);
        setErrorText();

        initialText_ = item->makePath().last();
        ui_->input->setText(initialText_);
        ui_->input->selectAll();
    }

    RenameDialog::~RenameDialog() {
        delete ui_;
    }

    QString RenameDialog::getTitle() const {
        return ui_->input->text().trimmed();
    }

    void RenameDialog::onTextEdited(const QString& _) {
        const QString title = getTitle();
        if (title == initialText_) {
            enableAcceptButton(true);
        } else {
            if (isDirty_) {
                const bool noErr = validateTitle(title);
                ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(noErr);
                enableAcceptButton(noErr);
            }
        }
    }

    void RenameDialog::onAcceptClick() {
        const QString title = getTitle();
        if (title == initialText_) {
            reject();
        } else {
            if (isDirty_) {
                accept();
            } else {
                isDirty_ = true;
                if (validateTitle(title)) {
                    accept();
                } else {
                    enableAcceptButton(false);
                }
            }
        }
    }

    bool RenameDialog::validateTitle(const QString& text) {
        if (text.isEmpty()) {
            setErrorText("The value can not be empty");
            return false;
        }

        const PboPath newName = item_->makePath().makeSibling(text);
        const QString error = validate_(newName);
        setErrorText(error);
        return error.isEmpty();
    }

    void RenameDialog::enableAcceptButton(bool enable) const {
        ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
    }

    void RenameDialog::setErrorText(const QString& error) const {
        ui_->error->setText(error);
    }
}
