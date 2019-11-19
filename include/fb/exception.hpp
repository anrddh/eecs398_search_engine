// Created by Jaeyoon Kim 11/6/2019
#pragma once

namespace fb {

    // Simple exception class for reporting errors
    template <typename T>
    struct ExceptionImpl {
        ExceptionImpl(const char *msg_) noexcept : msg(msg_) {}
        ExceptionImpl(const T &msg_) : msg(msg_) {}

        virtual ~ExceptionImpl() noexcept {}

        [[nodiscard]] virtual const char * what() const noexcept {
            return msg.data();
        }

        T msg = "";
    };

}
