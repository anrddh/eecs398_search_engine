#pragma once

#include "posting_list_builder.hpp"

constexptr SKIP_TABLE_SIZE = 

template<typename Hash, int NUM_SKIP_TABLE_BITS>
class IndexChunkBuilder {
public:
    IndexChunkBuilder(std::string filename, uint32_t capacity) : tableSize(capacity), file(open(input.filename, O_RDWR | O_CREAT)) 
        { 
        if(file == -1) 
            {
            // TODO: throw error or something
            }

        unsigned int dictionarySize = (tableSize + 1) * sizeof(unsigned int);
        start = (unsigned int *) mmap(nullptr, dictionarySize, PROT_READ | PROT_WRITE, MAP_PRIVATE, f, 0);
        *start = tableSize;
        dictionary = start + 1;

        nextAvailableLocation = (tableSize + 1) * sizeof(unsigned int);
        }

    ~IndexChunkBuilder() 
        {
        munmap(start, (tableSize + 1) * sizeof(unsigned int));
        }

    void addWord(const fb::String &word, const fb::vector<AbsoluteWordInfo> &offsets);

private:

    void writeToBucket(const fb::String &word, unsigned int offset);

    void writePostingList(const fb::String &word, const fb::vector<AbsoluteWordInfo> &offsets);

unsigned int * start;
unsigned int * dictionary;
uint32_t nextAvailableLocation;
uint32_t tableSize;
Hash hash;
int file;
};

void IndexChunkBuilder::addWord(const fb::String &word, const fb::vector<AbsoluteWordInfo> &offsets)
    {
    SizeT bucket = hash(word) % tableSize;
    while(dictionary[bucket] != 0) 
        {
        bucket = (bucket + 1) % tableSize;
        }

    dictionary[bucket] = nextAvailableLocation;
    
    nextAvailableLocation += writePostingList(word, offsets);
    }

void IndexChunkBuilder::writePostingList(const fb::String &word, const fb::vector<AbsoluteWordInfo> &offsets) 
    {
    unsigned long maxListLength = (word.size() + 1) + ((2 << NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int)) + (4 * offsets.size());
    char * postingListLocation = (char *) mmap(nullptr, maxListLength, PROT_READ | PROT_WRITE, MAP_PRIVATE, f, nextAvailabeLocation);

    PostingListBuilder builder(word, nextAvailableLocation);
    for( const &AbsoluteWordInfo word : offsets) 
        {
        builder.addPost(word);
        }

    builder.endList();
]   munmap(postingListLocation, maxListLength);
    return builder.getLength();
    }