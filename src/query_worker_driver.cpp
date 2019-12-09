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
#include <fb/shared_mutex.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <iostream>
#include <fstream>

#include <getopt.h>

#include <fb/vector.hpp>

// TCP protocol
// worker establishes socket to master (sends a verfication code)
// master to worker: query (string)
// worker to master: num (int) 
//    [ urlOffset (SizeT), rank (double), snippet (string)] x num
//    WARNING TO ANI - sending int and sizet are different tcp function calls
using namespace fb;

Vector<Thread> threads;

// gets an index info
void* RankPages( void * ) {
   // Just keep calling add to top pages

}

void* listen_to_master(void *) {
}

struct IndexInfo {
   String idxFileName;
   int idxSockFd;
   String pageStoreFileName;
   int pageStoreFd;
   void* mmapAddr;
};

Vector< PageStoreInfo > indexes;

// We will create an thread for each index
int main( int argc, char **argv ) {
}
