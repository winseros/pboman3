#pragma once

#include <QDialog>

#include "model/conflictsparcel.h"
#include "model/interactionparcel.h"

namespace Ui {
    class InsertDialog;
}

namespace pboman3 {
    class InsertDialog : public QDialog {
    Q_OBJECT

    public:
        InsertDialog(QWidget* parent, const PboPath* rootPath, NodeDescriptors* descriptors, const ConflictsParcel* conflicts);

        ~InsertDialog();

    private:
        Ui::InsertDialog* ui_;
        const PboPath* rootPath_;
        NodeDescriptors* descriptors_;
        const ConflictsParcel* conflicts_;

        void inflateView();
    };
}
