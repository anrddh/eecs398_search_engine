#pragma once

#include "fb/string.hpp"
#include "fb/vector.hpp"
#include "fb/unordered_map.hpp"

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
  // this number should be N where N is how many documents
  // we've seen thus far
  uint64_t docId;
};
