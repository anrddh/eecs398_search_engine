#include <iostream>
#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <fb/utility.hpp>
#include <cmath>
#include <isr/constraint_solver.hpp>
#include <parser/parser.hpp>
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

//positions_weights is a fb::Vector of indices corresponding to the words in the our query
//and their tfidf
//im getting rid of weights for now
snip_window snippet_window_rank(fb::Vector<fb::SizeT> &positions_weights, fb::SizeT max_window_size){
	snip_window result;
	if(positions_weights.size() < 2){
		result.start_word_index = positions_weights[0];
		result.end_word_index = positions_weights[0] + 1;
		result.value_captured = 1;
		return result;
	}
	size_t left = 0;
	size_t right = 0;
	size_t best_left = 0;
	size_t best_right = 0;
	size_t delta = 0;
	// double current_value = positions_weights[left].second;
    double current_value = 1;
	double max_value = current_value;
	while(right < positions_weights.size() - 1){
		++right;
		delta = positions_weights[right] - positions_weights[left];
		current_value += 1;
		while(delta > max_window_size){
			current_value -= 1;
			++left;
			delta = positions_weights[right] - positions_weights[left];
		}

		if(current_value > max_value){
			max_value = current_value;
			best_left = left;
			best_right = right;
		}
	}
	result.start_word_index = positions_weights[best_left];
	result.end_word_index = positions_weights[best_right] + 1; //one past the end, by Chandler's request
	result.value_captured = max_value;
	return result;
}

fb::Pair<fb::String, fb::String> GenerateSnippetsAndTitle( SnippetStats &Stats, fb::Vector<rank_stats> &documents_to_rank ){

    SnippetStats &stat = Stats[i];
    FILE *fptr = fopen(stat.filename.data(), "rb");
    if (fptr == NULL){
        std::cout << "error opening " << filename << " when generating snippets " << std::endl;
        snippets.PushBack(""); //if the file fails to open, just give empty string for snippet rather than crash
        continue;
    }
    fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_SET); //skip the cursor
    fb::SizeT NumPageStoreDocs;
    fread(&NumPageStoreDocs, fb::SizeT, 1, fptr); //read in the num of documents
    fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_CUR); //skip the counter
    fseek(fptr, sizeof(PageHeader) * stat.DocIndex, SEEK_CUR); //skip ahead in the vector of PageHeaders
    fb::SizeT PageOffset;
    fread(&PageOffset, fb::SizeT, 1, fptr); //read in the page offset
    fb::SizeT VectorOffset;
    fread(&VectorOffset, fb::SizeT, 1, fptr); //read in the vector offset
    fb::SizeT UrlId;
    fread(&UrlId, fb::SizeT, 1, fptr); //read in the UrlID
    fb::SizeT NextPageOffset;
    fread(&NextPageOffset, fb::SizeT, 1, fptr); //read in the next page offset, but be careful with next line!
    if (stat.DocIndex = NumPageStoreDocs - 1){
        NextPageOffset = VectorOffset + 100;
    }
    documents_to_rank[i].UrlId = UrlId; //set the UrlID in the rank_stats

    //this is the snippet code
    String snippet;
    char dummy[80]; //TODO: THIS IS SCARY!! do we have a max word size??
    fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
    for (int i = 0; i < stat.Offsets.begin; ++i){
        fscanf(fptr, "%s", dummy); //scan past all the words before begin offset
    }
    for (int j = stat.Offsets.begin; j < stat.Offsets.end; ++j){
        fscanf(fptr, "%s", dummy); //add all the words between begin offset and end offset
        snippet += dummy;
        snippet += " "; //dont forget to put a space between the words!
    }

    //this is the title code
    bool title_began = false;
    fb::SizeT title_begin = 0;
    fb::SizeT title_end = 0;
    uint8_t descriptor = 0;
    fseek(fptr, VectorOffset, SEEK_SET); //jump back to beginning of page
    for( int i = 0; i < 50 && i + VectorOffset < NextPageOffset; ++i){
        fread(&descriptor, uint8_t, 1, fptr);
        if (descriptor & INDEX_WORD_TITLE){
            if (!title_began){
                title_begin = i;
                title_began = true;
            }
            title_end = i + 1;
        }
    }

    fb::String title;
    if (!title_end){
        return fb::make_pair<snippet, title>
    }
    char dummy2[80]; //TODO: THIS IS SCARY!! do we have a max word size??
    fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
    for (int i = 0; i < title_begin; ++i){
        fscanf(fptr, "%s", dummy); //scan past all the words before begin offset
    }
    for (int j = title_begin; j < title_end; ++j){
        fscanf(fptr, "%s", dummy); //add all the words between begin offset and end offset
        title += dummy;
        title += " "; //dont forget to put a space between the words!
    }

    fclose(fptr); //don't forget this!
    return fb::make_pair<snippet, title>; //return the generated snippet
}