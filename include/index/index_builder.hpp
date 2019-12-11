#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <fb/string.hpp>
#include <fb/memory.hpp>
#include <fb/functional.hpp>
#include <fb/mutex.hpp>
#include <fb/thread.hpp>
#include <fb/unordered_set.hpp>

#include <index/index_chunk_builder.hpp>
#include <index/index_helpers.hpp>
#include <index/index_data_structures.hpp>

#include "porter_stemmer.hpp"

#include <debug.hpp>
#include <disk/logfile.hpp>

#include <fb/no_delete_unordered_map.hpp>

/*
 * inline char* add_num( char* curr, size_t num, uint8_t header = 0 )
 */

class IndexBuilder {
public:
    // root must contain a trailing '/'
    IndexBuilder(fb::String path)
        : root(path), tokenCount(1), porterStemmer(create_stemmer()) {}

    ~IndexBuilder() { free_stemmer(porterStemmer); }

    void build_chunk(uint64_t* start_of_file, int chunk) {
        // move past first 16 bytes
        // the page headers end where the first page starts
        uint64_t* current_doc_offset = start_of_file + 2;
        uint64_t* current_des_offset = start_of_file + 3;
        unsigned int num_pages = start_of_file[1];
        uint64_t* end_page_headers = start_of_file + 2 + (num_pages * 3);
        uint64_t doc_num = 0;
        uint8_t * start = (uint8_t *) start_of_file;
        while(current_doc_offset != end_page_headers){
            build_single_doc(start + *current_doc_offset, start + *current_des_offset, doc_num);
            current_doc_offset += 3;
            current_des_offset += 3;
            ++doc_num;
            if (!(doc_num % 5000))
                log(logfile, "Built ", doc_num, " docs\n");
        }
        flushToDisk(chunk);
        log(logfile, "Flushed to disk\n");
    }
private:
    // reads a single space terminated word
    // returns pointer to the beginning of the next word
    // increments tokenCount
    char* read_word(char* &word_begin, fb::String &word) {
       word.clear();
       while (*word_begin && *word_begin != ' ')
           word += tolower(*word_begin++);

       // check to see if we are at the end of a document
       if(!*word_begin)
           return word_begin;

       // otherwise move past the space and return the start
       // of the next word
       ++word_begin;

       // returns the last position of the resulting word
       word.resize(stem(porterStemmer, word.data(), word.size() - 1) + 1);
       return word_begin;
    }

    fb::UnorderedSet<fb::String> unique_words;

    void build_single_doc(uint8_t* doc_start,
                          uint8_t* des_start,
                          uint64_t docId) {
        unique_words.clear();
        fb::String word;
        uint8_t word_info;
        char* current_word = (char *)doc_start;
        uint8_t* current_des = des_start;

        while (*current_word) {
            current_word = read_word(current_word, word);
            word_info = *current_des;
            AbsoluteWordInfo absWord = {tokenCount, word_info};
            ++tokenCount;

            // wordPositions[word].pushBack(absWord);

            auto iter = wordCountsPositions.insert( word, {} );
            iter->second.pushBack( absWord );

            if(unique_words.insert(iter.key()))
                ++(iter->first);
        }

        // increment for EOD
        DocIdInfo doc_info = {tokenCount++, docId};
        documents.pushBack(doc_info);
    }

    void flushToDisk(int chunk) {
        fb::String filename(root + "Index" + fb::toString(chunk));
        // change this to use atomics

        IndexChunkBuilder<fb::Hash<fb::String>>
            indexChunkBuilder(filename,
                              wordCountsPositions.bucket_count(),
                              documents,
                              tokenCount);

        for (auto iter = wordCountsPositions.begin();
             iter != wordCountsPositions.end();
             ++iter)
            indexChunkBuilder.addWord(iter.key(), iter->second, iter->first);

        tokenCount = 1;
    }

    // folder for each index chunk, store root directory
    fb::String root;

    // the actual map that stores the positions of the words, is a
    // unique pointer so that we can pass this ownership to a thread.
    fb::NoDeleteUnorderedMap<fb::String,
                             fb::Pair<unsigned int,
                                      fb::Vector<AbsoluteWordInfo>>>
    wordCountsPositions;

  	fb::Mutex wordPositionsLock;
    fb::Vector<DocIdInfo> documents;

    // each chunk keeps track of its own word count
    unsigned int tokenCount;

    stemmer * porterStemmer;
};
