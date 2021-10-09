#pragma once

#include <QDialog>
#include <QFileInfo>
#include "ui_closedialog.h"

namespace Ui {
    class CloseDialog;
}

namespace pboman3 {
    class CloseDialog : public QDialog {
    Q_OBJECT
    public:
        CloseDialog(const QFileInfo& file, QWidget* parent = nullptr);

        ~CloseDialog() override;

    private:
        Ui::CloseDialog* ui_;
    };
}
