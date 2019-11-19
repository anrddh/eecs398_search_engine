//Created by Chandler Meyers 11/18/2019

#pragma once

#include <disk/disk_vec.hpp>

#include <fb/string_view.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/vector.hpp>

class AdjStore {
public:
    static void init(fb::StringView filename, bool init) {
        delete ptr;
        ptr = new AdjStore(filename, init);
    }

    static AdjStore & getStore() {
        return *ptr;
    }

    fb::Pair<fb::SizeT, fb::SizeT> addList(fb::Vector<fb::SizeT> in_list) {
        fb::SizeT idx = lists.insert(in_list.begin(), in_list.end());
        return {idx, idx + in_list.size()};
    }

    fb::Vector<fb::SizeT> getList(fb::SizeT begin, fb::SizeT end) const noexcept {
        fb::Vector<fb::SizeT> list;
        for (fb::SizeT idx = begin; idx < end; ++idx){
            list.pushBack(lists[idx]);
        }
        return list;
    }

private:
    AdjStore(fb::StringView filename, bool init) : lists(filename, init) {}

    static AdjStore *ptr;
    DiskVec<fb::SizeT> lists;
};
