//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
#include "string,hpp"
#include "vector.hpp"

using URL = String;

//Returns a vector of urls and urls offsets for the thread to process
//The caller is then expected to handle parsing the pages and return the info
Vector<Pair<URL, SizeT>> UrlPool::RequestUrls(){
    //TODO
}

//Adds URL offset and rank pair to the frontier
void UrlPool::AddToFrontier(SizeT urlOffset, SizeT urlRank){
    //TODO
    //frontier.push({urlOffset, urlRank});
}
