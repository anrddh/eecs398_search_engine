#pragma once

#include <fb/string_view.hpp>
#include <fb/string.hpp>

#include <stdlib.h>

constexpr auto DefaultRootDir       = "/tmp/crawler-store";
constexpr auto UrlStoreFile         = "/urlstore"_sv;
constexpr auto AnchorStoreFile      = "/anchorstore"_sv;
constexpr auto AdjStoreFile         = "/adjstore"_sv;
constexpr auto FrontierBinsPrefix   = "/frontier-bin."_sv;
constexpr auto MasterLogFile        = "/master.log"_sv;
constexpr auto WorkerLogFile        = "/worker.log"_sv;
constexpr auto UrlInfoTableFile     = "/urlinfo"_sv;
constexpr auto PageStoreFile        = "/pagestore"_sv;
constexpr auto PageStoreCounterFile = "/pagestore.counter"_sv;
constexpr auto PageStoreMergedFile  = "/mergedPS"_sv;
constexpr auto BlockedHostsFile     = "/blockedhosts"_sv;

inline fb::String getRootDir() {
    auto envStr = getenv("CRAWLERDIR");
    return envStr ? envStr : fb::String(DefaultRootDir);
}
