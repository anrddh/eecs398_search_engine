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
        std::cerr << "Usage: directory of pagestore files to merge" << std::endl;
        return 1;
    }

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
        //std::cout << "filename: " << filename << " count: " << count << std::endl;
        fclose(fptr);
        total += count;
    }

    fb::String fbDirname(argv[1]);
    fb::String MergedFileName = fbDirname + "/"_sv + PageStoreMergedFile;

    std::cout << "Created merged file: " << MergedFileName << std::endl;

    PageBin MergedBin(MergedFileName, total);

    dirp = opendir(dirname.c_str());
    while( dir = readdir(dirp), dir != NULL ){

        string filename = dirname + "/" + string(dir->d_name);
        if ( filename.find(PageStoreFile.data()) == string::npos || filename.find(PageStoreCounterFile.data()) != string::npos ) continue;

        PageBin CurBin(fb::StringView(filename.data(), filename.size()));
        std::cout << "CurBin: " << fb::StringView(filename.data(), filename.size()) << std::endl;
        fb::SizeT NumCur = CurBin.num_stored_pages();
        std::cout << "NumCur: " << NumCur << std::endl;
        char *CurHeadersBegin = CurBin.data() + sizeof(NumCur);

        // Note: the minus 1 is because I am lazy, we will lose one page per file
        // That's the cost of me writing this quickly
        for (fb::SizeT i = 0; i < NumCur - 1; ++i){
            char *CurHeader = CurHeadersBegin + (i * sizeof(PageHeader));
            char *NextHeader = CurHeadersBegin + ((i+1) * sizeof(PageHeader));
            PageHeader header = *(PageHeader *)CurHeader;
            PageHeader nHeader = *(PageHeader *)NextHeader;
            std::cout << "header: " << std::hex << header.UrlOffset << " " << header.beginOffset << " " << header.VecOffset << std::endl;
            std::cout << "nHeader: " << std::hex << nHeader.UrlOffset << " " << nHeader.beginOffset << " " << nHeader.VecOffset << std::endl;
            Page p;
            p.UrlOffset = header.UrlOffset;
            p.page_str = fb::String(CurBin.data() + header.beginOffset - sizeof(std::atomic<fb::SizeT>));
            for (fb::SizeT j = header.VecOffset; j < nHeader.beginOffset; ++j){
                p.word_headers.pushBack(*(WordDescriptors *)(CurBin.data() + j - sizeof(std::atomic<fb::SizeT>)));
            }
            MergedBin.addPage(std::move(p));
        }
    }

    std::cout << "Total number of stored pages: " << total << std::endl;
    return 0;
}
