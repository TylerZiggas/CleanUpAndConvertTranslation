// Author: Tyler Ziggas
// // Date: November 10, 2021
// // Project Description: This project either takes in a file and reads from it or takes user input if no file is given and then stores it into a temporary file to read from.
// //   This file is then scanned over to find all identifiers, integers, keywords, and operators, which are then parsed over to make sure they fit the criteria given for our language.
// //   While the different tokens are being parsed, they will be put into a tree so we can access them through nodes for later additions to the project.  They will also
// //   be displayed as a tree to showcase what exactly we are looking at for our tree (through preorder traversal).

#include "scanner.h"

int Scanner::scan(string& input, Token& token) {
    token.lineNumber = currentLineNumber; // Create and initialize everything necessary for our scanner
    int currentState = 0;
    int nextState = 0;
    int nextCategory = 0;
    string readValue;
    char nextCharacter;

    while (currentPointerPlacement <= input.length()) { // Read from our input until we hit the end of the length
        if (currentPointerPlacement < input.length()) { // If we are not at the end of the input check for comments
            if (currentPointerPlacement == (input.length() - 1)) { // If we are right before the end of the file check for a comment wit hthe second char acting as a space
                nextCharacter = checkComment(currentLineNumber, input.at(currentPointerPlacement), ' ');
            }
            else { // Otherwise check for it with the next character to see if we are going to start commenting
                nextCharacter = checkComment(currentLineNumber, input.at(currentPointerPlacement), input.at(currentPointerPlacement + 1));
            }
        }
        else { // Otherwise we want it to be a space
            nextCharacter = SCANNER_DELIMITER;
        }

        nextCategory = getCategory(nextCharacter); // Figure out our category and state through out FSA table
        nextState = FSA_TABLE[currentState][nextCategory];

        if (nextState < 0) { // Check to see if we hit an error state
            getError(currentLineNumber, nextState, nextCharacter);
            exit(EXIT_FAILURE);

        }
        else if (nextState > FINAL_STATE) { // If we are in a final state, figure out which one
            token.tokenInstance = readValue;

            switch (nextState) {
            case FINAL_ID: // In the case of an identifier we need to see if it is a keyword
                if (getKeyword(token) != -1) {
                    token.tokenID = keywordTk;
                    token.tokenData.assign(readValue);
                    token.tokenInstance.assign(readValue);

                }
                else {
                    token.tokenID = idTk;
                    token.tokenData.assign(readValue);
                    token.tokenInstance.assign("idToken " + readValue);
                }
                break;

            case FINAL_INT: // In the case of an integer
                token.tokenID = intTk;
                token.tokenData.assign(readValue);
                token.tokenInstance.assign("intToken " + readValue);
                break;

            case FINAL_OP: // In the case of an operator, need to see if it is a single character or two characters
                token.tokenID = opTk;

                if (currentPointerPlacement < input.length()) {
                    string stringOperator = readValue + input.at(currentPointerPlacement);
                    if (isStringOperator(stringOperator) != -1) { // Checking to see if it is multiple characters here
                        readValue = stringOperator;
                        currentPointerPlacement++;
                    }
                }

                token.tokenData.assign(readValue);
                token.tokenInstance.assign(readValue);
                getOperator(token);
                token.tokenData.assign(readValue);
                token.tokenInstance.assign(readValue);
                break;
            }

            if (isCommenting) { // If we are still commenting keep incrementing our pointer
                currentPointerPlacement++;
            }
            return 0;
        }


        currentState = nextState; // Change the state we are moving to
        currentPointerPlacement++;

        if (!isspace(nextCharacter)) {
            readValue.push_back(nextCharacter);
        }
    }

    currentPointerPlacement = 0; // Increment our character number and line number
    currentLineNumber++;
    return -1;
}

