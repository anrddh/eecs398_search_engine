// Written by Jaeyoon Kim added 12/10
//
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <query/query_result.hpp>
#include <query/page_result.hpp>

#include <disk/page_store.hpp>
#include <disk/url_store.hpp>
#include <disk/UrlInfo.hpp>
#include <disk/constants.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>
#include <fb/unordered_set.hpp>

#include <debug.hpp>

#include <iostream>
#include <fstream>

#include <getopt.h>

using std::cout;
using std::cerr;
using std::endl;

using namespace fb;

constexpr int MAX_NUM_PAGES = 100; // Number of pages sent back to front end server per query
static_assert( MAX_NUM_PAGES > 0 );

constexpr int TCP_TIMEOUT_LIMIT = 5; // Number of seconds before timeout (only applies to talking to workers)

// The value we scale the ranking based on url
constexpr double urlWeight = 0.01; 

// Server when accepting:
// verfication code (int)
// char Q (query)
// or char W (new worker)
constexpr char queryMessageType = 'Q';
constexpr char workerMessageType = 'W';

FileDesc parseArguments(int argc, char **argv);

//Hash instance for PageResult
template <>
struct fb::Hash<FileDesc> {
    // Jin Soo removed constexpr
    SizeT operator() ( const FileDesc& data ) const noexcept {
        return fnvHash( (char *) &data, sizeof(data) );
    }
};

Mutex socketsMtx;
UnorderedSet<FileDesc> socketsToWorkers;

struct WorkerArg {
    FileDesc* sock;
    StringView query;
    TopNQueue<PageResult>* topPages;
};

// TODO this is duplicated in worker driver
struct ArgError : std::exception
   {
   };

// If this is a querry, we do not return until the query is completed
// Closes sockets if necessary
void handle_connections( FileDesc&& sock );
void handle_query( FileDesc&& sock );
void send_page_result( int sock, const PageResult& );
// Takes a dynamically allocated Worker arg
// this function deletes the pointer
// returns nullptr
void* ask_workers(void* );
QueryResult recv_query_result( int sock );
void handle_new_worker( FileDesc&& sock );

int main( int argc, char **argv ) {
    FileDesc server_fd = parseArguments( argc, argv );

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while ( true ) {
        // Do not use FileDesc!
        // handle_connection will close sockets
        try {
            int sock = accept(server_fd, nullptr, nullptr);
            std::cout << "got new socket!" << std::endl;
            handle_connections( std::move( sock ) );
        } catch (SocketException& se) {
            cout << se.what() << endl;

        } catch (...) {
            cout << "failed in accept!" << endl;
            throw;
        }

    }
}

void handle_connections( FileDesc&& sock ) {
    if (recv_int(sock) != VERFICATION_CODE) {
        // We just ignore it!
        return;
    }

    char messageType = recv_char( sock );

    switch ( messageType ) {
        case queryMessageType:
            handle_query( std::move( sock ) );
            return;
        case workerMessageType:
            handle_new_worker( std::move( sock ) );
            return;
        default:
            return;
    }
}

void handle_query( FileDesc&& sock ) {
    String query = recv_str( sock );
    Vector<Thread> threads;
    TopNQueue<PageResult> topPages( MAX_NUM_PAGES );
    std::cout << "got new query!" << std::endl;

    // TODO handle exceptions

    for ( auto it = socketsToWorkers.begin(); it != socketsToWorkers.end(); ++it ) {
        threads.emplaceBack( ask_workers, new WorkerArg{ &*it, query, &topPages } );
    }

    for ( Thread& t : threads ) {
        t.join();
    }

    // testing code. TODO delete below
    /*

    String urlStr = "https://test_url";
    String titleStr = "Some good title ";
    String snippetStr = "blah blah blah ";
    for (int i = 0; i < 100; ++i ) {
        PageResult pr;
        String val = toString( ++n );
        pr.Url = urlStr + val;
        pr.Title = titleStr + val;
        pr.Snippet = snippetStr + val;
        pr.rank = n;
        topPages.push( std::move( pr ) );
    }
    */

    std::cout << "sending " << topPages.size() << " pages" << std::endl;
    send_int( sock, topPages.size() );
    while ( !topPages.empty() ) {
        send_page_result( sock, topPages.top() );
        topPages.pop();
    }
}

void send_page_result( int sock, const PageResult& pr ) {
    send_str( sock, pr.Url );
    send_str( sock, pr.Title );
    send_str( sock, pr.Snippet );
    send_double( sock, pr.rank );
}

void* ask_workers( void* worker_query ) {
    WorkerArg arg = * ( WorkerArg* ) worker_query; // copy
    delete (WorkerArg*) worker_query;

    try {
        send_str( *arg.sock, arg.query );
        int numPages = recv_int( *arg.sock );
        std::cout << "received " << numPages << " many pages " << std::endl;
        for ( int i = 0; i < numPages; ++i ) {
            std::cout << "adding" << i << std::endl;
            PageResult pr;
            pr.Url += UrlStore::getStore().getUrl( recv_uint64_t( *arg.sock ) );
            pr.Title = recv_str( *arg.sock );
            pr.Snippet = recv_str( *arg.sock );
            pr.rank = recv_double( *arg.sock );
            arg.topPages->push( std::move( pr ) );
            std::cout <<  pr.Url << " length " << std::endl; // TODO delete this
            std::cout << "\ttitle " << pr.Title << " length " << pr.Title.size() << std::endl; // TODO delete this
            std::cout << "\tsnippet " << pr.Snippet << " length " << pr.Snippet.size() << std::endl; // TODO delete this
            std::cout << "\ttfidf ranking: " << pr.Url << " url ranking " << urlWeight * RankUrl( pr.Url ) << std::endl; // TODO delete this
            pr.rank += urlWeight * RankUrl( pr.Url );
        }

    } catch ( SocketException& se ) {
        // Remove this socket from list of valid sockets!
        std::cout << "error" << se.what() << std::endl;
        AutoLock l( socketsMtx );
        socketsToWorkers.erase( *arg.sock );
    }

    return nullptr;
}

void handle_new_worker( FileDesc&& sock ) {
    int set = 1;
    setsockopt(sock, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));
    struct timeval timeout;
    timeout.tv_sec = TCP_TIMEOUT_LIMIT;
    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        std::cerr << "failed at setsockopt recv" << std::endl;

    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        std::cerr << "failed at setsockopt send" << std::endl;

    AutoLock l( socketsMtx );
    bool success = socketsToWorkers.insert( std::move( sock ) );
    assert( success );
}

FileDesc parseArguments( int argc, char **argv ) {
    try {
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
       auto urlstoreloc = rootDir + UrlStoreFile;
       UrlStore::init(urlstoreloc);

        AddrInfo info(nullptr, port.empty() ? DefaultPort : port.data());
        return info.getBoundSocket();
    } catch( const SocketException& ) {
        throw ArgError();
    } catch ( const FileDesc::ConstructionError &e ) {
        cerr << e.what() << '\n';
        throw ArgError();
    }
}
