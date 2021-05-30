#pragma once

#include <QDialog>
#include "treewidget/treewidgetitem.h"

namespace Ui {
    class RenameDialog;
}

namespace pboman3 {
    class RenameDialog : public QDialog {
    Q_OBJECT

    public:
        RenameDialog(QWidget* parent, QString title, std::function<TitleError(const QString&)> validate);

        ~RenameDialog();

        QString title() const;

    public slots:
        void onTextEdited(const QString& _) const;

        void onAcceptClick();

    private:
        Ui::RenameDialog* ui_;
        QString title_;
        const std::function<QString(const QString&)> validate_;
        bool isDirty_;

        bool setErrorState(const TitleError& err) const;

        void disableAccept(bool disable) const;
    };
}
