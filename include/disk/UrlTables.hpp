// Created by Jaeyoon Kim 11/7/19
#pragma once
/*

#include <cassert>
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
#include <fb/no_delete_unordered_map.hpp>

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
			 return add_link(url, "").first;
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
			fb::Pair<fb::NoDeleteUnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex>& info_hash
				 = info_hashes[hash % NumBins];

			info_hash.second.lock();

			// This we default initialize
			fb::SizeT& url_info_offset = info_hash.first[ url ];
			assert(url_info_offset);

			// url info object associated with this url
			UrlInfo& info = url_info[ url_info_offset ];

			assert( info.UrlOffset == pp.url_offset );
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
         fb::Vector<fb::SizeT> to_add_to_frontier;

			for (fb::Pair<fb::String, fb::String>& link : pp.links) {
				 // TODO we need to check with robots.txt if we should add should parse this link
				 // We will only add to the adj_list if it is allowed by robots.txt

				 link.second += ' '; // Ask Ani about why this is a thing
             // first is url_offset (if it was never seen before)
             // second is url_info_offset
				 auto offset_pair = add_link(link.first, link.second);
				 if ( offset_pair.first != 0 )
						to_add_to_frontier.pushBack( offset_pair.first );
                 adj_list.pushBack(offset_pair.second);
			}

			info_hash.second.lock();
			info.AdjListOffsets = AdjStore::getStore().addList( adj_list );
			info.state = 'p'; // set the page to be parsed
			info_hash.second.unlock();

			return to_add_to_frontier;
		}

		void assert_invariance() {
            std::cout << "Beginning consistency check..." << std::endl;

            for (auto &[_, l] : info_hashes)
                 l.lock();

            std::cout << url_info.size() << " urls to check." << std::endl;

            // For progress bar.
            auto modFactor = url_info.size() / 100;
            auto currPercentage = 1;

			 // First one is a dummy
			 for (fb::SizeT i = 1; i < url_info.size(); ++i) {
                 if (!(i % modFactor)) {
                     std::cout << "Progress: " << currPercentage << '%' << std::endl;
                     ++currPercentage;
                 }

                 fb::StringView url =
                     UrlStore::getStore().getUrl(url_info[ i ].UrlOffset);

                 fb::SizeT hash = hasher(url);
                 auto it = info_hashes[ hash % NumBins ].first.find( url );
                 if (it == info_hashes[ hash % NumBins ].first.end()) {
                     std::cout << "`" << url
                               << "' does not have a url info entry."
                               << std::endl;
                 }

                 if (*it != i) {
                     std::cout << "i = " << i << " has url " <<
                         UrlStore::getStore().getUrl( url_info[ i ].UrlOffset ) << std::endl;
                     std::cout << "*it = " << *it << " has url " <<
                         UrlStore::getStore().getUrl( url_info[ *it ].UrlOffset ) << std::endl;
                     break;
                 }

                 auto &adjStore = AdjStore::getStore();
                 auto adjList =
                     adjStore.getList(url_info[i].AdjListOffsets.first,
                                      url_info[i].AdjListOffsets.second);

                 for ( fb::SizeT link_info_offset : adjList) {
                     fb::StringView link_url =
                         UrlStore::getStore().getUrl( url_info[link_info_offset].UrlOffset );
                     fb::SizeT link_hash = hasher( link_url );
                     auto link_it = info_hashes[ link_hash % NumBins ].first.find( link_url );
                     if ( link_it == info_hashes[ link_hash % NumBins ].first.end() )
                        std::cout << "Link for " << url << " does not urlstore" << std::endl;
                     else if ( *link_it != link_info_offset )
                        std::cout << "Link for url " << url <<
                           " has incorrect url_info offset. in url_info: " <<
                           *link_it << " in adj_list " << link_info_offset << std::endl;
                 }
             }

            for (auto &[_, l] : info_hashes)
                l.unlock();
		}

		void print_info( fb::StringView url ) {
			 fb::SizeT hash = hasher( url );

			 auto it = info_hashes[ hash % NumBins ].first.find( url );
			 if (it == info_hashes[ hash % NumBins ].first.end()) {
					 std::cout << "This page does not exist!" << std::endl;
					return;
			 }
			 fb::SizeT url_info_offset = *it;

			 std::cout << "Info for page " << url << std::endl;

			for ( fb::SizeT link_offset : AdjStore::getStore().getList(
            url_info[ url_info_offset ].AdjListOffsets.first,
            url_info[ url_info_offset ].AdjListOffsets.second ) )
            {
            fb::StringView link_url =
               UrlStore::getStore().getUrl( link_offset );
                 std::cout << "\tHas link " << link_url << std::endl;
            }
		}

private:
	 // The constructor should be called only once
	 // It will recover the file name of the url_info (disk_vec)
	 // and load it. Then it will reconstruct the hash table that
	 // maps url_offset to url_info_offset
	 // Note that the lock will not be grabbed when the constructor is running
		UrlInfoTable(fb::StringView fname) : url_info(fname)
	 {
			// Place dummy url_info
			if (url_info.size() == 0) {
				 fb::SizeT dummy_url_offset = url_info.reserve(1);
				 assert(dummy_url_offset == 0);
			}

         double reserve_size = url_info.size() * 2 / NumBins;
         fb::SizeT next_pow_of_two = 2;
         while (next_pow_of_two < reserve_size) {
            next_pow_of_two *= 2;
         }
         std::cout << "Reserving size of " << next_pow_of_two << " for each of "
            << NumBins << " hashtables in UrlTable" << std::endl;
         for ( fb::SizeT i = 0; i < NumBins; ++i ) {
            info_hashes[i].first.reserve( next_pow_of_two );
         }


			for ( fb::SizeT url_info_offset = 0; url_info_offset < url_info.size();
						++url_info_offset )
			{
            fb::SizeT url_offset = url_info[ url_info_offset ].UrlOffset;
				 if ( url_offset == 0 )
				 {
						continue;
				 }

				 fb::StringView url = UrlStore::getStore().getUrl( url_offset );
				 fb::SizeT hash = hasher( url );

				 info_hashes[hash % NumBins].first[ url ] = url_info_offset;
			}
	 }


		// Adds the anchor text
      //
		// This function grabs locks from info_hashes, so the caller should ensure that
		// it is not holding any locks from info_hashes
		// link can't be const ref in order to use functionThatIsOnlyForJaeyoonInThatOneSpecialCase
      //
      // returns url_offset of link (if this was the first time it was seen) and returns
      // url_info_offset of the link.
      fb::Pair<fb::SizeT, fb::SizeT> add_link(fb::StringView link, fb::StringView anchor_text )
	 {
			fb::SizeT hash = hasher( link );
			fb::Pair<fb::NoDeleteUnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex>& info_hash
				 = info_hashes[hash % NumBins];

			fb::AutoLock<fb::Mutex> l(info_hash.second);

			// This we default initialize
         fb::NoDeleteUnorderedMap<fb::StringView, fb::SizeT>::Bucket& url_info_bucket =
				 info_hash.first.functionThatIsOnlyForJIaeyoonInThatOneSpecialCase(
							 link );

			bool is_new_url = (url_info_bucket.status == fb::MapStatus::Empty);

         // DO NOT erase this variable - talk to Jayeoon
         fb::SizeT info_offset = url_info_bucket.val;
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
				 assert( url_offset && "addUrl returned 0");
				 url_info_bucket.key = UrlStore::getStore().getUrl( url_offset );

             // Be very careful that after any unordered_map operation, the reference
             // url_info_bucket might be invalidated!
				 assert( url_info_bucket.key == link );
             // We need the variable info_offset if we want to do the assert
             // since operator[] invalidates the reference url_info_bucket
             info_offset =url_info.reserve(1);
				 url_info_bucket.val = info_offset;
             assert(info_offset);
             url_info_bucket.status = fb::MapStatus::Filled;
				 url_info[ info_offset ].state = 'u';
				 url_info[ info_offset ].UrlOffset = url_offset;

				 assert( info_hash.first[ link ] == info_offset );
			}

			url_info[ info_offset ].AnchorTextOffsets =
				 AnchorStore::getStore().addStr( anchor_text,
							 url_info[ info_offset ].AnchorTextOffsets );

			if ( is_new_url )
			{
				 return {url_info[ info_offset ].UrlOffset, info_offset };
			}
			else
			{
				 return {0, info_offset };
			}
		}

	 constexpr static fb::SizeT NumBins = 256;

		static UrlInfoTable *ptr;

	 //The unorderedmaps link the hashes of urls to the associated url infos
	 // unordered map from stringView (that points to disk vec of urls
	 fb::Pair<fb::NoDeleteUnorderedMap<fb::StringView, fb::SizeT>, fb::Mutex> info_hashes[NumBins];

	 // TODO we need to initialize this thing
	 // with a file
	 DiskVec<UrlInfo> url_info;
	 fb::Hash<fb::StringView> hasher;
};
*/
