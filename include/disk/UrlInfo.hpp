// Written by Jaeyoon Kim 11/17/2019
// TODO did Chandler contribute to this?
//Structure of UrlInfo
#include <string.h>
#include <fb/stddef.hpp>
#include <fb/stddef.hpp>
#include <fb/string_view.hpp>
#include <fb/string.hpp>
#include <fb/algorithm.hpp>

using namespace std;

struct UrlInfo {
   // Must gaurentee that the UrlOffset is always valid (otherwise it should be zero)
   std::atomic<fb::SizeT> UrlOffset;
   fb::Pair<fb::SizeT, fb::SizeT> AnchorTextOffsets; // [begin, end)
   fb::Pair<fb::SizeT, fb::SizeT> AdjListOffsets; // [begin, end)
   fb::SizeT AdjListBegin;
   fb::SizeT AdjListEnd;

   // 'u' for unfinished, 'm' for currently modifying, 'p' for finished parsing
   // We are purposely avoiding enums since the value of the enum may differ on systems
   std::atomci<char> state; 
};

inline fb::SizeT RankUrl(fb::StringView Url){
   fb::SizeT rank = 0;

   fb::SizeT start = 0;
   fb::SizeT end = Url.find( "://", 0, 3 );
   fb::StringView Service, Host, Domain;

   if ( end != fb::String::npos )
      {
      Service = Url.substr( start, end - start );
      if ( Service.compare("https") == 0 ) {
         rank += 1000;
      } else if ( Service.compare("http") == 0 ) {
         rank += 500;
      } else {
         // TODO assert false? This shouldn't happen
      }
      start = end + 3;
      }

   end = Url.find( "/", start, 1);
   int begin_TLD; // top level domain (com, org, ...)

   for (int i = end - 1; i >= start; --i) {
      if (Url[i] == '.') {
         begin_TLD = i;
         break;
      }
   }

   Domain = Url.substr(begin_TLD + 1, end - begin_TLD - 1);

   // TODO account for more TLD's
   if ( Domain.compare("gov") == 0 ) {
      rank += 1000;
   } else if ( Domain.compare("edu") == 0 ) {
      rank += 1000;
   } else if ( Domain.compare("org") == 0 ) {
      rank += 800;
   } else if ( Domain.compare("com") == 0 ) {
      rank += 500;
   } else if ( Domain.compare("net") == 0 ) {
      rank += 300;
   } else if ( Domain.compare("biz") == 0 ) {
      rank += 200;
   }

   int add_val = 1000 - 4 * Url.size();
   rank += (add_val > 0) ? add_val : 0;
   return rank;
}