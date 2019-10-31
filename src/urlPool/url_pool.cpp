//
//  url_pool.cpp
//  local_url_pool
//
//  Created by Jaeyoon Kim on 10/22/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#include "url_pool.hpp"
#include "UrlStorage.hpp"
#include <mutex>
#include <iostream>

using namespace std;

UrlStorage storage[NUM_BINS];
mutex counter_m;
int counter = 0;

UrlPool::UrlPool() {
    counter_m.lock();
    url_storage = &storage[counter++ % NUM_BINS];
    counter_m.unlock();
}

Url_t UrlPool::get_url() {
    while (cached_urls.empty()) {
        cached_urls = url_storage->get_url(); // copy elision
    }
    
    Url_t output = move(cached_urls.back());
    cached_urls.pop_back();
    return output;
}

void UrlPool::add_url(Url_t url) {
    storage[hasher(url.url) % NUM_BINS].add_url(url);
}
