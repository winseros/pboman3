#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QWidget>

namespace pboman3 {
    class ProgressWidget : public QWidget {
    public:
        ProgressWidget(QWidget* parent = nullptr);

        QSize sizeHint() const override;

        void setMinimum(int minimum) const;

        void setMaximum(int maximum) const;

        void setIndeterminate(bool indeterminate) const;

        void setValue(int value) const;

        void setText(const QString& text) const;

        void showText(bool show) const;

    protected:
        void resizeEvent(QResizeEvent* event) override;
        
    private:
        QProgressBar* progress_;
        QLabel* label_;
    };
}
