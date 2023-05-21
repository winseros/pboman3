#pragma once

#include <type_traits>
#include <QDialog>
#include "ui_settingsdialog.h"

namespace Ui {
    class SettingsDialog;
}

namespace pboman3::ui {
    class SettingsDialog : public QDialog {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent);

        ~SettingsDialog() override;

        void accept() override;

    private:
        Ui::SettingsDialog* ui_;

        void loadSettings() const;

        template <typename T>
        static void setRadioButtonValue(const QWidget* parent, T index) {
            static_assert(std::is_enum_v<T>);
            dynamic_cast<QRadioButton*>(parent->children().at(static_cast<int>(index) + 1))->setChecked(true);
        }

        template <typename T>
        static T getRadioButtonValue(const QWidget* parent) {
            static_assert(std::is_enum_v<T>);
            for (auto i = 0; i < parent->children().count(); i++) {
                const auto radioButton = dynamic_cast<QRadioButton*>(parent->children().at(i));
                if (radioButton && radioButton->isChecked()) {
                    return static_cast<T>(i - 1);
                }
            }
            return static_cast<T>(0);
        }
    };
}
