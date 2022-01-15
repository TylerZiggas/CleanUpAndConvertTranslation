// Author: Tyler Ziggas
// // Date: November 10, 2021
// // Project Description: This project either takes in a file and reads from it or takes user input if no file is given and then stores it into a temporary file to read from.
// //   This file is then scanned over to find all identifiers, integers, keywords, and operators, which are then parsed over to make sure they fit the criteria given for our language.
// //   While the different tokens are being parsed, they will be put into a tree so we can access them through nodes for later additions to the project.  They will also
// //   be displayed as a tree to showcase what exactly we are looking at for our tree (through preorder traversal).

#include "tree.h"

void traversePreorder(NodeT* node, int depth, string indent, bool last) {
    if (node == NULL) { // Return nothing if our node is empty
        return;
    }

    string indentSymbol = ""; // Our default indent symbol for displaying our tree
    if (depth > 0) { // Determining how to display based on depth
        indentSymbol = (last) ? " [_ " : "|- ";
    }
    else {
        indentSymbol = "   ";
    }
    cout << indent << indentSymbol << node->label << " ";

    for (unsigned int i = 0; i < node->tokens.size(); i++) { // Start displaying our node's token instance
        cout << node->tokens[i].tokenInstance;

        if (((i + 1) != node->tokens.size()) && (node->tokens[i].tokenID != opTk)) { // Add commas if we are going to continue looping through some variables
            cout << ",";
        }
        cout << " ";
    }
    cout << endl;

    indent += (last) ? "   " : "|  ";

    vector<NodeT*> nodes; // Checks to see whether there are nodes in any of the nodes listed under the node
    if (node->nodeOne != NULL) {
        nodes.push_back(node->nodeOne);
    }
    if (node->nodeTwo != NULL) {
        nodes.push_back(node->nodeTwo);
    }
    if (node->nodeThree != NULL) {
        nodes.push_back(node->nodeThree);
    }
    if (node->nodeFour != NULL) {
        nodes.push_back(node->nodeFour);
    }
    if (node->nodeFive != NULL) {
        nodes.push_back(node->nodeFive);
    }

    for (unsigned int i = 0; i < nodes.size(); i++) { // Continue to traverse through our tree
        traversePreorder(nodes[i], depth + 1, indent, i == nodes.size() - 1);
    }
}

