//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
#include "string,hpp"
#include "vector.hpp"

namespace fb {

//If the URL is already in the URL List, it adds the anchor text
void UrlPoolChunk::ProcessUrl(SizeT urlOffset) {
    //lookup the url
    String url = UrlList->get_str(urlOffset);
    //find the corresponding url info struct
    UrlInfo info = UrlInfos[url];
    //check if its the first time seeing the url
    if (info.Page == nullptr){
        SizeT url_offset = (*UrlLookup)[url];
        info.URLOffset = url_offset;
        //TODO: send the url to the parser, receive vector of found url, anchor text pairs, and a pointer to the stored page
        //Set the page pointer of info to the stored page
        //Look up the url infos, and handle their anchor texts
        //Compute url heuristic rankings for new urls
        //Look up all the urls, and send the vector of offsets to the adjacency list to be added
        //The adj list returns a pair of begin and end offsets, set those for info
        //Send url offset, heuristric ranking pairs to the appropriate frontier chunks
    }
    return;
}

void UrlPoolChunk::AddToFrontier(SizeT urlOffset, SizeT urlRank){
    //TODO
    //frontier.push({urlOffset, urlRank});
}

void UrlPoolChunk::Run(){
    //TODO
    //while(!frontier.empty()){
        //ProcessUrl(frontier.pop());
    //}
}

//Wrapper global function for running URLPoolChunks
void UrlPoolChunkRun(&UrlPoolChunk PoolChunk){
    PoolChunk.Run();
}

}; //namespace fb
