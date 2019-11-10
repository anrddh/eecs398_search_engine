// Created by Jaeyoon Kim 11/6/2019
#include "SavedStrings.hpp"
#include "vector.hpp"
#include <iostream>

using namespace std;
using namespace fb;

int main() {
   try {
   SavedAnchors file( "test_file" );

   Vector<Pair<String, Pair<SizeT, SizeT>>> offsets;

   offsets.push_back( {"Hello world!", file.add_str( "Hello world!", {0,0} )});
   offsets.push_back( {"Testing this thing", file.add_str( "Testing this thing", {0,0})});
   offsets.push_back( {"", file.add_str( "", {0,0} )});
   offsets.push_back( {"after empty string", file.add_str( "after empty string", {0,0} )});
   offsets.push_back( {"Hello world!", file.add_str( "Hello world!", {64,64} )});
   offsets.push_back( {"Testing this thing", file.add_str( "Testing this thing", {128,128})});
   offsets.push_back( {"", file.add_str( "", {0,0} )});
   offsets.push_back( {"after empty string", file.add_str( "after empty string", {192,192} )});
   offsets.push_back( {"Example: ", file.add_str("Very long example of anchor text string, I hope it to be longer than fifty-six characters, so as to test properly what happens when a string is split into multiple blocks.", {0,0})});

   for ( auto offset : offsets ) {
      //cout << file.get_str( offset ) << endl;
      cout << offset.first << " " << offset.second.first << " " << offset.second.second << endl;
   }

   } catch ( Exception& e) {
      cout << e.msg << endl;
   }
}
