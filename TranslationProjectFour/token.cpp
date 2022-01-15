// Author: Tyler Ziggas
// // Date: November 10, 2021
// // Project Description: This project either takes in a file and reads from it or takes user input if no file is given and then stores it into a temporary file to read from.
// //   This file is then scanned over to find all identifiers, integers, keywords, and operators, which are then parsed over to make sure they fit the criteria given for our language.
// //   While the different tokens are being parsed, they will be put into a tree so we can access them through nodes for later additions to the project.  They will also
// //   be displayed as a tree to showcase what exactly we are looking at for our tree (through preorder traversal).

#include "token.h"

int ScannerLanguage::isCharOperator(char currentCharOperator) { // Check to see if it is a single character operator
    for (unsigned int charCounter = 0; charCounter < charOperators.size(); charCounter++) { // Go through our vector of operators
        if (currentCharOperator == charOperators[charCounter]) {
            return 1;
        }
    }

    return -1; // Return -1 if it is not a single char operator
}

int ScannerLanguage::isStringOperator(string currentStringOperator) { // Chec kto see if it is a double char operator
    for (unsigned int stringCounter = 0; stringCounter < stringOperators.size(); stringCounter++) { // Go through our smalle vector of operators that arem ore than one character
        if (currentStringOperator.compare(stringOperators[stringCounter]) == 0) {
            return 1;
        }
    }

    return -1; // Return -1 if it is not a double char operator
}

int ScannerLanguage::getOperator(Token& token) { // Grab the operator and return whichever one it was
    for (unsigned int charCounter = 0; charCounter < charOperators.size(); charCounter++) { // Go through our operators of single char size
        string op(1, charOperators[charCounter]);
        if (token.tokenInstance.compare(op) == 0) {
            token.tokenInstance = allOperators[token.tokenInstance]; // return once we figure our which token it is
            return charCounter;
        }
    }

    for (unsigned int stringCounter = 0; stringCounter < stringOperators.size(); stringCounter++) { // Go through our operators of double char size
        if (token.tokenInstance.compare(stringOperators[stringCounter]) == 0) {
            token.tokenInstance = allOperators[token.tokenInstance]; // return once we figure out which token it is
            return stringCounter;
        }
    }

    return -1;
}

int ScannerLanguage::getKeyword(Token& token) { // Grab the keyword we find
    for (unsigned int keyCounter = 0; keyCounter < keywords.size(); keyCounter++) {
        if (token.tokenInstance.compare(keywords[keyCounter]) == 0) { // Go through out possible keywords and return it if we are done
            token.tokenInstance = allKeywords[token.tokenInstance];
            return keyCounter;
        }
    }

    return -1;
}
