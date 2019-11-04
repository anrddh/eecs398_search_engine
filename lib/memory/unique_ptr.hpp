#pragma once

#include "../stddef.hpp"
#include "../type_traits.hpp"
#include "default_delete.hpp"

#include <iosfwd>
#include <type_traits>
#include <utility>

namespace fb {

/*
 * An implementation of the unique_ptr type from the C++ Standard
 * Template Library.
 */
template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
public:
    using Pointer = T *;
    using ElementType = T;
    using DeleterType = Deleter;

private:
    static constexpr bool defConstPtr =
        std::is_default_constructible_v<DeleterType> &&
        !IsPointerV<DeleterType>;

public:
    constexpr UniquePtr(EnableIfT<defConstPtr, int> = 0) noexcept {}

    constexpr UniquePtr(NullPtrT,
                        EnableIfT<defConstPtr, int> = 0) noexcept {}

    constexpr explicit UniquePtr(Pointer p,
                                 EnableIfT<defConstPtr, int> = 0)
        noexcept : owner{p} {}

    constexpr explicit UniquePtr(Pointer p, DeleterType d)
        : owner{p}, deleter{std::move(d)} {}

    // move constructor
    constexpr
    UniquePtr(UniquePtr &&rhs,
              EnableIfT<std::is_move_constructible_v<DeleterType>,int>
                  = 0)
        noexcept : owner{rhs.release()}, deleter{std::move(rhs.getDeleter())} {}

    // move assignment operator
    [[nodiscard]] constexpr UniquePtr & operator=(UniquePtr &&rhs) noexcept {
        owner = rhs.release();
        deleter = rhs.getDeleter();
    }

    ~UniquePtr() noexcept {
        if (get())
            getDeleter()(get());
    }

    /* Modifiers */
    constexpr Pointer release() noexcept {
        return std::exchange(owner, nullptr);
    }

    constexpr void reset(Pointer newPtr = Pointer()) noexcept {
        auto oldPtr = std::exchange(owner, newPtr);
        if (oldPtr)
            getDeleter()(oldPtr);
    }

    constexpr void swap(UniquePtr &other) noexcept {
        std::swap(owner, other.owner);
        std::swap(deleter, other.deleter);
    }

    /* Observers */
    [[nodiscard]] constexpr Pointer get() const noexcept { return owner; }

    [[nodiscard]] constexpr const DeleterType &getDeleter() const noexcept {
        return deleter;
    }

    [[nodiscard]] constexpr DeleterType &getDeleter() noexcept {
        return deleter;
    }

    explicit constexpr operator bool() const noexcept {
        return get() != nullptr;
    }

    [[nodiscard]] constexpr std::add_lvalue_reference_t<T> operator*() const {
        return *get();
    }

    [[nodiscard]] constexpr Pointer operator->() const noexcept {
        return get();
    }

private:
    Pointer owner{};
    DeleterType deleter{};
};


template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    using Pointer = T *;
    using ElementType = T;
    using DeleterType = Deleter;

private:
    static constexpr bool defConstPtr =
        std::is_default_constructible_v<DeleterType> &&
        IsPointerV<DeleterType>;

public:
    constexpr UniquePtr(EnableIfT<defConstPtr, int> = 0) noexcept {}

    constexpr UniquePtr(NullPtrT,
                        EnableIfT<defConstPtr, int> = 0) noexcept {}

    template <typename U>
    constexpr explicit UniquePtr(U p) noexcept
        : owner{p} {}

    constexpr explicit UniquePtr(Pointer p, DeleterType d)
        : owner{p}, deleter{std::move(d)} {}

    // move constructor
    constexpr
    UniquePtr(UniquePtr &&rhs,
              EnableIfT<std::is_move_constructible_v<DeleterType>,int>
                  = 0)
        noexcept : owner{rhs.release()}, deleter{std::move(rhs.getDeleter())} {}

    // move assignment operator
    [[nodiscard]] constexpr UniquePtr & operator=(UniquePtr &&rhs) noexcept {
        owner = rhs.release();
        deleter = rhs.getDeleter();
    }

    ~UniquePtr() noexcept {
        if (get())
            getDeleter()(get());
    }

    /* Modifiers */
    constexpr Pointer release() noexcept {
        return std::exchange(owner, nullptr);
    }

    constexpr void reset(Pointer newPtr = Pointer()) noexcept {
        auto oldPtr = std::exchange(owner, newPtr);
        if (oldPtr)
            getDeleter()(oldPtr);
    }

    constexpr void swap(UniquePtr &other) noexcept {
        std::swap(owner, other.owner);
        std::swap(deleter, other.deleter);
    }

    /* Observers */
    [[nodiscard]] constexpr Pointer get() const noexcept { return owner; }

    [[nodiscard]] constexpr const DeleterType &getDeleter() const noexcept {
        return deleter;
    }

    [[nodiscard]] constexpr DeleterType &getDeleter() noexcept {
        return deleter;
    }

    explicit constexpr operator bool() const noexcept {
        return get() != nullptr;
    }

    T &operator[](SizeT i) const { return get()[i]; }

private:
    Pointer owner{};
    DeleterType deleter{};
};

template <typename T, typename... Args>
[[nodiscard]] EnableIfT<!IsArrayV<T>, UniquePtr<T>> makeUnique(Args &&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
[[nodiscard]] EnableIfT<IsArrayV<T>, UniquePtr<T>> makeUnique(SizeT size) {
    return UniquePtr<T>(new RemoveExtentT<T>[size]());
}

template <typename T>
[[nodiscard]] UniquePtr<T> makeUniqueDefaultInit() {
    return UniquePtr<T>(new T);
}

template <typename CharT, typename Traits, typename Y, typename D>
constexpr std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const UniquePtr<Y, D> &p) noexcept {
    os << p.get();
    return os;
}

} // namespace fb
