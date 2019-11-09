//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "SavedLists.hpp"
#include "string_pool.hpp"
#include "unordered_set.hpp"
#include "unordered_map.hpp"
#include "string.hpp"
#include "vector.hpp"

#define NumBins 256 //TODO: change if necessary

namespace fb {

using URL = String;

template<SizeT N>
class StringPool;

//struct which contains info about a specific url
struct UrlInfo {
    SizeT AdjListBegin;
    SizeT AdjListLength;
    //offset to first chunk of adjacenecy list
    SizeT AnchorTextBegin;
    //offset to last chunk of adjacency list
    SizeT AnchorTextEnd;
    SizeT StaticRank;
    SizeT URLOffset;
    //This is the pointer to the downloaded page for this url
    (uint *) Page = nullptr; //TODO: this
};

//class which handles URL lookup, defined in url_lookup.hpp
template<typename K, typename V, typename Hasher> class UrlLookup;

class UrlPoolChunk {
private:

    //UnorderedSet<SizeT urlOffset, SizeT urlRank> FrontierChunk; //The frontier chunk, whatever form it takes
    SavedStrings *UrlList; //The saved url list
    SavedAnchors *AnchorTextList; //The saved anchor text list
    SavedLists<SizeT> *AdList; //The saved adjlist

    StringPool<NumBins> *UrlLookup; //The url lookup handler
    InfoPool<NumBins> *InfoLookup; //The urlinfo lookup handler

    //If the URL is already in the URL List, it adds the anchor text
    void ProcessUrl(SizeT urlOffset);

public:

    //Constructor
    UrlPoolChunk(SavedStrings *urls, SavedAnchors *anchors, SavedLists<SizeT> *adjlist,
        StringPool<NumBins> *urllookup, InfoPool<NumBins> *infolookup) : UrlList(urls), AnchorTextList(anchors),
        AdList(adjlist), UrlLookup(urllookup), InfoLookup(infolookup) {}

    //Adds URL offset and rank pair to the frontier. To be called by other chunks
    void AddToFronter(SizeT urlOffset, SizeT urlRank);

    //Constantly pops off the frontier and process urls. A while loop
    void Run();

};

//Wrapper global function for running URLPoolChunks
void URLPoolChunkRun(UrlPoolChunk *PoolChunk);

}; //namespace fb
