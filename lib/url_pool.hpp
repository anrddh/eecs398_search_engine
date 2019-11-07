//Created on 11/6/19
#include "stddef.hpp"
//#include "string"
#include <string>
#define URL std::string

namespace fb {

struct UrlInfo {
    SizeT AdjListBegin;
    SizeT AdjListEnd;
    SizeT AnchorTextBegin;
    SizeT AnchorTextEnd;
    SizeT StaticRank;
};

// Compares a URL to the URL at offset
bool OffsetCompare(URL key, SizeT offset);

//Adds a URL to the URL List, returns its offset
SizeT OffsetCreate(URL key);

}; //namespace fb
