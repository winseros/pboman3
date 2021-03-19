#include "compressdialog.h"
#include <QFileInfo>
#include <qevent.h>
#include "ui_compressdialog.h"

CompressDialog::CompressDialog(QWidget* parent, const FilesystemFiles* files) :
    QDialog(parent),
    ui_(new Ui::CompressDialog),
    columnWidthSet_(false),
    files_(files) {

    model_ = QSharedPointer<CompressionListModel>(new CompressionListModel(this, files_));

    ui_->setupUi(this);
    ui_->list->setModel(model_.get());
}

CompressDialog::~CompressDialog() {
    delete ui_;
}

void CompressDialog::resizeEvent(QResizeEvent* evt) {
    if (!columnWidthSet_) {
        columnWidthSet_ = true;
        const int col1W = ui_->list->columnWidth(1);
        const int listW = ui_->list->width();
        ui_->list->setColumnWidth(0, listW - col1W - 20);
    }
}

void CompressDialog::keyReleaseEvent(QKeyEvent* event) {
    if (ui_->list->hasFocus()
        && event->modifiers().testFlag(Qt::NoModifier)
        && event->key() == Qt::Key_Space) {
        toggleSelectedItems();
    } else {
        QDialog::keyReleaseEvent(event);
    }
}

void CompressDialog::toggleSelectedItems() const {
    const QItemSelectionModel* selectionModel = ui_->list->selectionModel();
    const QModelIndexList selection = selectionModel->selectedRows(1);
    if (selection.length()) {
        int checkedCount = 0;
        int uncheckedCount = 0;

        auto* model = dynamic_cast<CompressionListModel*>(ui_->list->model());
        assert(model && "Must not be null");

        for (const QModelIndex& index : selection) {
            if (model->data(index, Qt::CheckStateRole) == Qt::Checked)
                checkedCount++;
            else
                uncheckedCount++;
        }

        Qt::CheckState compress;
        if (checkedCount == uncheckedCount) {
            compress = model->data(selection[0], Qt::CheckStateRole) == Qt::Unchecked ? Qt::Checked: Qt::Unchecked;
        } else {
            compress = uncheckedCount > checkedCount ? Qt::Checked : Qt::Unchecked;
        }

        for (const QModelIndex& index : selection) {
            model->setData(index, compress, Qt::CheckStateRole);
        }
    }
}
