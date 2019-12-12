#include <parse/parser.hpp>
#include <parse/query_parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
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

#define MAX_SNIP_WINDOW 150
#define NUM_QUERY_RESULTS 100

// TCP protocol
// worker establishes socket to master (sends a verfication code)
// master to worker: query (string)
// worker to master: num (int)
//    [ urlOffset (SizeT), rank (double), snippet (string)] x num
//    WARNING TO ANI - sending int and sizet are different tcp function calls
using namespace fb;

Vector<Thread> threads;
Vector<fb::UniquePtr<IndexReader>> Readers;
TopNQueue<QueryResult> Results(NUM_QUERY_RESULTS);

// to be used as arguments to a thread
struct IndexInfoArg {
    fb::UniquePtr<Expression>& e;
    fb::UniquePtr<IndexReader>& reader;
};

// gets an index info
void* RankPages( void *info ) {
    // Just keep calling add to top pages
    IndexInfoArg &arg = *(IndexInfoArg *) info; //get the args
    ConstraintSolver cSolver = arg.e->Constraints(*arg.reader); //make the constraint solver
    Vector<rank_stats> docsToRank = cSolver.GetDocumentsToRank(); //get the docs to rank
    Vector<SizeT> docFreqs = cSolver.GetDocFrequencies(); //get the doc frequencies
    tfidf_rank(docsToRank, docFreqs); //tf_idf the pages
    for( rank_stats& doc : docsToRank ){
        snip_window window = snippet_window_rank(MergeVectors(doc.occurrences), MAX_SNIP_WINDOW); //setting max_snip_window to 150
        SnippetStats stats = { fb::String(PageStoreFile.data()) + fb::toString((int)doc.page_store_number), doc.page_store_index, window };
        fb::Pair<fb::String, fb::String> SnipTit = GenerateSnippetsAndTitle(stats, doc);
        QueryResult result = { doc.UrlId, SnipTit.second, SnipTit.first, doc.rank };
        Results.push(std::move(result));
    }
    return nullptr;
}

void* listen_to_master(void *) {
}

Vector<String> GenerateSnippets( Vector<SnippetStats> Stats );

Vector<QueryResult> SearchQuery( String query );

// struct IndexInfo {
//    String idxFileName;
//    int idxSockFd;
//    String pageStoreFileName;
//    int pageStoreFd;
//    void* mmapAddr;
// };

// We will create an thread for each index
int main( int argc, char **argv ) {

    if(argc != 4){
        fb::String ErrorMessage = fb::String("Usage: ") + fb::String(argv[0]) + fb::String(" [PATH TO INDEX FOLDER] [INDEX FILE PREFIX] [NUM_INDEX_FILES]");
        std::cout << ErrorMessage << std::endl;
        exit(1);
    }

    fb::String dirname(argv[1]);
    fb::String Prefix(argv[2]);
    int num_index_files = atoi(argv[3]);
    // DIR *dirp = opendir(dirname.data());
    //
    // struct dirent *dir;
    // while( dir = readdir(dirp), dir != NULL ){
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

        char *IndexPtr = (char *)mmap(nullptr, details.st_size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, f, 0);
        Readers.pushBack(fb::makeUnique<IndexReader>(IndexPtr, i));
    }

    /* while(true){*/
        //TODO: GET QUERIES FROM MASTER
        fb::String query;

        QueryParser QuePasa(query);
        auto e = QuePasa.Parse();
        //now we spawn a thread for each index, and give it e
        for (auto& reader : Readers){
            pthread_t pt;
            IndexInfoArg info = { e, reader };
            pthread_create(&pt, NULL, RankPages, (void *)&info);
        }

        //TODO: Send TopNQueue to master
    /* } */
}
