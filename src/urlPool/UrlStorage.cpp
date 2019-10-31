//
//  url_pool.cpp
//  local_url_pool
//
//  Created by Jaeyoon Kim on 10/22/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#include "UrlStorage.hpp"
#include <stdlib.h>
#include <iostream>
#include <mutex>

using namespace std;

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
    for (int i = 0; i < NUM_TRY; ++i) {
        rand_num[i] = rand_r(&local_seed);
    }
    
    lock_guard<mutex> to_parse_lock(to_parse_m);
    if (to_parse.size() < NUM_TRY) {
        return {}; // empty url
    }
    
    int max_ranking = 0;
    int max_idx;
    // Find what to sample
    for (int i = 0; i < NUM_SAMPLE; ++i) {
        if (max_ranking < to_parse[rand_num[i] % to_parse.size()].ranking) {
            max_ranking = to_parse[rand_num[i] % to_parse.size()].ranking;
            max_idx = rand_num[i] % to_parse.size();
        }
    }
    
    urls_to_take.insert(max_idx);
    
    for (int i = NUM_SAMPLE; i < NUM_TRY; ++i) {
        if (to_parse[rand_num[i] % to_parse.size()].ranking >= max_ranking) {
            urls_to_take.insert(rand_num[i] % to_parse.size());
        }
    }
    
    // Now take the chosen urls
    // We should keep taking the highest index so that we don't ever copy
    // a already taken url
    vector<Url_t> urls_to_return;
    for (auto it = urls_to_take.rbegin(); it != urls_to_take.rend(); ++it) {
        urls_to_return.push_back(to_parse[*it]);
        to_parse[*it] = to_parse.back();
        to_parse.pop_back();
    }
    
    return urls_to_return;
}

// Saves to given file names. However, urls that were already taken to be parsed might not be saved
void UrlStorage::save(string parsed_file_name, string to_parse_file_name) const {
    // TODO implement!
}

