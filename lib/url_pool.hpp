//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "saved_adjlist.hpp"
#include "unordered_set.hpp"
//#include "string.hpp"
//#include "vector.hpp"
#include <string>
#include <vector>
#define Vector std::vector
#define URL std::string
#define String std::string

namespace fb {

//struct which contains info about a specific url
struct UrlInfo {
    SizeT AdjListBegin;
    SizeT AdjListLength;
    SizeT AnchorTextBegin;
    SizeT AnchorTextEnd;
    SizeT StaticRank;
    SizeT URLOffset;
    //This is the pointer to the downloaded page for this url
    (void *) Page;
};

//class which handles URL lookup, defined in url_lookup.hpp
template<typename K, typename V, typename Hasher> class UrlLookup;

class UrlPoolChunk {
private:
    //take this out later
    typedef SavedAnchorText (void *);

    //UnorderedSet<URL> FrontierChunk;
    SavedUrls UrlListChunk;
    SavedAdjList AdListChunk;
    SavedAnchorText AnchorTextChunk;

    UrlLookup UrlLookupChunk;
    SizeT ChunkId;

    Vector<UrlInfo> UrlInfos;

    //If the URL is already in the URL List, it adds the anchor text
    void ProcessUrl(URL url, String anchor_text);

public:

    //Default Constructor
    UrlPoolChunk(){
        UrlLookupChunk = UrlLookup(this);
    }

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
void URLPoolChunkRun(&UrlPoolChunk PoolChunk);

}; //namespace fb
