//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
//#include "string,hpp"
//#include "vector.hpp"
#include <string>
#include <vector>
#define Vector std::vector
#define String std::string

namespace fb {

//If the URL is already in the URL List, it adds the anchor text
void UrlPoolChunk::ProcessUrl(URL url, String anchor_text){
    //find the corresponding url info struct
    UrlInfo info = UrlInfos[url];
    //check if its the first time seeing the url
    if (info.Page == nullptr){
        SizeT url_offset = (*UrlLookup)[url];
        info.URLOffset = url_offset;
        //TODO: send the url to the parser, receive vector of found url, anchor text pairs, and a pointer to the stored page
        //Set the page pointer of info to the stored page
        //Send the url, anchor text pairs to the appropraite chunks
        //Look up all the urls, and send the vector of offsets to the adjacency list to be added
        //The adj list returns a pair of begin and end offsets, set those for info
    }
    //Send the current anchor text end, begin, and the new text to the anchor text list
    Pair<SizeT, SizeT> newAnchorOffsets = SavedAnchors->add_str(anchor_text, {info.AnchorTextBegin, info.AnchorTextEnd});
    //Set the new begin and end for info
    info.AnchorTextBegin = newAnchorOffsets.begin;
    info.AnchorTextEnd = newAnchorOffsets.end;
    return;
}

void UrlPoolChunk::AddToFrontier(URL url, String anchor_text){
    //TODO
}

void UrlPoolChunk::Run(){
    //TODO
}

//Wrapper global function for running URLPoolChunks
void UrlPoolChunkRun(&UrlPoolChunk PoolChunk){
    PoolChunk.Run();
}

}; //namespace fb
