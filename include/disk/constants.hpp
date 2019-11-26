#pragma once

#include <fb/string_view.hpp>

#define DefaultRootDir "../crawler_files"
constexpr char* DefaultUrlStoreFile         = DefaultRootDir "/urlstore";
constexpr char* DefaultAnchorStoreFile      = DefaultRootDir "/anchorstore";
constexpr char* DefaultAdjStoreFile         = DefaultRootDir "/adjstore";
constexpr char* DefaultFrontierBinsPrefix   = DefaultRootDir "/frontier-bin.";
constexpr char* DefaultLogFile              = DefaultRootDir "/logs";
constexpr char* DefaultWorkerLogFile        = DefaultRootDir "/worker.logs";
constexpr char* DefaultUrlInfoTableFile     = DefaultRootDir "/urlinfo";
constexpr char* DefaultPageStoreFile        = DefaultRootDir "/pagestore";
constexpr char* DefaultPageStoreCounterFile = DefaultRootDir "/pagestore.counter";
