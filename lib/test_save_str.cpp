// Created by Jaeyoon Kim 11/6/2019
#include "../src/crawler/AnchorStore.hpp"
#include "vector.hpp"
#include <iostream>

//using namespace std;
using namespace fb;

int main() {
   //try {
   AnchorStore::init( "test_file" );
   auto &file = AnchorStore::getStore();

   Vector<Pair<String, Pair<SizeT, SizeT>>> offsets;

   offsets.pushBack( {"Hello world!", file.addStr( "Hello world!"_sv, {0,0} )});
   offsets.pushBack( {"Testing this thing", file.addStr( "Testing this thing"_sv, {0,0})});
   offsets.pushBack( {"", file.addStr( "", {0,0} )});
   offsets.pushBack( {"after empty string", file.addStr( "after empty string", {0,0} )});
   offsets.pushBack( {"Hello world!", file.addStr( "Hello world!", {1,1} )});
   offsets.pushBack( {"Testing this thing", file.addStr( "Testing this thing", {2,2})});
   offsets.pushBack( {"", file.addStr( "", {0,0} )});
   offsets.pushBack( {"after empty string", file.addStr( "after empty string", {3,3} )});
   offsets.pushBack( {"Example: ", file.addStr("Very long example of anchor text string, I hope it to be longer than fifty-six characters, so as to test properly what happens when a string is split into multiple blocks.", {0,0})});

   for ( auto offset : offsets ) {
      //cout << file.get_str( offset ) << endl;
     std::cout << offset.first << " " << offset.second.first << " " << offset.second.second << std::endl;
   }

   //} catch ( Exception& e) {
     // cout << e.msg << endl;
   //}
}
