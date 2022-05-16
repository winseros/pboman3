#pragma once

#include "ui_insertdialog.h"
#include "model/conflictsparcel.h"

namespace Ui {
    class InsertDialog;
}

namespace pboman3::ui {
    class InsertDialog : public QDialog {
    Q_OBJECT

    public:
        enum class Mode;

        InsertDialog(QWidget* parent, Mode dialogMode, NodeDescriptors* descriptors, ConflictsParcel* conflicts);

        ~InsertDialog() override;

    public slots:
        void next() const;

        void back() const;

    private:
        Ui::InsertDialog* ui_;

    public:
        enum class Mode {
            InternalFiles,
            ExternalFiles
        };
    };
}
