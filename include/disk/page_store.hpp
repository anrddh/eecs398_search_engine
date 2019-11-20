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


//TODO: Change this structure to whatever jinsoo provides
//should in theory be just a byte, make this a lot easier if it is
//I'm begging you, let it be a byte, i dont want to do the math if it isnt
using WordDescriptors = char;

struct PageHeader {
    fb::SizeT beginOffset;
    fb::SizeT VecOffset;
    fb::SizeT UrlOffset;
};

class PageBin {
public:
    PageBin(fb::StringView filename, bool init);

    void addPage(fb::SizeT UrlOffset, fb::Pair<fb::StringView, fb::Vector<WordDescriptors>> page);

    fb::Pair<fb::String, fb::Vector<WordDescriptors>> getPage(fb::SizeT offset);

private:
    static constexpr fb::SizeT numPages = 5; //TODO: small for testing, raise for real deal
    fb::SizeT PageCount ;
    fb::SizeT PageCountOffset;
    fb::SizeT PageHeadersOffset;
    fb::SizeT PagesBeginOffset;
    DiskVec<char> Pages;
};

class PageStore {
    using Page = fb::Pair<fb::SizeT UrlOffset, fb::Pair<fb::StringView, fb::Vector<WordDescriptors>>>;
public:

    PageStore(const fb::String& prefix) : Prefix(prefix) {}

    void addPage(Page page){
        QueueMtx.lock();
        PagesToAdd.push(std::move(page));
        QueueMtx.unlock();
        ++PagesCounter;
        if(PagesCounter % numPages == 1){
            pthread_create (nullptr, NULL, runBin, NULL);
        }
        QueueNECV.signal();
    }

    int runBin(){
        PageBin Bin(Prefix + fb::toString(FileIndex), false);
        ++FileIndex;
        for(fb::SizeT i = 0; i < numPages; ++i){
            QueueMtx.lock();
            while (PagesToAdd.empty())
                QueueNECV.wait(QueueMtx);
            Page P = std::move(PagesToAdd.front());
            PagesToAdd.pop();
            QueueMtx.unlock();
            Bin.addPage(Page.first, Page.second);
        }
    }

private:
    static constexpr fb::SizeT numPages = 5; //TODO: small for testing, raise for real deal. this is the number of pages per file
    fb::Mutex QueueMtx;
    fb::CV QueueNECV;
    std::atomic<fb::SizeT> FileIndex;
    std::atomic<fb::SizeT> PagesCounter;
    const fb::String Prefix;
    fb::queue<Page> PagesToAdd;
};