void Scanner::filterScan(string inputFile, string outputFile) { // Start scanning with given file names
    ifstream sourceFile(inputFile.c_str());
    ofstream newOutputFile(outputFile.c_str());
    Token token;

    if (sourceFile.is_open()) { // Open the file we just received
        string input;
        while (getline(sourceFile, input)) {

            string scan_input;
            while (scan(input, token) == 0) {
                scan_input.append(token.tokenData);

                if (currentPointerPlacement < input.length()) {
                    scan_input.append(" ");
                }
            }

            newOutputFile << scan_input << endl;
        }
    }
    else { // If we can't open the file
        cout << "[ERROR] Can't open file!" << endl;
        exit(EXIT_FAILURE);
    }

    sourceFile.close(); // Close our files when finished and reset our flags
    newOutputFile.close();
    isCommenting = false;
    currentPointerPlacement = 0;
    currentLineNumber = 1;
}

char Scanner::checkComment(int currentLine, char currentCharacter, char nextCharacter) { // Check to see if we are suppose to change commenting state
    if (currentCharacter == COMMENT_DELIMITER && nextCharacter == COMMENT_DELIMITER) { // If we get two apersands in a row
        isCommenting = !isCommenting; // Switch commenting
        commentRecentChange = true; // Establish that we just changed comments

        if (isCommenting) { // Of we are mpw cmtomg
            ostringstream temp;
            temp << currentLine << ":" << currentPointerPlacement;
            lastCommentPosition = temp.str();
        }
        return SCANNER_DELIMITER;
        // If the first is ampersand and the second isnt, if we are not commenting and we didn't just change state we hit an error
    }
    else if ((currentCharacter == COMMENT_DELIMITER) && (nextCharacter != COMMENT_DELIMITER) && (isCommenting == false) && (commentRecentChange == false)) {
        getError(currentLine, ERROR_COM, COMMENT_DELIMITER);
        exit(EXIT_FAILURE);
    }

    if ((isCommenting == true) || (commentRecentChange == true)) { // If we are still commenting and the comment was recently changed unflag that boolean
        commentRecentChange = false;
        return SCANNER_DELIMITER;
    }
    else {
        return currentCharacter;
    }
}

int Scanner::getCategory(char ch) { // Return the number of the category that each of these provides
    if (isupper(ch)) {
        return 0;
    }
    else if (islower(ch)) {
        return 1;
    }
    else if (ch == '$') {
        return 2;
    }
    else if (isdigit(ch)) {
        return 3;
    }
    else if (isCharOperator(ch) != -1) {
        return 4;
    }
    else if (isspace(ch)) {
        return 5;
    }
    else {
        return 7;
    }
}

string Scanner::getScannerPosition() { // Getting our scanner position while we are parsing
    ostringstream temp;
    temp << "(L" << currentLineNumber << ":C" << currentPointerPlacement << ")";
    return temp.str();
}

void Scanner::invokeEOF(Token& tk) { // When we want to invoke our end of file and correct our line number
    tk.lineNumber = (currentLineNumber > 1) ? currentLineNumber - 1 : currentLineNumber;
    tk.tokenID = eofTk;
    tk.tokenInstance = "EOF";
}

void Scanner::getError(int currentLine, int state, char currentCharacter) { // Showcase an error for whichever error was just called
    cout << "SCANNER ERROR: at (L" << currentLine << ":C" << currentPointerPlacement << ") -> " << currentCharacter << ": ";

    if (state == ERROR_ID) { // If we get an error with indeitifer such as a capital letter start
        cout << "all identifiers must begin with a lowercase letter or dollar sign ($)." << endl;
        cout << "SCANNER ERROR CODE: " << ERROR_ID << endl;

    }
    else if (state == ERROR_INT) { // If we have a letter right next to a digit when only digits are allowed for integers
        cout << "all integer tokens can only have digits." << endl;
        cout << "SCANNER ERROR CODE: " << ERROR_INT << endl;

    }
    else if (state == ERROR_COM) { // If we have a comment ampersand with a second one missing
        cout << "comments must be used with &&, not a single &." << endl;
        cout << "SCANNER ERROR CODE: " << ERROR_COM << endl;

    }
    else if (state == ERROR_UNK) { // If we don't know what the token received is
        cout << "unknown token not allowed." << endl;
        cout << "SCANNER ERROR CODE: " << ERROR_UNK << endl;

    }
}
