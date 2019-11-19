//Created by Chandler Meyers 11/18/19
#include "DiskVec.hpp"

#include "../../lib/vector.hpp"
#include "../../lib/stddef.hpp"
#include "../../lib/utility.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/string.hpp"
#include "../../lib/mutex.hpp"
#include <cstring> //strncpy

#include "PageStore.hpp"


PageBin::PageBin(fb::StringView filename, bool init) : PageCount(0), PageCountOffset(0),
                PageHeadersOffset(0), PagesBeginOffset(0), Pages(filename, init) {
    PageCountOffset = Pages.reserve(sizeof(fb::SizeT));
    PageHeadersOffset = Pages.reserve(numPages * 2 * sizeof(fb::SizeT));
    PagesBeginOffset = Pages.size();
}

void PageBin::addPage(fb::SizeT UrlOffset, fb::Pair<fb::StringView, fb::Vector<WordDescriptors>> page){

    //copy the page data in
    auto idx = Pages.reserve(page.first.size() + 1);
    page.first.copy(Pages.data() + idx, page.first.size());
    Pages.data()[idx + page.first.size()] = 0;

    //copy the vector in
    Pages.insert(page.second.begin(), page.second.end());

    //put in the page header
    PageHeader header = { idx, UrlOffset };
    memcpy(Pages.data() + PageHeadersOffset + PageCount * sizeof(PageHeader), &header, sizeof(PageHeader));

    //increment the page counter
    ++PageCount;
    //fb::copy(&PageCount, &PageCount + sizeof(PageCount), Pages.data() + PageCountOffset);
    memcpy(Pages.data() + PageCountOffset, &PageCount, sizeof(PageCount));
}
