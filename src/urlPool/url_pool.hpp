//
//  url_pool.hpp
//  local_url_pool
//
//  Created by Jaeyoon Kim on 10/22/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#ifndef url_pool_hpp
#define url_pool_hpp

#include "TestUrl.h"

#include <string>
#include <vector>

class UrlStorage; // Forward declaration

constexpr int NUM_BINS = 16; // This indicates how many seperate UrlStorages there are.

// Interface for adding and getting urls.
class UrlPool {
public:
    UrlPool();
    void add_url(Url_t url);
    Url_t get_url();
private:
    std::hash<std::string> hasher;
    std::vector<Url_t> cached_urls; // Locally cached urls
    UrlStorage* url_storage;
};


#endif /* url_pool_hpp */
