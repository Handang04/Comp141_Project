#include "LexpParser.h"
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <memory>
#include <utility>
#include <stack>

using namespace std;

int evaluateAST(shared_ptr<ASTnode> node) {
    if (!node -> right && !node -> left) {
        return stoi(node -> value);
    }
    int leftVal = evaluateAST(node -> left);
    int rightVal = evaluateAST(node -> right);

    if (node -> value == "+") {
        return leftVal + rightVal;
    } else if (node -> value == "-") {
        return leftVal - rightVal;
    } else if (node -> value == "*") {
        return leftVal * rightVal;
    } else if (node -> value == "/") {
        if (rightVal == 0) {
            throw runtime_error("Division by zero");
        }
        return leftVal / rightVal;
    }
    throw runtime_error("Unknown operator");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./LexpInterpreter <input_file> <output_file>" << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    ifstream inputFile(inputFilePath);
    ofstream outputFile(outputFilePath);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "ERROR OPENING FILE" << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        vector<Token> tokens = scanLine(line);
        outputFile << "Tokens:" << endl;
        for (const Token &token : tokens)
        {
            if (token.type == "ERROR READING")
            {
                outputFile << "ERROR READING: \"" + token.value + "\""  << endl;
            }
            else
            {
                outputFile << token.value << ": " << token.type << endl;
            }
        }
        outputFile << endl;
        
        TokenStream ts(tokens);
        shared_ptr<ASTnode> root = parseExpression(ts, outputFile);
        Token nextToken = ts.peek();
        if (nextToken.type != "End of File") {
            outputFile << "ERROR IN PARSER: Unexpected token after expression: " << nextToken.value << endl;
            outputFile << endl;
            outputFile.close();
            exit(1);
        }
        
        outputFile << "AST:" << endl;
        printAST(root, outputFile);
        outputFile << endl;
        
        try {
            int result = evaluateAST(root);
            outputFile << "Result: " << result << endl;
        } catch (const exception &e) {
            outputFile << "Evaluation Error: " << e.what() << endl;
        }
    }
    
    inputFile.close();
    outputFile.close();
    return 0;
}
