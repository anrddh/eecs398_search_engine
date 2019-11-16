// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "../../lib/stddef.hpp"
#include "UrlStore.hpp"
#include "../../lib/unordered_set.hpp"
#include "offset_lookup.hpp"
#include "../../lib/mutex.hpp"
#include "../../lib/utility.hpp" // for pair
#include "../../lib/string.hpp"

// N represents the number of bins that the Hashes are separated into
template <fb::SizeT N>
class StringPool {
public:
   StringPool( fb::String filename ) : str_array( filename ) {
       for (int i = 0; i < N; ++i){
           offset_hashes[i].first.set_string_list(&str_array);
       }
   }

   // Will find the offset (if this string was seen
   fb::SizeT get_offset( fb::String str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(offset_hashes[hash % N].second);
      return offset_hashes[hash % N].first.find( str, hash );
   }

   char* access_offset( fb::SizeT offset ) {
      return str_array.get_str(offset);
   }
private:
   UrlStore str_array;
   fb::Pair<OffsetLookupChunk<fb::String, fb::SizeT, fb::Hash<fb::String>>, fb::Mutex> offset_hashes[N];
   fb::Hash<fb::String> hasher;
};

template <fb::SizeT N>
class InfoPool {
public:
    InfoPool(){}

    UrlInfo &get_info( fb::String str ) {
        fb::SizeT hash = hasher(str);
        fb::AutoLock<fb::Mutex> l(info_hashes[hash % N].second);
        return info_hashes[hash % N].first[str];
    }

private:
    //The unorderedmaps link the hashes of urls to the associated url infos
    fb::Pair<fb::UnorderedMap<fb::String, UrlInfo>, fb::Mutex> info_hashes[N];
    fb::Hash<fb::String> hasher;
};
