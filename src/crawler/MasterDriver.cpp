#include "../../lib/stddef.hpp"
#include "../../lib/file_descriptor.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/thread.hpp"
#include "handle_socket.hpp"

#include <exception>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

using fb::SizeT;
using fb::FileDesc;
using fb::StringView;
using fb::Thread;

using std::cerr;

struct ArgError : std::exception {};

FileDesc * parseArguments(int argc, char **argv);

constexpr auto UsageHint = "Usage: ./MasterDriver <port>\n"_sv;

int main(int argc, char **argv) try {
    auto sockptr = parseArguments( argc, argv );
    Thread socket_handler(handle_socket, sockptr);
    socket_handler.detach();

    while(true);

} catch (const ArgError &) {
    cerr << UsageHint;
    return 1;
}

struct AddrInfo {
    addrinfo *res = nullptr;

    struct AddrError : std::exception {};

    AddrInfo( const char *port ) {
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        auto rval = getaddrinfo(nullptr, port, &hints, &res);
        if (rval) {
           cerr << gai_strerror(rval) << '\n';
           throw AddrError();
        }
    }

    FileDesc * getBoundSocket() const {
        FileDesc *sock =
            new FileDesc(socket(res->ai_family,
                                res->ai_socktype,
                                res->ai_protocol));

        if (bind(*sock, res->ai_addr, res->ai_addrlen)) {
            delete sock;
            cerr << "Could not bind:\n";
            perror("");
            throw AddrError();
            return nullptr;
        }

        return sock;
    }

    ~AddrInfo() noexcept {
        if (res)
            freeaddrinfo(res);
    }
};

FileDesc * parseArguments( int argc, char **argv ) try {
    if (argc != 2)
        throw ArgError();

    AddrInfo info(argv[1]);
    return info.getBoundSocket();
} catch( const AddrInfo::AddrError & ) {
    throw ArgError();
} catch ( const FileDesc::ConstructionError &e ) {
   cerr << e.what() << '\n';
   throw ArgError();
}
