#pragma once

#include <cstddef>
#include <utility>

namespace fb {
namespace impl {

template <typename T, typename Deleter> class unique_ptr_base {
  public:
    using pointer = T *;
    using element_type = T;
    using deleter_type = Deleter;

  protected:
    static constexpr bool def_const_ptr =
        std::is_default_constructible_v<deleter_type> &&
        !std::is_pointer_v<deleter_type>;

    // disallow direct construction
    constexpr unique_ptr_base(pointer p, deleter_type d) noexcept
        : owner{p}, deleter{d} {}

  public:
    // copies disallowed
    unique_ptr_base(unique_ptr_base &) = delete;
    unique_ptr_base &operator=(unique_ptr_base &) = delete;

    // move constructor
    constexpr unique_ptr_base(unique_ptr_base &&rhs) noexcept
        : owner{rhs.release()}, deleter{rhs.get_deleter()} {}

    // move assignment operator
    [[nodiscard]] constexpr unique_ptr_base &
    operator=(unique_ptr_base &&rhs) noexcept {
        owner = rhs.release();
        deleter = rhs.get_deleter();
    }

    ~unique_ptr_base() noexcept {
        if (get())
            get_deleter()(get());
    }

    constexpr pointer release() noexcept {
        return std::exchange(owner, nullptr);
    }

    constexpr void swap(unique_ptr_base &other) noexcept {
        std::swap(owner, other.owner);
        std::swap(deleter, other.deleter);
    }

    [[nodiscard]] constexpr pointer get() const noexcept { return owner; }

    [[nodiscard]] constexpr const deleter_type &get_deleter() const noexcept {
        return deleter;
    }

    [[nodiscard]] constexpr deleter_type &get_deleter() noexcept {
        return deleter;
    }

    explicit constexpr operator bool() const noexcept {
        return get() != nullptr;
    }

  protected:
    pointer owner{};
    deleter_type deleter{};
};

} // namespace impl
} // namespace std398
