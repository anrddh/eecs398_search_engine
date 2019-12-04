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

using namespace std;

int main( int argc, char * argv[] ){

    fb::SizeT total = 0;
    string dirname = string(argv[1]);
    std::cout << "dirname: " << dirname << std::endl;
    DIR *dirp = opendir(dirname.c_str());

    struct dirent *dir;
    while( dir = readdir(dirp), dir != NULL ){
        string filename = dirname + "/" + string(dir->d_name);
        if ( filename.find(PageStoreFile.data()) == string::npos || filename.find(PageStoreCounterFile.data()) != string::npos ) continue;
        FILE *fptr = fopen(filename.c_str(), "rb");
        if (fptr == NULL){
            std::cout << "error opening " << filename << std::endl;
            return 1;
        }
        fseek(fptr, sizeof(atomic<fb::SizeT>), SEEK_SET);
        fb::SizeT count;
        fread(&count, sizeof(atomic<fb::SizeT>), 1, fptr);
        std::cout << "filename: " << filename << " count: " << count << std::endl;
        fclose(fptr);
        total += count;
    }

    std::cout << "Total number of stored pages: " << total << std::endl;
    return 0;
}
