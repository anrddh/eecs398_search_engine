// Created by Jaeyoon Kim 11/6/2019
// Generalized into templates by Jaeyoon Kim 11/7/2019
#pragma once

#include "../../lib/stddef.hpp"
#include "../../lib/exception.hpp"
#include "../../lib/vector.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/view.hpp"

#include "DiskVec.hpp"

#include <atomic>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


// This is the class where one can save a vector into a contiguous region of memory
// (It uses copy constructor thus it works best with small basic types)
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
template <typename T>
class SavedLists {
public:
    SavedLists( fb::StringView filename ) : disk_array( filename ) {}

    template <fb::SizeT S>
    SizeT add_list( const fb::View<T,S>& adjList ) {
        // obtain the old previous value and increment it
        SizeT offset = diskArray.cursor->fetch_add( adj_list.size() );
        SizeT* ptr = disk_array.file_ptr + offset;
        for (SizeT i = 0; i < adj_list.size(); ++i) {
            ptr[i] = adj_list[i];
        }
        return offset;
    }

    [[nodiscard]] constexpr T * operator[](fb::SizeT idx) noexcept {
    }

    inline T* get_list(SizeT offset) {
        return diskArray.file_ptr + offset;
    }

private:
   DiskVec<T> diskArray;
};
