//Created on 11/6/19
#pragma once

#include "../../lib/frontier_pool.hpp"
#include "../../lib/stddef.hpp"
#include "UrlStore.hpp"
#include "AnchorStore.hpp"
//#include "string_pool.hpp"
#include "../../lib/unordered_set.hpp"
#include "../../lib/unordered_map.hpp"
#include "../../lib/string.hpp"
#include "../../lib/vector.hpp"
#include "string_pool.hpp"

#define NumBins 256 //TODO: change if necessary
using URL = fb::String;

//struct which contains info about a specific url
struct UrlInfo {
    fb::SizeT AdjListBegin = 0; //offset to first entry in adjacency list
    fb::SizeT AdjListLength = 0; //offset to (one past?) last entry in adjacency list
    fb::SizeT AnchorTextBegin = 0; //offset to first chunk of adjacenecy list
    fb::SizeT AnchorTextEnd = 0; //offset to last chunk of adjacency list
    fb::SizeT StaticRank = 0; //the static rank associated to the page at this url
    fb::SizeT UrlOffset = 0; //offset of the url in the url list
    uint* Page = nullptr; //TODO: set type correctly. This is the pointer to the downloaded page for this url
};

class UrlPool {
private:
    //UnorderedSet<fb::SizeT urlOffset, fb::SizeT urlRank> FrontierChunk; //The frontier chunk, whatever form it takes
    UrlStore *UrlList; //The saved url list
    AnchorStore *AnchorTextList; //The saved anchor text list
    DiskVec<fb::SizeT> *AdList; //The saved adjlist

    StringPool<NumBins> *UrlLookup; //The url lookup handler
    InfoPool<NumBins> *InfoLookup; //The urlinfo lookup handler

public:
    //Constructor
    UrlPool(UrlStore *urls, AnchorStore *anchors, DiskVec<fb::SizeT> *adjlist,
        StringPool<NumBins> *urllookup, InfoPool<NumBins> *infolookup) : UrlList(urls), AnchorTextList(anchors),
        AdList(adjlist), UrlLookup(urllookup), InfoLookup(infolookup) {}
        
};
