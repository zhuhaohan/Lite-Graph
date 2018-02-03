/*
 * index.hpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */

#ifndef INDEX_HPP_
#define INDEX_HPP_

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct node
{
    int min;
    int max;
    int index_of_array;
    int min_overlap;
    int max_overlap;
    node *left;
    node *right;
};

struct bucket
{
    int min;
    int max;
    int index_of_array;
    int min_overlap;
    int max_overlap;
};

class Index
{
    public:
		Index();
        Index(const char* index_file, int number_buckets, int number_edges); // input file
        void insert(bucket b, node *leaf);
        node *search(int key, node *leaf);
        int start_index(int key);
        int end_index(int key);

    public:
        node *root;
        int I, E;
        int bucket_size;
        vector <bucket> v_bucket;
};

Index::Index(){
    I = 0;
    E = 0;
    bucket_size = 0;
    v_bucket.resize(I);
    root = NULL;
}

Index::Index(const char* index_file, int number_buckets, int number_edges)
{
    I = number_buckets; // Bucket number from 0 to number_buckets - 1
    E = number_edges;
    bucket_size = number_edges / number_buckets;
    v_bucket.resize(I);
    root = NULL;

    bucket b;
    int x;
    FILE* origin_file = fopen(index_file,"r");
    for(int i = 1; i < I; i++){
        x=fscanf(origin_file, "%d %d %d %d %d",&b.min, &b.max, &b.index_of_array, &b.min_overlap, &b.max_overlap);
        if (root==NULL) {
            root = new node;
            root->min = b.min;
            root->max = b.max;
            root->index_of_array = b.index_of_array;
            root->min_overlap = b.min_overlap;
            root->max_overlap = b.max_overlap;
            root->left = NULL;
            root->right = NULL;
        }
        else
            insert(b, root);
    }
}

void Index::insert(bucket b, node* leaf){ //Build the index
    if(b.min < leaf->min) {
        if(leaf->left!=NULL) {
            insert(b, leaf->left);
        }
        else {
            leaf->left = new node;
            leaf->left->min = b.min;
            leaf->left->max = b.max;
            leaf->left->index_of_array = b.index_of_array;
            leaf->left->min_overlap = b.min_overlap;
            leaf->left->max_overlap = b.max_overlap;
            leaf->left->left = NULL;    //Sets the left child of the child node to null
            leaf->left->right = NULL;   //Sets the right child of the child node to null
        }
    }
    else if(b.min >= leaf->min) {
        if(leaf->right!=NULL) {
            insert(b, leaf->right);
        }
        else {
            leaf->right = new node;
            leaf->right->min = b.min;
            leaf->right->max = b.max;
            leaf->right->index_of_array = b.index_of_array;
            leaf->right->min_overlap = b.min_overlap;
            leaf->right->max_overlap = b.max_overlap;
            leaf->right->left=NULL;  //Sets the left child of the child node to null
            leaf->right->right=NULL; //Sets the right child of the child node to null
        }
    }
}

node *Index::search(int key, node *leaf) { //Search the index, return a node
    if(leaf!=NULL) {
        if(key == leaf->min) {
            return leaf;
        }
        if(key < leaf->min) {
            if(leaf->left != NULL)
                return search(key, leaf->left);
            else
                return leaf; // or null
        }
        if(key > leaf->min) {
            if(key > leaf->max)
                if(leaf->right != NULL)
                    return search(key, leaf->right);
                else
                    return leaf; // or null
            else if (key < leaf->max)
                return leaf;
            else{
                if(leaf->max_overlap == false)
                    return leaf;
                else
                    return search(key, leaf->right);
            }
        }
    }
    return leaf;
}

int Index::start_index(int key) { //Search the index, return the index of starting bucket
    node *n_start = search(key, root);
    if (key == n_start->min && n_start->min_overlap == 1)
        return max(0, n_start->index_of_array - bucket_size);
    else {
        return n_start->index_of_array;
    }
}

int Index::end_index(int key) { //Search the index, return the index of ending bucket
    node *n_end = search(key, root);
    return min(E-1, n_end->index_of_array + bucket_size);
}

#endif /* INDEX_HPP_ */
