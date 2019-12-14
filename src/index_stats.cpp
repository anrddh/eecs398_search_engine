#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <queue>
#include <set>
#include <disk/constants.hpp>
#include <fb/stddef.hpp>
#include <atomic>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <disk/page_store.hpp>

#include "isr/index_reader.hpp"

using namespace std;

bool is_prefix( string prefix, char * filename )
    {
    return !strncmp(prefix.c_str( ), filename, prefix.size( ) );
    }

int main( int argc, char * argv[] ){
    if (argc != 3) {
        std::cerr << "Usage: [directory of pagestore files to merge] [index_prefix]" << std::endl;
        return 1;
    }

    fb::SizeT totalTokens = 0;
    fb::SizeT numDocs = 0;
    string dirname = string(argv[1]);
    string prefix(argv[2]);
    std::cout << "dirname: " << dirname << std::endl;
    DIR *dirp = opendir(dirname.c_str());

    struct dirent *dir;
    while( dir = readdir(dirp), dir != NULL ){
        
        if( is_prefix(prefix, dir->d_name) )
        {
            string filename = dirname + "/" + string(dir->d_name);
            int f = open(filename.c_str(), O_RDWR);
            if (f < 0) {
                std::cout << "error opening " << filename << ", skipping" << std::endl;
                continue;
            }
            struct stat info;
            fstat(f, &info);
            char * start = (char *) mmap(nullptr, info.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, f, 0);
            if(!start)
                {
                std::cout << "error mmaping " << filename << ", skipping" << std::endl;
                close(f);
                continue;
                }
            
            std::cout << "reading " << filename << " ... " << std::endl;
            IndexReader ir(start, 0);
            totalTokens += ir.GetNumTokens( );
            fb::UniquePtr<DocumentISR> doc = ir.OpenDocumentISR( );
            numDocs += doc->GetDocumentCount( );

            close(f);
            munmap(start, info.st_size);
        }
    }


    std::cout << "Done" << std::endl;
    std::cout << "Number of Documents: " << numDocs << std::endl;
    std::cout << "Number of Tokens: " << totalTokens << std::endl;
    return 0;
}