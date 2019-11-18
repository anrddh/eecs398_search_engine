#pragma once

#include "string.hpp"
#include "vector.hpp"
#include "unordered_map.hpp"

struct MasterIndexData {
    int numIndexes;
};

struct IndexWord {
  fb::String word;
  uint8_t type_flags; // should be or'ed INDEX_WORD_ above.
};

struct AbsoluteWordInfo {
    unsigned long position;
    uint8_t type_flags;
};

struct DocIdInfo {
  // position of last word in document + 1
  unsigned int position;
  uint64_t docId;
};

struct WriteToDiskInput {
    fb::String filename;
    fb::UnorderedMap<fb::String, fb::Vector<AbsoluteWordInfo>> map;
    fb::UnorderedMap<fb::String, unsigned int> wordDocCounts;
};