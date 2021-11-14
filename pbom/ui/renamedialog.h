#pragma once

#include <QDialog>
#include "domain/pbonode.h"
#include "ui_renamedialog.h"

namespace pboman3 {
    using namespace domain;

    class RenameDialog : public QDialog {
    Q_OBJECT

    public:
        RenameDialog(QWidget* parent, PboNode* node);

        ~RenameDialog() override;
       
    public slots:
        void onTextEdited(const QString& title) const;

        void accept() override;

    private:
        Ui::RenameDialog* ui_;
        QSharedPointer<PboNodeTransaction> transaction_;
        QString initialTitle_;
        bool isDirty_;

        void setErrorState(const QString& err) const;

        void disableAccept(bool disable) const;

        void setTextAndSelect(const QString& title) const;
    };
}
