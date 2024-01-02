#pragma once

#include <QWidget>
#include "ui_insertdialogbuttons.h"

namespace pboman3::ui {
    class InsertDialogButtons : public QWidget {
        Q_OBJECT

    public:
        InsertDialogButtons(QWidget* parent = nullptr);

        ~InsertDialogButtons() override;

        void setIsTwoStep() const;

    signals:
        void next();

        void back();

        void ok();

        void cancel();

    private:
        Ui::InsertDialogButtons* ui_;

        void onNextClick();

        void onBackClick();

        void onOkClick();

        void onCancelClick();
    };
}
