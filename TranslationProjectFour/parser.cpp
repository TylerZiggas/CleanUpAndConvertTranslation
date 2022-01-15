// Author: Tyler Ziggas
// // Date: November 10, 2021
// // Project Description: This project either takes in a file and reads from it or takes user input if no file is given and then stores it into a temporary file to read from.
// //   This file is then scanned over to find all identifiers, integers, keywords, and operators, which are then parsed over to make sure they fit the criteria given for our language.
// //   While the different tokens are being parsed, they will be put into a tree so we can access them through nodes for later additions to the project.  They will also
// //   be displayed as a tree to showcase what exactly we are looking at for our tree (through preorder traversal).

#include "parser.h"

Parser::Parser(string fileName) { // Parser function that makes sure the file can be read
    string outputFileName = "parsertemp.dat";
    scanner.filterScan(fileName, outputFileName);

    file.open(outputFileName.c_str());
    if (!file.is_open()) { // Make sure the file is open and show an error if not
        cout << "ERROR: Can't open file!" << endl;
        exit(EXIT_FAILURE);
    }

    emptyToken.tokenInstance = "empty";
    scannerStatus = 0;
    getline(file, fileInput);
}

NodeT* Parser::parse() { // Function to continue are parsing and creation of our tree
    NodeT* root = NULL;
    nextScan(); // Scan for our tokens
    root = program(); // Start parsing and create the tree

    if (tk.tokenID != eofTk) { // Should read eof when we hit here, so display an error if not
        expectedToken.assign("eofTk");
        parserError();
        exit(EXIT_FAILURE);
    }
    else {
        cout << "PARSER: No errors found while parsing, checking semantics..." << endl;
        file.close();
        return root;
    }
}

void Parser::nextScan() { // Scanning our next tokens
    do {
        if (scannerStatus == -1) { // Make sure that our scanner status is ready to read a new line
            if (!getline(file, fileInput)) {
                if (file.eof()) { // If we reached the end of the file create our EOF token
                    scanner.invokeEOF(tk);
                    return;
                }
            }
        }

        scannerStatus = scanner.scan(fileInput, tk); // Go through the rest of our tokens

    } while (scannerStatus == -1);
}

NodeT* Parser::createNode(string labelName) { // Creation of our new node with it's name and null nodes under it
    NodeT* node = new NodeT;
    node->label = labelName;
    node->nodeOne = NULL;
    node->nodeTwo = NULL;
    node->nodeThree = NULL;
    node->nodeFour = NULL;
    node->nodeFive = NULL;
    return node;
}

NodeT* Parser::program() { // Our creation of the root
    NodeT* node = createNode("<program>");

    node->nodeOne = vars(); // Vars are read first

    if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "programToken")) { // Next should be a program token, if not we display an error
        nextScan();

    }
    else {
        expectedToken.assign("program");
        parserError();
    }

    node->nodeTwo = block(); // Blcok token should end our program statement

    return node;
}

NodeT* Parser::block() { // Creation of our block node
    NodeT* node = createNode("<block>");

    if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "startToken")) { // Blocks start with the start keyword, display an error if not found
        nextScan();

        node->nodeOne = vars(); // Vars and stats are found after the start keyword
        node->nodeTwo = stats();

        if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "stopToken")) { // Stop is used as a way to end the block we are on, error if not found
            nextScan();
            return node;

        }
        else {
            expectedToken.assign("stop");
            parserError();
        }
    }
    else {
        expectedToken.assign("start");
        parserError();
    }

    exit(EXIT_FAILURE); // Exit the program if we for some reason hit here
}

