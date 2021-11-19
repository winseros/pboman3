#pragma once

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

namespace pboman3::ui {
    class AboutDialog : public QDialog {
    public:
        AboutDialog(QWidget* parent = nullptr);

        ~AboutDialog() override;

    private:
        Ui::AboutDialog* ui_;

        void onCopyClicked() const;
    };
}
