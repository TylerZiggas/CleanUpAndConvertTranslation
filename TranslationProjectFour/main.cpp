// // Project Description: This project either takes in a file and reads from it or takes user input if no file is given and then stores it into a temporary file to read from.
// //   This file is then scanned over to find all identifiers, integers, keywords, and operators, which are then parsed over to make sure they fit the criteria given for our language.
// //   While the different tokens are being parsed, they will be put into a tree so we can access them through nodes for later additions to the project.  They will also
// //   be displayed as a tree to showcase what exactly we are looking at for our tree (through preorder traversal).

#include <iostream> // Project inclusions for basics such as cout, cin, cin, getline, files, etc
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "parser.h" // Headers needed for main
#include "tree.h"
#include "semantics.h"

using namespace std;

int main(int argc, char* argv[]) {
    string fileName; // Create strings we need for the projects
    string outputFileName;
    string userInput;

    switch (argc) {
    case 1: { // If there is one argument we are reading from the keyboard
        ofstream tempFile;
        fileName = "input.fs";
        tempFile.open(fileName.c_str());

        cout << "SCANNER: Please input tokens and enter an empty line when you are finished." << endl;

        do { // Loop through the input until the end of the line and store it
            getline(cin, userInput);
            tempFile << userInput << endl;

        } while (!userInput.empty());

        cout << "SCANNER: Done reading from keyboard..." << endl << endl; // Tell user when we are done reading input
        tempFile.close();
        outputFileName = "kb.fs"; // default name for the file
        break;
    }

    case 2: { // If there are two arguments we are reading from a file
        cout << "SCANNER: Reading from file." << endl << endl;

        fileName = argv[1];
        size_t fileType = fileName.find(".");

        if (fileType == string::npos) {
            outputFileName = fileName;
            fileName.append(".fs");
        }
        else {
            outputFileName = fileName.substr(0, fileType);

        }
        break;
    }

    default: { // If there are more arguments output an error, there shoouldnt be more than a file if any
        cout << "SCANNER ERROR: Too many arguments received!" << endl;
        cout << "Use by: " << argv[0] << " [file]" << endl;
        cout << "With [file] is an optional parameter." << endl;
        return EXIT_FAILURE;

    }
    }

    Parser parser(fileName); // Start our parsing and eventually print out parse tree through preorder traversal
    NodeT* root = parser.parse();
    Semantics semantics(outputFileName);
    semantics.generateCode(root);

    return 0;
}

