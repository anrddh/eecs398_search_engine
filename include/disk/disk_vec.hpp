// Created by Jaeyoon Kim 11/7/2019
// Edited by Aniruddh Agarwal 11/9/2019
#pragma once

#include <fb/stddef.hpp>
#include <fb/exception.hpp>
#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/algorithm.hpp>

#include <iostream> // TODO delete
#include <atomic>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

constexpr fb::SizeT MAXFILESIZE = 0x1000000000; // 128 GiB

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
    DiskVec(fb::StringView fname, bool init = false)  {
        fb::FileDesc fd = open(fname.data(),
                               O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

        if (ftruncate(fd, MAXFILESIZE))
            throw fb::Exception("SavedObj: Failed to truncate file.");

        auto ptr = mmap(nullptr, MAXFILESIZE, PROT_WRITE | PROT_READ | PROT_EXEC,
                        MAP_SHARED, fd, 0);

        if (ptr == (void *) -1)
            throw fb::Exception("SavedObj: Failed to mmap.");

        if (init)
            cursor = new (ptr) std::atomic<fb::SizeT>(0);
        else
            cursor = static_cast<std::atomic<fb::SizeT> *>(ptr);
        filePtr = reinterpret_cast<T *>(cursor + 1);
    }

    ~DiskVec() noexcept {
        munmap(static_cast<void *>(cursor), MAXFILESIZE);
    }

    [[nodiscard]] constexpr T * data() const noexcept {
        return filePtr;
    }

    [[nodiscard]] T & operator[](fb::SizeT idx) const noexcept {
        return data()[idx];
    }

    [[nodiscard]] constexpr fb::SizeT size() const noexcept {
        return cursor->load();
    }

    [[nodiscard]] constexpr fb::SizeT empty() const noexcept {
        return !size();
    }

    fb::SizeT reserve(fb::SizeT n) noexcept {
        return cursor->fetch_add(n);
    }

    template <typename It>
    fb::SizeT insert(It begin, It end) {
        auto firstIdx = reserve(fb::distance(begin,end));
        fb::copy(begin, end, data() + firstIdx);
        return firstIdx;
    }

    fb::SizeT pushBack(const T &elt) {
        return insert(&elt, &elt + 1);
    }

    // WARNING popBack might introduce race conditions
    // Only use popBack when you locked this object
    void popBack() {
        --cursor;
    }

    T * begin() {
        return filePtr;
    }

    T * end() {
        return filePtr + *cursor;
    }

    T & front() {
        return *begin();
    }

    T & back() {
        return *(end() - 1);
    }

private:
    T *filePtr;
    std::atomic<fb::SizeT> *cursor;
};
