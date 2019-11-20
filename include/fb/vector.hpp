#pragma once

#include "stddef.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "algorithm.hpp"

#include <initializer_list>
#include <stdexcept>
#include <type_traits>

namespace fb {

    template <typename T>

    class Vector {
        struct Deleter {
            void operator()(void *buf) {
                operator delete[](buf);
            }
        };

        using UniqPtrType = UniquePtr<void, Deleter>;
    public:
        using ValueType = T;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;
        using Reference = T &;
        using ConstReference = const T &;
        using Pointer = T *;
        using ConstPointer = const T *;
        using Iterator = Pointer;
        using ConstIterator = const T *;
        using ReverseIterator = fb::ReverseIterator<Iterator>;
        using ConstReverseIterator = fb::ReverseIterator<ConstIterator>;

        Vector( ) : buf(nullptr, Deleter()) { }

        Vector(SizeT n, const T &val) : Vector() {
            alloc_mem(n);
            size_ = n;
            uninitializedFill(begin(), end(), val);
         }

        Vector(SizeT n) : Vector() {
            alloc_mem(n);
            size_ = n;
            uninitializedDefaultConstruct(begin(), end());
        }

        Vector( const Vector<T>& v ) : Vector() {
            alloc_mem(v.size());
            size_ = v.size();
            uninitializedCopy(v.begin(), v.end(), begin());
        }

        Vector( Vector<T>&& v ) noexcept
            : buf(std::move(v.buf)),
              size_(v.size()),
              cap_(v.capacity()) {
           v.size_ = 0;
           v.cap_ = 0;
        }

        template <typename It>
        Vector(EnableIfT<std::is_base_of_v<InputIteratorTag,
               typename IteratorTraits<It>::IteratorCategory>,
               It> first, It last) : Vector() {
            // TODO: optimize if It is RandomAccessIterator
            for (; first != last; ++first)
                pushBack(*first);
        }

        Vector(std::initializer_list<T> init)
            : Vector(init.begin(), init.end()) {}

        Vector & operator=( const Vector<T>& v ) {
            Vector<T> temp(v);
            swap(temp);
            return *this;
        }

        Vector & operator=( Vector<T>&& v ) noexcept {
            swap(v);
            return *this;
        }

        ~Vector() {
            fb::destroy(begin(),end());
        }

        void assign(SizeType count, const T &value) {
            fill(begin(), begin() + count, value);
        }

        template <typename It>
        void assign(It first, It last) {
            copy(first, last, begin());
        }

        void assign(std::initializer_list<T> ilist) {
            assign(ilist.begin(), ilist.end());
        }

        /*  Element access  */
        Reference at(SizeType pos) {
            if (pos >= size_)
                throw std::out_of_range("Vector: Reference at(SizeType pos)");

            return *this[pos];
        }

        ConstReference at(SizeType pos) const {
            if (pos >= size_)
                throw std::out_of_range("Vector: ConstReference at(SizeType pos)");

            return *this[pos];
        }

        Reference operator[](SizeType pos) {
            return data()[pos];
        }

        ConstReference operator[](SizeType pos) const {
            return data()[pos];
        }

        Reference front() {
            return data()[0];
        }

        ConstReference front() const {
            return data()[0];
        }

        Reference back() {
            return data()[size() - 1];
        }

        ConstReference back() const {
            return data()[size() - 1];
        }

        Pointer data() noexcept {
            return static_cast<Pointer>(buf.get());
        }

        ConstPointer data() const noexcept {
            return static_cast<ConstPointer>(buf.get());
        }

        /* Iterators */
        Iterator begin() noexcept {
            return data();
        }

        ConstIterator begin() const noexcept {
            return data();
        }

        ConstIterator cbegin() const noexcept {
            return data();
        }

        Iterator end() noexcept {
            return data() + size();
        }

        ConstIterator end() const noexcept {
            return data() + size();
        }

        ConstIterator cend() const noexcept {
            return data() + size();
        }

        ReverseIterator rbegin() noexcept {
            return ReverseIterator(end());
        }

        ConstReverseIterator rbegin() const noexcept {
            return ConstReverseIterator(end());
        }

        ConstReverseIterator crbegin() const noexcept {
            return ConstReverseIterator(cend());
        }

        ReverseIterator rend() noexcept {
            return ReverseIterator(begin());
        }

        ConstReverseIterator rend() const noexcept {
            return ConstReverseIterator(begin());
        }

        ConstReverseIterator crend() const noexcept {
            return ConstReverseIterator(cbegin());
        }

        /* Capacity */
        [[nodiscard]] constexpr bool empty() const noexcept {
            return !size();
        }

        [[nodiscard]] constexpr SizeType size() const noexcept {
            return size_;
        }

        void reserve(SizeT n) {
            alloc_mem(n);
        }

        [[nodiscard]] constexpr SizeType capacity() const noexcept {
            return cap_;
        }

        /* Modifiers */
        void clear() noexcept {
            destroy(begin(), end());
            size_ = 0;
        }

