#pragma once

#ifndef TREE_H
#define TREE_H
#include <cstdio> // Inclusions needed for our tree
#include <iostream>
#include <string>
#include <vector>
#include "node.h" // Needed header files
using namespace std;

void traversePreorder(NodeT* node, int depth = 0, string indent = "", bool last = true); // Function declaration and our starting point given through this line

#endif
