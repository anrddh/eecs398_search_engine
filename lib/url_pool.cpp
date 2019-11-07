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
#define URL std::string
#define String std::string

namespace fb {

// Compares a URL to the URL at offset
bool OffsetCompare(URL key, SizeT offset){
    //TODO
    //this wont work until get_str returns a usable type for this
    return key == UrlListChunk.get_str(offset);
}

//Adds a URL to the URL List, returns its offset
SizeT OffsetCreate(URL key){
    return UrlListChunk.add_url(key);
}

//If the URL is already in the URL List, it adds the anchor text
void UrlPoolChunk::ProcessUrl(URL url, String anchor_text){
    //TODO
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
