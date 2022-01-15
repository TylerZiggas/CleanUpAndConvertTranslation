#include "semantics.h"

Semantics::Semantics(string fileName)
{
	string newFile = fileName.substr(0, fileName.find("."));
	outputFile.assign(newFile + ".asm");
}

void Semantics::verify(NodeT* node) {
	if (node == NULL) {
		return;
	}

	if (node->label == "<vars>") {
		for (unsigned int tokenCounter = 0; tokenCounter < node->tokens.size(); tokenCounter++) {
			if (node->tokens[tokenCounter].tokenID == idTk && node->tokens[tokenCounter].tokenInstance != "empty") {
				checkDeclaration(node->tokens[tokenCounter], node->tokens[tokenCounter + 1]);
			}
		}
	}
	else {
		vector<string> labels = { "<R>", "<in>", "<assign>", "<label>", "<goto>" };
		for (unsigned int labelCounter = 0; labelCounter < labels.size(); labelCounter++) {
			if (node->label == labels[labelCounter]) {
				for (unsigned int tokenCounter = 0; tokenCounter < node->tokens.size(); tokenCounter++) {
					if (node->tokens[tokenCounter].tokenID == idTk) {
						checkDefined(node->tokens[tokenCounter]);
					}
				}
			}
		}
	}

	verify(node->nodeOne);
	verify(node->nodeTwo);
	verify(node->nodeThree);
	verify(node->nodeFour);
	verify(node->nodeFive);
}

void Semantics::checkDeclaration(Token id, Token val) {
	int isDeclared = -1;
	for (unsigned int tableCounter = 0; tableCounter < st.size(); tableCounter++) {
		if (st[tableCounter].identifier.tokenInstance == id.tokenInstance) {
			isDeclared = tableCounter;
			break;
		}
	}

	if (isDeclared > -1) {
		cout << endl << "SEMANTICS ERROR: The variable [" << id.tokenInstance << "] at line (L " << id.lineNumber <<
			") -> has already been declared at line (L " << st[isDeclared].identifier.lineNumber << ")" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		tableHolder.identifier = id;
		tableHolder.value = val;
		st.push_back(tableHolder);
	}
}


void Semantics::checkDefined(Token tk) {
	int isDeclared = -1;
	for (unsigned int tableCounter = 0; tableCounter < st.size(); tableCounter++) {
		if (st[tableCounter].identifier.tokenInstance == tk.tokenInstance) {
			isDeclared = tableCounter;
			break;
		}
	}

	if (isDeclared < 0) {
		cout << endl << "SEMANTICS ERROR: The variable [" << tk.tokenInstance << "] at line (L " << tk.lineNumber << ") has not been declared" << endl;
		exit(EXIT_FAILURE);
	}
}

