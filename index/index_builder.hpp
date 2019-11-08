#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "fb/string"
#include "fb/memory.hpp"

class IndexBuilder {
public:
    IndexBuilder(fb::string path) : root(path) { 
        if((int f = open((path + "master").c_str(), O_RDWR) > 0) {
            masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(MasterIndexData), PROT_READ | PROT_WRITE, MAP_PRIVATE, f, 0);
        } else {

        }
        masterIndexData = 
    }

    bool addDocument(fb::unique_ptr<fb::unordered_map<fb::string, fb::vector<int>>> * words) {
        
    }



private:
    fb::string root;
    MasterIndexData * masterIndexData;
};

struct MasterIndexData {
    int wordCount;
}