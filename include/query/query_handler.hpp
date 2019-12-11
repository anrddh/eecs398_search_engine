#pragma once

#include <query/query_result.hpp>
#include <fb/thread.hpp>
#include <disk/constants.hpp>
#include <algorithm>

struct IndexInfoArg {
    fb::UniquePtr<Expression> &e;
    fb::UniquePtr<IndexReader> &reader;
    TopPages &pages;
};

//merge together the rank stats vector into the snippet
//for now, assuming the weight of each word is 1
//just to get something working quickly
//im literally just going to sort it for now lol
fb::Vector<fb::SizeT> MergeVectors(const fb::Vector<fb::Vector<uint32_t>> &occurrences);

class QueryHandler {
public:
   QueryHandler( fb::String path , fb::String prefix, int num_indices );

   ~QueryHandler( )

   fb::Vector<QueryResult> Query( fb::String q );

private:
   Vector<fb::UniquePtr<IndexReader>> Readers;

};