void Semantics::generation(NodeT* node)
{
	if (node == NULL) {
		return;
	}

	if (node->label == "<program>") {
		generation(node->nodeOne);
		generation(node->nodeTwo);

		file << "STOP" << endl;

		for (unsigned int counter = 0; counter < st.size(); counter++) {
			file << st[counter].identifier.tokenData << " " << st[counter].value.tokenData << endl;
		}
		for (unsigned int counter = 0; counter < tempVariableNumber; counter++) {
			file << "_V" << counter << " 0" << endl;
		}

		return;
	}

	if (node->label == "<block>") {
		generation(node->nodeOne);
		generation(node->nodeTwo);
		return;
	}

	if (node->label == "<vars>") {
		generation(node->nodeOne);
		return;
	}

	if (node->label == "<expr>") {
		if (node->tokens.empty()) {
			generation(node->nodeOne);
		}
		else {
			int variableNumber = tempVariableNumber++;

			generation(node->nodeTwo);
			file << "STORE _V" << variableNumber << endl;
			generation(node->nodeOne);

			if (allOperators[node->tokens[0].tokenData] == "plusToken") {
				file << "ADD _V" << variableNumber << endl;
			}
		}
		return;
	}

	if (node->label == "<N>") {
		if (node->tokens.empty()) {
			generation(node->nodeOne);
		}
		else {
			int variableNumber = tempVariableNumber++;

			generation(node->nodeTwo);
			file << "STORE _V" << variableNumber << endl;
			generation(node->nodeOne);

			if (allOperators[node->tokens[0].tokenData] == "divideToken") {
				file << "DIV _V" << variableNumber << endl;
			}
			else if (allOperators[node->tokens[0].tokenData] == "multiplyToken") {
				file << "MULT _V" << variableNumber << endl;
			}
		}
		return;
	}

	if (node->label == "<A>") {
		if (node->tokens.empty()) {
			generation(node->nodeOne);
		}
		else {
			int variableNumber = tempVariableNumber++;

			generation(node->nodeTwo);
			file << "STORE _V" << variableNumber << endl;
			generation(node->nodeOne);

			if (allOperators[node->tokens[0].tokenData] == "minusToken") {
				file << "SUB _V" << variableNumber << endl;
			}
		}
		return;
	}

	if (node->label == "<M>") {
		generation(node->nodeOne);
		if (!node->tokens.empty()) {
			file << "MULT -1" << endl;
		}
		return;
	}

	if (node->label == "<R>") {
		if (node->tokens.empty()) {
			generation(node->nodeOne);
		}
		else {
			Token tk = node->tokens[0];
			file << "LOAD " << tk.tokenData << endl;
		}
		return;
	}

	if (node->label == "<stats>") {
		generation(node->nodeOne);
		generation(node->nodeTwo);
		return;
	}

	if (node->label == "<mStat>") {
		if (node->tokens.empty()) {
			generation(node->nodeOne);
			generation(node->nodeTwo);
		}
		return;
	}

	if (node->label == "<stat>") {
		generation(node->nodeOne);
		return;
	}

	if (node->label == "<in>") {
		file << "READ " << node->tokens[0].tokenData << endl;
		return;
	}

	if (node->label == "<out>") {
		int variableNumber = tempVariableNumber++;

		generation(node->nodeOne);
		file << "STORE _V" << variableNumber << endl;
		file << "WRITE _V" << variableNumber << endl;

		return;
	}

	if (node->label == "<if>") {
		int variableNumber = tempVariableNumber++;

		generation(node->nodeOne);
		file << "STORE _V" << variableNumber << endl;

		if (node->nodeTwo->tokens[0].tokenData != "%") {
			generation(node->nodeThree);
			file << "SUB _V" << variableNumber << endl;
		} 

		if (node->nodeThree->nodeOne->nodeOne->nodeOne->tokens.empty()) {
			isNegative = false;
		} else {
			isNegative = true;
		}

		int exitNumber = labelsNumber;
		carryLabelNumber = labelsNumber++;
		generation(node->nodeTwo);
		generation(node->nodeFour);
		file << "_L" << exitNumber << ": NOOP" << endl;

		exitNumber = labelsNumber;
		carryLabelNumber = labelsNumber++;

		if (node->nodeFive != NULL) {
			generation(node->nodeOne);
			file << "STORE _V" << variableNumber << endl;

			if (node->nodeTwo->tokens[0].tokenData != "%") {
				generation(node->nodeThree);
				file << "SUB _V" << variableNumber << endl;
			}

			if (firstOption) {
				if (ifStatement == "lessThan") {
					file << "BRZPOS _L" << carryLabelNumber << endl;
				}
				else if (ifStatement == "equalEqual") {
					file << "BRZERO _L" << carryLabelNumber << endl;
				}
				else {
					file << "BRZNEG _L" << carryLabelNumber << endl;
				}
			}
			else {
				if (ifStatement == "greaterThan") {
					file << "BRZNEG _L" << carryLabelNumber << endl;
				}
				else if (ifStatement == "notEqual") {
					file << "BRNEG _L" << carryLabelNumber << endl;
					file << "BRPOS _L" << carryLabelNumber << endl;
				}
				else {
					file << "BRZPOS _L" << carryLabelNumber << endl;
				}
			}

			generation(node->nodeFive);
			file << "_L" << exitNumber << ": NOOP" << endl;
		}

		return;
	}

	if (node->label == "<loop>") {
		int loopNumber = labelsNumber++;
		int variableNumber = tempVariableNumber++;

		file << "_L" << loopNumber << ": NOOP" << endl;
		generation(node->nodeOne);
		file << "STORE _V" << variableNumber << endl;

		if (node->nodeTwo->tokens[0].tokenData != "%") {
			generation(node->nodeThree);
			file << "SUB _V" << variableNumber << endl;
		}

		if (node->nodeThree->nodeOne->nodeOne->nodeOne->tokens.empty()) {
			isNegative = false;
		} else {
			isNegative = true;
		}

		int exitNumber = labelsNumber;
		carryLabelNumber = labelsNumber++;
		generation(node->nodeTwo);
		generation(node->nodeFour);
		file << "BR _L" << loopNumber << endl;
		file << "_L" << exitNumber << ": NOOP" << endl;

		return;
	}

	if (node->label == "<assign>") {
		generation(node->nodeOne);
		file << "STORE " << node->tokens[0].tokenData << endl;
		return;
	}

	if (node->label == "<label>") {
		file << node->tokens[0].tokenData << ": NOOP" << endl;
	}

	if (node->label == "<goto>") {
		file << "BR " << node->tokens[0].tokenData << endl;
	}

	if (node->label == "<RO>") {

		if (allOperators[node->tokens[0].tokenData] == "lessThanToken") {
			file << "BRNEG _L" << carryLabelNumber << endl;
			firstOption = true;
			ifStatement = "lessThan";
		}

		else if (allOperators[node->tokens[0].tokenData] == "greaterThanToken") {
			file << "BRPOS _L" << carryLabelNumber << endl;
			firstOption = false;
			ifStatement = "greaterThan";
		}

		else if (allOperators[node->tokens[0].tokenData] == "equalEqualToken") {
			file << "BRNEG _L" << carryLabelNumber << endl;
			file << "BRPOS _L" << carryLabelNumber << endl;
			firstOption = true;
			ifStatement = "equalEqual";
		}

		else if (allOperators[node->tokens[0].tokenData] == "rightCurlyToken") {
			file << "BRZERO _L" << carryLabelNumber << endl;
			firstOption = false;
			ifStatement = "notEqual";
		}

		else if (allOperators[node->tokens[0].tokenData] == "modulusToken") {
			if (isNegative) {
				file << "BRPOS _L" << carryLabelNumber << endl;
				firstOption = true;
				ifStatement = "modulusPos";
			} else {
				file << "BRNEG _L" << carryLabelNumber << endl;
				firstOption = false;
				ifStatement = "modulusNeg";
			}
		}

		return;
	}
}


void Semantics::generateCode(NodeT* node)
{
	verify(node);
	cout << endl << "SEMANTICS: Processed successfully, generating code..." << endl;
	file.open(outputFile.c_str());
	generation(node);
	cout << endl << "GENERATION: Code generation complete..." << endl;
	cout << endl << "GENERATION: File output => \"" << outputFile << "\"" << endl;
	file.close();
}