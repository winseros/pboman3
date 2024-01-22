#include "insertdialogbuttons.h"
#include <QPushButton>
#include "util/log.h"

#define LOG(...) LOGGER("ui/InsertDialogButtons", __VA_ARGS__)

namespace pboman3::ui {
    InsertDialogButtons::InsertDialogButtons(QWidget* parent)
        : QWidget(parent),
          ui_(new Ui::InsertDialogButtons) {
        ui_->setupUi(this);
        ui_->btnNext->hide();
        ui_->btnBack->hide();

        connect(ui_->btnOk, &QAbstractButton::clicked, this, &InsertDialogButtons::onOkClick);
        connect(ui_->btnCancel, &QAbstractButton::clicked, this, &InsertDialogButtons::onCancelClick);
        connect(ui_->btnNext, &QAbstractButton::clicked, this, &InsertDialogButtons::onNextClick);
        connect(ui_->btnBack, &QAbstractButton::clicked, this, &InsertDialogButtons::onBackClick);
    }

    InsertDialogButtons::~InsertDialogButtons() {
        delete ui_;
    }

    void InsertDialogButtons::setIsTwoStep() const {
        LOG(info, "Set the buttons panel for a 2-step dialog")

        ui_->btnOk->setAutoDefault(false);
        ui_->btnOk->setDefault(false);
        ui_->btnOk->hide();

        ui_->btnNext->show();
        ui_->btnNext->setFocus();
        ui_->btnNext->setDefault(true);
        ui_->btnNext->setAutoDefault(true);
    }

    void InsertDialogButtons::onNextClick() {
        LOG(info, "User clicked the Next button")
        ui_->btnNext->setAutoDefault(false);
        ui_->btnNext->setDefault(false);
        ui_->btnNext->hide();

        ui_->btnOk->show();
        ui_->btnOk->setFocus();
        ui_->btnOk->setDefault(true);
        ui_->btnOk->setAutoDefault(true);
        
        ui_->btnBack->show();
        emit next();
    }

    void InsertDialogButtons::onBackClick() {
        LOG(info, "User clicked the Back button")

        ui_->btnOk->setAutoDefault(false);
        ui_->btnOk->setDefault(false);
        ui_->btnOk->hide();

        ui_->btnNext->show();
        ui_->btnNext->setFocus();
        ui_->btnNext->setDefault(true);
        ui_->btnNext->setAutoDefault(true);

        ui_->btnBack->hide();
        emit back();
    }

    void InsertDialogButtons::onOkClick() {
        LOG(info, "User clicked the Ok button")
        emit ok();
    }

    void InsertDialogButtons::onCancelClick() {
        LOG(info, "User clicked the Cancel button")
        emit cancel();
    }
}
