#pragma once
#ifndef NODE_H
#define NODE_H
#include <string> // Inclusions needed for the creation of our node
#include <vector>
#include "token.h" // Headers needed for this file
using namespace std;

typedef struct node_t { // Creation of our node with all of its parts
    int depth;
    string label;
    vector<Token> tokens;
    struct node_t* nodeOne, * nodeTwo, * nodeThree, * nodeFour, * nodeFive;
} NodeT;

#endif
