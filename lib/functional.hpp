#pragma once

namespace fb {
    template <typename T = void>
    struct less {
        constexpr bool operator()(const T &lhs, const T &rhs) const {
            return lhs < rhs;
        }
    };
};
