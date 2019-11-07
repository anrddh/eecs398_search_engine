// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
//#include "string,hpp"
//#include "vector.hpp"
#include <string>
#include <vector>

namespace fb {

template <SizeT N>
SizeT get_offset( std::string str ) {
   SizeT hash = 0;// TODO figure out the hash
   offset_hashes[hash % N].second.lock();
   // TODO implement
   
}

template <SizeT N>
char* access_offset( SizeT offset ) {
// TODO implement
}
