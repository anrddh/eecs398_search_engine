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

// N represents the number of bins that the Hashes are separated into
template <SizeT N>
class StringPool {
public:
   StringPool( std::string filename );
   SizeT get_offset( std::string str );
private:

};

}; //namespace fb
