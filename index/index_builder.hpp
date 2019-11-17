#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "fb/string"
#include "fb/memory.hpp"

#include "index_chunk_builder.hpp"

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */

//
// constexptr int MAX_BITS_PER_CHUNK = 27;
// constexptr int TOKEN_THRESHOLD = 2 << MAX_BITS_PER_CHUNK; //134217728; // 2^27



struct WriteToDiskInput {
    fb::String filename;
    fb::UniquePtr<fb::unordered_map<fb::string, fb::vector<offsetInfo>>> map;
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
};

struct DocIdInfo {
  // position of last word in document + 1
  unsigned int pos;
  unsigned int docId;
}

struct AbsoluteWordInfo {
    unsigned long position;
    uint8_t type_flags;
};

template<int MAX_BITS_PER_CHUNK, int NUM_SKIP_TABLE_BITS>
class IndexBuilder {
public:
    IndexBuilder(fb::string path) : root(path) { 
        if((int f = open((path + "master").c_str(), O_RDWR) > 0)) {
            masterIndexData = (MasterIndexData *) mmap(nullptr, sizeof(MasterIndexData), PROT_READ | PROT_WRITE, MAP_PRIVATE, f, 0);
        } else {

        }
        close(f);
        
        wordPositions = fb::makeUnique();
        tokenCount = 1;
    }
    //takes in vector of IndexWords, maps words to indices in vector/flags
    bool addDocument(unsigned int docId, fb::vector<IndexWord>> &words) {
        wordPositionsLock.lock();
        for(auto word : words)
        {
            OffsetInfo info = {tokenCount, word.type_flags}; 
            (*wordPositions)[word.word].pushBack(info);
            ++tokenCount;
        }

        // what is emplace_back? emplace back forwards the arguments you pass it to the constructor of type T. It constructs an object of type T in place which is why it is useufl
        documents.emplaceBack(tokenCount, docId);
        ++tokenCount;

        constexpr MAX_TOKEN_COUNT = 2 << MAX_BITS_PER_CHUNK;
        if(tokenCount > MAX_TOKEN_COUNT) {
            flushToDisk();
        }
        wordPositionsLock.unlock();
    }

    // create dictionary: maps words to offset in the file where posting list is
    static void writeToDisk(void * arg) {
    WriteToDiskInput input = *(WriteToDiskInput *)arg;

    IndexChunkBuilder<fb::Hash, NUM_SKIP_TABLE_BITS> indexChunkBuilder(input.filename, *(input.map).bucket_count());
    for(const fb::pair<fb::String, fb::vector<AbsoluteWordInfo>> &entry : *(input.map))
        {
        indexChunkBuilder.addWord(pair.first, pair.second);
        }
    


    delete arg;
}

private:
    void flushToDisk() 
        {
        WriteToDiskInput * input = new WriteToDiskInput;
        input->filename = (path + "index_" + to_string(masterIndexData->numIndexes)).c_str();
        input->map = std::move(wordPositions);

        fb::Thread worker(writeToDisk, (void *) input);
        worker.detach();
        
        tokenCount = 1;
        wordPositions = fb::makeUnique();

        ++masterIndexData->numIndexes;
        }
    
    // folder for each index chunk, store root directory
    fb::string root;
    
    // some meta data about the index chunks in this index
    MasterIndexData * masterIndexData;
    fb::Mutex masterIndexDataLock;

    // the actual map that stores the positions of the words, is a unique pointer so that we can pass this ownership to a thread.
    fb::UniquePtr<fb::unordered_map<fb::string, fb::vector<AbsoluteWordInfo>>> wordPositions;
  	fb::Mutex wordPositionsLock;
  	
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
int buildPostingList(char* start, const fb::string &word, const fb::vector<AbsoluteWordInfo> &info){
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

/*
// create dictionary: maps words to offset in the file where posting list is
void writeToDisk(void * arg) {
    WriteToDiskInput input = *(WriteToDiskInput *)arg;

    IndexChunkBuilder<fb::Hash, MAX_BITS_PER_CHUNK, TOKEN_THRESHOLD, NUM_SKIP_TABLE_BITS> indexChunkBuilder(input.filename, *(input.map).bucket_count());
    for(const fb::pair<fb::String, fb::vector<AbsoluteWordInfo>> &entry : *(input.map))
        {
        indexChunkBuilder.addWord(pair.first, pair.second);
        }

    delete arg;
}
*/