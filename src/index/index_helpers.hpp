#pragma once

int getSizeOfSkipTable(int NUM_SKIP_TABLE_BITS) {
    return ((1 << NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int));
}

int getSizeOfRankingData() {
    return (2 * sizeof(unsigned int));
}

int getSizeOfPostingList(const fb::String &word, int NUM_SKIP_TABLE_BITS) {
    return (word.size() + 1) + getSizeOfRankingData() + getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);
}




