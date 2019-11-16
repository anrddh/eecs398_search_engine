//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include "../../lib/vector.hpp"
#include "../../lib/stddef.hpp"
#include "../../lib/utility.hpp"

struct FrontierUrl {
    fb::SizeT offset;
    fb::SizeT ranking;
};

void frontierAddUrl(FrontierUrl url);
fb::Vector<fb::SizeT> frontierGetUrls(); // will return -1
