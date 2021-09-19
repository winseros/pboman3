#include "progresswidget.h"
#include <QLayout>
#include <qevent.h>

namespace pboman3 {
    ProgressWidget::ProgressWidget(QWidget* parent)
        : QWidget(parent) {
        progress_ = new QProgressBar(this);
        progress_->setTextVisible(false);
        setSizePolicy(progress_->sizePolicy());

        label_ = new QLabel(this);
        label_->setText("Label text");
        label_->setAlignment(Qt::AlignCenter);
        label_->setContentsMargins(20, 0, 20, 0);
    }

    QSize ProgressWidget::sizeHint() const {
        return progress_->sizeHint();
    }

    void ProgressWidget::setMinimum(int minimum) const {
        progress_->setMinimum(minimum);
    }

    void ProgressWidget::setMaximum(int maximum) const {
        progress_->setMaximum(maximum);
    }

    void ProgressWidget::setValue(int value) const {
        progress_->setValue(value);
    }

    void ProgressWidget::setText(const QString& text) const {
        label_->setText(text);
    }

    void ProgressWidget::showText(bool show) const {
        label_->setVisible(show);
    }

    void ProgressWidget::resizeEvent(QResizeEvent* event) {
        const QSize size = event->size();
        progress_->setGeometry(0, 0, size.width(), size.height());
        label_->setGeometry(0, 0, size.width(), size.height());
    }
}
