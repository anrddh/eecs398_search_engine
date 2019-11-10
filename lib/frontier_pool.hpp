//  Created by Jaeyoon Kim on 11/10/19.
#pragma once

#include "vector.hpp"
#include "stddef.hpp"

void add_url(fb::SizeT url_offset, fb::SizeT url_ranking);
Vector<fb::SizeT> get_url(); // will return -1
