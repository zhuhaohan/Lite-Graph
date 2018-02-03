/*
 * existing_snapshot.hpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */

#ifndef EXISTING_SNAPSHOT_HPP_
#define EXISTING_SNAPSHOT_HPP_

#include <vector>
#include "index.hpp"
#include "load_graph.hpp"

using namespace std;

class Existing_Snapshot
{
    public:
		Existing_Snapshot(const char* graph_file, Index index_structure, int start, int end);
		int binary_search(int begin, int end, int key);
		void get_duration_pairs();
		void retrieve_results();
		int tes();

    public:
        Index index;
        int x, y, pre;
        int index_min, index_max, index_mid;
        int B, B_pre; // B for # buckets
        vector<Edge> candidates;
        vector< std::pair <int,int> > duration_pairs;
        int number_results;
        vector<Edge> results;
        vector< std::pair <int,int> > tes_pairs;
};

Existing_Snapshot::Existing_Snapshot(const char* graph_file, Index index_structure, int start_input, int end_input) {
	pre = 0; //user defined
	x = start_input;
	y = end_input;
	index = index_structure;
	index_min = index.start_index(pre);
	index_mid = index.start_index(x);
	index_max = index.end_index(y);
	B = (index_max - index_min) / index.bucket_size;
	B_pre = (index_mid - index_min) / index.bucket_size;
	Graph g(graph_file, index_min, index_max);
	candidates = g.edge_stream;

	number_results = 0;
}

void Existing_Snapshot::get_duration_pairs() {
	duration_pairs.clear();
	for(int i = 0; i < B_pre + 1; i++){ // [x, inf] for bucket from [0, x]
		int key = binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, x);
		if (key != (i+1)*index.bucket_size) {
			duration_pairs.push_back(make_pair(key, (i+1)*index.bucket_size -1));
			number_results += (i+1)*index.bucket_size - key;
		}
	}
	for(int i = B_pre + 1; i < B-1; i++){ // [x, inf] for bucket from (x, y)
		duration_pairs.push_back(make_pair(i*index.bucket_size, (i+1)*index.bucket_size-1));
		number_results += index.bucket_size;
	}
	for(int i = B*index.bucket_size - 1; i >= (B-1)*index.bucket_size; i--){ // [x, inf] for bucket with y
		if(candidates[i].te > y){
			if(candidates[i].ts <= y){
				duration_pairs.push_back(make_pair(i, i));
				number_results++;
			}
		}
		else{
			duration_pairs.push_back(make_pair((B-1)*index.bucket_size, i));
			number_results += i - (B-1)*index.bucket_size + 1;
			break;
		}
	}
}

int Existing_Snapshot::tes() {
	tes_pairs.clear();
	int tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].ts <= y){
				if(candidates[j+i*index.bucket_size].te >= x) {
					tes_pairs.push_back(make_pair(j, j));
					tes_number_results++;
				}
			}
			else {
				break;
			}
		}
	}
	return tes_number_results;
}

int Existing_Snapshot::binary_search(int begin, int end, int key) {
	// Find the index p, where candidates[p-1] < key <= candidates[p];
	// This is for Existing graph, duration is [p, end]
	int mid;
	if(end - begin == 1) {
		if (candidates[end].te < key)
			return end + 1;
		else if (candidates[begin].te >= key)
			return begin;
		else
			return end;
	}
	else {
		mid = (end - begin)/2 + begin;
	}
	if(candidates[mid].te < key )
		return binary_search(mid, end, key);
	else
		return binary_search(begin, mid, key);
}

void Existing_Snapshot::retrieve_results(){
	results.resize(number_results);
	int count = 0;
	for(int i = 0; i < duration_pairs.size(); i++){
		for(int j = duration_pairs[i].first; j <= duration_pairs[i].second; j++){
			results[count] = candidates[j];
			count++;
		}
	}
}

#endif /* EXISTING_SNAPSHOT_HPP_ */
