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

// gets an index info
void* RankPages( void * ) {
   // Just keep calling add to top pages

}

void* listen_to_master(void *) {
}

Vector<String> GenerateSnippets( Vector<SnippetStats> Stats );

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
