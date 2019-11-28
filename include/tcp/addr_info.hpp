#pragma once

#include <tcp/url_tcp.hpp>

#include <fb/file_descriptor.hpp>
#include <fb/memory.hpp>

#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

struct AddrInfo {
private:
    struct Deleter {
        void operator()(addrinfo *res) {
            freeaddrinfo(res);
        }
    };

public:
    fb::UniquePtr<addrinfo, Deleter> res;

    AddrInfo() {}

    AddrInfo( const char * hostname, const char *port ) {
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        addrinfo *ptr;
        int rval = getaddrinfo(hostname, port, &hints, &ptr);
        if (rval) {
            std::cerr << gai_strerror(rval) << '\n';
            throw SocketException("AddrInfor: Could not connect");
        }

        res = fb::UniquePtr<addrinfo, Deleter>(ptr, Deleter());
    }

    fb::FileDesc getSocket() const {
        return socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    }

    fb::FileDesc getBoundSocket() const {
        auto sock = getSocket();

        if (bind(sock, res->ai_addr, res->ai_addrlen)) {
            std::cerr << "Could not bind: ";
            perror("");
            throw SocketException("AddrInfor: Could not connect");
        }

        return sock;
    }

    fb::FileDesc getConnectedSocket() const {
        auto sock = getSocket();

        if (connect(sock, res->ai_addr, res->ai_addrlen)) {
            std::cerr << "Could not connect: ";
            perror("");
            throw SocketException("AddrInfor: Could not connect");
        }

        return sock;
    }
};
