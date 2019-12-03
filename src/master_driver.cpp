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
using fb::SizeT;
using fb::Vector;
using fb::Pair;

using std::cerr;
using std::atomic;
using std::cout;
using std::endl;
using std::ifstream;

constexpr auto DriverPrompt = ">> ";
constexpr SizeT num_threads_adding_bloom_filter = 16;

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

    if (line.empty())
        return {};

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
void addSeed(StringView fname);
void * logThread(void *);
void* add_to_bloom_filter(void * val);

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
           //UrlInfoTable::getTable().assert_invariance();
        } else if (firstWord == "make_bloom_filter"_sv) {
            SizeT size = UrlStore::getStore().access_disk().size();
            Vector<Thread> threads;
            for (SizeT i = 0; i < num_threads_adding_bloom_filter; ++i) {
               threads.emplaceBack(add_to_bloom_filter, new
                     Pair<char*, SizeT>( UrlStore::getStore().access_disk().data()
                        + i * size / num_threads_adding_bloom_filter,
                        size /num_threads_adding_bloom_filter));
               }

            for (SizeT i = 0; i < num_threads_adding_bloom_filter; ++i) {
               threads[i].join();
               cout << "Joined thread " << i + 1 << " out of "
                    << num_threads_adding_bloom_filter << endl;
            }

            cout << "Done adding to bloom filter" << endl;
        } else if (firstWord == "print-urls"_sv) {
            line.removePrefix(firstSpace + 1);
            Frontier::getFrontier().printUrls();
        } else if (firstWord == "url-info"_sv) {
            line.removePrefix(firstSpace + 1);

            if (isspace(line.back()))
                line.removeSuffix(1);

           //UrlInfoTable::getTable().print_info( line );
        } else if (firstWord == "shutdown"_sv) {
           terminate_workers();
              Frontier::shutdown();
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

       String port;
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
    //UrlInfoTable::init(urlinfo);

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
    Vector< String > urls;
    while (fb::getline(file, url)) {
       urls.pushBack( std::move( url ) );
    }
    frontier.addUrls( std::move( urls ) );
}

void * logThread(void *) {
    while (true) {
        log(logfile, "Current status:\n",
            "Frontier size:\t\t", Frontier::getFrontier().size(), '\n',
            "Num connections:\t", num_threads_alive(), '\n');

        sleep(60);
    }

    return nullptr;
}

atomic<SizeT> num_added_to_bloom_filter = 0;

void* add_to_bloom_filter(void * val) {
   Pair<char*, SizeT>* val_casted = (Pair<char*, SizeT>*) val;
   char* ptr = val_casted->first;
   SizeT size = val_casted->second;
   delete val_casted;
   Frontier& f = Frontier::getFrontier();
   for ( SizeT n = 0; n + 1 < size; ++n) {
      if ( ptr[ n ] == '\0' ) {
         f.addSeen( ptr + n + 1 );
         if (++num_added_to_bloom_filter % 1000000 == 0) {
            std::cout << "Num added = " << num_added_to_bloom_filter << std::endl;
         }
      }
   }
   return nullptr;
}
