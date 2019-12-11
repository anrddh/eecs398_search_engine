// Written by Jaeyoon Kim added 12/10
//
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>

#include <debug.hpp>

#include <iostream>
#include <fstream>

#include <getopt.h>

using std::cout;
using std::endl;

using fb;

Mutex socketsMtx;
unordered_set<FileDesc> socketsToWorkers;

FileDesc parseArguments(int argc, char **argv);

// If this is a querry, we do not return until the query is completed
// Closes sockets if necessary
void handle_connections( FileDesc&& sock );

void handle_query( FileDesc&& sock );
// Takes a pair of string view and socket number
// returns nullptr
void* ask_workers(void* );

void handle_new_worker( FileDesc&& sock );

// Server when accepting:
// verfication code (int)
// char Q (query)
// or char W (new worker)
constexpr char queryMessageType = 'Q';
constexpr char workerMessageType = 'W';
int main( int argc, char **argv ) {
    FileDesc server_fd = parseArguments( int argc, char **argv );

    while ( true ) {
        // Do not use FileDesc!
        // handle_connection will close sockets
        FileDesc sock;
        if ((sock = accept(server_fd, nullptr, nullptr)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handle_connections( sock );
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
            handle_query( sock );
            return;
        case workerMessageType:
            handle_new_worker( sock );
            return;
        default:
            return;
    }
}

void handle_query( FileDesc&& sock ) {
    String query = recv_str( sock );
    Vector<Thread> threads;
    for ( auto it = socketsToWorkers.begin(); it != socketsToWorkers.end(); ++it ) {
        threads.emplaceBack( new Pair<FileDesc*, StringView>( &*it, query ) );
    }

    for ( Thread& t : threads ) {
        t.join();
    }
}

void* ask_workers( void* worker_query ) {
    StringView query = ( ( Pair<FileDesc*, StringView> * ) worker_query )->first;
    FileDesc* sock = ( ( Pair<FileDesc*, StringView> * ) worker_query )->second;
    delete worker_query;

    try {
        send_str( *sock, query );

    } catch ( SocketException& se ) {
        // TODO delete things
    }

    socketsMtx;
    return nullptr;
}


void handle_new_worker( FileDesc&& sock ) {
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

        AddrInfo info(nullptr, port.empty() ? DefaultPort : port.data());
        return info.getBoundSocket();
    } catch( const SocketException& ) {
        throw ArgError();
    } catch ( const FileDesc::ConstructionError &e ) {
        cerr << e.what() << '\n';
        throw ArgError();
    }
}
