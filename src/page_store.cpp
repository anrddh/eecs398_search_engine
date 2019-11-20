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


PageBin::PageBin(fb::StringView filename, bool init) : PageCount(0), PageCountOffset(0),
                PageHeadersOffset(0), PagesBeginOffset(0), Pages(filename, init) {
    PageCountOffset = Pages.reserve(sizeof(fb::SizeT));
    PageHeadersOffset = Pages.reserve(numPages * sizeof(PageHeader));
    PagesBeginOffset = Pages.size();
}

void PageBin::addPage(fb::SizeT UrlOffset, fb::Pair<fb::StringView, fb::Vector<WordDescriptors>> page){

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
}
