//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include "../../lib/vector.hpp"
#include "../../lib/stddef.hpp"

void frontier_add_url(fb::SizeT url_offset, fb::SizeT url_ranking);
fb::Vector<fb::SizeT> frontier_get_url(); // will return -1
