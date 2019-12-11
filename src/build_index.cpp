#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#include <fb/string.hpp>

#include <index/index_builder.hpp>

using fb::String;
using fb::toString;

using std::cerr;
using std::cout;
using std::endl;

int main( int argc, char ** argv ) {
    if (argc != 4) {
        cout << "USAGE: " << argv[0]
             << " [PATH TO INDEX FOLDER] [PREFIX FILE NAME] [NUM_PAGE_STORE_FILES]\n";
        exit(1);
    }

    logfile.open("index_building_log.txt");
    if (!logfile.is_open())
        cerr << "Could not open logfile" << endl;

    String path(argv[1]);
    if (path.back() != '/')
        path.pushBack('/');

    String PageStorePrefix(argv[2]);
    int num_page_store_files = atoi(argv[3]);

    for (int i = 0; i < num_page_store_files; ++i) {
        log(logfile, "Currently parsing ", i, ".\n");

        IndexBuilder builder(path);
        String PageStoreFileName = path + PageStorePrefix + toString(i);
        cout << PageStoreFileName << endl;

        int PageStoreFile = open(PageStoreFileName.data(), O_RDWR);

        if(PageStoreFile < 0)
            cout << "ERROR: PageStore" << i << " not found" << endl;
        else {
            struct stat details;
            fstat(PageStoreFile, &details);
            uint64_t * start = (uint64_t *) mmap(nullptr, details.st_size,
                                                 PROT_READ,
                                                 MAP_PRIVATE,
                                                 PageStoreFile, 0);
            builder.build_chunk(start, i);
        }

        log(logfile,
            "Finished parsing ", i, ". ", num_page_store_files - i,
            " left to go.\n");
    }
}
