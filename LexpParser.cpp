/*
Name: Hoang Mai Han Dang, Yazi Zhang  
Phase 2.1: Parser for Lexp  
Description: This program implements a parser module for Lexp.  
             The parser processes a stream of tokens produced by the scanner  
             according to the language grammar.  
             The output of the parser is an Abstract Syntax Tree (AST) (a preorder traverse)
             that represents the parsed code structure.  
*/

#include "LexpScanner.h"
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <memory>
#include <utility>

using namespace std;

struct ASTnode {
    string value;
    string type;
    shared_ptr<ASTnode> left;
    shared_ptr<ASTnode> right;

    ASTnode(string val, string t, shared_ptr<ASTnode> l = nullptr, shared_ptr<ASTnode> r = nullptr)
        : value(val), type(t) ,left(l), right(r) {}
};

class TokenStream {
    public:
        vector<Token> tokens;
        size_t index = 0;

        TokenStream(vector<Token> token) : tokens(std::move(token)) {}
        // move(token) transfer token's contents to tokens without copying
        // it allows resources to be moved instead of copied
        // after the move, token is left in a valid but unspecified state

        Token peek() {
            if (index < tokens.size()) {
                return tokens[index]; // return the current token
            }
            else {
                return Token{"End of File", ""};
            }
        }

        Token get() {
            if (index < tokens.size()) {
                return tokens[index++]; // return the current token then increase the index by 1
            }
            else {
                return Token{"End of File", ""};
            }
        }
};

/*
Grammar for Lexp:
expression ::= term { + term }
term ::= factor { - factor }
factor ::= piece { / piece }
piece ::= element { * element }
element ::= ( expression ) | NUMBER | IDENTIFIER
*/

shared_ptr<ASTnode> parseExpression(TokenStream& tokens, ofstream& outputFile);
shared_ptr<ASTnode> parseTerm(TokenStream& tokens, ofstream& outputFile);
shared_ptr<ASTnode> parseFactor(TokenStream& tokens, ofstream& outputFile);
shared_ptr<ASTnode> parsePiece(TokenStream& tokens, ofstream& outputFile);
shared_ptr<ASTnode> parseElement(TokenStream& tokens, ofstream& outputFile);

shared_ptr<ASTnode> parseExpression(TokenStream& tokens, ofstream& outputFile) {
    auto node = parseTerm(tokens, outputFile);
    while (tokens.peek().value == "+") {
        tokens.get();
        node = make_shared<ASTnode>("+", "SYMBOL", node, parseTerm(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseTerm(TokenStream& tokens, ofstream& outputFile) {
    auto node = parseFactor(tokens, outputFile);
    while (tokens.peek().value == "-") {
        tokens.get();
        node = make_shared<ASTnode>("-", "SYMBOL", node, parseFactor(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseFactor(TokenStream& tokens, ofstream& outputFile) {
    auto node = parsePiece(tokens, outputFile);
    while (tokens.peek().value == "/") {
        tokens.get();
        node = make_shared<ASTnode>("/", "SYMBOL", node, parsePiece(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parsePiece(TokenStream& tokens, ofstream& outputFile) {
    auto node = parseElement(tokens, outputFile);
    while (tokens.peek().value == "*") {
        tokens.get();
        node = make_shared<ASTnode>("*", "SYMBOL", node, parseElement(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseElement(TokenStream& tokens, ofstream& outputFile) {
    Token token = tokens.get();
    if (token.type == "NUMBER" || token.type == "IDENTIFIER") {
        return make_shared<ASTnode>(token.value, token.type);
    } else if (token.value == "(") {
        auto node = parseExpression(tokens, outputFile);
        if (tokens.get().value != ")") {
            outputFile << "ERROR IN PARSER: Expected closing parenthesis but only found: " << token.value << endl;
            outputFile.close();
            exit(1);
        }
        return node;
    }

    outputFile << "ERROR IN PARSER: Unexpected token: " << token.value << endl;
    outputFile.close();
    exit(1);
}

void printAST(const shared_ptr<ASTnode>& node, ofstream& outputFile, int depth = 0) {
    if (!node) return;
    outputFile << string(depth * 2, ' ') << node->value << " : " << node->type << endl;
    printAST(node->left, outputFile, depth + 1);
    printAST(node->right, outputFile, depth + 1);
}

/*
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        
        // Ensure that the program does not crash
        // when required agruments are missing
        
        cout << "Type: ./LexpParser <input_file> <output_file>" << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    string line;

    ifstream inputFile(inputFilePath);
    ofstream outputFile(outputFilePath);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        cerr << "ERROR OPENING FILE" << endl;
        return 1;
    }

    while (getline(inputFile, line))
    {
        if (isOnlyWhiteSpace(line))
        {
            continue;
        }
        
        outputFile << "Tokens: " << endl;
        vector<Token> tokens = scanLine(line);

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
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
*/
