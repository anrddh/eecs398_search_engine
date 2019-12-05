#ifndef RANKER_HPP
#define RANKER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <pair>
#include <cmath>
#include <algorithm>
using namespace std;
//OBSERVATION: In calculating the inverse document frequency,
//the total number of documents does not change. Need only calculate once.
//OR: Pick some arbitrary number and stay consistent

//OBSERVATION: across a single query, the document frequency (per word) stays
//constant

//The only thing that we need to return per query per document is the term frequency
//and total term count per document

constexpr TOTAL_DOCUMENTS = 1;

struct rank_stats{
	uint64_t unique_doc_id; //possibly change to storing chunk index and offset
	unsigned int total_term_count; //number of words in this document
	vector<unsigned int> term_freq; //frequencies of each of the words in the query
};

class Ranker{
	bool operator()(pair<uint64_t,uint64_t> &lhs, pair<uint64_t,uint64_t> &rhs){
		return lhs.second < rhs.second;
	}
};

//takes in vector with strings of the query
//takes in vector with document frequencies of each of the words in the query 
vector<pair<uint64_t, uint64_t>> tfidf_rank(vector<rank_stats> &documents_to_rank, vector<string> &query, vector<size_t> &doc_freq){
	vector<pair<uint64_t,uint64_t>> result;
	result.reserve(documents_to_rank.size());
	for(rank_stats document : documents_to_rank){
		uint64_t current_doc_id = document.unique_doc_id;
		uint64_t current_rank = 0;
		for(int i = 0; i < document.term_freq.size(); ++i){
			current_rank += (document.term_freq[i]/total_term_count)*log2(TOTAL_DOCUMENTS/doc_freq[i]);
		}
		result.push_back(pair<uint64_t,uint64_t> (current_doc_id, current_rank));
	}
	return sort(result.begin(), result.end(), Ranker compare());
}

struct snip_window{
	size_t start_word_index;
	size_t end_word_index;
	size_t value_captured;
};

//TODO: incorporate bold, italic, header, etc. into ranking

//positions_weights is a vector of indices corresponding to the words in the our query
//and their tfidf
snip_window snippet_window_rank(vector<pair<size_t,size_t>> &positions_weights, size_t max_window_size){
	snip_window result;
	if(positions_weights.size() < 2){
		snip_window.start_word_index = positions_weights[0].first;
		snip_window.end_word_index = positions_weights[0].first;
		snip_window.value_captured = positions_weights[0].second;
		return snip_window
	}
	size_t left = 0;
	size_t right = 0;
	size_t best_left = 0;
	size_t best_right = 0;
	size_t delta = 0;
	size_t current_value = positions[left]->second;
	size_t max_value = current_value;
	while(right != positions_weights.size()){
		if(delta <= max_window_size){
			++right;
			delta = positions_weights[right]->first - positions_weights[left]->first;
			current_value += positions_weights[right]->second;
		}else{
			if(current_value > max_value){
				max_value = current_value;
				best_left = left;
				best_right = right;
			}
			current_value -= positions_weights[left]->second;
			++left;
			delta = positions_weights[right]->first - positions_weights[left]->first;
		}
	}
	result.start_word_index = positions_weights[best_left]->first;
	result.end_word_index = positions_weights[best_right]->first;
	result.value_captured = max_value;
	return result;
}

#endif