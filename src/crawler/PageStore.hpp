//Created by Chandler Meyers 11/18/19
#pragma once

#include "DiskVec.hpp"

#include "../../lib/vector.hpp"
#include "../../lib/stddef.hpp"
#include "../../lib/utility.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/string.hpp"
#include "../../lib/mutex.hpp"


//TODO: Change this structure to whatever jinsoo provides
//should in theory be just a byte, make this a lot easier if it is
//I'm begging you, let it be a byte, i dont want to do the math if it isnt
using WordDescriptors = char;

struct PageHeader {
    fb::SizeT beginOffset;
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

/*
class PageStore {
public:
    static void init(fb::String prefix, bool init);

    static PageStore & getStore();

    void addPage(fb::StringView page);

    fb::String getPage(fb::SizeT filenum, fb::SizeT offset) const;

private:
    static PageStore *ptr;
    static char frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

    PageStore() = default;
}; */
