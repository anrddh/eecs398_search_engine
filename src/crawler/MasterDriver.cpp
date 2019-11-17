#include "UrlStore.hpp"
#include "Frontier.hpp"

#include "../../lib/stddef.hpp"
#include "../../lib/file_descriptor.hpp"
#include "../../lib/string_view.hpp"
//#include "../../lib/thread.hpp"
#include "../../lib/memory.hpp"
#include "../../lib/string.hpp"

#include <exception>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

using fb::SizeT;
using fb::FileDesc;
using fb::StringView;
using fb::String;
//using fb::Thread;

using std::cerr;
using std::cout;
using std::ifstream;

struct ArgError : std::exception {};

template <typename T>
struct FreeDeleter { void operator()(char *p) { free(p); } };
template <typename T> using MUniquePtr = fb::UniquePtr<T,FreeDeleter<T>>;

FileDesc * parseArguments(int argc, char **argv);

constexpr auto UsageHint = "Usage: ./MasterDriver <port>\n"_sv;
constexpr auto UrlStoreFileName = "/tmp/urlstore.file"_sv;

int main(int argc, char **argv) try {
    UrlStore::init(UrlStoreFileName);
    Frontier::init("/tmp/frontier-bin.");
    auto &urlStore = UrlStore::getStore();
    auto &frontier = Frontier::getFrontier();

    // auto sockptr = parseArguments( argc, argv );
    // Thread socket_handler(handle_socket, sockptr);

    while (true) {
        MUniquePtr<char> buf(readline(">> "), FreeDeleter<char>());
        if (!buf)
            break;

        StringView line = buf.get();
        if (!line.empty())
            add_history(line.data());
        auto firstSpace = line.find(' ');
        auto firstWord = line.substr(0, firstSpace);

        if (firstWord == "add-seed"_sv) {
            line.removePrefix(firstSpace + 1);

            ifstream file;
            file.open(line.data());

            String url;
            while (fb::getline(file, url)) {
                auto idx = urlStore.addUrl(url);
                frontier.addUrl({ idx, 0 });
                cout << url << "\t\t\t\tidx: " << idx << '\n';
            }
        } else if (firstWord == "status"_sv) {

        } else if (firstWord == "shutdown"_sv) {

        } else if (firstWord == "info"_sv) {

        }
    }

    // socket_handler.join();
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
