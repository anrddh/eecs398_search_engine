//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include "DiskVec.hpp"

#include "../../lib/vector.hpp"
#include "../../lib/stddef.hpp"
#include "../../lib/utility.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/string.hpp"
#include "../../lib/mutex.hpp"

constexpr fb::SizeT NumFrontierBins = 16;

struct FrontierUrl {
    fb::SizeT offset;
    fb::SizeT ranking;
};

class FrontierBin {
public:
    FrontierBin(fb::StringView filename);

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
    static void init(fb::String prefix);

    static Frontier & getFrontier();

    void addUrl(const FrontierUrl &url);

    fb::Vector<fb::SizeT> getUrl() const;

private:
    static Frontier *ptr;
    static char frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

    Frontier() = default;
};
