#ifndef LIMP_PARSER_H
#define LIMP_PARSER_H

#include "LimpScanner.h"
#include <string>
#include <vector>
#include <fstream>
#include <memory>

using namespace std;

struct ASTnode
{
    string value;
    string type;
    shared_ptr<ASTnode> left;
    shared_ptr<ASTnode> right;
    shared_ptr<ASTnode> clone() const {
        auto newNode = make_shared<ASTnode>(value, type);
        if (left) newNode->left = left->clone();
        if (right) newNode->right = right->clone();
        return newNode;
    }

    ASTnode(string val, string t, shared_ptr<ASTnode> l = nullptr, shared_ptr<ASTnode> r = nullptr)
        : value(val), type(t), left(l), right(r) {}
};

class TokenStream
{
public:
    vector<Token> tokens;
    size_t index = 0;

    TokenStream(vector<Token> token) : tokens(std::move(token)) {}
    // move(token) transfer token's contents to tokens without copying
    // it allows resources to be moved instead of copied
    // after the move, token is left in a valid but unspecified state

    Token peek()
    {
        if (index < tokens.size())
        {
            return tokens[index]; // return the current token
        }
        else
        {
            return Token{"End of File", ""};
        }
    }

    Token get()
    {
        if (index < tokens.size())
        {
            return tokens[index++]; // return the current token then increase the index by 1
        }
        else
        {
            return Token{"End of File", ""};
        }
    }
};

shared_ptr<ASTnode> parseStatement(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseBaseStatement(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseAssignment(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseIfStatement(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseWhileStatement(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseExpression(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseTerm(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseFactor(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parsePiece(TokenStream &tokens, ofstream &outputFile);
shared_ptr<ASTnode> parseElement(TokenStream &tokens, ofstream &outputFile);

void printAST(const shared_ptr<ASTnode> &node, ofstream &outputFile, int depth = 0);

#endif 