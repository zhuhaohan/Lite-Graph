/*
 * covering_snapshot.hpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */

#ifndef COVERING_SNAPSHOT_HPP_
#define COVERING_SNAPSHOT_HPP_

#include <vector>
#include "index.hpp"
#include "load_graph.hpp"

using namespace std;

class Covering_Snapshot
{
    public:
		Covering_Snapshot(const char* graph_file, Index index_structure, int start_input, int end_input);
		int binary_search(int begin, int end, int key);
		void get_duration_pairs();
		void retrieve_results();
		int tes();

    public:
        Index index;
        int x, y;
        int index_min, index_max;
        int B; // B for # buckets
        vector<Edge> candidates;
        vector< std::pair <int,int> > duration_pairs;
        int number_results;
        vector<Edge> results;
        vector< std::pair <int,int> > tes_pairs;
};

Covering_Snapshot::Covering_Snapshot(const char* graph_file, Index index_structure, int start_input, int end_input) {
	x = start_input;
	y = end_input;
	index = index_structure;
	index_min = index.start_index(x);
	index_max = index.end_index(y);
	B = (index_max - index_min) / index.bucket_size;
	Graph g(graph_file, index_min, index_max);
	candidates = g.edge_stream;
	number_results = 0;
}

void Covering_Snapshot::get_duration_pairs() {
	duration_pairs.clear();
	for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
		if(candidates[j].te <= y) {
			if(candidates[j].ts >= x){
				duration_pairs.push_back(make_pair(j, j));
				number_results++;
			}
		}
		else {
			break;
		}
	}
	for(int i = 1; i < B; i++){ // [x, y] for bucket from (x, y]
		int key = binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, y);
		if (key != i*index.bucket_size - 1) {
			duration_pairs.push_back(make_pair (i*index.bucket_size, key));
			number_results += key - i*index.bucket_size + 1;
		}
	}
}

int Covering_Snapshot::tes() {
	tes_pairs.clear();
	int tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].te <= y) {
				if(candidates[j+i*index.bucket_size].ts >= x){
					tes_pairs.push_back(make_pair(j, j));
					tes_number_results++;
				}
			}
		}
	}
	return tes_number_results;
}

int Covering_Snapshot::binary_search(int begin, int end, int key) {
	// Find the index p, where candidates[p] <= key < candidates[p+1];
	// This is for Covering graph, duration is [begin, p]
	int mid;
	if(end - begin == 1) {
		if (candidates[begin].te > key)
			return begin - 1;
		else if (candidates[end].te <= key)
			return end;
		else
			return begin;
	}
	else {
		mid = (end - begin)/2 + begin;
	}
	if (candidates[mid].te > key)
		return binary_search(begin, mid, key);
	else
		return binary_search(mid, end, key);
}

void Covering_Snapshot::retrieve_results(){
	results.resize(number_results);
	int count = 0;
	for(int i = 0; i < duration_pairs.size(); i++){
		for(int j = duration_pairs[i].first; j <= duration_pairs[i].second; j++){
			results[count] = candidates[j];
			count++;
		}
	}
}

#endif /* COVERING_SNAPSHOT_HPP_ */
