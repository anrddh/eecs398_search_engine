#pragma once

#include "default_delete.hpp"
#include "unique_ptr_base.hpp"

#include <cstddef>
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
        !std::is_pointer_v<DeleterType>;
  public:
    constexpr UniquePtr(std::enable_if_t<defConstPtr, int> = 0) noexcept {}

    constexpr UniquePtr(std::nullptr_t,
                        std::enable_if_t<defConstPtr, int> = 0) noexcept {}

    constexpr explicit UniquePtr(
        Pointer p, std::enable_if_t<defConstPtr, int> = 0) noexcept
        : owner{p} {}


    // copies disallowed
    UniquePtr(UniquePtr &) = delete;
    UniquePtr &operator=(UniquePtr &) = delete;

    // move constructor
    constexpr UniquePtr(UniquePtr &&rhs) noexcept
        : owner{rhs.release()}, deleter{rhs.getDeleter()} {}

    // move assignment operator
    [[nodiscard]] constexpr UniquePtr &
    operator=(UniquePtr &&rhs) noexcept {
        owner = rhs.release();
        deleter = rhs.getDeleter();
    }

    ~UniquePtr() noexcept {
        if (get())
            getDeleter()(get());
    }

    constexpr Pointer release() noexcept {
        return std::exchange(owner, nullptr);
    }

    constexpr void swap(UniquePtr &other) noexcept {
        std::swap(owner, other.owner);
        std::swap(deleter, other.deleter);
    }

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

    /*
     * Modifiers
     */
    constexpr void reset(Pointer newPtr = Pointer()) noexcept {
        auto oldPtr = std::exchange(owner, newPtr);
        if (oldPtr)
            getDeleter()(oldPtr);
    }

    [[nodiscard]] constexpr std::add_lvalue_reference_t<T> operator*() const
        noexcept {
        return *get();
    }

    [[nodiscard]] constexpr Pointer operator->() const noexcept {
        return get();
    }

  protected:
    Pointer owner{};
    DeleterType deleter{};
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : public impl::UniquePtrBase<T, Deleter> {
    using impl::UniquePtrBase<T, Deleter>::get;
    using impl::UniquePtrBase<T, Deleter>::defConstPtr;

public:
    using impl::UniquePtrBase<T, Deleter>::Pointer;
    using impl::UniquePtrBase<T, Deleter>::ElementType;
    using impl::UniquePtrBase<T, Deleter>::DeleterType;

    constexpr UniquePtr(std::enable_if_t<defConstPtr, int> = 0) noexcept {}

    constexpr UniquePtr(std::nullptr_t,
                        std::enable_if_t<defConstPtr, int> = 0) noexcept {}

    T &operator[](std::size_t i) const { return get()[i]; }
};

template <typename T, typename... Args>
[[nodiscard]] constexpr UniquePtr<T> makeUnique(Args &&... args) {
    return UniquePtr<T>(new T(std::forward(args)...));
}

template <typename T>[[nodiscard]] UniquePtr<T> makeUnique(std::size_t size) {
    return UniquePtr<T>(new std::remove_extent_t<T>[size]());
}

template <typename T>[[nodiscard]] UniquePtr<T> makeUniqueDefaultInit() {
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
