#include "insertdialogbuttons.h"
#include <QPushButton>

namespace pboman3 {
    InsertDialogButtons::InsertDialogButtons(QWidget* parent)
        : QDialogButtonBox(StandardButtons(Ok | Cancel), parent),
          btnNext_(nullptr),
          btnBack_(nullptr) {
        connect(this, &InsertDialogButtons::clicked, this, &InsertDialogButtons::onButtonClicked);
    }

    void InsertDialogButtons::setIsTwoStep() {
        btnNext_ = addButton("Next", ActionRole);
        btnNext_->setFocus();
        btnBack_ = addButton("Back", ActionRole);
        button(Ok)->hide();
        btnBack_->hide();
    }

    void InsertDialogButtons::onButtonClicked(QAbstractButton* btn) {
        if (btn == btnNext_) {
            button(Ok)->show();
            button(Ok)->setFocus();
            btnNext_->hide();
            btnBack_->show();
            emit next();
        } else if (btn == btnBack_) {
            button(Ok)->hide();
            btnNext_->show();
            btnNext_->setFocus();
            btnBack_->hide();
            emit back();
        }
    }
}
