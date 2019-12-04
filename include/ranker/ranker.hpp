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

//OBSERVATION: multiplication is distributive, so we can sum term frequencies prior
//to dividing by the total word count (per document)

constexpr TOTAL_DOCUMENTS = 1;

struct rank_stats{
	uint64_t unique_doc_id;
	unsigned int total_term_count;
	vector<unsigned int> term_freq;
};

class Ranker{
	bool operator()(pair<uint64_t,uint64_t> &lhs, pair<uint64_t,uint64_t> &rhs){
		return lhs.second < rhs.second;
	}
};

vector<pair<uint64_t, uint64_t>> rank(vector<rank_stats> &documents_to_rank, vector<string> &query, vector<size_t> &doc_freq){
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