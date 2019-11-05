#pragma once

#include "stddef.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "algorithm.hpp"

#include <initializer_list>

namespace fb {

    template <typename T>

    class Vector {
        using UniqPtrType = UniquePtr<void *, decltype(operator delete[])>;
    public:
        using ValueType = T;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;
        using Reference = T &;
        using ConstReference = const Reference;
        using Pointer = T *;
        using ConstPointer = const Pointer;
        using Iterator = Pointer;
        using ConstIterator = const Iterator;
        using ReverseIterator = fb::ReverseIterator<Iterator>;
        using ConstReverseIterator = const ReverseIterator;

        Vector( ) { }

        Vector(SizeT n, const T &val) : size_(n) {
            alloc_mem(n);
            uninitializedFill(begin(), end(), val);
         }

        Vector(SizeT n) : size_(n) {
            alloc_mem(n);
            uninitializedDefaultConstruct(begin(), end());
        }

        Vector( const Vector<T>& v ) : size_(v.size()) {
            alloc_mem(v.size());
            uninitalizedCopy(v.begin(), v.end(), begin());
        }

        Vector( Vector<T>&& v ) noexcept
            : size_(v.size()), cap_(v.cap()), buf(std::move(v.buf)) {
           v.size_ = 0;
           v.cap_ = 0;
        }

        template <typename It>
        Vector (It first, It last) {
            while (first != last)
                pushBack(*first++);
        }

        Vector(std::initializer_list<T> init)
            : Vector(init.begin(), init.end()) {}

        Vector & operator=( const Vector<T> v ) {
            std::swap(buf, v.buf);
            std::swap(size_, v.size_);
            std::swap(cap_, v.cap_);
        }

        Vector operator=( Vector<T>&& v ) noexcept {
            std::swap(buf, v.buf);
            std::swap(size_, v.size_);
            std::swap(cap_, v.cap_);
        }

        ~Vector() = default;

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
                throw std::out_of_range{};

            return *this[pos];
        }

        ConstReference at(SizeType pos) const {
            if (pos >= size_)
                throw std::out_of_range{};

            return *this[pos];
        }

        Reference operator[](SizeType pos) {
            return data()[pos];
        }

        ConstReference operator[](SizeType pos) const {
            return data()[pos];
        }

        Reference front() {
            return *this[0];
        }

        ConstReference front() const {
            return *this[0];
        }

        Reference back() {
            return *this[size() - 1];
        }

        ConstReference back() const {
            return *this[size() - 1];
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
            destroy(begin, end());
            size_ = 0;
        }

        Iterator insert(ConstIterator pos, const T &value) {
            auto idx = distance(cbegin(), pos);
            alloc_mem(size() + 1);

            for (auto i = size() - 1; i > idx; --i)
                data()[i] = std::move(data()[i-1]);
            data()[idx] = value;
            ++size_;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, T &&value) {
            auto idx = distance(cbegin(), pos);
            alloc_mem(size() + 1);
            for (auto i = size(); i > idx; --i)
                data()[i] = std::move(data()[i-1]);
            data()[idx] = std::move(value);
            ++size_;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, SizeType count, const T &value) {
            auto idx = distance(cbegin(), pos);
            alloc_mem(size() + count);

            for (auto i = size() + count - 1; i > (idx + count); --i)
                data()[i] = std::move(data()[i-count]);

            for (auto i = 0; i < count; ++i)
                data()[i] = value;
            size_ += count;
            return begin() + idx;
        }

        template <typename It>
        Iterator insert(ConstIterator pos, It first, It last) {
            auto count = distance(first, last);

            auto idx = distance(cbegin(), pos);
            alloc_mem(size() + count);

            for (auto i = size() + count - 1; i > (idx + count); --i)
                data()[i] = std::move(data()[i-count]);

            copy(first, last, begin() + idx);
            size_ += count;
            return begin() + idx;
        }

        Iterator insert(ConstIterator pos, std::initializer_list<T> ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        template <typename ... Args>
        Iterator emplace(ConstIterator pos, Args &&... args) {
            auto idx = distance(cbegin(), pos);
            alloc_mem(size() + 1);
            for (auto i = size(); i > idx; --i)
                data()[i] = std::move(data()[i-1]);
            destroy(data() + idx);
            new (data() + idx) T(std::forward<Args>(args)...);
            ++size_;
            return begin() + idx;
        }

        Iterator erase(ConstIterator pos) {
            destroyAt(const_cast<Pointer>(pos));
            auto idx = distance(cbegin(), pos);
            --size_;
            for (auto i = idx; i < size(); ++i)
                data()[i] = std::move(data()[i+1]);
            return ++pos;
        }

        Iterator erase(ConstIterator first, ConstIterator last) {
            if (first == last)
                return first;

            auto count = distance(first,last);
            destroy(first,last);
            auto it = last;

            while (first != last && it != end()) {
                *first = std::move(*it);
                ++it; ++first;
            }

            size_ -= count;
            return last;
        }

        void pushBack(const T &value) {
            insert(end(), value);
        }

        void pushBack(T &&value) {
            insert(end(), std::move(value));
        }

        template <typename ... Args>
        Reference emplaceBack(Args &&... args) {
            emplace(end(), std::forward<Args>(args)...);
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
            swap(buf, other.buf);
            swap(size_, other.size_);
            swap(cap_, other.cap_);
        }

    private:
        UniqPtrType buf { nullptr, operator delete[] };
        SizeType size_ = 0;
        SizeType cap_ = 0;

        void alloc_mem(SizeT mem) {
            if (cap_ >= mem)
                return;

            auto newCap = cap_ ? 3 * cap_ / 2 : mem;

            UniqPtrType newBuf(new[](newCap * sizeof(T)), operator delete[]);
            uninitializedMove(begin(), end(), data());

            cap_ = newCap;
            buf = std::move(newBuf);
        }
   };

}
