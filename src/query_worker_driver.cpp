// Written by Jaeyoon Kim and Chandler

#include <parse/parser.hpp>
#include <parse/query_parser.hpp>
#include <http/download_html.hpp>
#include <tcp/url_tcp.hpp>
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>
#include <ranker/ranker.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <iostream>
#include <fstream>
#include <cstdio>

#include <getopt.h>

#include <fb/vector.hpp>
#include <fb/string.hpp>

#include <query/query_result.hpp>

#define MAX_SNIP_WINDOW 100
constexpr int NUM_QUERY_RESULTS = 100;

// TCP protocol
// worker establishes socket to master (sends a verfication code)
// master to worker: query (string)
// worker to master: num (int)
//    [ urlOffset (SizeT), rank (double), snippet (string)] x num
using namespace fb;

Vector<Thread> threads;
Vector<fb::UniquePtr<IndexReader>> Readers;
TopPages Results(NUM_QUERY_RESULTS);
fb::String dirname;

// to be used as arguments to a thread
struct IndexInfoArg {
    Expression* e;
    IndexReader* reader;
};

// gets an index info
void* RankPages( void *info ) {
    // Just keep calling add to top pages
    IndexInfoArg arg = *(IndexInfoArg *) info; //get the args
    delete (IndexInfoArg *) info;

    ConstraintSolver cSolver = arg.e->Constraints(*arg.reader); //make the constraint solver
    cSolver.solve( );
    // Vector<rank_stats> docsToRank = cSolver.GetDocumentsToRank(); //get the docs to rank
    // Vector<SizeT> docFreqs = cSolver.GetDocFrequencies(); //get the doc frequencies
    // tfidf_rank(docsToRank, docFreqs); //tf_idf the pages
    // for( rank_stats& doc : docsToRank ){
    //     snip_window window = snippet_window_rank(MergeVectors(doc.occurrences), doc.total_term_count, MAX_SNIP_WINDOW); //setting max_snip_window to 150
    //     SnippetStats stats = { dirname + fb::String(PageStoreFile.data()) + fb::toString((int)doc.page_store_number), doc.page_store_index, window };
    //     fb::Pair<fb::String, fb::String> SnipTit = GenerateSnippetsAndTitle(stats, doc);
    //     QueryResult result = { doc.UrlId, SnipTit.second, SnipTit.first, doc.rank };
    //     Results.add(std::move(result));
    // }
    for(auto &result : cSolver.results)
        Results.add(std::move(result));

    return nullptr;
}


AddrInfo masterLoc;

fb::FileDesc open_socket_to_master() {
    while (true) {
        try {
            auto sock = masterLoc.getConnectedSocket();

            // Finished establishing socket
            // Send verfication message
            send_int(sock, VERFICATION_CODE);

            // Tell them we are workers
            send_char(sock, 'W');
            return sock;
        } catch (SocketException &se) {
            std::cerr << "failed establish connection with master. "
                << se.what() << std::endl;
            std::cerr << "retrying" << std::endl;
        }
    }
}

// We will create an thread for each index
int main( int argc, char **argv ) {
    bool local_mode = false;

    if(argc == 4)
        {
        local_mode = true;
        }
    else if(argc != 6)
        {
        fb::String ErrorMessage = fb::String("Usage: ") + fb::String(argv[0]) + fb::String(" [PATH TO INDEX FOLDER] [INDEX FILE PREFIX] [NUM_INDEX_FILES] [SERVER_IP] [SERVER_PORT]");
        std::cout << ErrorMessage << std::endl;
        exit(1);
        }


    dirname = fb::String(argv[1]);
    fb::String Prefix(argv[2]);
    int num_index_files = atoi(argv[3]);
    fb::String server_name, server_port;

    for (int i = 0; i < num_index_files; ++i) {
        fb::String filename = dirname + "/" + Prefix + fb::toString(i);
        int f = open(filename.data(), O_RDWR);
        if(f < 0){
            // write debug message
            std::cout << "ERROR OPENING FILE: " << filename << std::endl;
            exit(1);
        }

        struct stat details;
        fstat(f, &details);

        char *IndexPtr = (char *)mmap(nullptr, details.st_size, PROT_READ, MAP_PRIVATE, f, 0);
        Readers.pushBack(fb::makeUnique<IndexReader>(IndexPtr, i));
    }


    fb::FileDesc sock;
    if(!local_mode)
        {
        server_name = fb::String(argv[4]);
        server_port = fb::String(argv[5]);
        masterLoc = AddrInfo(server_name.data(), server_port.data());

        sock = open_socket_to_master();
    }

    while(true) {
        fb::String query;
        if(local_mode)
            {
            std::cout << "Search: ";
            std::string qString;
            std::getline(std::cin, qString);
            query = fb::String( qString.c_str( ) );
            }
        else
            {
            try {
                query = recv_str( sock );
                std::cout << "Got query " << query << std::endl;
            } catch( SocketException& se ) {
                std::cerr << "Got exception " << se.what() << std::endl;
                sock = open_socket_to_master();
            }
        }
        QueryParser QuePasa(query);
        auto e = QuePasa.Parse();
        //now we spawn a thread for each index, and give it e
        Vector<Thread> threads;
        for ( auto& reader : Readers ) {
            IndexInfoArg* info = new IndexInfoArg( {e.get(), reader.get()} );
            threads.emplaceBack(RankPages, info);
        }

        for ( auto& thread : threads ) {
            thread.join();
        }
        std::cout << "attached" << std::endl;

        if(local_mode)
            {
            fb::Vector<QueryResult> pages = Results.GetTopResults( );
            std::cout << "\n=====================\n||     results     || \n====================="<< std::endl;
            for(QueryResult &res : pages)
                {
                std::cout << "Title: " << res.Title << std::endl << "UrlID: " << res.UrlId << std::endl << "Snippet: " << res.Snippet << std::endl << std::endl;
                }
            }
        else {
            try {
                Results.send_and_reset( sock );
            } catch( SocketException& se ) {
                std::cerr << "Got exception " << se.what() << std::endl;
                sock = open_socket_to_master();
            }
        }
    }
}
