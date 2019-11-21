#include <disk/url_store.hpp>
#include <disk/frontier.hpp>

#include <fb/stddef.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/string_view.hpp>
#include <fb/thread.hpp>
#include <fb/memory.hpp>
#include <fb/string.hpp>
#include <tcp/handle_socket.hpp>

#include <exception>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

using fb::FileDesc;
using fb::StringView;
using fb::String;
using fb::Thread;

using std::cerr;
using std::cout;
using std::ifstream;

constexpr auto DriverPrompt = ">> ";
constexpr auto UsageHint =
    "Usage: ./MasterDriver <port> [PARSE:] []\n"_sv;
constexpr auto UrlStoreFileName = "/tmp/urlstore.file"_sv;

template <typename T>
struct FreeDeleter { void operator()(char *p) { free(p); } };
template <typename T> using MUniquePtr = fb::UniquePtr<T,FreeDeleter<T>>;

#if __has_include(<readline/readline.h>)

#include <readline/readline.h>

MUniquePtr<char> getReadline() {
    return MUniquePtr<char>(readline(DriverPrompt), FreeDeleter<char>());
}

#else

MUniquePtr<char> getReadline() {
    std::cout << DriverPrompt;

    fb::String line;
    fb::getline(std::cin, line);

    auto ptr = MUniquePtr<char>(static_cast<char *>(malloc(line.size())),
                                FreeDeleter<char>());
    strcpy(ptr.get(), line.data());
    return ptr;
}

#endif

#if __has_include(<readline/history.h>)

#include <readline/history.h>

#else

void add_history(const char *) {}

#endif

struct ArgError : std::exception {};

struct Args {
    FileDesc desc;
    StringView UrlStore;
    StringView FrontierPrefix;
};

FileDesc parseArguments( int argc, char **argv );

int main(int argc, char **argv) try {
    UrlStore::init(UrlStoreFileName, false);
    Frontier::init("/tmp/frontier-bin.", false);

    auto sock = parseArguments( argc, argv );
    Thread socket_handler(handle_socket, &sock);

    while (true) {
        auto &urlStore = UrlStore::getStore();
        auto &frontier = Frontier::getFrontier();

        auto buf = getReadline();
        if (!buf)
            break;

        StringView line = buf.get();
        if (!line.empty())
            add_history(line.data());

        auto firstSpace = line.find(' ');
        auto firstWord = line.substr(0, firstSpace);

        if (firstWord == "add-seed"_sv) {
            line.removePrefix(firstSpace + 1);

            if (isspace(line.back()))
                line.removeSuffix(1);

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

        } else if (firstWord == "init"_sv) {
            std::cout << "Are you sure? Type \"jaeyoon\" to confirm: ";
            String str;
            std::cin >> str;
            if (str != "jaeyoon"_sv)
                continue;

            std::cout << "Alright...it's your funeral if something goes wrong...\n";

            UrlStore::init(UrlStoreFileName, true);
            Frontier::init("/tmp/frontier-bin.", true);
        }
    }

    socket_handler.join();
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

    FileDesc getBoundSocket() const {
        FileDesc sock (socket(res->ai_family,
                              res->ai_socktype,
                              res->ai_protocol));

        if (bind(sock, res->ai_addr, res->ai_addrlen)) {
            cerr << "Could not bind:\n";
            perror("");
            throw AddrError();
        }

        return sock;
    }

    ~AddrInfo() noexcept {
        if (res)
            freeaddrinfo(res);
    }
};

FileDesc parseArguments( int argc, char **argv ) try {
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
