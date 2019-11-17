// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "UrlStore.hpp"
#include "offset_lookup.hpp"
#include "UrlInfo.hpp"

#include "../../lib/mutex.hpp"
#include "../../lib/utility.hpp" // for pair
#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/stddef.hpp"
#include "../../lib/unordered_map.hpp"
#include "../../lib/unordered_set.hpp"

// Number of bins is currently just hardcoded to 256,
// because template singletons are apparently very
// difficult to compile
class StringPool {
public:
   static void init() {
       delete ptr;
       ptr = new StringPool();
   }

   static StringPool & getPool() {
       return *ptr;
   }

   // Will find the offset (if this string was seen
   fb::SizeT getOffset( fb::String str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(offset_hashes[hash % NumBins].second);
      return offset_hashes[hash % NumBins].first.find( str, hash );
   }

   fb::StringView accessOffset(fb::SizeT offset) {
      return UrlStore::getStore().getUrl(offset);
   }

private:
   StringPool() {
      for (int i = 0; i < NumBins; ++i){
         offset_hashes[i].first.set_string_list();
      }
   }

   constexpr static fb::SizeT NumBins = 256;
   static StringPool *ptr;

   fb::Pair<OffsetLookupChunk<fb::StringView,
                              fb::SizeT,
                              fb::Hash<fb::StringView>>,
            fb::Mutex> offset_hashes[NumBins];
   fb::Hash<fb::StringView> hasher;
};

// Same as above
class InfoPool {
public:
    static void init() {
        delete ptr;
        ptr = new InfoPool();
    }

    static InfoPool & getPool() {
        return *ptr;
    }

   UrlInfo &get_info( fb::StringView str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(info_hashes[hash % NumBins].second);
      return info_hashes[hash % NumBins].first[str];
   }

private:
   InfoPool(){}

   constexpr static fb::SizeT NumBins = 256;
   static InfoPool *ptr;
   //The unorderedmaps link the hashes of urls to the associated url infos
   fb::Pair<fb::UnorderedMap<fb::StringView, UrlInfo>, fb::Mutex> info_hashes[256];
   fb::Hash<fb::StringView> hasher;
};
