#pragma once

class PostingListBuilder {
public:

    PostingListBuilder(const fb::String &word, char * location) : beginning(location), lastLocation(0) {
        strcpy(beginning, word.c_str());
        current = begining + word.size() + 1;
        // chunk out area for skip table

    }

    void addPost(OffsetInfo offset);

    void endList();

    char * getPointerPastEnd();

private:

    void buildSkipTable();

    char * begining;
    char * current;
    unsigned int lastLocation;

};

void PostingListBuilder::addPost(AbsoluteWordInfo word) {
    // TODO: check that lastLocation < word.position
    if((word.position >> (27 - 12)) > nextSkipTableEntry) {
        // add numbers to skip table

    }
    current = add_num(current, word.position - lastLocation, word.type_flags);
    lastLocation = word.position;
}

void PostingListBuilder::endList() {
    // TODO: check that lastLocation < offset
    current = add_num(current, 0, 0);

    fillInSkipTable();
}

char * PostingListsBuilder::getLength() {
    return current - beginning;
}