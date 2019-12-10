#include <parse/parser.hpp>
#include <parse/query_parser.hpp>
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
#include <cstdio>

#include <getopt.h>

#include <fb/vector.hpp>
#include <fb/string.hpp>

#include "query_result.hpp" //move this to whatever appropriate location

// TCP protocol
// worker establishes socket to master (sends a verfication code)
// master to worker: query (string)
// worker to master: num (int)
//    [ urlOffset (SizeT), rank (double), snippet (string)] x num
//    WARNING TO ANI - sending int and sizet are different tcp function calls
using namespace fb;

Vector<Thread> threads;
Vector<fb::UniquePtr<IndexReader>> Readers;

// to be used as arguments to a thread
struct IndexInfo {
    fb::UniquePtr<Expression> e;
    fb::UniquePtr<IndexReader> reader;
}

// gets an index info
void* RankPages( void *info ) {
    // Just keep calling add to top pages
    ConstraintSolver info.e->eval(info.reader);
    //TODO
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

Vector< PageStoreInfo > indexes;

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
        Readers.PushBack(fb::makeUnique<IndexReader>(IndexPtr, i));
    }

    //TODO: GET QUERIES
    fb::String query;

    fb::UniquePtr<Expression> e = ParseQuery(query);
    //now we spawn a thread for each index, and give it e
    for (auto& reader : Readers){
        pthread_t pt;
        IndexInfo info = { e, reader };
        pthread_create(&pt, NULL, RankPages, (void *)&reader);
    }

}

Vector<String> GenerateSnippets( Vector<SnippetStats> Stats ){

    Vector<String> snippets;
    for(auto& stat : Stats ){
        FILE *fptr = fopen(stat.filename.data(), "rb");
        if (fptr == NULL){
            std::cout << "error opening " << filename << " when generating snippets " << std::endl;
            snippets.PushBack(""); //if the file fails to open, just give empty string for snippet rather than crash
            continue;
        }
        fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_SET); //skip the cursor
        fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_CUR); //skip the counter
        fseek(fptr, sizeof(PageHeader) * stat.DocIndex, SEEK_CUR); //skip ahead in the vector of PageHeaders
        fb::SizeT PageOffset;
        fread(&PageOffset, fb::SizeT, 1, fptr); //read in the page offset

        String snippet;
        char dummy[80]; //TODO: THIS IS SCARY!! do we have a max word size??
        fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
        for (int i = 0; i < stat.Offsets.begin; ++i){
            fscanf(fptr, "%s", dummy); //scan past all the words before begin offset
        }

        for (int j = stat.Offsets.begin; j < stat.Offsets.end; ++j){
            fscanf(fptr, "%s", dummy); //add all the words between begin offset and end offset
            snippet += dummy;
            snippet += " "; //dont forget to put a space between the words!
        }

        fclose(fptr); //don't forget this!
        snippets.PushBack(snippet); //add the generated snippet to vector
    }
    return snippets;
}

fb::UniquePtr<Expression> ParseQuery( fb::String query ){

    // First: we send the query to the query compiler
    Parser p(query);
    //TODO
}
