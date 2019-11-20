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

/*

   I don't think this code is no long needed

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

*/

// Same as above
// We need to hard code the file we save to
class UrlInfoTable {
public:
   // Meyer's method for singletons
    static UrlInfoTable & getTable() {
       static UrlInfoTable unique_obj;
       return &unique_obj;
    }

    // Adds a new url
    // copies url to the disk, and initializes url_info struct
    // returns true on success
    // returns false if this url was already seen before
    bool addSeed( fb::StringView url ) 
    {
       return (0 != add_link(url, ""));
    }

    // This code should be only used on pages
    // of whose url we have already seen before
    // to add a seed use the add seed function
    //
    // Returns url_offsets of urls we have seen for the first time
    // (i.e. these urls should be added to the frontier)
    fb::Vector<SizeT> HandleParsedPage(ParsedPage&& pp) {
       // When we are adding links we have found:
       //
       // if this url was never seen before, then add the url
       // to disk_vec. Then construct a new url_info in disk_vec.
       // Then add the url to the frontier (check robots.txt first)
      fb::SizeT hash = hasher( UrlStore::getStore().getUrl(pp.url_offset) );
      fb::Pair<fb::UnorderedMap<fb::StringView, SizeT>, fb::Mutex> info_hash 
         = info_hashes[hash % NumBins];

      info_hash.second.lock();

      // This we default initialize 
      fb::Pair<StringView&, SizeT&> url_info_pair = 
         info_hash.functionThatIsOnlyForJaeyoonInThatOneSpecialCase(
               UrlStore::getStore().getUrl(pp.url_offset));

      // This means that we have never seen before
      assert(url_info_pair.second != 0);

      // url info object associated with this url
      UrlInfo& info = url_info.[ url_info_pair.second ];

      // We need to call copy on the atomic to get a correct snapshot
      char state = info.state;
      if (state == 'm' || state == 'p') 
      {
         // This page has already been parsed before
         // this might happen if the master is sending urls via tcp.
         // The child received a couple of urls, but the tcp failed. 
         // In this case the master will add the urls back to the frontier
         // but the worker might parse the page and send it back to master
         info_hash.second.unlock();
         return;
      }

      // Even though we set it to be parsed,
      // it is technically not parsed. This is to ensure
      // that other threads will not modify this object while we
      // work on it
      info.state = 'm'; // set the page to modifying
      info_hash.second.unlock();


      Vector<SizeT> adj_list;

      for (const fb::Pair<fb::String, fb::String>& link : pp.links) {
         // TODO we need to check with robots.txt if we should add should parse this link
         // We will only add to the adj_list if it is allowed by robots.txt
         SizeT url_offset = adj_list.pushBack( add_link(link.first, link.second) );
         if ( url_offset != 0 )
         {
            adj_list.url_offset;
         }
      }

      info_hash.second.lock();
      info.AdjListOffset = addList( adj_list );
      info_hashes[hash % NumBins].second.unlock();
      info.state = 'p'; // set the page to be parsed
      info_hash.second.unlock();

      return adj_list;
    }


    // TODO is this function even used...?
   UrlInfo &get_info( fb::StringView str ) {
      fb::SizeT hash = hasher(str);
      fb::AutoLock<fb::Mutex> l(info_hashes[hash % NumBins].second);
      return info_hashes[hash % NumBins].first[str];
   }

private:
   UrlInfoTable(){}

    // Adds the anchor text
    // If this was the first occurence this url was seen, initialize urls info
    // and the url string on disk. It then returns the url_offset of the link
    // otherwise, it returns 0.
    // This function grabs locks from info_hashes, so the caller should ensure that
    // it is not holding any locks from info_hashes
    SizeT add_link(const String& link, const String& anchor_text ) {
      fb::SizeT hash = hasher( UrlStore::getStore().getUrl( link ) );
      fb::Pair<fb::UnorderedMap<fb::StringView, SizeT>, fb::Mutex> info_hash 
         = info_hashes[hash % NumBins];

      AutoLock<Mutex> l(info_hash.second);

      // This we default initialize 
      fb::Pair<StringView&, SizeT&> url_info_pair = 
         info_hash.functionThatIsOnlyForJaeyoonInThatOneSpecialCase(
               UrlStore::getStore().getUrl(pp.url_offset)
               );

      bool is_new_url = (url_info_pair.second == 0);

      if ( is_new_url )
      {
         // This url has never been added before.
         // Thus we need to add this url to the disk
         //
         // This is implemented by the following:
         // We will add a copy of the url to the disk
         // We modify the the key to point to the new
         // string on the disk.
         // Usually, it would be very bad to modify the key
         // However in this case, the equality operator and the hash does not
         // change since they both represent the same string
         SizeT url_offset = UrlStore::getStore().addUrl( link );
         url_info_pair.first = UrlStore::getStore().getUrl( url_offset );
         url_info_pair.second = url_info.reserve(1);
         url_info[url_info_pair.second].state = 'u';
         url_info[url_info_pair.second].UrlOffset = url_offset;
      }

      url_info[url_info_pair.second].AnchorTextOffsets = 
         AnchorStore::getStore().addStr( anchor_text, 
               url_info[ url_info_pair.second ].AnchorTextOffsets );

      if ( is_new_url)
      {
         return url_info_pair.second;
      }
      else
      {
         return 0;
      }
    }

   constexpr static fb::SizeT NumBins = 256;
   //The unorderedmaps link the hashes of urls to the associated url infos
   // unordered map from stringView (that points to disk vec of urls
   fb::Pair<fb::UnorderedMap<fb::StringView, SizeT>, fb::Mutex> info_hashes[NumBins];

   // TODO we need to initialize this thing
   // with a file
   DiskVec<UrlInfo> url_info;
   fb::Hash<fb::SizeT> hasher;
};
