//
//  UrlStorage.hpp
//  local_url_pool
//
//  Created by Jaeyoon Kim on 10/24/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#ifndef UrlStorage_hpp
#define UrlStorage_hpp

#include "TestUrl.h"
#include <mutex>
#include <map>
#include <set>
#include <unordered_set>
#include <string>

// To get urls, we randomly select NUM_TRY urls.
// From there we see the highest ranking of the first NUM_SAMPLE
// randomly chosen urls. We only return the urls with ranking
// greater or equal that higest ranking.
constexpr int NUM_TRY = 16;
constexpr int NUM_SAMPLE = 4;
static_assert(NUM_SAMPLE < NUM_TRY, "Invalid number of samples!");

// Pool of urls. Keeps track of what has already been parsed or
// needs to be parsed
class UrlStorage {
public:
    UrlStorage();
    void add_url(Url_t url);
    std::vector<Url_t> get_url();
    // Saves to given file names. However, urls that were already taken to be parsed might not be saved
    void save(std::string parsed_file_name, std::string to_parse_file_name) const;
private:
    std::mutex to_parse_m;
    std::vector<Url_t> to_parse;
    std::mutex parsed_m;
    std::unordered_set<Url_t> parsed;
    std::mutex local_seed_m;
    unsigned int local_seed;
};

#endif /* UrlStorage_hpp */
