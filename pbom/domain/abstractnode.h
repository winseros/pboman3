#pragma once

#include <QSharedPointer>
#include "util/qpointerlistiterator.h"

namespace pboman3::domain {
    template <typename T>
    class AbstractNode : public QObject {

    public:
        AbstractNode(T* parentNode)
            : parentNode_(parentNode) {
        }

        T* parentNode() const {
            return parentNode_;
        }

        T* at(qsizetype index) const {
            return children_.at(index).get();
        }

        int depth() const {
            int result = 0;
            T* parentNode = parentNode_;
            while (parentNode) {
                result++;
                parentNode = parentNode->parentNode_;
            }
            return result;
        }

        int count() const {
            return static_cast<int>(children_.count());
        }

        QPointerListIterator<T> begin() {
            return QPointerListIterator(children_.data());
        }

        QPointerListIterator<T> end() {
            return QPointerListIterator(children_.data() + children_.count());
        }

        QPointerListConstIterator<T> begin() const {
            return QPointerListConstIterator(children_.data());
        }

        QPointerListConstIterator<T> end() const {
            return QPointerListConstIterator(children_.data() + count());
        }

    protected:
        T* parentNode_;
        QList<QSharedPointer<T>> children_;
    };
}
