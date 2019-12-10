#ifndef RANKER_HPP
#define RANKER_HPP

#include <iostream>
#include "fb/vector.hpp"
#include "fb/string.hpp"
#include <utility>
#include <cmath>
#include <algorithm>
#include "isr/constraint_solver.hpp"

//OBSERVATION: In calculating the inverse document frequency,
//the total number of documents does not change. Need only calculate once.
//OR: Pick some arbitrary number and stay consistent

//OBSERVATION: across a single query, the document frequency (per word) stays
//constant

//The only thing that we need to return per query per document is the term frequency
//and total term count per document

constexpr size_t TOTAL_DOCUMENTS = 1;

//takes in vector with stirngs of the query
//takes in vector with document frequencies of each of the words in the query 
void tfidf_rank(fb::Vector<rank_stats> &documents_to_rank, fb::Vector<fb::String> &query, fb::Vector<size_t> &doc_freq){
	for(rank_stats document : documents_to_rank){
		double current_rank = 0;
		for(size_t i = 0; i < document.term_freq.size(); ++i){
			current_rank += (document.term_freq[i]/document.total_term_count)*log2(TOTAL_DOCUMENTS/doc_freq[i]);
		}
		document.rank = current_rank;
	}
}

struct snip_window{
	size_t start_word_index;
	size_t end_word_index;
	size_t value_captured;
};

//TODO: incorporate bold, italic, header, etc. into ranking

//positions_weights is a fb::Vector of indices corresponding to the words in the our query
//and their tfidf
snip_window snippet_window_rank(fb::Vector<std::pair<size_t,size_t>> &positions_weights, size_t max_window_size){
	snip_window result;
	if(positions_weights.size() < 2){
		result.start_word_index = positions_weights[0].first;
		result.end_word_index = positions_weights[0].first;
		result.value_captured = positions_weights[0].second;
		return result;
	}
	size_t left = 0;
	size_t right = 0;
	size_t best_left = 0;
	size_t best_right = 0;
	size_t delta = 0;
	size_t current_value = positions_weights[left].second;
	size_t max_value = current_value;
	while(right != positions_weights.size()){
		if(delta <= max_window_size){
			++right;
			delta = positions_weights[right].first - positions_weights[left].first;
			current_value += positions_weights[right].second;
		}else{
			if(current_value > max_value){
				max_value = current_value;
				best_left = left;
				best_right = right;
			}
			current_value -= positions_weights[left].second;
			++left;
			delta = positions_weights[right].first - positions_weights[left].first;
		}
	}
	result.start_word_index = positions_weights[best_left].first;
	result.end_word_index = positions_weights[best_right].first;
	result.value_captured = max_value;
	return result;
}

#endif