#include "insertdialog.h"
#include "ui_insertdialog.h"

namespace pboman3 {
    InsertDialog::InsertDialog(QWidget* parent, const PboPath* rootPath, NodeDescriptors* descriptors,
                               const ConflictsParcel* conflicts)
        : QDialog(parent),
          ui_(new Ui::InsertDialog),
          rootPath_(rootPath),
          descriptors_(descriptors),
          conflicts_(conflicts) {
        ui_->setupUi(this);
        inflateView();
    }

    InsertDialog::~InsertDialog() {
        delete ui_;
    }

    void InsertDialog::inflateView() {
        for (const NodeDescriptor& descriptor : *descriptors_) {
            ui_->list->addItem(new QListWidgetItem(rootPath_->makeChild(descriptor.path()).toString()));
        }
    }
}
