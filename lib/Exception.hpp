// Created by Jaeyoon Kim 11/6/2019
#pragma once

#include <exception>
#include <utility>

namespace fb {

    // Simple exception class for reporting String errors
    // TODO remove std::exception!
    struct Exception : std::exception {
        Exception(const char *msg_) : msg(msg_) {}

        [[nodiscard]] virtual const char * what() const noexcept override {
            return msg;
        }

        const char * msg = "";
    };

}
