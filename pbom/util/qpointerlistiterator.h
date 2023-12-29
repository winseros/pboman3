#pragma once

#include <QSharedPointer>

namespace pboman3::util {
    template <typename TElem, typename TIt = typename QList<QSharedPointer<TElem>>::iterator>
    class QPointerListIterator {
    public:
        explicit QPointerListIterator(TIt it)
            : it_(std::move(it)) {
        }

        TElem* operator*() const { return it_->get(); }
        TElem* operator->() const { return it_->get(); }
        TElem& operator[](qsizetype j) const { return (it_ + j)->get(); }
        bool operator==(QPointerListIterator<TElem, TIt> o) const { return it_ == o.it_; }
        bool operator!=(QPointerListIterator<TElem, TIt> o) const { return it_ != o.it_; }
        bool operator<(QPointerListIterator<TElem, TIt> other) const { return it_ < other.it_; }
        bool operator<=(QPointerListIterator<TElem, TIt> other) const { return it_ <= other.it_; }
        bool operator>(QPointerListIterator<TElem, TIt> other) const { return it_ > other.it_; }
        bool operator>=(QPointerListIterator<TElem, TIt> other) const { return it_ >= other.it_; }
        bool operator==(TElem* p) const { return it_->get() == p; }
        bool operator!=(TElem* p) const { return it_->get() != p; }

        QPointerListIterator<TElem, TIt>& operator++() {
            ++it_;
            return *this;
        }

        QPointerListIterator<TElem, TIt> operator++(int) {
            const auto n = it_;
            ++it_;
            return QPointerListIterator<TElem, TIt>(n);
        }

        QPointerListIterator<TElem, TIt>& operator--() {
            --it_;
            return *this;
        }

        QPointerListIterator<TElem, TIt> operator--(int) {
            const auto n = it_;
            --it_;
            return QPointerListIterator<TElem, TIt>(n);
        }

        QPointerListIterator<TElem, TIt>& operator+=(qsizetype j) {
            it_ += j;
            return *this;
        }

        QPointerListIterator<TElem, TIt>& operator-=(qsizetype j) {
            it_ -= j;
            return *this;
        }

        QPointerListIterator<TElem, TIt> operator+(qsizetype j) const { return QPointerListIterator<TElem, TIt>(it_ + j); }
        QPointerListIterator<TElem, TIt> operator-(qsizetype j) const { return QPointerListIterator<TElem, TIt>(it_ - j); }
        friend QPointerListIterator<TElem, TIt> operator+(qsizetype j, QPointerListIterator<TElem, TIt> k) { return k + j; }
        qsizetype operator-(QPointerListIterator<TElem, TIt> j) const { return it_->get() - j.it_->get(); }
        operator TElem*() const { return it_->get(); }

    private:
        TIt it_;
    };

    template <typename TElem, typename TIt = typename QList<QSharedPointer<TElem>>::const_iterator>
    class QPointerListConstIterator {
    public:
        explicit QPointerListConstIterator(TIt it)
            : it_(std::move(it)) {
        }

        const TElem* operator*() const { return it_->get(); }
        const TElem* operator->() const { return it_->get(); }
        const TElem& operator[](qsizetype j) const { return (it_ + j)->get(); }
        bool operator==(QPointerListConstIterator<TElem, TIt> o) const { return it_ == o.it_; }
        bool operator!=(QPointerListConstIterator<TElem, TIt> o) const { return it_ != o.it_; }
        bool operator<(QPointerListConstIterator<TElem, TIt> other) const { return it_ < other.it_; }
        bool operator<=(QPointerListConstIterator<TElem, TIt> other) const { return it_ <= other.it_; }
        bool operator>(QPointerListConstIterator<TElem, TIt> other) const { return it_ > other.it_; }
        bool operator>=(QPointerListConstIterator<TElem, TIt> other) const { return it_ >= other.it_; }
        bool operator==(TElem* p) const { return it_->get() == p; }
        bool operator!=(TElem* p) const { return it_->get() != p; }

        QPointerListConstIterator<TElem, TIt>& operator++() {
            ++it_;
            return *this;
        }

        QPointerListConstIterator<TElem, TIt> operator++(int) {
            const auto n = it_;
            ++it_;
            return QPointerListConstIterator(n);
        }

        QPointerListConstIterator<TElem, TIt>& operator--() {
            --it_;
            return *this;
        }

        QPointerListConstIterator<TElem, TIt> operator--(int) {
            const auto n = it_;
            --it_;
            return QPointerListConstIterator(n);
        }

        QPointerListConstIterator<TElem, TIt>& operator+=(qsizetype j) {
            it_ += j;
            return *this;
        }

        QPointerListConstIterator<TElem, TIt>& operator-=(qsizetype j) {
            it_ -= j;
            return *this;
        }

        QPointerListConstIterator<TElem, TIt> operator+(qsizetype j) const { return QPointerListConstIterator<TElem, TIt>(it_ + j); }
        QPointerListConstIterator<TElem, TIt> operator-(qsizetype j) const { return QPointerListConstIterator<TElem, TIt>(it_ - j); }
        friend QPointerListConstIterator<TElem, TIt> operator+(qsizetype j, QPointerListConstIterator<TElem, TIt> k) { return k + j; }
        qsizetype operator-(QPointerListConstIterator<TElem, TIt> j) const { return it_->get() - j.it_->get(); }
        operator const TElem*() const { return it_->get(); }

    private:
        TIt it_;
    };
}
