#pragma once

#include "fb/indexEntry.hpp"

template<int NUM_SKIP_TABLE_BITS>
class PostingListBuilder {
public:

    PostingListBuilder(const fb::String &word, char * location) : beginning(location), lastLocation(0) {
        strcpy(beginning, word.c_str());
        current = begining + (word.size() + 1) + ((2 <<  NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int));
        skipTableStart = (unsigned int *) (begining + word.size() + 1 + 2 * sizeof(unsigned int)); // past word and past the num of documents and num of occurences of word
        memset(skipTableStart, 0, ((2 <<  NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int))); // 0 out skip table
    }

    void addPost(OffsetInfo offset);

    void endList();

    char * getPointerPastEnd();

private:

    void buildSkipTable();

    char * begining;
    char * currentPostPosition;
    unsigned int * skipTableStart;
    unsigned int lastLocation;
    unsigned int nextSkipTableEntry = 0;
};

void PostingListBuilder::addPost(AbsoluteWordInfo word) {
    if((word.position >> (MAX_BITS_PER_CHUNK - NUM_SKIP_TABLE_BITS)) > nextSkipTableEntry) 
        {
        skipTableStart[2 * nextSkipTableEntry] = currentPostPosition - beginning;
        skipTableStart[2 * nextSkipTableEntry + 1] = word.position;
        ++nextSkipTableEntry;
        }

    currentPostPosition = add_num(currentPostPosition, word.position - lastLocation, word.type_flags);
    lastLocation = word.position;
}

void PostingListBuilder::endList() {
    currentPostPosition = add_num(current, 0, 0);
    fillInSkipTable();
}

char * PostingListsBuilder::getLength() {
    return currentPostPosition - beginning;
}