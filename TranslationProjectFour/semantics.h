#pragma once
#ifndef SEMANTICS_H
#define SEMANTICS_H
#include <fstream>
#include <iostream> // Includes for file, strings, and vectors
#include <string> 
#include <vector>      
#include <string>
#include "node.h" // Inclusions for different nodes we have and token options
#include "token.h"
using namespace std;

typedef struct symbol_table { // Our symbol table that stores our tokens
	Token identifier;
	Token value;
} SymbolTable;

class Semantics {
protected:
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

	ofstream file;
	string outputFile;
	string ifStatement;
	SymbolTable tableHolder; // Creation of our symbol table
	vector<SymbolTable> st;

	unsigned int tempVariableNumber = 0;
	unsigned int carryLabelNumber = 0;
	unsigned int labelsNumber = 0;
	bool isNegative = false;
	bool firstOption = false;

public:
	Semantics(string);
	void verify(NodeT*);
	void checkDeclaration(Token, Token); // Function declarations to check variables
	void checkDefined(Token);
	void generation(NodeT*);
	void generateCode(NodeT*);
};

#endif