/*
Name: Hoang Mai Han Dang, Yazi Zhang
Phase 2.1: Parser for Lexp
Description: This program implements a lexical scanner for Lexp,
            identifying identifiers, numbers, and symbols.
*/

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

const regex IDENTIFIER_REGEX("^[a-zA-Z][a-zA-Z0-9]*");
const regex NUMBER_REGEX("^[0-9]+");
const regex SYMBOL_REGEX("^[+\\-*/()]");
const regex WHITESPACE_REGEX(R"(\s+)");

struct Token {
    string type;
    string value;
};

struct ASTnode {
    string value;
    string type;
    shared_ptr<ASTnode> left;
    shared_ptr<ASTnode> right;

    ASTnode(string val, string t, shared_ptr<ASTnode> l = nullptr, shared_ptr<ASTnode> r = nullptr)
        : value(val), type(t) ,left(l), right(r) {}
};

bool isOnlyWhiteSpace(const string& line)
{
    return all_of(line.begin(), line.end(), [](char c) { return isspace(c); });
}

vector<Token> scanLine(const string& line)
{
    vector<Token> tokens;
    size_t index = 0;

    while (index < line.length())
    {
        char currChar = line[index];
        string remaining = line.substr(index);
        string token;
        smatch match;
        size_t tokenLength = 0;

        if (regex_search(remaining, match, WHITESPACE_REGEX) && match.position() == 0)
        {
            tokenLength = match.length();
            index += match.length();
            continue;
        }
        else if (regex_search(remaining, match, IDENTIFIER_REGEX))
        {
            tokenLength = match.length();
            token = match.str();
            tokens.push_back({"IDENTIFIER", token});
        }
        else if (regex_search(remaining, match, NUMBER_REGEX))
        {
            tokenLength = match.length();
            token = match.str();
            tokens.push_back({"NUMBER", token});
        }
        else if (regex_search(remaining, match, SYMBOL_REGEX))
        {
            tokenLength = match.length();
            token = match.str();
            tokens.push_back({"SYMBOL", token});
        }
        else
        {
            tokenLength = 1;
            tokens.push_back({"ERROR READING", string(1, currChar)});
            break;
        }
        /*
        when nothing matches, the length of match is 0
        ensure index always move forward
        */
        index += tokenLength;
    }

    return tokens;
}

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

shared_ptr<ASTnode> parseExpression(TokenStream& tokens);
shared_ptr<ASTnode> parseTerm(TokenStream& tokens);
shared_ptr<ASTnode> parseFactor(TokenStream& tokens);
shared_ptr<ASTnode> parsePiece(TokenStream& tokens);
shared_ptr<ASTnode> parseElement(TokenStream& tokens);

// FIX: MAKE A BETTER VALUE AND TYPE FOR AST NODE
shared_ptr<ASTnode> parseExpression(TokenStream& tokens) {
    auto node = parseTerm(tokens);
    while (tokens.peek().value == "+") {
        tokens.get();
        node = make_shared<ASTnode>("+", "SYMBOL", node, parseTerm(tokens));
    }
    return node;
}

shared_ptr<ASTnode> parseTerm(TokenStream& tokens) {
    auto node = parseFactor(tokens);
    while (tokens.peek().value == "-") {
        tokens.get();
        node = make_shared<ASTnode>("-",  "SYMBOL", node, parseFactor(tokens));
    }
    return node;
}

shared_ptr<ASTnode> parseFactor(TokenStream& tokens) {
    auto node = parsePiece(tokens);
    while (tokens.peek().value == "/") {
        tokens.get();
        node = make_shared<ASTnode>("/",  "SYMBOL", node, parsePiece(tokens));
    }
    return node;
}
shared_ptr<ASTnode> parsePiece(TokenStream& tokens) {
    auto node = parseElement(tokens);
    while (tokens.peek().value == "*") {
        tokens.get();
        node = make_shared<ASTnode>("*",  "SYMBOL", node, parseElement(tokens));
    }
    return node;
}
shared_ptr<ASTnode> parseElement(TokenStream& tokens) {
    Token token = tokens.get();
    if (token.type == "NUMBER" || token.type == "IDENTIFIER") {
        return make_shared<ASTnode> (token.value, token.type);
    } else if (token.value == "(") {
        auto node = parseExpression(tokens);
        tokens.get();
        return node;
    }

    return nullptr;
}

void printAST(const shared_ptr<ASTnode>& node, ofstream& outputFile, int depth = 0) {
    if (!node) return;
    outputFile << string (depth * 2, ' ') << node -> value << " : " << node -> type << endl;
    printAST(node->left, outputFile, depth + 1);
    printAST(node->right, outputFile, depth + 1);
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        /*
        Ensure that the program does not crash
        when required agruments are missing
        */
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

        outputFile << "Line: " << line << endl;
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
        shared_ptr<ASTnode> root = parseExpression(ts);
        outputFile << "AST:" << endl;
        printAST(root, outputFile);
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