NodeT* Parser::vars() { // Our function for dealing with vars and creation of the node
    NodeT* node = createNode("<vars>");

    if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "declareToken")) { // Vars are made with a declare token
        nextScan();

        if (tk.tokenID == idTk) { // An identifier is next
            node->tokens.push_back(tk);
            nextScan();

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "equalToken")) { // An equal token is needed to declare an identifier to something
                nextScan();

                if (tk.tokenID == intTk) { // An integer is what is needed to declare an identifier to
                    node->tokens.push_back(tk);
                    nextScan();

                    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // It should end in a semi colon
                        nextScan();

                        node->nodeOne = vars(); // It would then add more vars necessary if needed
                        return node;

                    }
                    else { // Display errors if any of the parts needed for declaration were missed
                        expectedToken.assign(";");
                        parserError();
                    }
                }
                else {
                    expectedToken.assign("Integer");
                    parserError();
                }
            }
            else {
                expectedToken.assign("=");
                parserError();
            }
        }
        else {
            expectedToken.assign("Identifier");
            parserError();
        }
    }
    else {
        node->tokens.push_back(emptyToken);
        return node;
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::expr() { // Creation of our expression node and making sure order of operations is working
    NodeT* node = createNode("<expr>");

    node->nodeOne = N(); // Find our 'N' token

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "plusToken")) { // Afterwords should be a plus token
        node->tokens.push_back(tk);
        nextScan();

        node->nodeTwo = expr(); // Followed by an expression
    }

    return node; // Possible to return just N
}

NodeT* Parser::N() { // Creation of our N node
    NodeT* node = createNode("<N>");

    node->nodeOne = A(); // Find our 'A' token

    if (tk.tokenID == opTk) { // Either way we are looking for a divide or multiply token after
        if (allOperators[tk.tokenInstance] == "divideToken") {
            node->tokens.push_back(tk);
            nextScan();

            node->nodeTwo = N(); // Find 'N' token after divide or multiply

        }
        else if (allOperators[tk.tokenInstance] == "multiplyToken") {
            node->tokens.push_back(tk);
            nextScan();

            node->nodeTwo = N();
        }
    }

    return node; // possible to return just A
}

NodeT* Parser::A() { // Creation of our A node
    NodeT* node = createNode("<A>");

    node->nodeOne = M(); // First find our 'M' token

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "minusToken")) { // Minus operator should be found after
        node->tokens.push_back(tk);
        nextScan();

        node->nodeTwo = A(); // Find our 'A' token afterwords
    }

    return node; // Possible to just return M
}

NodeT* Parser::M() { // Creation of our M node
    NodeT* node = createNode("<M>");

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "dotToken")) { // Should be looking for a period
        node->tokens.push_back(tk);
        nextScan();

        node->nodeOne = M(); // Should be an M after that
        return node;
    }

    node->nodeOne = R(); // If a period was not found then we are looking for an R token instead
    return node;
}