        Iterator insert(ConstIterator pos, const T &value) {
            auto idx = static_cast<SizeT>(fb::distance(cbegin(), pos));
            alloc_mem(size() + 1);

            // TODO: make more efficient by avoiding default construction
            uninitializedDefaultConstruct(end(), end() + 1);
            for (auto i = size(); i > idx; --i)
                data()[i] = std::move(data()[i-1]);

            data()[idx] = value;
            ++size_;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, T &&value) {
            auto idx = static_cast<SizeT>(fb::distance(cbegin(), pos));
            alloc_mem(size() + 1);

            uninitializedDefaultConstruct(end(), end() + 1);
            for (auto i = size(); i > idx; --i)
                data()[i] = std::move(data()[i-1]);

            data()[idx] = std::move(value);
            ++size_;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, SizeType count, const T &value) {
            auto idx = fb::distance(cbegin(), pos);
            alloc_mem(size() + count);

            uninitializedDefaultConstruct(end(), end() + count);
            for (auto i = size() + count - 1; i > (idx + count - 1); --i)
                data()[i] = std::move(data()[i-count]);

            for (auto i = 0; i < count; ++i)
                data()[i + idx] = value;

            size_ += count;
            return begin() + idx;
        }

        template <typename It>
        Iterator insert(ConstIterator pos,
                        EnableIfT<
                        std::is_base_of_v<InputIteratorTag,
                        typename IteratorTraits<It>::IteratorCategory>,
                        It> first, It last) {
            auto count = static_cast<SizeT>(fb::distance(first, last));
            auto idx = static_cast<SizeT>(fb::distance(cbegin(), pos));

            alloc_mem(size() + count);

            uninitializedDefaultConstruct(end(), end() + count);
            for (auto i = size() + count - 1; i > (idx + count - 1); --i)
                data()[i] = std::move(data()[i-count]);

            copy(first, last, begin() + idx);
            size_ += count;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, std::initializer_list<T> ilist) {
            return iterInsert(pos, ilist.begin(), ilist.end());
        }

        template <typename ... Args>
        Iterator emplace(ConstIterator pos, Args &&... args) {
            auto idx = static_cast<SizeT>(fb::distance(cbegin(), pos));
            alloc_mem(size() + 1);
            uninitializedDefaultConstruct(end(), end() + 1);
            for (auto i = size(); i > idx; --i)
                data()[i] = std::move(data()[i-1]);
            destroyAt(data() + idx);
            new (data() + idx) T(std::forward<Args>(args)...);
            ++size_;
            return begin() + idx;
        }

        Iterator erase(ConstIterator pos) {
            auto idx = fb::distance(cbegin(), pos);
            --size_;
            for (auto i = idx; i < size(); ++i)
                data()[i] = std::move(data()[i+1]);
            destroyAt(const_cast<Pointer>(begin() + size()));
            return begin() + idx;
        }

        Iterator erase(ConstIterator first, ConstIterator last) {
            if (first == last)
                return begin() + fb::distance(cbegin(), first);

            auto idx = fb::distance(cbegin(), first);
            auto count = fb::distance(first,last);
            destroy(first,last);

            for (auto first_it = begin() + idx; last != end(); ++last, ++first_it)
                *first_it = std::move(*last);

            size_ -= count;
            return begin() + idx + count;
        }

        void pushBack(const T &value) {
            insert(end(), value);
        }

        void pushBack(T &&value) {
            insert(end(), std::move(value));
        }

        template <typename ... Args>
        Reference emplaceBack(Args &&... args) {
            return *emplace(end(), std::forward<Args>(args)...);
        }

        void popBack() {
            erase(end() - 1);
        }

        void resize(SizeType count) {
            if (count <= size()) {
                destroy(begin() + count, end());
                size_ = count;
                return;
            }

            alloc_mem(count);
            uninitializedDefaultConstruct(begin() + size_, begin() + count);
            size_ = count;
        }

        void resize(SizeType count, const ValueType &value) {
            if (count <= size()) {
                destroy(begin() + count, end());
                size_ = count;
                return;
            }

            alloc_mem(count);
            uninitializedFill(begin() + size_, begin() + count, value);
            size_ = count;
        }

        void swap(Vector &other) noexcept {
            fb::swap(buf, other.buf);
            fb::swap(size_, other.size_);
            fb::swap(cap_, other.cap_);
        }

    private:
        UniqPtrType buf;
        SizeType size_ = 0;
        SizeType cap_ = 0;

        void alloc_mem(SizeT mem) {
            if (cap_ >= mem)
                return;

            auto newCap = (3 * mem) / 2;

            UniqPtrType newBuf(operator new[](newCap * sizeof(T)), Deleter());
            uninitializedMove(begin(), end(), static_cast<T *>(newBuf.get()));
            fb::destroy(begin(), end());

            cap_ = newCap;
            buf = std::move(newBuf);
        }
   };

    template <typename T>
    void swap(Vector<T> &lhs, Vector<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

}
