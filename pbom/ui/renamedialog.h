#pragma once

#include <QDialog>
#include "treewidgetitem.h"
#include "model/pbopath.h"

namespace Ui {
    class RenameDialog;
}

namespace pboman3 {
    class RenameDialog : public QDialog {
    Q_OBJECT

    public:
        RenameDialog(QWidget* parent, const TreeWidgetItem* item, std::function<QString(const PboPath&)> validate);

        ~RenameDialog();

        QString getTitle() const;

    public slots:
        void onTextEdited(const QString& _);

        void onAcceptClick();

    private:
        Ui::RenameDialog* ui_;
        const TreeWidgetItem* item_;
        const std::function<QString(const PboPath&)> validate_;
        bool isDirty_;
        QString initialText_;

        bool validateTitle(const QString& text);

        void enableAcceptButton(bool enable) const;

        void setErrorText(const QString& error = "") const;
    };
}
