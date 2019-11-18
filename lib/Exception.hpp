// Created by Jaeyoon Kim 11/6/2019
#pragma once

#include "string.hpp"

#include <exception>
#include <utility>
#include <stdexcept>

namespace fb {

    // Simple exception class for reporting String errors
    // TODO remove std::exception!
    struct Exception {
        Exception(const char *msg_) noexcept : msg(msg_) {}
        Exception(const String &msg_) : msg(msg_) {}

        [[nodiscard]] virtual const char * what() const noexcept {
            return msg.data();
        }

        String msg = "";
    };

}
