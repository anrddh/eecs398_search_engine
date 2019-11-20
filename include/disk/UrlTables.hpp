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
class UrlOffsetTable {
public:
   static UrlOffsetTable & getTable() {
      static UrlOffsetTable unique_obj();
      return unique_obj;
   }

   // Will find the offset (if this string was seen
   fb::SizeT getOffset( fb::StringView str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(offset_hashes[hash % NumBins].second);
      return offset_hashes[hash % NumBins].first.find( str, hash );
   }

   fb::StringView accessOffset(fb::SizeT offset) {
      return UrlStore::getStore().getUrl(offset);
   }

private:
   UrlOffsetTable() {
      for (int i = 0; i < NumBins; ++i){
         offset_hashes[i].first.set_string_list();
      }
   }

   constexpr static fb::SizeT NumBins = 256;
   static UrlOffsetTable *ptr;

   fb::Pair<OffsetLookupChunk<fb::StringView,
                              fb::SizeT,
                              fb::Hash<fb::StringView>>,
            fb::Mutex> offset_hashes[NumBins];
   fb::Hash<fb::StringView> hasher;
};

// Same as above
// We need to hard code the file we save to
class UrlInfoTable {
public:
   // Meyer's method for singletons
    static UrlInfoTable & getTable() {
       static UrlInfoTable unique_obj;
       return &unique_obj;
    }

    void HandleParsedPage(ParsedPage&& pp) {
      fb::SizeT hash = hasher(pp.url_offset);
      info_hashes[hash % NumBins].second.lock();
      UrlInfo& info = info_hashes[hash % NumBins].first[ pp.url_offset ];
      assert(info.state != 'p');
      info.UrlOffset = pp.url_offset;
      info.state = 'p';
      Vector<SizeT> adj_list;
      for (const fb::Pair<fb::String, fb::String>& link : pp.links) {
         SizeT link_offset = UrlOffsetTable::getTable().getOffset(link.first());
         adj_list.pushBack(link_offset);
      }
      auto adj_pair = addList( adj_list );
      info.AdjListBegin = adj_pair.first;
      info.AdjListEnd = adj_pair.second;
      info_hashes[hash % NumBins].second.unlock();

      for (int i = 0; i < adj_list.size(); ++i) {
         fb::SizeT link_hash = hasher(adj_list[i]);
         info_hashes[hash % NumBins].second.lock();
         
         UrlInfor& link_info = info_hashes[hash % NumBins].first[ adj_list[i] ];
         link_info.AnchorTextOffsets = addStr(pp.link[i].second,
               link_info.AnchorTextOffsets);
         if (link_info.state == 'u') {
            link_info.state = 'f'; // will add it to the frontier
            info_hashes[hash % NumBins].second.unlock();
            
         } else {
            info_hashes[hash % NumBins].second.unlock();
         }
      }
    }


    // TODO is this function even used...?
   UrlInfo &get_info( fb::StringView str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(info_hashes[hash % NumBins].second);
      return info_hashes[hash % NumBins].first[str];
   }

private:
   UrlInfoTable(){}

   constexpr static fb::SizeT NumBins = 256;
   //The unorderedmaps link the hashes of urls to the associated url infos
   fb::Pair<fb::UnorderedMap<fb::SizeT, UrlInfo>, fb::Mutex> info_hashes[NumBins];
   fb::Hash<fb::SizeT> hasher;
};
