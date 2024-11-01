//Created by Chandler Meyers 11/18/19
#pragma once

#include <disk/disk_vec.hpp>

#include <fb/vector.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/thread.hpp>
#include <fb/string.hpp>
#include <fb/mutex.hpp>
#include <fb/queue.hpp>
#include <fb/cv.hpp>
#include <pthread.h>

constexpr fb::SizeT numPages = 100000; //TODO: small for testing, raise for real deal
extern std::atomic<fb::SizeT> NumThreads;

// Number of pages parsed in this process
fb::SizeT get_num_parsed();

//singleton class for pagestore
class PageStoreCounter{
public:
    static void init(fb::StringView filename) {
        delete ptr;
        ptr = new PageStoreCounter(filename);
    }

    static PageStoreCounter & getCounter() {
        return *ptr;
    }

    fb::SizeT index() {
        return PageStoreCounterFile.reserve(1);
    }

    fb::SizeT getCount() {
        return PageStoreCounterFile.reserve(0);
    }

private:
    PageStoreCounter(fb::StringView filename) : PageStoreCounterFile(filename) {}

    static PageStoreCounter *ptr;
    DiskVec<char> PageStoreCounterFile;
};

//TODO: Change this structure to whatever jinsoo provides
//should in theory be just a byte, make this a lot easier if it is
//I'm begging you, let it be a byte, i dont want to do the math if it isnt
using WordDescriptors = uint8_t;

// Written by Jaeyoon
struct Page {
   fb::SizeT UrlOffset;
   fb::String page_str;
   fb::Vector<WordDescriptors> word_headers;
};

void page_store_shutdown();

struct PageHeader {
    fb::SizeT beginOffset;
    fb::SizeT VecOffset;
    fb::SizeT UrlOffset;
};

class PageBin {
public:
    struct Error : fb::Exception {};

    PageBin(fb::StringView filename, fb::SizeT number_of_pages  = numPages);

    fb::SizeT addPage(Page&& p);

    fb::Pair<fb::String, fb::Vector<WordDescriptors>> getPage(fb::SizeT offset);

    int file_descriptor() const {
        return Pages.file_descriptor();
    }

    fb::SizeT num_stored_pages() const {
        return *(fb::SizeT *)Pages.data();
    }

    char * data() const {
        return Pages.data();
    }

    fb::SizeT size(){
        return Pages.size();
    }

private:
    fb::SizeT PageCount ;
    fb::SizeT PageCountOffset;
    fb::SizeT PageHeadersOffset;
    fb::SizeT PagesBeginOffset;
    DiskVec<char> Pages;
};

void initializeFileName(fb::String fname);

void addPage(Page&& page);

void * runBin(void *);
