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
template <typename T, typename Deleter = default_delete<T>>
class unique_ptr : public impl::unique_ptr_base<T, Deleter> {
    using impl::unique_ptr_base<T, Deleter>::def_const_ptr;
    using impl::unique_ptr_base<T, Deleter>::owner;
    using impl::unique_ptr_base<T, Deleter>::get_deleter;
    using impl::unique_ptr_base<T, Deleter>::get;

  public:
    using typename impl::unique_ptr_base<T, Deleter>::pointer;
    using typename impl::unique_ptr_base<T, Deleter>::element_type;
    using typename impl::unique_ptr_base<T, Deleter>::deleter_type;

    constexpr unique_ptr(std::enable_if_t<def_const_ptr, int> = 0) noexcept {}

    constexpr unique_ptr(std::nullptr_t,
                         std::enable_if_t<def_const_ptr, int> = 0) noexcept {}

    constexpr explicit unique_ptr(
        pointer p, std::enable_if_t<def_const_ptr, int> = 0) noexcept
        : impl::unique_ptr_base<T, Deleter>(p, nullptr) {}

    /*
     * Modifiers
     */
    constexpr void reset(pointer new_ptr = pointer()) noexcept {
        auto old_ptr = std::exchange(owner, new_ptr);
        if (old_ptr)
            get_deleter()(old_ptr);
    }

    [[nodiscard]] constexpr std::add_lvalue_reference_t<T> operator*() const
        noexcept {
        return *get();
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept {
        return get();
    }
};

template <typename T, typename Deleter>
class unique_ptr<T[], Deleter> : public impl::unique_ptr_base<T, Deleter> {
    using impl::unique_ptr_base<T, Deleter>::get;
    using impl::unique_ptr_base<T, Deleter>::def_const_ptr;

  public:
    using impl::unique_ptr_base<T, Deleter>::pointer;
    using impl::unique_ptr_base<T, Deleter>::element_type;
    using impl::unique_ptr_base<T, Deleter>::deleter_type;

    constexpr unique_ptr(std::enable_if_t<def_const_ptr, int> = 0) noexcept {}

    constexpr unique_ptr(std::nullptr_t,
                         std::enable_if_t<def_const_ptr, int> = 0) noexcept {}

    T &operator[](std::size_t i) const { return get()[i]; }
};

template <typename T, typename... Args>
[[nodiscard]] constexpr unique_ptr<T> make_unique(Args &&... args) {
    return unique_ptr<T>(new T(std::forward(args)...));
}

template <typename T>[[nodiscard]] unique_ptr<T> make_unique(std::size_t size) {
    return unique_ptr<T>(new std::remove_extent_t<T>[size]());
}

template <typename T>[[nodiscard]] unique_ptr<T> make_unique_default_init() {
    return unique_ptr<T>(new T);
}

template <typename CharT, typename Traits, typename Y, typename D>
constexpr std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const unique_ptr<Y, D> &p) noexcept {
    os << p.get();
    return os;
}

} // namespace fb
