#pragma once

#ifndef SCANNER_H
#define SCANNER_H
#include <iostream> // Inculsions necessary for our scanner
#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include "token.h" // Headers needed for different tokens
using namespace std;

class Scanner : public ScannerLanguage {
protected:
    unsigned int currentPointerPlacement = 0; // Establish our pointer for our character number and line number for where we arei n the file
    unsigned int currentLineNumber = 1;
    bool isCommenting = false; // Variables used for comments
    bool commentRecentChange = false;
    int commentCounter = 0;
    string lastCommentPosition = "";

    enum { // Enum for our FSA states
        ERROR_UNK = -4,
        ERROR_COM = -3,
        ERROR_INT = -2,
        ERROR_ID = -1,
        STATE_0 = 0,
        STATE_1 = 1,
        STATE_2 = 2,
        STATE_3 = 3,
        FINAL_STATE = 1000,
        FINAL_ID = 1001,
        FINAL_INT = 1003,
        FINAL_OP = 1004,
        FINAL_EOF = 1005
    };

    // FSA table for figuring out our tokens
    // //       0          1          2      3        4         5           6  7    8
    // // ROWS: Uppercase, Lowercase, $    , digit  , operator, whitespace, &, EOF, unknown
    const int FSA_TABLE[4][8] = {
        { ERROR_ID, STATE_2, STATE_2, STATE_3, STATE_1, STATE_0, FINAL_EOF, ERROR_UNK }, // s0 (initial)
        { FINAL_OP, FINAL_OP, FINAL_OP, FINAL_OP, FINAL_OP, FINAL_OP, FINAL_OP, ERROR_UNK }, // s1 (operator)
        { STATE_2, STATE_2, FINAL_ID, STATE_2, FINAL_ID, FINAL_ID, FINAL_ID, ERROR_UNK }, // s2 (indentifer)
        { ERROR_INT, ERROR_INT, ERROR_INT, STATE_3, FINAL_INT, FINAL_INT, FINAL_INT, ERROR_UNK }, // s3 (integer)
    };

    const char SCANNER_DELIMITER = ' ';


public:
    void filterScan(string, string); // Function declarations for our scanner
    int scan(string&, Token&);
    char checkComment(int, char, char);
    int getCategory(char);
    void getError(int, int, char);
    string getScannerPosition();
    void invokeEOF(Token&);
};

#endif