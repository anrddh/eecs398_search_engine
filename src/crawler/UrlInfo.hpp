//Structure of UrlInfo
#include "../../lib/stddef.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/algorithm.hpp"

struct UrlInfo{
    fb::SizeT UrlOffset;
    fb::SizeT AnchorTestBegin;
    fb::SizeT AnchorTestEnd;
    fb::SizeT AdjListBegin;
    fb::SizeT AdjListEnd;
    fb::SizeT UrlRank; //TODO: does this need to be here?
    int * Page; //TODO: reference to the saved page
};

inline fb::SizeT RankUrl(fb::StringView Url){
    fb::SizeT rank = 1000;
    if (Url.find(".gov"_sv) > 0) rank *= 4;
    else if (Url.find(".edu"_sv) > 0) rank *= 4;
    else if (Url.find(".net"_sv) > 0) rank *= 3;
    else if (Url.find(".org"_sv) > 0) rank *= 3;
    else if (Url.find(".com"_sv) > 0) rank *= 2;

    fb::SizeT length = Url.size();
    if( length > 50 && rank > 400 ) rank -= 400;
    if( length > 100 && rank > 800 ) rank -= 800;
    if( length > 150 && rank > 1200 ) rank -= 1200;
    if( length > 200 && rank > 1500 ) rank -= 1500;
    if( length > 300 && rank > 2000 ) rank -= 2000;

    return rank;
}
