#include <parse/parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>

#include <iostream>

#include <getopt.h>

constexpr fb::SizeT NumThreadsToSpawn = 50;

fb::Mutex endLock;
fb::CV endCV;

struct ArgError : std::exception {};

fb::SizeT parseArguments( int argc, char **argv );
void * parsePages( void * );

int main(int argc, char **argv) try {
    auto numThreads = parseArguments(argc, argv);

    SSLWrapper::SSLInit();

    fb::Vector<fb::Thread> threads;
    threads.reserve(numThreads);

    std::cout << "Invoking " << numThreads << " threads\n";
    for (fb::SizeT i = 0; i < numThreads; ++i)
        threads.emplaceBack(parsePages, nullptr);

    fb::String userInput;
    do {
        std::cout << "Shut down? (type 'YES')\n";
        std::cin >> userInput;
        if ( userInput == "YES" ) {
            std::cout << "Are you sure? (type 'YES')\n";
            std::cin >>  userInput;
            if ( userInput == "YES" ) {
                std::cout << "Really really sure? (type 'JAEYOON')\n";
                std::cin >> userInput;
                if ( userInput == "JAEYOON" ) {
                    std::cout << "OK... Bye World" <<  std::endl;
                    break;
                }
			}
        }
    } while (std::cin >> userInput);

   initiate_shut_down();
   std::cout << "Shutting down." << std::endl;
   for (auto &thread : threads)
       thread.join();
} catch (const ArgError &) {
    std::cerr << "Usage: " << argv[0]
              << " [-p port] [-o hostname] [-a pagestore] [-l logs] [-f frontier] [-t threads]\n\n"
              << "The `port' parameter accepts an integer in the range "
              << "[1024, 65536). Default value: `" << DefaultPort << "'\n"
              << "The `hostname' parameter accepts a valid filename. Default value: `"
              << DefaultHostname << "'\n"
              << "The `pagestore' parameter accepts a valid filename. Default value: `"
              << DefaultPageStoreFile << "'\n"
              << "The `logs' parameter accepts a valid filename. Default value: `"
              << DefaultLogFile << "'\n"
              << "The `threads' parameter accepts a valid non-negative integer. Default value: `"
              << NumThreadsToSpawn << "'\n";

    return 1;
}

fb::SizeT parseArguments( int argc, char **argv ) {
    option long_opts[] = {
        {"hostname",  required_argument, nullptr, 'o'},
        {"port",      required_argument, nullptr, 'p'},
        {"pagestore", required_argument, nullptr, 'a'},
        {"help",      no_argument,       nullptr, 'h'},
        {"logs",      no_argument,       nullptr, 'l'},
        {"threads",   no_argument,       nullptr, 't'},
        {nullptr, 0, nullptr, 0}
    };
    opterr = true;

    int option_idx;
    auto choice = 0;

    fb::String hostname, port, pagebin, logs, threads;

    while ((choice =
            getopt_long(argc, argv, "o:p:a:hl:t:", long_opts, &option_idx))
           != -1) {
        switch (choice) {
        case 'p':
            port = optarg;
            break;
        case 'o':
            hostname = optarg;
            break;
        case 'a':
            pagebin = optarg;
            break;
        case 'l':
            logs = optarg;
            break;
        case 't':
            threads = optarg;
            break;
        case 'h':
        default:
            throw ArgError();
        }
    }

    if (pagebin.empty() || logs.empty()) {
        auto rval = mkdir("/tmp/crawler", S_IRWXU | S_IRWXG | S_IRWXO);
        if (rval && errno != EEXIST) {
            std::cerr << "Error when creating /tmp/crawler: " << strerror(errno)
                      << '\n';
            throw ArgError();
        }
    }

    if (logs.empty()) {
        std::cout << "Using default logfile: " << DefaultWorkerLogFile << '\n';
        logfile.open(DefaultWorkerLogFile.data());
    } else {
        logfile.open(logs.data());
    }

    if (!logfile.is_open()) {
        std::cerr << "Could not open logfile." << std::endl;
        throw ArgError();
    }

    if (pagebin.empty()) {
        std::cout << "Using default pagebin file: " << DefaultPageStoreFile << '\n';
        initializeFileName({
                DefaultPageStoreFile.data(),
                DefaultPageStoreFile.size()
            });
    } else {
        initializeFileName(pagebin);
    }

    set_master_ip({
            hostname.empty() ? DefaultHostname : hostname.data(),
            port.empty() ? DefaultPort : port.data()
        });

    return threads.empty() ?
        NumThreadsToSpawn :
        static_cast<fb::SizeT>(stoll(threads));
}

void * parsePages( void * )
   {
   while( true )
      {
      auto urlPair = get_url_to_parse( );

	  if ( urlPair.second.empty( ) )
         {
         std::cout << "Got empty link" << std::endl;
         endCV.signal();
         return nullptr;
         }

	  try
         {
         auto downloader = HTTPDownloader( );
         fb::String result = downloader.PrintHtml( urlPair.second );

         ParsedUrl url( downloader.finalUrl );

         fb::Parser parser( result, url );

         parser.parse( );

         fb::Vector< fb::Pair<fb::String, fb::String> > links;

         add_parsed( { urlPair.first, parser.urlAnchorText.convert_to_vector() } );

         addPage( parser.extractPage( urlPair.first ) ); // TODO I think move ctor will be called? -Jaeyoon
   		}
		catch ( ConnectionException e )
            {
            }
      }
   std::cout << "True evaluating to false?" << std::endl;
   return nullptr;
   }
