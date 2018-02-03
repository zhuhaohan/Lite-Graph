/*
 * existing_delta.hpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */

#ifndef EXISTING_DELTA_HPP_
#define EXISTING_DELTA_HPP_

#include <vector>
#include "index.hpp"
#include "load_graph.hpp"

using namespace std;

class Existing_Delta
{
    public:
		Existing_Delta(const char* graph_file, Index index_structure, int start_input, int end_input, int delta_input, int option_input);
		int upperbound_binary_search(int begin, int end, int key);
		int lowerbound_binary_search(int begin, int end, int key);
		void get_added_duration_pairs();
		void get_removed_duration_pairs();
		void retrieve_added_results();
		void retrieve_removed_results();
		int added_tes();
		int removed_tes();

    public:
        Index index;
        int x, y, delta, pre;
        int min, max, index_min, index_max;
        int B; // B for # buckets
        int option;
        vector<Edge> candidates;
        vector< std::pair <int,int> > added_duration_pairs;
        vector< std::pair <int,int> > removed_duration_pairs;
        int number_added_results;
        int number_removed_results;
        vector<Edge> added_results;
        vector<Edge> removed_results;
        vector< std::pair <int,int> > added_tes_pairs;
        vector< std::pair <int,int> > removed_tes_pairs;
};

Existing_Delta::Existing_Delta(const char* graph_file, Index index_structure, int start_input, int end_input, int delta_input, int option_input) {
	option = option_input;
	x = start_input;
	y = end_input;
	delta = delta_input;
	pre = 0;
	if(option == 0)	{ //Remove [0, x] + [x, x+delta]
		min = pre;
		max = start_input + delta_input;
	}
	else { //Add [y, y+delta]
		min = end_input;
		max = end_input + delta_input;
	}
	index = index_structure;
	index_min = index.start_index(min);
	index_max = index.end_index(max);
	B = (index_max - index_min) / index.bucket_size;
	Graph g(graph_file, index_min, index_max);
	candidates = g.edge_stream;
	number_added_results = 0;
	number_removed_results = 0;
}

void Existing_Delta::get_added_duration_pairs() {
	added_duration_pairs.clear();
	for(int j = index.bucket_size - 1; j >= 0; j--){ // Add [y, inf] for bucket with y
		if(candidates[j].te >= y) {
			if(candidates[j].ts >= y){
				if(candidates[j].ts <= y + delta) {
					added_duration_pairs.push_back(make_pair(j, j));
					number_added_results++;
				}
			}
		}
		else {
			break;
		}
	}
	for(int i = 1; i < B-1; i++){ // Add [y, inf] for bucket from (y, y+delta)
		added_duration_pairs.push_back(make_pair(i*index.bucket_size, (i+1)*index.bucket_size-1));
		number_added_results += index.bucket_size;
	}
	for(int j = B*index.bucket_size - 1; j >= (B-1)*index.bucket_size; j--){ //Add [y, inf] for bucket with y+delta
		if(candidates[j].te > y+delta){
			if(candidates[j].ts <= y+delta){
				if(candidates[j].ts >= y){
					added_duration_pairs.push_back(make_pair(j, j));
					number_added_results++;
				}
			}
		}
		else{
			added_duration_pairs.push_back(make_pair((B-1)*index.bucket_size, j));
			number_added_results += j - (B-1)*index.bucket_size + 1;
			break;
		}
	}
}

void Existing_Delta::get_removed_duration_pairs() {
	removed_duration_pairs.clear();
	for(int i = 0; i < B; i++){ // [x, x+delta] for bucket from [0, x+delta]
		int upperbound_key = upperbound_binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, x+delta);
		int lowerbound_key = lowerbound_binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, x);
		if (upperbound_key != i*index.bucket_size - 1 && lowerbound_key != (i+1)*index.bucket_size) {
			removed_duration_pairs.push_back(make_pair (lowerbound_key, upperbound_key));
			number_removed_results += upperbound_key - lowerbound_key + 1;
		}
	}
}

int Existing_Delta::added_tes() {
	added_tes_pairs.clear();
	int added_tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].ts <= y + delta) {
				if(candidates[j+i*index.bucket_size].ts >= y) {
					if(candidates[j+i*index.bucket_size].te >= x+delta){
						added_tes_pairs.push_back(make_pair(j, j));
						added_tes_number_results++;
					}
				}
			}
			else {
				break;
			}
		}
	}
	return added_tes_number_results;
}

int Existing_Delta::removed_tes() {
	removed_tes_pairs.clear();
	int removed_tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].ts <= y) {
				if(candidates[j+i*index.bucket_size].te >= x){
					if(candidates[j+i*index.bucket_size].te <= x + delta){
						removed_tes_pairs.push_back(make_pair(j, j));
						removed_tes_number_results++;
					}
				}
			}
			else {
				break;
			}
		}
	}
	return removed_tes_number_results;
}

int Existing_Delta::upperbound_binary_search(int begin, int end, int key) {
	// Find the index p, where candidates[p] <= key < candidates[p+1];
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
		return upperbound_binary_search(begin, mid, key);
	else
		return upperbound_binary_search(mid, end, key);
}

int Existing_Delta::lowerbound_binary_search(int begin, int end, int key) {
	// Find the index p, where candidates[p-1] < key <= candidates[p];
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
		return lowerbound_binary_search(mid, end, key);
	else
		return lowerbound_binary_search(begin, mid, key);
}

void Existing_Delta::retrieve_added_results(){
	added_results.resize(number_added_results);
	int count = 0;
	for(int i = 0; i < removed_duration_pairs.size(); i++){
		for(int j = removed_duration_pairs[i].first; j <= removed_duration_pairs[i].second; j++){
			added_results[count] = candidates[j];
			count++;
		}
	}
}

void Existing_Delta::retrieve_removed_results(){
	removed_results.resize(number_removed_results);
	int count = 0;
	for(int i = 0; i < added_duration_pairs.size(); i++){
		for(int j = added_duration_pairs[i].first; j <= added_duration_pairs[i].second; j++){
			removed_results[count] = candidates[j];
			count++;
		}
	}
}

#endif /* EXISTING_DELTA_HPP_ */
