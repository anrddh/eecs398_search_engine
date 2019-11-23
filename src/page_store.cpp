//Created by Chandler Meyers 11/18/19

#include <disk/disk_vec.hpp>
#include <disk/page_store.hpp>

#include <fb/vector.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <fb/thread.hpp>
#include <fb/string.hpp>
#include <fb/mutex.hpp>

#include <string.h> //strncpy
#include <iostream>

constexpr fb::SizeT numPages = 500; //TODO: small for testing, raise for real deal. this is the number of pages per file
fb::Mutex QueueMtx;
fb::CV QueueNECV;
std::atomic<fb::SizeT> FileIndex(0);
std::atomic<fb::SizeT> PagesCounter(0);
std::atomic<fb::SizeT> NumThreads(0);
fb::String Prefix;
fb::Queue<Page> PagesToAdd;

PageBin::PageBin(fb::StringView filename, bool init) : PageCount(0), PageCountOffset(0),
                PageHeadersOffset(0), PagesBeginOffset(0), Pages(filename, init) {
    PageCountOffset = Pages.reserve(sizeof(fb::SizeT));
    PageHeadersOffset = Pages.reserve(numPages * sizeof(PageHeader));
    PagesBeginOffset = Pages.size();
}

//returns the end offset
fb::SizeT PageBin::addPage(fb::SizeT UrlOffset, fb::Pair<fb::String, fb::Vector<WordDescriptors>> page){

    //copy the page data in
    auto idx = Pages.reserve(page.first.size() + 1);
    page.first.copy(Pages.data() + idx, page.first.size());
    Pages.data()[idx + page.first.size()] = 0;

    //copy the vector in
    auto idy = Pages.insert(page.second.begin(), page.second.end());

    //put in the page header
    //Note, we add sizeof(std::atomic<fb::SizeT>) to the in-file offsets, due to the cursor at the beginning of the file
    PageHeader header = { idx + sizeof(std::atomic<fb::SizeT>), idy + sizeof(std::atomic<fb::SizeT>), UrlOffset };
    memcpy(Pages.data() + PageHeadersOffset + PageCount * sizeof(PageHeader), &header, sizeof(PageHeader));

    //increment the page counter
    ++PageCount;
    memcpy(Pages.data() + PageCountOffset, &PageCount, sizeof(PageCount));

    return idy + page.second.size();
}

void initializeFileName(fb::String fname){
    Prefix = std::move(fname);
}

void addPage(Page page){
   QueueMtx.lock();
   PagesToAdd.push(std::move(page));
   QueueMtx.unlock();
   ++PagesCounter;
   if(PagesCounter % numPages == 1){
       pthread_t p;
       pthread_create(&p, nullptr, runBin, NULL);
       pthread_detach(p);
   }
   QueueNECV.signal();
}

void * runBin(void *){
    NumThreads.fetch_add(1);
    fb::SizeT Index = FileIndex.fetch_add(1);
    // std::cout << "PrefiX: " << Prefix << std::endl;
    PageBin Bin(Prefix + fb::toString(Index), false);
    fb::SizeT i = 0;
    fb::SizeT EndOffset = 0;
    for( ; i < numPages; ++i){
        QueueMtx.lock();
        while (PagesToAdd.empty())
            QueueNECV.wait(QueueMtx);
        Page P = std::move(PagesToAdd.front());
        PagesToAdd.pop();
        QueueMtx.unlock();
        EndOffset = Bin.addPage(P.first, P.second);
    }
    ftruncate(Bin.file_descriptor(), Bin.size() + 32);
    NumThreads.fetch_sub(1);
    return nullptr;
}
