//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "SavedLists.hpp"
#include "string_pool.hpp"
#include "unordered_set.hpp"
//#include "string.hpp"
#include "vector.hpp"
#include <string>
#define String std::string
#define NumBins 256 //TODO: change if necessary

namespace fb {

using URL = String;

template<SizeT N>
class StringPool;

//struct which contains info about a specific url
struct UrlInfo {
    SizeT AdjListBegin;
    SizeT AdjListLength;
    SizeT AnchorTextBegin;
    SizeT AnchorTextEnd;
    SizeT StaticRank;
    SizeT URLOffset;
    //This is the pointer to the downloaded page for this url
    //(void *) Page; TODO: this
};

//class which handles URL lookup, defined in url_lookup.hpp
template<typename K, typename V, typename Hasher> class UrlLookup;

class UrlPoolChunk {
private:

    //UnorderedSet<URL> FrontierChunk;
    SavedStrings *UrlList; //The global url list
    SavedStrings *AnchorTextList; //The global anchor text list
    SavedLists<SizeT> *AdList; //TODO: Probably needs to be Pair<SizeT,SizeT>. The global adjlist

    StringPool<NumBins> *UrlLookup; //The url lookup handler
    StringPool<NumBins> *AnchorTextLookup; //The anchor text lookup handler

    Vector<UrlInfo> UrlInfos;

    //If the URL is already in the URL List, it adds the anchor text
    void ProcessUrl(URL url, String anchor_text);

public:

    //Constructor
    UrlPoolChunk(SavedStrings *urls, SavedStrings *anchors, SavedLists<SizeT> *adjlist,
        StringPool<NumBins> *urllookup, StringPool<NumBins> *anchorlookup) : UrlList(urls), AnchorTextList(anchors),
        AdList(adjlist), UrlLookup(urllookup), AnchorTextLookup(anchorlookup) {}

    // Compares a URL to the URL at offset
    bool OffsetCompare(URL key, SizeT offset);

    //Adds a URL to the URL List, returns its offset
    SizeT OffsetCreate(URL key);

    //Adds URLs and anchor text to the frontier, to be called by other pools chunks
    void AddToFronter(URL url, String anchor_text);

    //Constantly pops off the frontier and process urls
    void Run();

};

//Wrapper global function for running URLPoolChunks
void URLPoolChunkRun(UrlPoolChunk *PoolChunk);

}; //namespace fb
