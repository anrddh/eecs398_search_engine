#pragma once

#include "../stddef.hpp"

namespace fb {
    template <typename C>
    constexpr auto size(const C &c) -> decltype(c.size()) {
        return c.size();
    }
    //
    //    template <typename C>
    //    constexpr auto ssize(const C &c)
    //        -> std::common_type_t<PtrDiffT,
    //                              std::make_signed_t<decltype(c.size())>> {
    //        using R = std::common_type_t<PtrDiffT,
    //                                     std::make_signed_t<decltype(c.size())>>;
    //        return static_cast<R>(c.size());
    //    }

    template <typename T, SizeT N>
    constexpr SizeT size(const T (&array)[N]) noexcept {
        return N;
    }

    template <typename T, PtrDiffT N>
    constexpr PtrDiffT ssize(const T (&array)[N]) noexcept {
        return N;
    }
};
