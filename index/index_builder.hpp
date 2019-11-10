#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "fb/string"
#include "fb/memory.hpp"
#include "fb/indexEntry.hpp"

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */

struct docIdInfo {
  //position of last word in document + 1
  unsigned int pos;
  unsigned int docId;
}

//flags
constexpr uint8_t INDEX_WORD_ITALIC = 0b0100;
constexpr uint8_t INDEX_WORD_BOLD = 0b1000;
constexpr uint8_t INDEX_WORD_HEADER = 0b0010;
constexpr uint8_t INDEX_WORD_ANCHOR = 0b0001;
//single post
struct IndexWord {
  fb::string word;
  uint8_t type_flags; // should be or'ed INDEX_WORD_ above.
}

struct AbsoluteWordInfo {
    unsigned long position;
    uint8_t type_flags;
}

class IndexBuilder {
public:
    IndexBuilder(fb::string path) : root(path) { 
        if((int f = open((path + "master").c_str(), O_RDWR) > 0)) {
            masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(MasterIndexData), PROT_READ | PROT_WRITE, MAP_PRIVATE, f, 0);
        } else {

        }
        close(f);
        
      	for(int i = 0; i < NUM_WORD_MAPS; ++i) 
        	{
          	wordPositionsMaps.emplace_back(fb::makeUnique());
        	}
    }
    //takes in vector of IndexWords, maps words to indices in vector/flags
    bool addDocument(unsigned int docId, fb::vector<IndexWord>> &words) {
      	currentMapNumLock.lock();
 		int mapToUse = currentMapNum
        for(auto word : words)
        {
            OffsetInfo info = {tokenCount, word.type_flags}; 
            wordPositions[word.word].pushBack(info);
            ++tokenCount;
        }
        //what is emplace_back?
        documents.emplaceBack(tokenCount, docId);
        ++tokenCount;
      
        if(tokenCount > THRESHOLD) {
            writeToDisk();
            tokenCount = 0;
            wordPositions.clear();
        }
    }

private:
    bool writeToDisk();
    void buildPostingList(char* start, const fb::string &word);
    
    //folder for each index chunk, store root directory
    fb::string root;
    MasterIndexData * masterIndexData;
    fb::vector<fb::UniquePtr<fb::unordered_map<fb::string, fb::vector<offsetInfo>>>> wordPositionsMaps;
  	fb::vector<fb::UniquePtr<fb::Mutex *> > mapLocks;
  	fb::Mutex currentMapNumLock;
  	int currentMapNum;
  	
    fb::vector<docIdInfo> documents;
    //each chunk keeps track of its own word count
    int tokenCount;
  	
};

struct MasterIndexData {
    int numIndexes;
}

// take in pointer to space that is allocated
// take in the word and vector of abs info
// write data to file
// return how much space actually used
int IndexBuilder::buildPostingList(char* start, const fb::string &word, const fb::vector<AbsoluteWordInfo> &info){
    // start is a pointer to the "beginning of file"
    // write string into disk
    strcpy(start, word.c_str());
    // iterate through the vector of abs info, subtract adjacent values and write
    // to disk
    // write to the next byte after the string
    // offset is at most 4 bytes
    char* current = word.size() + 1;
    current = add_num(current, info[0].position, info[0].type_flags);
    for(int i = 1; i < info.size(); ++i){
        current = add_num(current, info[i].position - info[i-1].position, info[i].type_flags);
    }
}

// create dictionary: maps words to offset in the file where posting list is
bool IndexBuilder::writeToDisk() {
    int f = open((path + "index_" + to_string(masterIndexData->numIndexes)).c_str(), O_RDWR | O_CREAT);
    ++masterIndexData->numIndexes;
    unsigned int dictionaryDataLength = wordPositions.capacity();
    unsigned int dictionarySize = (dictionaryDataLength + 1) * sizeof(unsigned int);
    unsigned int * dictionary = (unsigned int *) mmap(nullptr, dictionarySize, PROT_READ | PROT_WRITE, MAP_PRIVATE, f, 0);
    *dictionary = wordPositions.capacity();
    dictionaryData = dictionary + 1;
    
    unsigned int currentPos = dictionarySize;
  
    for(const fb::pair<fb::string, fb::vector<offsetInfo>> & entry : wordPositions) {
        // write currentPos to bucket this word hashes to
        int bucket = hashfunc(entry.first) % dictionaryDataLength;
        while(dictionaryData[bucket] != 0) {
          bucket = (bucket + 1) % dictionaryDataLength;
        }
        dictionaryData[bucket] = currentPos;
      
      
        unsigned long postingListLength = (entry.first.size() + 1) + (4 * entry.second.size());
        char * postingList = (char *) mmap(nullptr, postingListLength, PROT_READ | PROT_WRITE, MAP_PRIVATE, f, currentPos);
        int actualSize = buildPostingList(postingList, entry);
        currentPos += actualSize;
        munmap(postingList, postingListLength);
    }
    
  munmap(dictionary, dictionarySize); 
    
}
