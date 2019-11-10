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
using URL = String;

template<SizeT N>
class StringPool;

//struct which contains info about a specific url
struct UrlInfo {
    SizeT AdjListBegin; //offset to first entry in adjacency list
    SizeT AdjListLength; //offset to (one past?) last entry in adjacency list
    SizeT AnchorTextBegin; //offset to first chunk of adjacenecy list
    SizeT AnchorTextEnd; //offset to last chunk of adjacency list
    SizeT StaticRank; //the static rank associated to the page at this url
    SizeT URLOffset; //offset of the url in the url list
    (uint *) Page = nullptr; //TODO: set type correctly. This is the pointer to the downloaded page for this url
};

class UrlPool {
private:
    //UnorderedSet<SizeT urlOffset, SizeT urlRank> FrontierChunk; //The frontier chunk, whatever form it takes
    SavedStrings *UrlList; //The saved url list
    SavedAnchors *AnchorTextList; //The saved anchor text list
    SavedLists<SizeT> *AdList; //The saved adjlist

    StringPool<NumBins> *UrlLookup; //The url lookup handler
    InfoPool<NumBins> *InfoLookup; //The urlinfo lookup handler

public:
    //Constructor
    UrlPool(SavedStrings *urls, SavedAnchors *anchors, SavedLists<SizeT> *adjlist,
        StringPool<NumBins> *urllookup, InfoPool<NumBins> *infolookup) : UrlList(urls), AnchorTextList(anchors),
        AdList(adjlist), UrlLookup(urllookup), InfoLookup(infolookup) {}

    //Returns a vector of urls for the thread to process
    //The caller is then expected to handle parsing the pages and return the info
    Vector<Pair<URL, SizeT>> RequestUrls();

    //Adds URL offset and rank pair to the frontier
    void AddToFrontier(SizeT urlOffset, SizeT urlRank);

};
