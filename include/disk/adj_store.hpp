//Created by Chandler Meyers 11/18/2019

#pragma once

#include <disk/disk_vec.hpp>

#include <fb/string_view.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/vector.hpp>

class AdjStore {
public:
    struct List {
        fb::SizeT begin_;
        fb::SizeT end_;
        const DiskVec<fb::SizeT> &vec_;

        List(fb::SizeT begin, fb::SizeT end, 
             const DiskVec<fb::SizeT> &vec)
            : begin_(begin), end_(end), vec_(vec) {}

        fb::SizeT * begin() {
            return vec_.data() + begin_;
        }

        fb::SizeT * end() {
            return vec_.data() + end_;
        }
    };

    static void init(fb::StringView filename) {
        delete ptr;
        ptr = new AdjStore(filename);
    }

    static AdjStore & getStore() {
        return *ptr;
    }

    // Returns begin of adj list and then end of adj list
    fb::Pair<fb::SizeT, fb::SizeT> addList(fb::Vector<fb::SizeT> in_list) {
        fb::SizeT idx = lists.insert(in_list.begin(), in_list.end());
        return {idx, idx + in_list.size()};
    }

    List getList(fb::SizeT begin, fb::SizeT end) const noexcept {
        return List(begin, end, lists);
    }

    [[nodiscard]] fb::SizeT operator[](fb::SizeT idx) const noexcept {
        return lists[idx];
    }

private:
    AdjStore(fb::StringView filename) : lists(filename) {}

    static AdjStore *ptr;
    DiskVec<fb::SizeT> lists;
};
