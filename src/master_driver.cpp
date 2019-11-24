#include <disk/url_store.hpp>
#include <disk/frontier.hpp>
#include <disk/UrlTables.hpp>
#include <disk/UrlInfo.hpp>
#include <disk/anchor_store.hpp>
#include <disk/adj_store.hpp>
#include <disk/constants.hpp>
#include <disk/logfile.hpp>
#include <debug.hpp>

#include <tcp/handle_socket.hpp>
#include <tcp/constants.hpp>
#include <tcp/addr_info.hpp>

#include <fb/stddef.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/string_view.hpp>
#include <fb/thread.hpp>
#include <fb/memory.hpp>
#include <fb/string.hpp>

#include <exception>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
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

    auto ptr = MUniquePtr<char>(static_cast<char *>(malloc(line.size() + 1)),
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
void addSeed(fb::StringView fname);

int main(int argc, char **argv) try {
    FileDesc sock = parseArguments( argc, argv );
    Thread socket_handler(handle_socket, &sock );

    auto &frontier = Frontier::getFrontier();

    while (true) {
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

            addSeed(line);
        } else if (firstWord == "status"_sv) {
           cout << "Frontier size: " << frontier.size() << endl;
        } else if (firstWord == "assert"_sv) {
           UrlInfoTable::getTable().assert_invariance();
        } else if (firstWord == "url-info"_sv) {
            line.removePrefix(firstSpace + 1);

            if (isspace(line.back()))
                line.removeSuffix(1);

           UrlInfoTable::getTable().print_info( line );
        } else if (firstWord == "shutdown"_sv) {
           terminate_workers();
           //socket_handler.join();
           return 0;
        } else if (firstWord == "info"_sv) {

        }
    }

    socket_handler.join();
} catch (const ArgError &) {
    cerr << "Usage: " << argv[0]
         << " [-p port] [-u urls] [-a anchors] [-d adj] [-f frontier] "
         << "[-d adj] [-f frontier] [-i urlinfo] [-l log]\n\n"
         << "The `port' parameter accepts an integer in the range "
         << "[1024, 65536). Default value: `" << DefaultPort << "'\n"
         << "The `urls' parameter accepts a valid filename. Default value: `"
         << DefaultUrlStoreFile << "'\n"
         << "The `anchors' parameter accepts a valid filename. Default value: `"
         << DefaultAnchorStoreFile << "'\n"
         << "The `adj' parameter accepts a valid filename. Default value: `"
         << DefaultAdjStoreFile << "'\n"
         << "The `frontier' parameter accepts a valid filename prefix. Default value: `"
         << DefaultFrontierBinsPrefix << "'\n"
         << "The `log' parameter accepts a valid filename. Default value: `"
         << DefaultLogFile << "'\n";

    return 1;
}

FileDesc parseArguments(int argc, char **argv) try {
    option long_opts[] = {
        {"port",     required_argument, nullptr, 'p'},
        {"urls",     required_argument, nullptr, 'u'},
        {"anchors",  required_argument, nullptr, 'a'},
        {"adj",      required_argument, nullptr, 'd'},
        {"frontier", required_argument, nullptr, 'f'},
        {"urlinfo",  required_argument, nullptr, 'i'},
        {"log",      required_argument, nullptr, 'l'},
        {"help",     no_argument,       nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    opterr = true;

    int option_idx;
    auto choice = 0;

    fb::String port, urls, anchors, adj, frontier, urlinfo, logs;

    while ((choice =
            getopt_long(argc, argv, "p:u:a:d:f:i:l:h", long_opts, &option_idx))
           != -1) {
        switch (choice) {
        case 'p':
            port = optarg;
            break;
        case 'u':
            urls = optarg;
            break;
        case 'a':
            anchors = optarg;
            break;
        case 'd':
            adj = optarg;
            break;
        case 'f':
            frontier = optarg;
            break;
        case 'i':
            urlinfo = optarg;
            break;
        case 'l':
            logs = optarg;
            break;
        case 'h':
        default:
            throw ArgError();
        }
    }

    if (urls.empty() || frontier.empty() || anchors.empty() || adj.empty() ||
        urlinfo.empty() || logs.empty()) {
        auto rval = mkdir("/tmp/crawler", S_IRWXU | S_IRWXG | S_IRWXO);
        if (rval && errno != EEXIST) {
            std::cerr << "Error when creating /tmp/crawler: " << strerror(errno);
            throw ArgError();
        }
    }

    if (logs.empty()) {
        std::cout << "Using default logfile: " << DefaultLogFile << '\n';
        logfile.open(DefaultLogFile.data());
    } else {
        logfile.open(logs.data());
    }

    if (!logfile.is_open()) {
        std::cerr << "Could not open logfile." << std::endl;
        throw ArgError();
    }

    if (urls.empty()) {
        std::cout << "Using default url store file: " << DefaultUrlStoreFile << '\n';
        UrlStore::init(DefaultUrlStoreFile);
    } else {
        UrlStore::init(urls);
    }

    std::cout << std::endl;

    if (frontier.empty()) {
        std::cout << "Using default frontier prefix: " << DefaultFrontierBinsPrefix << '\n';
        Frontier::init(fb::String(DefaultFrontierBinsPrefix.data(), DefaultFrontierBinsPrefix.size()));
    } else {
        Frontier::init(fb::String(DefaultFrontierBinsPrefix.data(), DefaultFrontierBinsPrefix.size()));
        Frontier::init(fb::String(frontier.data(), frontier.size()));
    }

    if (anchors.empty()) {
        std::cout << "Using default anchor store file: " << DefaultAnchorStoreFile << '\n';
        AnchorStore::init(DefaultAnchorStoreFile);
    } else {
        AnchorStore::init(anchors);
    }

    if (adj.empty()) {
        std::cout << "Using default adjacency store file: " << DefaultAdjStoreFile << '\n';
        AdjStore::init(DefaultAdjStoreFile);
    } else {
        AdjStore::init(adj);
    }

    if (urlinfo.empty()) {
        std::cout << "Using default url info store file: " << DefaultUrlInfoTableFile << '\n';
        UrlInfoTable::init(DefaultUrlInfoTableFile);
    } else {
        UrlInfoTable::init(urlinfo);
    }

    AddrInfo info(nullptr, port.empty() ? DefaultPort : port.data());
    return info.getBoundSocket();
} catch( const AddrInfo::AddrError & ) {
   throw ArgError();
} catch ( const FileDesc::ConstructionError &e ) {
   cerr << e.what() << '\n';
   throw ArgError();
}

void addSeed(StringView fname) {
    auto &frontier = Frontier::getFrontier();

    ifstream file;
    file.open(fname.data());

    String url;
    while (fb::getline(file, url)) {
        fb::SizeT url_offset = UrlInfoTable::getTable().addSeed(url);
        if ( url_offset == 0) {
            logfile << "url " << url << " was already in UrlInfoTable" << std::endl;
            continue;
        }

        logfile << "Got url " << url << " stored it as " << UrlStore::getStore().getUrl( url_offset ) << std::endl;
        //fb::SizeT rank = RankUrl( UrlStore::getStore().getUrl( url_offset ) );
        frontier.addUrl({ url_offset, 0 });
        logfile << url << "\t\t\t\toffset: " << url_offset << '\n';
    }
}
