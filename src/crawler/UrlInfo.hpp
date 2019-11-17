//Structure of UrlInfo
#include "../../lib/stddef.hpp"

struct UrlInfo{
    fb::SizeT UrlOffset;
    fb::SizeT AnchorTestBegin;
    fb::SizeT AnchorTestEnd;
    fb::SizeT AdjListBegin;
    fb::SizeT AdjListEnd;
    int * Page; //TODO: reference to the saved page
};
