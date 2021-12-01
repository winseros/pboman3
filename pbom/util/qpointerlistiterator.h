#pragma once

#include <QSharedPointer>

namespace pboman3::util {
    template <class T>
    class QPointerListIterator {
    public:
        QPointerListIterator(QSharedPointer<T>* ptr)
            : ptr_(ptr) {
        }

        T* operator*() const { return ptr_->get(); }
        T* operator->() const { return ptr_->get(); }
        T& operator[](qsizetype j) const { return *(ptr_->get() + j); }
        bool operator==(QPointerListIterator<T> o) const { return ptr_ == o.ptr_; }
        bool operator!=(QPointerListIterator<T> o) const { return ptr_ != o.ptr_; }
        bool operator<(QPointerListIterator<T> other) const { return ptr_ < other.ptr_; }
        bool operator<=(QPointerListIterator<T> other) const { return ptr_ <= other.ptr_; }
        bool operator>(QPointerListIterator<T> other) const { return ptr_ > other.ptr_; }
        bool operator>=(QPointerListIterator<T> other) const { return ptr_ >= other.ptr_; }
        bool operator==(T* p) const { return ptr_->get() == p; }
        bool operator!=(T* p) const { return ptr_->get() != p; }

        QPointerListIterator<T>& operator++() {
            ++ptr_;
            return *this;
        }

        QPointerListIterator<T> operator++(int) {
            const auto n = ptr_;
            ++ptr_;
            return n;
        }

        QPointerListIterator<T>& operator--() {
            --ptr_;
            return *this;
        }

        QPointerListIterator<T> operator--(int) {
            const auto n = ptr_;
            --ptr_;
            return n;
        }

        QPointerListIterator<T>& operator+=(qsizetype j) {
            ptr_ += j;
            return *this;
        }

        QPointerListIterator<T>& operator-=(qsizetype j) {
            ptr_ -= j;
            return *this;
        }

        QPointerListIterator<T> operator+(qsizetype j) const { return QPointerListIterator<T>(ptr_ + j); }
        QPointerListIterator<T> operator-(qsizetype j) const { return QPointerListIterator<T>(ptr_ - j); }
        friend QPointerListIterator<T> operator+(qsizetype j, QPointerListIterator<T> k) { return k + j; }
        qsizetype operator-(QPointerListIterator<T> j) const { return ptr_->get() - j.ptr_->get(); }
        operator T*() const { return ptr_->get(); }

    private:
        QSharedPointer<T>* ptr_;
    };

    template <class T>
    class QPointerListConstIterator {
    public:
        QPointerListConstIterator(const QSharedPointer<T>* ptr)
            : ptr_(ptr) {
        }

        const T* operator*() const { return ptr_->get(); }
        const T* operator->() const { return ptr_->get(); }
        const T& operator[](qsizetype j) const { return *(ptr_->get() + j); }
        bool operator==(QPointerListConstIterator<T> o) const { return ptr_ == o.ptr_; }
        bool operator!=(QPointerListConstIterator<T> o) const { return ptr_ != o.ptr_; }
        bool operator<(QPointerListConstIterator<T> other) const { return ptr_ < other.ptr_; }
        bool operator<=(QPointerListConstIterator<T> other) const { return ptr_ <= other.ptr_; }
        bool operator>(QPointerListConstIterator<T> other) const { return ptr_ > other.ptr_; }
        bool operator>=(QPointerListConstIterator<T> other) const { return ptr_ >= other.ptr_; }
        bool operator==(T* p) const { return ptr_->get() == p; }
        bool operator!=(T* p) const { return ptr_->get() != p; }

        QPointerListConstIterator<T>& operator++() {
            ++ptr_;
            return *this;
        }

        QPointerListConstIterator<T> operator++(int) {
            const auto n = ptr_;
            ++ptr_;
            return n;
        }

        QPointerListConstIterator<T>& operator--() {
            --ptr_;
            return *this;
        }

        QPointerListConstIterator<T> operator--(int) {
            const auto n = ptr_;
            --ptr_;
            return n;
        }

        QPointerListConstIterator<T>& operator+=(qsizetype j) {
            ptr_ += j;
            return *this;
        }

        QPointerListConstIterator<T>& operator-=(qsizetype j) {
            ptr_ -= j;
            return *this;
        }

        QPointerListConstIterator<T> operator+(qsizetype j) const { return QPointerListConstIterator<T>(ptr_ + j); }
        QPointerListConstIterator<T> operator-(qsizetype j) const { return QPointerListConstIterator<T>(ptr_ - j); }
        friend QPointerListConstIterator<T> operator+(qsizetype j, QPointerListConstIterator<T> k) { return k + j; }
        qsizetype operator-(QPointerListConstIterator<T> j) const { return ptr_->get() - j.ptr_->get(); }
        operator const T*() const { return ptr_->get(); }

    private:
        const QSharedPointer<T>* ptr_;
    };
}
