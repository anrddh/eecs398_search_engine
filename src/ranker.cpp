#include <iostream>
#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <cmath>
#include <algorithm> //std::sort
#include <isr/constraint_solver.hpp>
#include <parse/parser.hpp>
#include <ranker/ranker.hpp>

double TOTAL_DOCUMENTS = 10000;

void tfidf_rank(fb::Vector<rank_stats> &documents_to_rank, const fb::Vector<fb::SizeT> &doc_freq){
	for(rank_stats &document : documents_to_rank){
		double current_rank = 0;
		for(size_t i = 0; i < document.occurrences.size(); ++i){
			current_rank += (double(document.occurrences[i].size( ))/double(document.total_term_count))*log2(TOTAL_DOCUMENTS/double(doc_freq[i]));
		}
		document.rank = current_rank;
	}
}

// Merges all the occurence vectors together for snippet rank
// Ditching the weights for now for time's sake
// Also just using std::sort right now, hopefully not a bottleneck
fb::Vector<fb::SizeT> MergeVectors(const fb::Vector<fb::Vector<uint32_t>>& occurrences){
    fb::Vector<fb::SizeT> merged;
    for (auto& v : occurrences){
        for (auto slot : v){
            merged.pushBack(slot - 1);
        }
    }
    std::sort(merged.begin(), merged.end());
    return merged;
}

//positions_weights is a fb::Vector of indices corresponding to the words in the our query
//and their tfidf
//im getting rid of weights for now
snip_window snippet_window_rank(const fb::Vector<fb::SizeT> &positions_weights, const fb::SizeT max_window_size){
	snip_window result;
    std::cout << "VECTOR BEGIN! " << std::endl;
    for (auto i : positions_weights) std::cout << i << " " << std::endl;
	// if(positions_weights.size() < 2){
	// 	result.start_word_index = positions_weights[0];
	// 	result.end_word_index = positions_weights[0] + 1;
	// 	result.value_captured = 1;
	// 	return result;
	// }
	fb::SizeT left = 0;
	fb::SizeT right = 0;
	fb::SizeT best_left = 0;
	fb::SizeT best_right = 0;
	fb::SizeT delta = 0;
    fb::SizeT best_delta = 0;
	// double current_value = positions_weights[left].second;
    double current_value = 0;
	double max_value = current_value;
	while(right < positions_weights.size()){
		delta = positions_weights[right] - positions_weights[left];
		current_value += 1;
		while(delta > max_window_size){
			current_value -= 1;
			++left;
			delta = positions_weights[right] - positions_weights[left];
		}

		if(current_value >= max_value && delta > best_delta){
			max_value = current_value;
			best_left = left;
			best_right = right;
            best_delta = delta;
		}
        ++right;
	}
	result.start_word_index = positions_weights[best_left] - 1; //Note: off by one error in index
	result.end_word_index = positions_weights[best_right]; //one past the end, by Chandler's request
	result.value_captured = max_value;
    std::cout << "result left: " << result.start_word_index << " result right: " << result.end_word_index << std::endl;
	return result;
}

fb::Pair<fb::String, fb::String> GenerateSnippetsAndTitle( SnippetStats &stat, rank_stats &doc ){

    FILE *fptr = fopen(stat.FileName.data(), "rb");
    if (fptr == NULL){
        std::cout << "error opening " << stat.FileName << " when generating snippets and title " << std::endl;
        return { " ", " " }; //if the file fails to open, just give empty strings rather than crash
    }
    fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_SET); //skip the cursor
    fb::SizeT NumPageStoreDocs;
    fread(&NumPageStoreDocs, sizeof(fb::SizeT), 1, fptr); //read in the num of documents
    fseek(fptr, sizeof(PageHeader) * stat.DocIndex, SEEK_CUR); //skip ahead in the vector of PageHeaders
    fb::SizeT PageOffset;
    fread(&PageOffset, sizeof(fb::SizeT), 1, fptr); //read in the page offset
    fb::SizeT VectorOffset;
    fread(&VectorOffset, sizeof(fb::SizeT), 1, fptr); //read in the vector offset
    fb::SizeT UrlId;
    fread(&UrlId, sizeof(fb::SizeT), 1, fptr); //read in the UrlID
    fb::SizeT NextPageOffset;
    fread(&NextPageOffset, sizeof(fb::SizeT), 1, fptr); //read in the next page offset, but be careful with next line!
    if (stat.DocIndex == (NumPageStoreDocs - 1)){
        NextPageOffset = VectorOffset + 100;
    }
    doc.UrlId = UrlId; //set the UrlID in the rank_stats

    //this is the snippet code
    fb::String snippet;
    char dummy[80]; //TODO: THIS IS SCARY!! do we have a max word size??
    fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
    for (fb::SizeT i = 0; i < stat.Offsets.start_word_index; ++i){
        fscanf(fptr, "%80s", dummy); //scan past all the words before begin offset
    }
    for (fb::SizeT j = stat.Offsets.start_word_index; j < stat.Offsets.end_word_index; ++j){
        fscanf(fptr, "%80s", dummy); //add all the words between begin offset and end offset
        snippet += dummy;
        snippet += " "; //dont forget to put a space between the words!
    }
    if (snippet.size() == 0) snippet += " ";

    //this is the title code
    bool title_began = false;
    fb::SizeT title_begin = 0;
    fb::SizeT title_end = 0;
    uint8_t descriptor = 0;
    fseek(fptr, VectorOffset, SEEK_SET); //jump back to beginning of page
    for(fb::SizeT i = 0; i < 50 && ((i + VectorOffset) < NextPageOffset); ++i){
        fread(&descriptor, 1, 1, fptr);
        if (descriptor & INDEX_WORD_TITLE){
            if (!title_began){
                title_begin = i;
                title_began = true;
            }
            title_end = i + 1;
        }
    }

    fb::String title;
    if (!title_began){
        title += " ";
        return fb::make_pair<fb::String, fb::String>(std::move(snippet), std::move(title));
    }
    char dummy2[80]; //TODO: THIS IS SCARY!! do we have a max word size??
    fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
    for (fb::SizeT i = 0; i < title_begin; ++i){
        fscanf(fptr, "%80s", dummy2); //scan past all the words before begin offset
    }
    for (fb::SizeT j = title_begin; j < title_end; ++j){
        fscanf(fptr, "%80s", dummy2); //add all the words between begin offset and end offset
        title += dummy2;
        title += " "; //dont forget to put a space between the words!
    }
    if (title.size() == 0) title += " ";

    fclose(fptr); //don't forget this!
    return fb::make_pair<fb::String, fb::String>(std::move(snippet), std::move(title)); //return the generated snippet
}
