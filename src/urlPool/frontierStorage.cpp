//
//  url_pool.cpp
//  local_url_pool
//
//  Created by Jaeyoon Kim on 10/22/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#include "frontierStorage.hpp"
#include <stdlib.h>
#include <iostream>
#include <mutex>

using namespace std;

// Each UrlStorage object is given a random seed
mutex rand_mtx;
unsigned int seed = 0;

UrlStorage::UrlStorage() {
    rand_mtx.lock();
    local_seed = ++seed;
    rand_mtx.unlock();
    
}

void UrlStorage::add_url(Url_t url) {
    {
        lock_guard<mutex> parsed_lock(parsed_m);
        if (!parsed.insert(url).second) {
            return; // We already handled this url
        }
    }
    
    to_parse_m.lock();
    to_parse.push_back(url);
    to_parse_m.unlock();
}

vector<Url_t> UrlStorage::get_url() {
    int rand_num[NUM_TRY];
    std::set<int> urls_to_take;
    local_seed_m.lock();
    for (int i = 0; i < NUM_TRY; ++i) {
        rand_num[i] = rand_r(&local_seed);
    }
    local_seed_m.unlock();
    
    vector<Url_t> urls_to_return;
    int max_ranking = 0; // Requires that any ranking of urls to be greater than 0
    int max_idx;

    to_parse_m.lock();
    if (to_parse.size() < NUM_TRY) {
        return {}; // empty url
    }
    
    // Find what to sample
    // Compute the highest ranking amongst first NUM_SAMPLE randomly picked urls
    for (int i = 0; i < NUM_SAMPLE; ++i) {
        if (max_ranking < to_parse[rand_num[i] % to_parse.size()].ranking) {
            max_ranking = to_parse[rand_num[i] % to_parse.size()].ranking;
            max_idx = rand_num[i] % to_parse.size();
        }
    }

    urls_to_return.push_back( move(to_parse[max_idx]) );
    to_parse[ max_idx ] = move(to_parse.back());
    to_parse.pop_back();
    
    // We randomly check urls
    // If their ranking is greater than or equal to max_ranking,
    // then we will take them to be parsed
    // Note that it is possible that same url might be checked multiple times
    // However, this is not likely since there should be many urls in here each time
    for ( int i = NUM_SAMPLE; i < NUM_TRY; ++i ) {
        if (to_parse[rand_num[i] % to_parse.size()].ranking >= max_ranking) {
            urls_to_return.push_back( move( to_parse[rand_num[i] % to_parse.size()] ) );
            to_parse[rand_num[i] % to_parse.size()] = move( to_parse.back() );
            to_parse.pop_back();
        }
    }

    to_parse_m.unlock();
    
    return urls_to_return;
}

// Saves to given file names. However, urls that were already taken to be parsed might not be saved
void UrlStorage::save(string parsed_file_name, string to_parse_file_name) const {
    // TODO implement!
}

