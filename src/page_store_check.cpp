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

using namespace std;

int main( int argc, char * argv[] ){

    if (argc < 2) {
        std::cerr << "Usage: directory of pagestore files" << std::endl;
        return 1;
    }

    fb::SizeT old_total = 0;
    fb::SizeT new_total = 0;
    string dirname = string(argv[1]);
    std::cout << "dirname: " << dirname << std::endl;
    DIR *dirp = opendir(dirname.c_str());

    struct dirent *dir;
    while( dir = readdir(dirp), dir != NULL ){
        string filename = dirname + "/" + string(dir->d_name);
        if ( filename.find(PageStoreFile.data()) == string::npos || filename.find(PageStoreCounterFile.data()) != string::npos ) continue;
        FILE *fptr = fopen(filename.c_str(), "r+b");
        if (fptr == NULL){
            std::cout << "error opening " << filename << std::endl;
            return 1;
        }
        fseek(fptr, 0, SEEK_END); //jump to end
        fb::SizeT file_size = ftell(fptr); //get file size
        fseek(fptr, sizeof(atomic<fb::SizeT>), SEEK_SET); //jump to count
        fb::SizeT count;
        fread(&count, sizeof(atomic<fb::SizeT>), 1, fptr); //get the count
        std::cout << filename << " claims to have count: " << count << std::endl;
        old_total += count;

        if ((2 * sizeof(atomic<fb::SizeT>) + (count - 1) * sizeof(PageHeader)) > file_size){
            std::cout << "ERROR: " << filename << " was truncated into the metadata, and thus cannot be recovered. Please delete " << filename << std::endl;
            fclose(fptr);
            continue;
        }

        do{
            fseek(fptr, 2 * sizeof(atomic<fb::SizeT>), SEEK_SET);
            fseek(fptr, (count - 1) * sizeof(PageHeader), SEEK_CUR);
            fseek(fptr, sizeof(fb::SizeT), SEEK_CUR); //jump over the doc offset
            fb::SizeT vOffset;
            fread(&vOffset, sizeof(fb::SizeT), 1, fptr); //get the VecOffset
            //std::cout << "debug: vOffset: " << vOffset << " file_size: " << file_size << " count: " << count << std::endl;
            if (vOffset > file_size){
                --count;
            }
            else break;
        } while(true);
        std::cout << filename << " actually has count: " << count << std::endl;
        fseek(fptr, sizeof(atomic<fb::SizeT>), SEEK_SET);
        int wrote = fwrite(&count, sizeof(fb::SizeT), 1, fptr);
        if (wrote < 1){
            std::cout << "Error writing new count to " << filename << std::endl;
        }
        new_total += count;

        fclose(fptr);
    }

    std::cout << "Original number of stored pages: " << old_total << std::endl;
    std::cout << "New number of stored pages: " << new_total << std::endl;
    return 0;
}
