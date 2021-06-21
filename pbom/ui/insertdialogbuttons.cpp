#include "insertdialogbuttons.h"
#include <QPushButton>
#include "util/log.h"

#define LOG(...) LOGGER("ui/InsertDialogButtons", __VA_ARGS__)

namespace pboman3 {
    InsertDialogButtons::InsertDialogButtons(QWidget* parent)
        : QDialogButtonBox(StandardButtons(Ok | Cancel), parent),
          btnNext_(nullptr),
          btnBack_(nullptr) {
        connect(this, &InsertDialogButtons::clicked, this, &InsertDialogButtons::onButtonClicked);
    }

    void InsertDialogButtons::setIsTwoStep() {
        LOG(info, "Set the buttons panel for a 2-step dialog")
        btnNext_ = addButton("Next", ActionRole);
        btnNext_->setFocus();
        btnBack_ = addButton("Back", ActionRole);
        button(Ok)->hide();
        btnBack_->hide();
    }

    void InsertDialogButtons::onButtonClicked(QAbstractButton* btn) {
        if (btn == btnNext_) {
            LOG(info, "User clicked the Next button")
            button(Ok)->show();
            button(Ok)->setFocus();
            btnNext_->hide();
            btnBack_->show();
            emit next();
        } else if (btn == btnBack_) {
            LOG(info, "User clicked the Back button")
            button(Ok)->hide();
            btnNext_->show();
            btnNext_->setFocus();
            btnBack_->hide();
            emit back();
        }
    }
}
