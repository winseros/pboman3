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
        const int col1w = ui_->list->columnWidth(1);
        const int listw = ui_->list->width();
        ui_->list->setColumnWidth(0, listw - col1w - 20);
    }
}
