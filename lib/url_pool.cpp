//Created on 11/6/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
#include "string,hpp"
#include "vector.hpp"

using URL = String;

//Returns a vector of urls for the thread to process
//The caller is then expected to handle parsing the pages and updating the url info
Vector<URL> UrlPool::RequestUrls(){
    //TODO
}

//returns by reference the url info for the given url
UrlInfo & UrlPool::UrlInfoLookup(Url url){
    return InfoLookup->get_info(url);
}

//Adds URL offset and rank pair to the frontier
void UrlPool::AddToFrontier(SizeT urlOffset, SizeT urlRank){
    //TODO
    //frontier.push({urlOffset, urlRank});
}
