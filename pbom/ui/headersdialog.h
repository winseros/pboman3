#pragma once

#include "ui_headersdialog.h"
#include "domain/documentheaders.h"
#include <QDialog>

namespace Ui {
    class HeadersDialog;
}


namespace pboman3::ui {
    using namespace domain;

    class HeadersDialog : public QDialog {
    public:
        HeadersDialog(DocumentHeaders* headers, QWidget* parent = nullptr);

        ~HeadersDialog() override;

        void accept() override;

    protected:
        void showEvent(QShowEvent*) override;

    private:
        Ui::HeadersDialog* ui_;
        QSharedPointer<DocumentHeadersTransaction> tran_;

        void renderHeaderItems() const;

        void setupConnections() const;

        void contextMenuRequested(const QPoint& pos) const;

        void onInsertClick(int index) const;

        void onMoveClick(int index) const;

        void onRemoveClick() const;
    };
}
