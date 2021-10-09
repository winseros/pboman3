#pragma once

#include "ui_headersdialog.h"
#include "model/headersmodel.h"
#include <QDialog>

namespace Ui {
    class HeadersDialog;
}


namespace pboman3 {
    class HeadersDialog : public QDialog {
    public:
        HeadersDialog(HeadersModel* model, QWidget* parent = nullptr);

        ~HeadersDialog() override;

        void accept() override;

    protected:
        void showEvent(QShowEvent*) override;

    private:
        Ui::HeadersDialog* ui_;
        HeadersModel* model_;

        void renderHeaderItems() const;

        void setupConnections() const;

        void contextMenuRequested(const QPoint& pos) const;

        bool isValidHeader(const QString& name, const QString& value) const;

        void onInsertClick(int index) const;

        void onMoveClick(int index) const;

        void onRemoveClick() const;
    };
}
