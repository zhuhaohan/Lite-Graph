/*
 * load_graph.hpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */

#ifndef LOAD_GRAPH_HPP_
#define LOAD_GRAPH_HPP_

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct Edge
{
    int u, v, ts, te;
    bool operator < (const Edge that) const {
        if (ts != that.ts)
            return ts < that.ts;
        if (te != that.te)
            return te > that.te;
        if (u != that.u)
            return u < that.u;
        else
        	return v < that.v;
    }
};

class Graph
{
    public:
		Graph(const char* READ_FILE, int input_e); // load whole graph
        Graph(const char* READ_FILE, int begin, int end); // load part of graph

    public:
    vector< Edge > edge_stream;
    int E;
};

Graph::Graph(const char* READ_FILE, int begin, int end) {
    E = end-begin;
    edge_stream.resize(E);
    ifstream binary_file (READ_FILE, ios::in | ios::binary);
    binary_file.seekg(sizeof(struct Edge)*begin, ios::beg);
    binary_file.read ((char*)&edge_stream[0], sizeof(struct Edge)*(end-begin));
    binary_file.close();
};

Graph::Graph(const char* READ_FILE, int input_e) {
    E = input_e;
    edge_stream.resize(E);
    ifstream binary_file (READ_FILE, ios::in | ios::binary);
    binary_file.read ((char*)&edge_stream[0], sizeof(struct Edge)*E);
    binary_file.close();
};

#endif /* LOAD_GRAPH_HPP_ */
