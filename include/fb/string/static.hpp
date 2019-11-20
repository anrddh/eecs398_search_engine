#pragma once

#include "../stddef.hpp"
#include "../type_traits.hpp"
#include "../string_view.hpp"

namespace fb {

template <SizeT N>
struct StaticString {
    constexpr StaticString(StringView s1,
                           StringView s2) noexcept {
        static_assert(s1.size() + s2.size() <= N);
        for (SizeT i = 0; i < s1.size(); ++i)
            array[i] = s1[i];

        for (SizeT i = 0; i < s2.size(); ++i)
            array[i + s1.size()] = s2[i];

        array[s1.size() + s2.size()] = 0;
    }

    char array[N + 1] = {0};
};

}
