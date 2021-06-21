#include "insertdialog.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/InsertDialog", __VA_ARGS__)

namespace pboman3 {
    InsertDialog::InsertDialog(QWidget* parent, Mode dialogMode,
                               NodeDescriptors* descriptors, ConflictsParcel* conflicts)
        : QDialog(parent),
          ui_(new Ui::InsertDialog) {
        ui_->setupUi(this);

        if (dialogMode == Mode::ExternalFiles) {
            LOG(info, "Running in ExternalFiles mode")
            ui_->conflictsWidget->hide();
            ui_->compressList->setDataSource(descriptors);
            if (conflicts->hasConflicts()) {
                ui_->conflictsList->setDataSource(descriptors, conflicts);
                ui_->buttons->setIsTwoStep();
            }
        } else if (dialogMode == Mode::InternalFiles) {
            LOG(info, "Running in InternalFiles mode")
            ui_->compressWidget->hide();
            ui_->conflictsList->setDataSource(descriptors, conflicts);
        }
    }

    InsertDialog::~InsertDialog() {
        delete ui_;
    }

    void InsertDialog::next() {
        LOG(info, "User clicked the Next button")
        ui_->compressWidget->hide();
        ui_->conflictsWidget->show();
    }

    void InsertDialog::back() {
        LOG(info, "User clicked the Back button")
        ui_->compressWidget->show();
        ui_->conflictsWidget->hide();
    }
}
