#pragma once

#include <fb/string_view.hpp>

constexpr auto DefaultUrlStoreFile       = "/tmp/crawler/urlstore"_sv;
constexpr auto DefaultAnchorStoreFile    = "/tmp/crawler/anchorstore"_sv;
constexpr auto DefaultAdjStoreFile       = "/tmp/crawler/adjstore"_sv;
constexpr auto DefaultFrontierBinsPrefix = "/tmp/crawler/frontier-bin."_sv;
constexpr auto DefaultLogFile            = "/tmp/crawler/logs"_sv;
constexpr auto DefaultWorkerLogFile      = "/tmp/crawler/worker.logs"_sv;
constexpr auto DefaultUrlInfoTableFile   = "/tmp/crawler/urlinfo"_sv;
constexpr auto DefaultPageStoreFile      = "/tmp/crawler/pagestore"_sv;
