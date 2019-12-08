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

constexpr fb::SizeT NumThreads = 25;

using namespace fb;

Vector<Thread> threads;

void* listen_to_master(void *) {
}

int main( int argc, char **argv ) {
}
