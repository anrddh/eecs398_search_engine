// Created by Jaeyoon Kim 11/6/2019
#include "saved_urls.hpp"
#include <vector>
#include <iostream>

using namespace std;
using namespace fb;

int main() {
   try {
   SavedUrls file("test_file");

   vector<SizeT> offsets;

   offsets.push_back( file.add_url( "Hello world!" ));
   offsets.push_back( file.add_url( "Testing this thing"));
   offsets.push_back( file.add_url( "" ));
   offsets.push_back( file.add_url( "after empty string" ));

   for ( auto offset : offsets ) {
      cout << file.get_url( offset ) << endl;
   }
   


   } catch ( Exception& e) {
      cout << e.msg << endl;
   }
}
