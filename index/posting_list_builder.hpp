#pragma once

#include "fb/indexEntry.hpp"

#include "index_helpers.hpp"
#include "index_data_structures.hpp"

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
class PostingListBuilder {
public:

    PostingListBuilder(const fb::String &word, char * location, int numDocs, int numOccurences) : beginning(location), lastLocation(0) 
        {
        strcpy(beginning, word.data());
        
        unsigned int * rankingData = (unsigned int *) (beginning + (word.size() + 1));

        rankingData[0] = numDocs;
        rankingData[1] = numOccurences;

        skipTableStart = (unsigned int *) (beginning + (word.size() + 1) + getSizeOfRankingData()); // past word and past the num of documents and num of occurences of word
        memset(skipTableStart, 0, getSizeOfSkipTable); // 0 out skip table

        currentPostPosition = beginning + (word.size() + 1) + getSizeOfRankingData() + getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);

        
        }

    void addPost(AbsoluteWordInfo offset);

    void endList();

    char * getLength();

private:

    void buildSkipTable();

    char * beginning;
    char * currentPostPosition;
    unsigned int * skipTableStart;
    unsigned int lastLocation;
    unsigned int nextSkipTableEntry = 0;
};

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
void PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::addPost(AbsoluteWordInfo word) {
    if((word.position >> (MAX_BITS_PER_CHUNK - NUM_SKIP_TABLE_BITS)) > nextSkipTableEntry) 
        {
        skipTableStart[2 * nextSkipTableEntry] = currentPostPosition - beginning;
        skipTableStart[2 * nextSkipTableEntry + 1] = word.position;
        ++nextSkipTableEntry;
        }

    currentPostPosition = add_word_post(currentPostPosition, word.position - lastLocation, word.type_flags);
    lastLocation = word.position;
}

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
void PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::endList() {
    currentPostPosition = add_word_post(currentPostPosition, 0, 0);
}

template<int NUM_SKIP_TABLE_BITS, int MAX_BITS_PER_CHUNK>
char * PostingListBuilder<NUM_SKIP_TABLE_BITS, MAX_BITS_PER_CHUNK>::getLength() {
    return currentPostPosition - beginning;
}