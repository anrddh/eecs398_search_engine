//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
//#include "string"
#include <string>
#define URL std::string
#define String std::string

namespace fb {

void UrlPoolChunk::ProcessUrl(URL url, String anchor_text){

}

void UrlPoolChunk::AddToFronter(URL url, String anchor_text){

}

void UrlPoolChunk::Run(){

}

//Wrapper global function for running URLPoolChunks
void UrlPoolChunkRun(&UrlPoolChunk PoolChunk){
    PoolChunk.Run();
}

}; //namespace fb
