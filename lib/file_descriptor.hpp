#pragma once

#include "utility.hpp"

#include <exception>

#include <unistd.h>

namespace fb {

    struct FileDesc {
        struct ConstructionError : std::exception {
            const char *msg = "";

            ConstructionError(const char *msg_) : msg(msg_) {}

            virtual const char * what() const noexcept override {
                return msg;
            }
        };

        int fd = -1;

        FileDesc() = default;

        FileDesc(int fd_in) : fd(fd_in) {
            if (fd_in < -1)
                throw ConstructionError("Invalid file descriptor.");
        }

        FileDesc(FileDesc &&rhs) noexcept : fd(rhs.fd) {
            rhs.fd = -1;
        }

        FileDesc & operator=(FileDesc &&rhs) noexcept {
            fb::swap(fd, rhs.fd);
        }

        FileDesc(FileDesc &rhs) {
            auto new_desc = dup(rhs.fd);
            if (new_desc == -1)
                throw ConstructionError("Failed to dup.");
            fd = new_desc;
        }

        FileDesc & operator=(FileDesc rhs) {
            fb::swap(fd, rhs.fd);
        }

        ~FileDesc() {
            close(fd);
        }

        constexpr operator int() noexcept {
            return fd;
        }
    };

};