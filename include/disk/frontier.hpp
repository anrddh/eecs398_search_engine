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

struct FrontierUrl {
    fb::SizeT offset;
    fb::SizeT ranking;
};

class FrontierBin {
public:
    FrontierBin(fb::StringView filename, bool init);

    void addUrl(const FrontierUrl &url);

    fb::Vector<fb::SizeT> getUrl( );

private:
    fb::Mutex localSeedM;
    fb::Mutex toParseM;
    unsigned int localSeed;
    unsigned int idx;
    DiskVec<FrontierUrl> toParse;
};

class Frontier {
public:
    static void init(fb::String prefix, bool init);

    static Frontier & getFrontier();

    void addUrl(const FrontierUrl &url);

    fb::Vector<fb::SizeT> getUrl() const;

private:
    static Frontier *ptr;
    static char frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

    Frontier() = default;
};
