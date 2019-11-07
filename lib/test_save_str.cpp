// Created by Jaeyoon Kim 11/6/2019
#include "SavedStrings.hpp"
#include <vector>
#include <iostream>

using namespace std;
using namespace fb;

int main() {
   try {
   SavedStrings file( "test_file" );

   vector<SizeT> offsets;

   offsets.push_back( file.add_str( "Hello world!" ));
   offsets.push_back( file.add_str( "Testing this thing"));
   offsets.push_back( file.add_str( "" ));
   offsets.push_back( file.add_str( "after empty string" ));

   for ( auto offset : offsets ) {
      cout << file.get_str( offset ) << endl;
   }
   
   } catch ( Exception& e) {
      cout << e.msg << endl;
   }
}
