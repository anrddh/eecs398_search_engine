// Created by Jaeyoon Kim 11/6/2019
#pragma once
//#include "string.hpp"
#include <string> // TODO delete
#include <cstring> //TODO
#define String std::string //TODO
#define strcpy std::strcpy //TODO
#include "Exception.hpp"
#include "SavedObject.hpp"
#include "utility.hpp"

#define ANCHOR_BLOCK_SIZE 64
#define ANCHOR_TEXT_SIZE 56

namespace fb {

// This is the class where one can save list of urls
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
class SavedStrings {
public:
   SavedStrings( String filename ) : disk_array( filename ) {}

   SizeT add_str( String url) {
      // Make sure to increment the cursor before writing so that
      // other threads can work on the array as well
      // We also need to write the null byte
      SizeT str_begin = disk_array.cursor->fetch_add(url.length() + 1);
      strcpy( disk_array.file_ptr + str_begin, url.c_str() );
      return str_begin;
   }

   inline char* get_str(SizeT offset)
      {
      return disk_array.file_ptr + offset;
      }
private:
   SavedObj<char> disk_array;
};

class SavedAnchors {
public:
    SavedAnchors( String filename ) : disk_array( filename ) {
        add_str("dummy text", {0, 0});
    }

    //Recieves a new anchor text and a current begin and end offset for the
    //URL's anchor text. Adds the anchor text and returns the new begin and end.
    //Note: the begin offset should only be different if this is the first time
    //seeing anchor text for this url.
    Pair<SizeT, SizeT> add_str( String anchor_text, Pair<SizeT, SizeT> curOffsets) {
        //Special case: if the anchor text is empty we dont change anything
        if (anchor_text == "") return curOffsets;

        //the working string
        String working_str = anchor_text;

        //the return pair
        Pair<SizeT, SizeT> newOffsets;
        newOffsets.first = curOffsets.first;
        newOffsets.second = curOffsets.second;

        //The loop
        while (working_str != ""){
            SizeT str_begin = disk_array.cursor->fetch_add(ANCHOR_BLOCK_SIZE);
            if (newOffsets.first == 0) {
                newOffsets.first = str_begin;
                newOffsets.second = str_begin;
            }
            else *(SizeT *)(disk_array.file_ptr + newOffsets.second + ANCHOR_TEXT_SIZE) = str_begin; //check that byte count here is correct TODO
            newOffsets.second = str_begin;
            strncpy( disk_array.file_ptr + str_begin, working_str.c_str(), ANCHOR_TEXT_SIZE );
            if (working_str.size() >= ANCHOR_TEXT_SIZE) {
                working_str = working_str.substr(ANCHOR_TEXT_SIZE);
            }
            else working_str = "";
        }
        //Set the final block next offset to null
        *(SizeT *)(disk_array.file_ptr + newOffsets.second + ANCHOR_TEXT_SIZE) = 0;
        return newOffsets;
    }

    //Do we need this? not for now at least probably TODO
    //char *get_str(SizeT offset) {}
private:
    SavedObj<char> disk_array;
};

};
