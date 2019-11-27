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
    cout << DriverPrompt;

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
void * logThread(void *);

int main(int argc, char **argv) try {
    FileDesc sock = parseArguments( argc, argv );
    Thread socket_handler(handle_socket, &sock );

    Thread logger(logThread, nullptr);
    logger.detach();

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
           cout << "Frontier size: " << frontier.size() << '\n'
                << "Num connections: " << num_threads_alive() << endl;
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
         << " [-p port]\n\n"
         << "The `port' parameter accepts an integer in the range "
         << "[1024, 65536). Default value: `" << DefaultPort << "'\n";

    return 1;
}

FileDesc parseArguments(int argc, char **argv) try {
    option long_opts[] = {
        {"port",     required_argument, nullptr, 'p'},
        {"help",     no_argument,       nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    opterr = true;

    int option_idx;
    auto choice = 0;

    fb::String port;

    while ((choice =
            getopt_long(argc, argv, "p:h", long_opts, &option_idx))
           != -1) {
        switch (choice) {
        case 'p':
            port = optarg;
            break;
        case 'h':
        default:
            throw ArgError();
        }
    }

    auto rootDir = getRootDir();
    cout << "Writing to " << rootDir << '\n';

    auto logfileloc = rootDir + MasterLogFile;
    logfile.open(logfileloc.data());
    if (!logfile.is_open()) {
        cerr << "Could not open logfile `" << logfileloc
             << "'." << endl;
        throw ArgError();
    }

    auto urlstoreloc = rootDir + UrlStoreFile;
    UrlStore::init(urlstoreloc);

    auto frontierloc = rootDir + FrontierBinsPrefix;
    Frontier::init(frontierloc);

    auto anchorsloc = rootDir + AnchorStoreFile;
    AnchorStore::init(anchorsloc);

    auto adjloc = rootDir + AdjStoreFile;
    AdjStore::init(adjloc);

    auto urlinfo = rootDir + UrlInfoTableFile;
    UrlInfoTable::init(urlinfo);

    AddrInfo info(nullptr, port.empty() ? DefaultPort : port.data());
    return info.getBoundSocket();
} catch( const SocketException& ) {
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
        if ( url_offset == 0)
            continue;

        frontier.addUrl({ url_offset, 0 });
    }
}

void * logThread(void *) {
    while (true) {
        cout << "Current status:\n"
             << "Frontier size:\t" << Frontier::getFrontier().size() << '\n'
             << "Num connections:\t" << num_threads_alive() << endl;

        log(logfile, "Current status:\n",
            "Frontier size:\t", Frontier::getFrontier().size(), '\n',
            "Num connections:\t", num_threads_alive(), '\n');

        sleep(5);
    }

    return nullptr;
}
