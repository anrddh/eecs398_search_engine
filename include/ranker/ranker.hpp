#ifndef RANKER_HPP
#define RANKER_HPP

#include <iostream>
#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <cmath>
#include <isr/constraint_solver.hpp>
#include <parser/parser.hpp>

//OBSERVATION: In calculating the inverse document frequency,
//the total number of documents does not change. Need only calculate once.
//OR: Pick some arbitrary number and stay consistent

//OBSERVATION: across a single query, the document frequency (per word) stays
//constant

//The only thing that we need to return per query per document is the term frequency
//and total term count per document

//TODO: fill this in
constexpr double TOTAL_DOCUMENTS;

//takes in vector with stirngs of the query
//takes in vector with document frequencies of each of the words in the query
void tfidf_rank(fb::Vector<rank_stats> &documents_to_rank, const fb::Vector<fb::SizeT> &doc_freq);

struct snip_window{
	fb::SizeT start_word_index;
	fb::SizeT end_word_index;
	double value_captured;
};

struct SnippetStats {
    //since we currently think each worker computer will only have one merged
    //pagestore file, we can probably later make this static
    fb::String FileName; //the corresponding pagestore file
    fb::SizeT DocIndex; //gives which number document in that pagestore file
    snip_window Offsets; //gives the begin and end word number within that document
};

//TODO: incorporate bold, italic, header, etc. into ranking

//positions_weights is a fb::Vector of indices corresponding to the words in the our query
//and their tfidf
//im getting rid of weights for now
snip_window snippet_window_rank(fb::Vector<fb::SizeT> &positions_weights, fb::SizeT max_window_size);

fb::Pair<fb::String, fb::String> GenerateSnippetsAndTitle( SnippetStats &Stats, fb::Vector<rank_stats> &documents_to_rank );

#endif