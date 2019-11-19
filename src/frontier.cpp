// * Created by Jaeyoon Kim
// * Edited by Aniruddh Agarwal to improve conformance with style and
//   memory map frontier

#include <disk/frontier.hpp>
#include <disk/disk_vec.hpp>

#include <fb/mutex.hpp>
#include <fb/utility.hpp>
#include <fb/string_view.hpp>

#include <atomic>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>

using fb::SizeT;
using fb::Pair;
using fb::Vector;
using fb::AutoLock;
using fb::StringView;
using fb::String;

using std::atomic;

// TODO set to 0?
atomic<int> insertCounter = 0;
atomic<int> getCounter = 0;
atomic<int> randSeedCounter = 0;

constexpr SizeT NUM_TRY = 1000;
constexpr SizeT NUM_SAMPLE = 3;

FrontierBin::FrontierBin( StringView filename, bool init )
    : localSeed( ++randSeedCounter ),
      idx( localSeed ),
      toParse(filename, init) {}

void FrontierBin::addUrl(const FrontierUrl &url) {
    AutoLock lock( toParseM );
    toParse.pushBack( url );
}

Vector<SizeT> FrontierBin::getUrl( ) {
    int rand_num[ NUM_TRY ];
    localSeedM.lock( );
    for ( int i = 0;  i < NUM_TRY;  ++i )
        rand_num[ i ] = rand_r( &localSeed );
    localSeedM.unlock( );

    Vector<SizeT> urls_to_return;
    AutoLock lock( toParseM );
    if (toParse.size( ) < NUM_SAMPLE)
        return {}; // empty url

    int max_ranking = 0; // Requires that any ranking of urls to be greater than 0
    int max_idx;

    // Find what to sample
    // Compute the highest ranking amongst first NUM_SAMPLE randomly picked urls
    for (int i = 0; i < NUM_SAMPLE; ++i) {
        if ( max_ranking < toParse[ rand_num[i] % toParse.size() ].ranking ) {
            max_ranking = toParse[ rand_num[i] % toParse.size() ].ranking;
            max_idx = rand_num[i] % toParse.size();
        }
    }

    urls_to_return.pushBack( toParse[ max_idx ].offset );
    toParse[ max_idx ] = toParse.back( );
    toParse.popBack( );

    // We randomly check urls
    // If their ranking is greater than or equal to max_ranking,
    // then we will take them to be parsed
    // Note that it is possible that same url might be checked multiple times
    // However, this is not likely since there should be many urls in here each time
    for ( int i = NUM_SAMPLE; i < NUM_TRY && !toParse.empty(); ++i ) {
        if ( toParse[ rand_num[i] % toParse.size() ].ranking >= max_ranking ) {
            urls_to_return.pushBack( toParse[ rand_num[i] % toParse.size() ].offset ) ;
            toParse[ rand_num[i] % toParse.size() ] = toParse.back();
            toParse.popBack( );
        }
    }

    return urls_to_return;
}

Frontier *Frontier::ptr = nullptr;
char Frontier::frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

void Frontier::init(String prefix, bool init) {
    delete ptr;

    ptr = new Frontier;

    FrontierBin *fbptr = reinterpret_cast<FrontierBin *>(frontiers);
    for (int i = 0; i < NumFrontierBins; ++i) {
        auto fname = prefix + fb::toString(i);
        new (fbptr + i) FrontierBin(fname.data(), init);
    }
}

Frontier & Frontier::getFrontier() {
    return *ptr;
}

void Frontier::addUrl(const FrontierUrl &url) {
    FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
    ptr[ (++insertCounter) % NumFrontierBins ].addUrl(url);
}

Vector<SizeT> Frontier::getUrl() const {
    FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
    return ptr[ (++getCounter) % NumFrontierBins ].getUrl();
}
