/*
 * persisting_delta.hpp
 *
 *  Created on: Sep 19, 2014
 *      Author: zhu
 */

#ifndef PERSISTING_DELTA_HPP_
#define PERSISTING_DELTA_HPP_

#include <vector>
#include "index.hpp"
#include "load_graph.hpp"

using namespace std;

class Persisting_Delta
{
    public:
		Persisting_Delta(const char* graph_file, Index index_structure, int start_input, int end_input, int delta_input, int option_input);
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

Persisting_Delta::Persisting_Delta(const char* graph_file, Index index_structure, int start_input, int end_input, int delta_input, int option_input) {
	option = option_input;
	x = start_input;
	y = end_input;
	delta = delta_input;
	pre = 0;
	if(option == 0)	{ // Remove [0, x]
		min = pre;
		max = start_input;
	}
	else { //add
		min = start_input; // Add [x, x+delta]
		max = start_input + delta_input;
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

void Persisting_Delta::get_added_duration_pairs() {
	added_duration_pairs.clear();
	for(int j = index.bucket_size - 1; j >= 0; j--) { // Add [y+delta, inf] for bucket with x
		if(candidates[j].te >= y+delta) {
			if(candidates[j].ts >= x) {
				if(candidates[j].ts <= x + delta){
					added_duration_pairs.push_back(make_pair(j, j));
					number_added_results++;
				}
			}
		}
		else {
			break;
		}
	}
	for(int i = 1; i < B-1; i++){ // Add [y+delta, inf] for bucket from (x, x+delta)
		int lowerbound_key = lowerbound_binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, y+delta);
		if (lowerbound_key != (i+1)*index.bucket_size) {
			added_duration_pairs.push_back(make_pair (lowerbound_key, (i+1)*index.bucket_size-1));
			number_added_results += (i+1)*index.bucket_size - lowerbound_key;
		}
	}
	for(int j = B*index.bucket_size - 1; j >= (B-1)*index.bucket_size; j--){ //Add [y+delta, inf] for bucket with x+delta
		if(candidates[j].te >= y+delta) {
			if(candidates[j].ts <= x+delta) {
				if(candidates[j].ts >= x) {
					added_duration_pairs.push_back(make_pair(j, j));
					number_added_results++;
				}
			}
		}
		else {
			break;
		}
	}
}

void Persisting_Delta::get_removed_duration_pairs() {
	removed_duration_pairs.clear();
	for(int i = 0; i < B-1; i++){ // [y, y+delta] for bucket from [0, x)
		int upperbound_key = upperbound_binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, y+delta);
		int lowerbound_key = lowerbound_binary_search(i*index.bucket_size, (i+1)*index.bucket_size -1, y);
		if (upperbound_key != i*index.bucket_size - 1 && lowerbound_key != (i+1)*index.bucket_size) {
			removed_duration_pairs.push_back(make_pair (lowerbound_key, upperbound_key));
			number_removed_results += upperbound_key - lowerbound_key + 1;
		}
	}
	for(int j = B*index.bucket_size - 1; j >= (B-1)*index.bucket_size; j--){ // [y, y+delta] for bucket with x
		if(candidates[j].te >= y) {
			if(candidates[j].te <= y+delta){
				if(candidates[j].ts <= x){
					removed_duration_pairs.push_back(make_pair(j, j));
					number_removed_results++;
				}
			}
		}
		else {
			break;
		}
	}
}

int Persisting_Delta::added_tes() {
	added_tes_pairs.clear();
	int added_tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].ts <= x + delta) {
				if(candidates[j+i*index.bucket_size].ts >= x) {
					if(candidates[j+i*index.bucket_size].te >= y+delta){
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

int Persisting_Delta::removed_tes() {
	removed_tes_pairs.clear();
	int removed_tes_number_results = 0;
	for(int i  = 0; i < B; i++) {
		for(int j = 0; j < index.bucket_size; j++){ // [x, y] for bucket with x
			if(candidates[j+i*index.bucket_size].ts <= x) {
				if(candidates[j+i*index.bucket_size].te >= y){
					if(candidates[j+i*index.bucket_size].te <= y + delta){
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

int Persisting_Delta::upperbound_binary_search(int begin, int end, int key) {
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

int Persisting_Delta::lowerbound_binary_search(int begin, int end, int key) {
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

void Persisting_Delta::retrieve_added_results(){
	added_results.resize(number_added_results);
	int count = 0;
	for(int i = 0; i < removed_duration_pairs.size(); i++){
		for(int j = removed_duration_pairs[i].first; j <= removed_duration_pairs[i].second; j++){
			added_results[count] = candidates[j];
			count++;
		}
	}
}

void Persisting_Delta::retrieve_removed_results(){
	removed_results.resize(number_removed_results);
	int count = 0;
	for(int i = 0; i < added_duration_pairs.size(); i++){
		for(int j = added_duration_pairs[i].first; j <= added_duration_pairs[i].second; j++){
			removed_results[count] = candidates[j];
			count++;
		}
	}
}

#endif /* PERSISTING_DELTA_HPP_ */
