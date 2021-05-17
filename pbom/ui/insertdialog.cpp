#include "insertdialog.h"

namespace pboman3 {
    InsertDialog::InsertDialog(QWidget* parent, Mode dialogMode,
                               NodeDescriptors* descriptors, ConflictsParcel* conflicts)
        : QDialog(parent),
          ui_(new Ui::InsertDialog) {
        ui_->setupUi(this);

        if (dialogMode == Mode::ExternalFiles) {
            ui_->conflictsWidget->hide();
            ui_->compressList->setDataSource(descriptors);
            if (conflicts->hasConflicts()) {
                ui_->conflictsList->setDataSource(descriptors, conflicts);
                ui_->buttons->setIsTwoStep();
            }
        } else if (dialogMode == Mode::InternalFiles) {
            ui_->compressWidget->hide();
            ui_->conflictsList->setDataSource(descriptors, conflicts);
        }
    }

    InsertDialog::~InsertDialog() {
        delete ui_;
    }

    void InsertDialog::next() {
        ui_->compressWidget->hide();
        ui_->conflictsWidget->show();
    }

    void InsertDialog::back() {
        ui_->compressWidget->show();
        ui_->conflictsWidget->hide();
    }
}
