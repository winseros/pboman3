#pragma once

#include <QDialog>
#include <QDir>
#include "compressionlistmodel.h"
#include "model/filesystemfiles.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class CompressDialog;
}

QT_END_NAMESPACE

using namespace pboman3;

class CompressDialog : public QDialog {
Q_OBJECT

public:
    CompressDialog(QWidget* parent, const FilesystemFiles* files);

    ~CompressDialog();

protected:
    void resizeEvent(QResizeEvent*) override;

    void keyReleaseEvent(QKeyEvent* event) override;

private:
    Ui::CompressDialog* ui_;
    bool columnWidthSet_;
    const FilesystemFiles* files_;
    QSharedPointer<CompressionListModel> model_;

    void toggleSelectedItems() const;
};
