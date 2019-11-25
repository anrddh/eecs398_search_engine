//Created by Chandler Meyers 11/18/19

#include <disk/disk_vec.hpp>
#include <disk/page_store.hpp>

#include <fb/vector.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/thread.hpp>
#include <fb/string.hpp>
#include <fb/mutex.hpp>

#include <iostream>

#include <string.h>
#include <errno.h>

// TODO: small for testing, raise for real deal. this is the number of
// pages per file


fb::Mutex QueueMtx;
fb::CV QueueNECV;
//std::atomic<fb::SizeT> FileIndex(0);
std::atomic<fb::SizeT> PagesCounter(0);

fb::Mutex numThreadsMtx;
fb::CV numThreadsCV;
std::atomic<fb::SizeT> NumThreads(0);

DiskVec<char> PageStoreCounter("/tmp/crawler/page_store_counter.bin"); //note: this is global so be aware of initialization order
fb::String Prefix;
fb::Queue<Page> PagesToAdd;
std::atomic<bool> need_to_shutdown = false;

void page_store_shutdown() {
   need_to_shutdown = true;
   QueueNECV.broadcast();
   numThreadsMtx.lock();
   while (NumThreads != 0) {
      numThreadsCV.wait(numThreadsMtx);
   }
   numThreadsMtx.unlock();
}

PageBin::PageBin(fb::StringView filename) : PageCount(0), PageCountOffset(0),
                PageHeadersOffset(0), PagesBeginOffset(0), Pages(filename) {
    PageCountOffset = Pages.reserve(sizeof(fb::SizeT));
    PageHeadersOffset = Pages.reserve(numPages * sizeof(PageHeader));
    PagesBeginOffset = Pages.size();
}

//returns the end offset
fb::SizeT PageBin::addPage(Page&& p) {
    //copy the page data in
    auto idx = Pages.reserve(p.page_str.size() + 1);
    p.page_str.copy(Pages.data() + idx, p.page_str.size());
    Pages.data()[idx + p.page_str.size()] = 0;

    //copy the vector in
    auto idy = Pages.insert(p.word_headers.begin(), p.word_headers.end());

    //put in the page header Note, we add
    //sizeof(std::atomic<fb::SizeT>) to the in-file offsets, due to
    //the cursor at the beginning of the file
    PageHeader header = { idx + sizeof(std::atomic<fb::SizeT>),
        idy + sizeof(std::atomic<fb::SizeT>), p.UrlOffset };
    memcpy(Pages.data() + PageHeadersOffset + PageCount * sizeof(PageHeader),
           &header, sizeof(PageHeader));

    //increment the page counter
    ++PageCount;
    memcpy(Pages.data() + PageCountOffset, &PageCount, sizeof(PageCount));

    return idy + p.word_headers.size();
}

void initializeFileName(fb::String fname){
    Prefix = std::move(fname);
}

void addPage(Page&& page){
   QueueMtx.lock();
   PagesToAdd.push(std::move(page));
   QueueMtx.unlock();
   ++PagesCounter;
   if(PagesCounter % numPages == 1){
       pthread_t p;
       NumThreads.fetch_add(1);
       pthread_create(&p, nullptr, runBin, NULL);
       pthread_detach(p);
   }
   QueueNECV.signal();
}

void * runBin(void *){
    fb::SizeT Index = PageStoreCounter.reserve(1);

    PageBin Bin(Prefix + fb::toString(Index));
    fb::SizeT i = 0;
    for( ; i < numPages; ++i){
        QueueMtx.lock();
        while (PagesToAdd.empty()) {
           if (need_to_shutdown) {
              QueueMtx.unlock();
              goto exit_point;
           }
           QueueNECV.wait(QueueMtx);
        }
        Page P = std::move(PagesToAdd.front());
        PagesToAdd.pop();
        QueueMtx.unlock();
        Bin.addPage(std::move( P ));
    }

exit_point:

    if (ftruncate(Bin.file_descriptor(), Bin.size() + 32)) {
        fb::String err = fb::String("Failed to truncate file: ") +
            fb::String(strerror(errno));
        throw fb::Exception(err);
    }

    if (--NumThreads == 0) {
      numThreadsCV.signal();
    }
    return nullptr;
}
