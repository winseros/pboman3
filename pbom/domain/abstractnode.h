#pragma once

#include <QSharedPointer>
#include <QHash>
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
            return childList_.at(index).get();
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

        qsizetype count() const {
            return childList_.size();
        }

        QPointerListIterator<T> begin() {
            return QPointerListIterator<T>(childList_.begin());
        }

        QPointerListIterator<T> end() {
            return QPointerListIterator<T>(childList_.end());
        }

        QPointerListConstIterator<T> begin() const {
            return QPointerListConstIterator<T>(
                childList_.cbegin());
        }

        QPointerListConstIterator<T> end() const {
            return QPointerListConstIterator<T>(childList_.cend());
        }

    protected:
        T* parentNode_;
        QHash<QString, QSharedPointer<T>> childMap_;
        QList<QSharedPointer<T>> childList_;

        qsizetype addChild(const QSharedPointer<T>& item) {
            childMap_.insert(normalizeTitle(item->title()), item);

            const qsizetype index = getIndexForChild(item);
            childList_.insert(index, item);

            return index;
        }

        qsizetype removeChild(const QSharedPointer<T>& item) {
            childMap_.remove(normalizeTitle(item->title()));

            const qsizetype index = getIndexForChild(item);
            childList_.removeAt(index);

            return index;
        }

        bool hasChild(const QString& title) const {
            const bool has = childMap_.contains(normalizeTitle(title));
            return has;
        }

        QSharedPointer<T> getChild(const QString& title) {
            auto it = childMap_.find(normalizeTitle(title));
            auto child = it == childMap_.end() ? nullptr : *it;
            return child;
        }

        QSharedPointer<const T> getChild(const QString& title) const {
            auto it = childMap_.find(normalizeTitle(title));
            auto child = it == childMap_.end() ? nullptr : *it;
            return child;
        }

    private:
        static QString normalizeTitle(const QString& title) {
            return title.toUpper();
        }

        qsizetype getIndexForChild(const QSharedPointer<T>& child) const {
            auto it = std::lower_bound(childList_.begin(), childList_.end(), child,
                                             [](const QSharedPointer<T> c1, const QSharedPointer<T> c2) {
                                                 return *c1 < *c2;
                                             });
            const auto index = std::distance(childList_.begin(), it);

            return index;
        }
    };
}
