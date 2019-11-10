// Created by Jaeyoon Kim 11/7/2019
// Edited by Aniruddh Agarwal 11/9/2019
#pragma once

#include "../../lib/stddef.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"

#include <iostream> // TODO delete
#include <atomic>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

constexpr SizeT MAXFILESIZE = 0x1000000000; // 128 GiB

// This is the class that represents an array saved on disk
// ASSUMES that there won't be more than 128 Gb of data
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
// - Is thread safe.
// - DO NOT USE WITH non-POD TYPES
template <typename T>
class DiskVec {
public:
    DiskVec(fb::StringView fname)  {
        fd = open(fname.data(),
                  O_RDWR | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

        if (fd == -1)
            throw fb::exception("SavedObj: Failed to open file.");

        if (!ftruncate(fd, MAXFILESIZE))
            throw fb::exception("SavedObj: Failed to truncate file.");

        auto ptr = mmap(nullptr, MAXFILESIZE, PROT_WRITE | PROT_READ | PROT_EXEC,
                        MAP_SHARED, fd, 0);

        if (ptr == (void *) -1)
            throw fb::exception("SavedObj: Failed to mmap.");

        cursor = static_cast<std::atomic<fb::SizeT> *>(ptr);
        filePtr = reinterpret_cast<T *>(cursor + 1);
    }

    ~DiskVec() {
        close(fd);
    }

    T & operator[](fb::SizeT idx) {
        return filePtr[idx];
    }

    fb::SizeT reserve(fb::SizeT n) {
        return cursor->fetch_add(n):
    }

private:
    T *filePtr;
    int fd;
    std::atomic<SizeT> *cursor;
};
