#pragma once

#ifndef TOKEN_H
#define TOKEN_H
#include <iostream> // Different inclusions needed for our tokens
#include <string>
#include <map>
#include <vector>
using namespace std;

enum tokenIDEnum { idTk, keywordTk, intTk, opTk, eofTk }; // Different types of tokens

typedef struct token { // Our struct for the pieces that make up a token
    tokenIDEnum tokenID;
    string tokenInstance;
    string tokenData;
    int lineNumber = 0;
} Token;

class ScannerLanguage { // Create a class to keep all constants needed and maps for what operators and keywords equal to
protected:
    // Declarations on what operators and keywords equal to
    map<string, string> allOperators = {
            {"=", "equalToken"}, {">", "greaterThanToken"}, {"<", "lessThanToken"}, {"==", "equalEqualToken"}, {":", "colonToken"}, {":=", "colonEqualToken"}, {"+", "plusToken"},
            {"-", "minusToken"}, {"*", "multiplyToken"}, {"/", "divideToken"}, {"%", "modulusToken"}, {".", "dotToken"}, {"(", "leftParenToken"}, {")", "rightParenToken"},
            {",", "commaToken"}, {"{", "leftCurlyToken"}, {"}", "rightCurlyToken"}, {";", "semiColonToken"}, {"[", "leftBracketToken"}, {"]", "rightBracketToken"}
    };

    map<string, string> allKeywords = {
            {"start", "startToken"}, {"stop", "stopToken"}, {"loop", "loopToken"}, {"while", "whileToken"}, {"for", "forToken"}, {"label", "labelToken"},
            {"exit", "exitToken"}, {"listen", "listenToken"}, {"talk", "talkToken"}, {"program", "programToken"}, {"if", "ifToken"},
            {"then", "thenToken"}, {"assign", "assignToken"}, {"declare", "declareToken"}, {"jump", "jumpToken"}, {"else", "elseToken"}
    };

    // Vectors that store different token types as well as keywords and operators
    const vector<string> tokenTypes = {
            "Identifier", "Keyword", "Integer", "Operator", "End Of File"
    };

    const vector<string> keywords = {
            "start", "stop", "loop", "while", "for", "label", "exit", "listen", "talk", "program", "if", "then", "assign", "declare", "jump", "else"
    };

    const vector<char> charOperators = { // For single character operators
            '=', '>', '<', ':', '+', '-', '*', '/', '%', '.', '(', ')', ',', '{', '}', ';', '[', ']'
    };

    const vector<string> stringOperators = { // For multiple character operators
            ":=", "=="
    };

    const char COMMENT_DELIMITER = '&';

public:
    int isCharOperator(char); // Functions to check whether or not the operator is a single or double char operator, or neither
    int isStringOperator(string);
    int getOperator(Token&); // Obtaining which one that it is for the operators and keywords
    int getKeyword(Token&);
};

#endif