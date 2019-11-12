// Created by Jaeyoon Kim 11/6/2019
// Edited by Aniruddh Agarwal 11/10/2019

#pragma once

#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/exception.hpp"
#include "../../lib/utility.hpp"

#include "DiskVec.hpp"

// This is the class where one can save list of urls
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.

// TODO: make UrlStore a singleton

class UrlStore {
public:
    static void init(fb::StringView filename) {
        delete ptr;
        ptr = new UrlStore(filename);
    }

    UrlStore & getStore() {
        return *ptr;
    }

    constexpr fb::SizeT addUrl(fb::StringView url) {
        auto idx = urls.reserve(url.size() + 1);
        url.copy(urls.data() + idx, url.size());
        urls.data()[idx + url.size()] = 0;
        return idx;
    }

    constexpr fb::StringView getUrl(fb::SizeT idx) const noexcept {
        return { urls.data() + idx };
    }

private:
    constexpr UrlStore(fb::StringView filename) : urls(filename) {}

    static UrlStore *ptr;
    DiskVec<char> urls;
};
