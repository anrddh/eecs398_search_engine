//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include <disk/disk_vec.hpp>

#include <fb/vector.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/thread.hpp>
#include <fb/string.hpp>
#include <fb/mutex.hpp>
#include <fb/cv.hpp>
#include <fb/bloom_filter.hpp>

#include <tcp/url_tcp.hpp>

constexpr fb::SizeT NumFrontierBins = 13;
// we will only randomly choose from first SEARCH_RESTRICTION number of elements
constexpr fb::SizeT SEARCH_RESTRICTION= 16384;

constexpr fb::SizeT NUM_TRY = 4000;
constexpr fb::SizeT NUM_SAMPLE = 3;

// The minimum number of bits required to
// check 2 billion urls, have 0.1 false positive
// with 3 hash tables
constexpr fb::SizeT MIN_NUM_BITS = 9616654723;
constexpr fb::SizeT PAGE_SIZE = 4096 * 8; // in bits
constexpr fb::SizeT NUM_PAGES = (MIN_NUM_BITS / PAGE_SIZE / NumFrontierBins) + 1;
constexpr fb::SizeT BLOOM_FILTER_SIZE = PAGE_SIZE * NUM_PAGES;
constexpr uint8_t NUM_HASHES = 3;

struct FrontierUrl {
    fb::SizeT offset;
    fb::SizeT ranking;
};

// This blocks
void frontierTerminate();

void* addQueueToToParsed( void * );

class FrontierBin {
public:
    FrontierBin(fb::String filename);

    void addToQueue( fb::Vector< fb::String >&& urls );

    // Adds to list of urls already seen
    // Does not actually add to the frontier
    // Does not lock! not thread safe
    void addSeen( fb::StringView url );

    fb::Vector<fb::SizeT> getUrl( );

    fb::SizeT size() const;

    void printUrls() const;
    fb::CV toAddQueueCV;

private:
    // Needs to be locked
    inline fb::SizeT search_index(fb::SizeT rand_num, fb::SizeT region_num) {
      return ((rand_num % SEARCH_RESTRICTION) + region_num) % toParse.size();
    }

    friend void* addQueueToToParsed( void* );
    bool keepAddingFromQueue(); // helper for addToFrontierFromQueue
    void addToFrontierFromQueue();

    fb::Thread t;
    fb::Vector< fb::Vector< fb::String > > toAddQueue;
    fb::Mutex toAddQueueM;
    fb::Mutex localSeedM;
    fb::Mutex toParseM;
    unsigned int localSeed;
    DiskVec<FrontierUrl> toParse;
    BloomFilter<NUM_HASHES, BLOOM_FILTER_SIZE, DiskVec, fb::StringView> bloom;
};

class Frontier {
public:
    static void init(fb::String prefix);

    static Frontier & getFrontier();

   // If this is a url we have seen for the first time
   // then (most of the times) add the link to the frontier
    void addUrls( fb::Vector< fb::String >&& urls );

   // If this is a url we have seen for the first time
   // then (most of the times) add the link to the frontier
    void addUrls( fb::Vector< ParsedPage >&& urls );

    // Adds to list of urls already seen
    // Does not actually add to the frontier
    // Does not lock! not thread safe
    void addSeen(fb::StringView url);

    // Note that this prints the estimate for the current size
    // but due to race conditions, it might not be exact
    fb::SizeT size() const;

    fb::Vector<fb::SizeT> getUrl() const;

    static void shutdown();

    void printUrls() const;

private:
    static Frontier *ptr;
    static char frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

    Frontier() = default;
};
