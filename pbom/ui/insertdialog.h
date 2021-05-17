#pragma once

#include <QComboBox>
#include <QStyledItemDelegate>
#include "ui_insertdialog.h"
#include "model/conflictsparcel.h"

namespace Ui {
    class InsertDialog;
}

namespace pboman3 {
    class InsertDialog : public QDialog {
    Q_OBJECT

    public:
        enum class Mode;

        InsertDialog(QWidget* parent, Mode dialogMode, NodeDescriptors* descriptors, ConflictsParcel* conflicts);

        ~InsertDialog();

    public slots:
        void next();

        void back();

    private:
        Ui::InsertDialog* ui_;

    public:
        enum class Mode {
            InternalFiles,
            ExternalFiles
        };
    };
}