NodeT* Parser::R() { // Creation of our R token
    NodeT* node = createNode("<R>");

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "leftParenToken")) { // If we find left parentheses we are looking for more than other options
        nextScan();

        node->nodeOne = expr(); // Afterwards we want an expression

        if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "rightParenToken")) { // Finally incase in parentheses completely
            nextScan();
            return node;
        }
        else { // If not we are displaying an error
            expectedToken.assign(")");
            parserError();
        }

    }
    else if (tk.tokenID == idTk) { // Could be an indentifier token instead
        node->tokens.push_back(tk);
        nextScan();
        return node;

    }
    else if (tk.tokenID == intTk) { // Could be an integer token instead as well
        node->tokens.push_back(tk);
        nextScan();
        return node;

    }
    else { // If none of these things we need to dipslay an error
        expectedToken.assign("( or Identifier or Integer");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::stats() { // Creation of our stats node
    NodeT* node = createNode("<stats>");

    node->nodeOne = stat(); // Stats creates both stat and mStat every time
    node->nodeTwo = mStat();

    return node;
}

NodeT* Parser::mStat() { // Creation of our mStat node
    NodeT* node = createNode("<mStat>");

    if (((tk.tokenID == keywordTk) && ( // Our mStat is looking to see if any of the keywords needed are next, otherwise it is possible for mStat to be empty
        (allKeywords[tk.tokenInstance] == "listenToken") || (allKeywords[tk.tokenInstance] == "talkToken") ||
        (allKeywords[tk.tokenInstance] == "ifToken") || (allKeywords[tk.tokenInstance] == "loopToken") ||
        (allKeywords[tk.tokenInstance] == "labelToken") || (allKeywords[tk.tokenInstance] == "jumpToken") ||
        (allKeywords[tk.tokenInstance] == "assignToken") || (allKeywords[tk.tokenInstance] == "startToken") ||
        (allKeywords[tk.tokenInstance] == "whileToken")))) {
        node->nodeOne = stat();
        node->nodeTwo = mStat();
        return node;

    }
    else { // In case we find none of these assume it is empty
        node->tokens.push_back(emptyToken);
        return node;
    }
}

NodeT* Parser::stat() { // Creation of our stat node
    NodeT* node = createNode("<stat>");

    if (tk.tokenID == keywordTk && !(allKeywords[tk.tokenInstance] == "startToken")) { // If we find a keyword that is not the start token
        if (allKeywords[tk.tokenInstance] == "listenToken") { // If we are looking at a listen keyword
            nextScan();

            node->nodeOne = in(); // Go to our listen function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
        }
        else if (allKeywords[tk.tokenInstance] == "talkToken") { // If we are looking at a talk keyword
            nextScan();

            node->nodeOne = out(); // Go to our talk function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
        }
        else if (allKeywords[tk.tokenInstance] == "ifToken") { // If we are looking at an if keyword
            nextScan();

            node->nodeOne = iff(); // Go to our if function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
            return node;
        }
        else if (allKeywords[tk.tokenInstance] == "whileToken") { // If we are looking at a while keyword
            nextScan();

            node->nodeOne = loop(); // Go to our while function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
            return node;
        }
        else if (allKeywords[tk.tokenInstance] == "labelToken") { // If we are looking at a label keyword
            nextScan();

            node->nodeOne = label(); // Go to our label function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
            return node;
        }
        else if (allKeywords[tk.tokenInstance] == "jumpToken") { // If we are looking at a jump keyword
            nextScan();

            node->nodeOne = gotoo(); // Go to our jump function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
            return node;
        }
        else if (allKeywords[tk.tokenInstance] == "assignToken") { // If we are looking at an assign keyword
            nextScan();

            node->nodeOne = assign(); // Go to our assign function

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "semiColonToken")) { // Semicolon should follow any of these options
                nextScan();
                return node;

            }
            else {
                expectedToken.assign(";");
                parserError();
            }
            return node;
        }
        else { // Error in case we find none of these things
            expectedToken.assign("listen or talk or if or while or label or jump or start");
            parserError();
        }
    }
    else if (tk.tokenID == keywordTk && allKeywords[tk.tokenInstance] == "startToken") { // Otherwise we need to make sure it is not the start of a block
        node->nodeOne = block();
        return node;

    }
    else {
        expectedToken.assign("listen or talk or if or while or label or jump or start");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::in() { // Function for our listen keyword
    NodeT* node = createNode("<in>");

    if (tk.tokenID == idTk) { // Listen should be followed by an identifier token
        node->tokens.push_back(tk);
        nextScan();
        return node;

    }
    else { // Error if no identifier is found
        expectedToken.assign("Identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::out() { // Function for our talk keyword
    NodeT* node = createNode("<out>");

    node->nodeOne = expr(); // We need an expression after talk
    return node;
}

NodeT* Parser::iff() { // Function for our if keyoword
    NodeT* node = createNode("<if>");

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "leftBracketToken")) { // If should start with a left bracket
        nextScan();

        node->nodeOne = expr(); // We then look for two expressions surrounding a relational operator
        node->nodeTwo = RO();
        node->nodeThree = expr();

        if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "rightBracketToken")) { // It ends with a right bracket
            nextScan();

            if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "thenToken")) { // Followed by a then keyword token
                nextScan();

                node->nodeFour = stat(); // Lastely ending with a stat

                if ((tk.tokenID == keywordTk) && (allKeywords[tk.tokenInstance] == "elseToken")) { // It is possible to end with else and another stat, but can end right before
                    nextScan();

                    node->nodeFive = stat(); // Looking for our stat after else
                    return node;

                }
                else { // If we are just done after then
                    return node;
                }
            }
            else { // Error cases if we miss a then, or brackets
                expectedToken.assign("then");
                parserError();
            }
        }
        else {
            expectedToken.assign("]");
            parserError();
        }
    }
    else {
        expectedToken.assign("[");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::loop() { // Creation of our loop node
    NodeT* node = createNode("<loop>");

    if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "leftBracketToken")) { // We are looking for a left bracket
        nextScan();

        node->nodeOne = expr(); // Like if, we look for two expressions surrounding a relational operator
        node->nodeTwo = RO();
        node->nodeThree = expr();

        if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "rightBracketToken")) { // After we finish encasing with brackets
            nextScan();

            node->nodeFour = stat(); // stat should be after
            return node;

        }
        else { // Errors for brackets as other errors dealt in other areas
            expectedToken.assign("]");
            parserError();
        }
    }
    else {
        expectedToken.assign("[");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::assign() { // Creation of our assign token
    NodeT* node = createNode("<assign>");

    if (tk.tokenID == idTk) { // We are looking for an identifier after assign
        node->tokens.push_back(tk);
        nextScan();

        if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "equalToken")) { // Then we make sure an equal sign is later
            nextScan();
            node->nodeOne = expr(); // Should be an expression afterwards
            return node;

        }
        else { // Error if no equal sign is found
            expectedToken.assign("=");
            parserError();
        }
    }
    else { // Error if no indeitifer is found
        expectedToken.assign("Identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::RO() { // Creation of our RO node
    NodeT* node = createNode("<RO>");

    if (tk.tokenID == opTk) { // Should find an operator token
        if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "lessThanToken")) { // Less than token is one of our options
            node->tokens.push_back(tk);
            nextScan();

            return node;

        }
        else if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "greaterThanToken")) { // Greater than token is one of our options
            node->tokens.push_back(tk);
            nextScan();

            return node;

        }
        else if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "equalEqualToken")) { // Two equal signs is another one of our options
            node->tokens.push_back(tk);
            nextScan();

            return node;

        }
        else if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "leftCurlyToken")) { // Left curly brace is the start of one of our options
            nextScan();

            if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "equalEqualToken")) { // It should be followed by an equal equal sign
                nextScan();

                if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "rightCurlyToken")) { // Lastely ending in a curly brace to incase it
                    tk.tokenInstance = "{==}";
                    node->tokens.push_back(tk);
                    nextScan();

                    return node; // Return our node after creating it
                }
                else { // Error if we never received the } part of the operator
                    expectedToken.assign("}");
                    parserError();
                }

            }
            else { // Error if we never cevied the == part of the operator
                expectedToken.assign("==");
                parserError();
            }
        }
        else if ((tk.tokenID == opTk) && (allOperators[tk.tokenInstance] == "modulusToken")) { // Another option is modulus
            node->tokens.push_back(tk);
            nextScan();

            return node;

        }
        else { // Error portions in case the operator is not allowed or no operator at all
            expectedToken.assign("> or < or == or {==} or %");
            parserError();
        }
    }
    else {
        expectedToken.assign("> or < or == or {==} or %");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::label() { // Creation of label node
    NodeT* node = createNode("<label>");

    if (tk.tokenID == idTk) { // After label we are looking for an identifier
        node->tokens.push_back(tk);
        nextScan();
        return node;

    }
    else { // Error our if we find no identifier
        expectedToken.assign("Identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

NodeT* Parser::gotoo() { // Creation of our jump node
    NodeT* node = createNode("<goto>");

    if (tk.tokenID == idTk) { // Afterwards should be an identifier
        node->tokens.push_back(tk);
        nextScan();
        return node;

    }
    else { // Error if we find no identifier after jump
        expectedToken.assign("Identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

void Parser::parserError() { // Our default function for displaying errors
    cout << "PARSER ERROR: Error at " << scanner.getScannerPosition() << " -> expected [" << expectedToken << "] but received [" << tk.tokenInstance << "]" << endl;
    file.close();
    exit(EXIT_FAILURE);
}
