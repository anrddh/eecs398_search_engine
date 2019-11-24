// Created by Jaeyoon Kim 11/7/19
#pragma once

#include <disk/url_store.hpp>
#include <disk/offset_lookup.hpp>
#include <disk/UrlInfo.hpp>
#include <disk/adj_store.hpp>
#include <disk/anchor_store.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <tcp/url_tcp.hpp> // for ParsedPage

#include <fb/mutex.hpp>
#include <fb/utility.hpp> // for pair
#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <fb/stddef.hpp>
#include <fb/unordered_map.hpp>
#include <fb/unordered_set.hpp>

// Same as above
// We need to hard code the file we save to
class UrlInfoTable {
public:
    static void init(fb::StringView filename) {
        delete ptr;
        ptr = new UrlInfoTable(filename);
    }

   // Meyer's method for singletons
   // Note that c++11 standard guarantees
   // that the constructor will finish the constructor
   // before the object is ever returned
    static UrlInfoTable & getTable() {
        return *ptr;
    }

    // Adds a new url
    // copies url to the disk, and initializes url_info struct
    // returns url_offset of the page if this the first time seen
    // returns false if this url was already seen before
    fb::SizeT addSeed( fb::StringView url )
    {
       return add_link(url, "");
    }

    // This code should be only used on pages
    // of whose url we have already seen before
    // to add a seed use the add seed function
    //
    // Returns url_offsets of urls we have seen for the first time
    // (i.e. these urls should be added to the frontier)
    fb::Vector<fb::SizeT> HandleParsedPage(ParsedPage&& pp) {
       // When we are adding links we have found:
       //
       // if this url was never seen before, then add the url
       // to disk_vec. Then construct a new url_info in disk_vec.
       // Then add the url to the frontier (check robots.txt first)
      fb::StringView url =
               UrlStore::getStore().getUrl( pp.url_offset );

      fb::SizeT hash = hasher( url );
      fb::Pair<fb::UnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex>& info_hash
         = info_hashes[hash % NumBins];

      info_hash.second.lock();

      // This we default initialize
      fb::SizeT& url_info_offset = info_hash.first[ url ];

      // This means that we have never seen before
      if ( url_info_offset == 0)
      {
         std::cerr << "Error: HandleParsedPage has never seen url offset of "
            << pp.url_offset << std::endl;
         info_hash.second.unlock();
      }

      // url info object associated with this url
      UrlInfo& info = url_info[ url_info_offset ];

      assert( info.UrlOffset == 0 || info.UrlOffset == pp.url_offset );
      info.UrlOffset = pp.url_offset;

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
         return {};
      }

      // Even though we set it to be parsed,
      // it is technically not parsed. This is to ensure
      // that other threads will not modify this object while we
      // work on it
      info.state = 'm'; // set the page to modifying
      info_hash.second.unlock();


      fb::Vector<fb::SizeT> adj_list;

      for (const fb::Pair<fb::String, fb::String>& link : pp.links) {
         // TODO we need to check with robots.txt if we should add should parse this link
         // We will only add to the adj_list if it is allowed by robots.txt
         fb::SizeT url_offset = add_link(link.first, link.second);
         if ( url_offset != 0 )
         {
            adj_list.pushBack( url_offset );
         }
      }

      info_hash.second.lock();
      info.AdjListOffsets = AdjStore::getStore().addList( adj_list );
      info.state = 'p'; // set the page to be parsed
      info_hash.second.unlock();

      return adj_list;
    }

private:
   // The constructor should be called only once
   // It will recover the file name of the url_info (disk_vec)
   // and load it. Then it will reconstruct the hash table that
   // maps url_offset to url_info_offset
   // Note that the lock will not be grabbed when the constructor is running
    UrlInfoTable(fb::StringView fname) : url_info(fname)
   {
      for ( fb::SizeT url_info_offset = 0; url_info_offset < url_info.size();
            ++url_info_offset )
      {
         if ( url_info[ url_info_offset ].UrlOffset == 0 )
         {
            continue;
         }

         fb::StringView url = UrlStore::getStore().getUrl(
               url_info[ url_info_offset ].UrlOffset );
         log(logfile, "in url info table ctor add url ", url, '\n');
         fb::SizeT hash = hasher( url );

         info_hashes[hash % NumBins].first[ url ] = url_info_offset;
      }
   }


    // Adds the anchor text
    // If this was the first occurence this url was seen, initialize urls info
    // and the url string on disk. It then returns the url_offset of the link
    // otherwise, it returns 0.
    // This function grabs locks from info_hashes, so the caller should ensure that
    // it is not holding any locks from info_hashes
    // link can't be const ref in order to use functionThatIsOnlyForJaeyoonInThatOneSpecialCase
   fb::SizeT add_link(fb::StringView link, fb::StringView anchor_text )
   {
      fb::SizeT hash = hasher( link );
      fb::Pair<fb::UnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex>& info_hash
         = info_hashes[hash % NumBins];

      fb::AutoLock<fb::Mutex> l(info_hash.second);

      // This we default initialize
      fb::Pair<fb::StringView*, fb::SizeT*> url_info_pair =
         info_hash.first.functionThatIsOnlyForJaeyoonInThatOneSpecialCase(
               link );

      bool is_new_url = (*url_info_pair.second == 0);

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
         fb::SizeT url_offset = UrlStore::getStore().addUrl( link );
         *url_info_pair.first = UrlStore::getStore().getUrl( url_offset );
         *url_info_pair.second = url_info.reserve(1);
         url_info[ *url_info_pair.second].state = 'u';
         url_info[ *url_info_pair.second].UrlOffset = url_offset;

         // TODO this is just for debugging
         // delete below
         assert( info_hash.first[ link ] == *url_info_pair.second );
      }

      url_info[ *url_info_pair.second ].AnchorTextOffsets =
         AnchorStore::getStore().addStr( anchor_text,
               url_info[ *url_info_pair.second ].AnchorTextOffsets );

      if ( is_new_url )
      {
         return url_info[ *url_info_pair.second ].UrlOffset;
      }
      else
      {
         return 0;
      }
    }

   constexpr static fb::SizeT NumBins = 256;

    static UrlInfoTable *ptr;

   //The unorderedmaps link the hashes of urls to the associated url infos
   // unordered map from stringView (that points to disk vec of urls
   fb::Pair<fb::UnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex> info_hashes[NumBins];

   // TODO we need to initialize this thing
   // with a file
   DiskVec<UrlInfo> url_info;
   fb::Hash<fb::StringView> hasher;
};
