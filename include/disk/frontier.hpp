//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include <disk/disk_vec.hpp>

#include <fb/vector.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/thread.hpp>
#include <fb/string.hpp>
#include <fb/mutex.hpp>

constexpr fb::SizeT NumFrontierBins = 16;

// we will only randomly choose from first SEARCH_RESTRICTION number of elements
constexpr fb::SizeT SEARCH_RESTRICTION= 16384; 

constexpr fb::SizeT NUM_TRY = 4000;
constexpr fb::SizeT NUM_SAMPLE = 3;

struct FrontierUrl {
    fb::SizeT offset;
    fb::SizeT ranking;
};

class FrontierBin {
public:
    FrontierBin(fb::StringView filename);

    void addUrl(const FrontierUrl &url);

    fb::Vector<fb::SizeT> getUrl( );

    fb::SizeT size() const;

private:
    // Needs to be locked
    inline fb::SizeT search_index(fb::SizeT rand_num, fb::SizeT region_num) {
      return ((rand_num % SEARCH_RESTRICTION) + region_num) % toParse.size(); 
    }
    fb::Mutex localSeedM;
    fb::Mutex toParseM;
    unsigned int localSeed;
    DiskVec<FrontierUrl> toParse;
};

class Frontier {
public:
    static void init(fb::String prefix);

    static Frontier & getFrontier();

   // If this is a url we have seen for the first time
   // then (most of the times) add the link to the frontier
    void addUrl(const fb::String& url);

    // Note that this prints the estimate for the current size
    // but due to race conditions, it might not be exact
    fb::SizeT size() const;

    fb::Vector<fb::SizeT> getUrl() const;

private:
    static Frontier *ptr;
    static char frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

    Frontier() = default;
};
