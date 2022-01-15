#pragma once

#ifndef PARSER_H
#define PARSER_H
#include <cstdio> // Inclusions needed for our parser
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "node.h" // Headers needed for our parser with nodes and our scanner
#include "scanner.h"
using namespace std;

class Parser : public Scanner { // Include our scanner as part of our parser
private:
    ifstream file; // Variable declarations for things like files and tokens
    string fileInput;

    Token tk;
    Token emptyToken;
    string expectedToken;

    Scanner scanner; // Create a scanner and a status to keep track of our scanner
    int scannerStatus;

    NodeT* program(); // Create node functions for each of our possible BNF points to read from
    NodeT* block();
    NodeT* vars();
    NodeT* expr();
    NodeT* N();
    NodeT* A();
    NodeT* M();
    NodeT* R();
    NodeT* stats();
    NodeT* mStat();
    NodeT* stat();
    NodeT* in();
    NodeT* out();
    NodeT* iff();
    NodeT* loop();
    NodeT* assign();
    NodeT* RO();
    NodeT* label();
    NodeT* gotoo();


public:
    Parser(string); // Functions for our parser that deal with parsing, scanning, nodes, and errors themselves
    NodeT* parse();
    void nextScan();
    NodeT* createNode(string);
    void parserError();

};
#endif
