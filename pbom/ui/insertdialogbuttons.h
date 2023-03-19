#pragma once

#include <QDialogButtonBox>

namespace pboman3::ui {
    class InsertDialogButtons : public QDialogButtonBox {
    Q_OBJECT

    public:
        InsertDialogButtons(QWidget* parent = nullptr);

        void setIsTwoStep();

    signals:
        void next();

        void back();

    private:
        QPushButton* btnNext_;
        QPushButton* btnBack_;

        void onButtonClicked(const QAbstractButton* btn);
    };
}
