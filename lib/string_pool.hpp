// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "unordered_set.hpp"
#include "offset_lookup.hpp"
#include "mutex.hpp"
<<<<<<< HEAD
#include "utilities.hpp" // for pair
#include "string.hpp"
=======
#include "utility.hpp" // for pair
#include <string> // TODO switch it out
#define String std::string
>>>>>>> 493d51dfaba6c0df9dbb667cf830de38e3e31557

namespace fb {

// N represents the number of bins that the Hashes are separated into
template <SizeT N>
class StringPool {
public:
   StringPool( String filename ) : str_array( filename ) {
       for (int i = 0; i < N; ++i){
           offset_hashes[i].first.set_string_list(&str_array);
       }
   }

   // Will find the offset (if this string was seen
   SizeT get_offset( String str ) {
      SizeT hash = hasher(str);
      AutoLock<Mutex> l(offset_hashes[hash % N].second);
      return offset_hashes[hash % N].first.find( str, hash );
   }

   char* access_offset( SizeT offset ) {
      return str_array.get_str(offset);
   }
private:
   SavedStrings str_array;
   Pair<OffsetLookupChunk<String, SizeT, Hash<String>>, Mutex> offset_hashes[N];
   Hash<String> hasher;
};

template <SizeT N>
class InfoPool {
public:
    InfoPool(){}

    UrlInfo &get_info( String str ) {
        SizeT hash = hasher(str);
        AutoLock<Mutex> l(info_hashes[hash % N].second);
        return info_hashes[hash % N].first[str];
    }

private:
    //The unorderedmaps link the hashes of urls to the associated url infos
    Pair<UnorderedMap<String, UrlInfo>, Mutex> info_hashes[N];
    Hash<String> hasher;
};

}; //namespace fb
