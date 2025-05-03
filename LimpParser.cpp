/*
Name: Hoang Mai Han Dang, Yazi Zhang  
Phase 2.2: Parser for Limp  
Description: This program implements a parser module for Limp.  
             The parser processes a stream of tokens produced by the scanner  
             according to the language grammar.  
             The output of the parser is an Abstract Syntax Tree (AST) (a preorder traverse)
             that represents the parsed code structure.  
*/

#include "LimpScanner.h"
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

const regex IDENTIFIER_REGEX("^[a-zA-Z][a-zA-Z0-9]*");
const regex NUMBER_REGEX("^[0-9]+");
const regex SYMBOL_REGEX(R"(\+|\-|\*|\/|\(|\)|\;|\:=)");
const regex KEYWORD_REGEX(R"(\b(if|then|else|endif|while|do|endwhile|skip)\b)");
const regex WHITESPACE_REGEX(R"(\s+)");

struct ASTnode
{
    string value;
    string type;
    shared_ptr<ASTnode> left;
    shared_ptr<ASTnode> right;
    shared_ptr<ASTnode> clone() const {
        /*
        First, create a new ASTnode with the same value and type with the current node, null pointer for left and right
        If the current node has a left child (not nullptr),
        it calls clone() on that left child and assigns the result to the new node's left pointer.
        Similarly, if there's a right child,
        it calls clone() on that right child and assigns the result to the new node's right pointer.
        */
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

/*
Grammar of Limp is defined as follows:
statement ::= basestatement { ; basestatement }
basestatement ::= assignment | ifstatement | whilestatement | skip
assignmet ::= IDENTIFIER := expression
ifstatement ::= if expression then statement else statement endif
whilestatement ::= while expression do statement endwhile

expression ::= term { + term }
term ::= factor { - factor }
factor ::= piece { / piece }
piece ::= element { * element }
element ::= ( expression ) | NUMBER | IDENTIFIER
*/

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

shared_ptr<ASTnode> parseStatement(TokenStream &tokens, ofstream &outputFile)
{
    auto node = parseBaseStatement(tokens, outputFile);
    while (tokens.peek().value == ";")
    {
        tokens.get();
        node = make_shared<ASTnode>(";", "SYMBOL", node, parseBaseStatement(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseBaseStatement(TokenStream &tokens, ofstream &outputFile)
{
    Token token = tokens.peek();
    if (token.type == "IDENTIFIER")
    {
        return parseAssignment(tokens, outputFile);
    }
    else if (token.value == "if")
    {
        return parseIfStatement(tokens, outputFile);
    }
    else if (token.value == "while")
    {
        return parseWhileStatement(tokens, outputFile);
    }
    else if (token.value == "skip")
    {
        tokens.get();
        return make_shared<ASTnode>("skip", "KEYWORD");
    }
    outputFile << "ERROR IN PARSER: Unexpected statement: " << token.value << endl;
    outputFile.close();
    exit(1);
}

shared_ptr<ASTnode> parseAssignment(TokenStream &tokens, ofstream &outputFile)
{
    Token id = tokens.get();
    if (tokens.get().value != ":=")
    {
        outputFile << "ERROR IN PARSER: Expected ':=' symbol in assignment \"" << id.value << "\""<< endl;
        outputFile.close();
        exit(1);
    }
    return make_shared<ASTnode>(":=", "SYMBOL", make_shared<ASTnode>(id.value, "IDENTIFIER"), parseExpression(tokens, outputFile));
}

shared_ptr<ASTnode> parseIfStatement(TokenStream &tokens, ofstream &outputFile)
{
    tokens.get();
    auto condition = parseExpression(tokens, outputFile);
    if (tokens.get().value != "then")
    {
        outputFile << "ERROR IN PARSER: Expected 'then' in if statement, but found \"" << tokens.peek().value << "\" instead."<< endl;
        outputFile.close();
        exit(1);
    }

    auto thenBranch = parseStatement(tokens, outputFile);
    if (tokens.get().value != "else")
    {
        outputFile << "ERROR IN PARSER: Expected 'else' in if statement, but found \"" << tokens.peek().value << "\" instead."<< endl;
        outputFile.close();
        exit(1);
    }

    auto elseBranch = parseStatement(tokens, outputFile);
    if (tokens.get().value != "endif")
    {
        outputFile << "ERROR IN PARSER: Expected 'endif' in if statement, but found \"" << tokens.peek().value << "\" instead."<< endl;
        outputFile.close();
        exit(1);
    }

    // Create a special structure for the AST to handle the if statement's three branches
    auto ifNode = make_shared<ASTnode>("if", "IF-STATEMENT");
    ifNode->left = condition;
    // Create a dummy node to hold the then and else branches
    auto dummyNode = make_shared<ASTnode>("", "");
    dummyNode->left = thenBranch;
    dummyNode->right = elseBranch;

    ifNode->right = dummyNode;

    return ifNode;
}

shared_ptr<ASTnode> parseWhileStatement(TokenStream &tokens, ofstream &outputFile)
{
    // Consume "while"
    tokens.get();
    auto condition = parseExpression(tokens, outputFile);

    Token doToken = tokens.peek();
    if (doToken.value != "do")
    {
        outputFile << "ERROR IN PARSER: Expected 'do' in while statement, but found \"" << doToken.value << "\" instead." << endl;
        outputFile.close();
        exit(1);
    }
    // Consume "do"
    tokens.get();

    auto body = parseStatement(tokens, outputFile);
    Token endToken = tokens.peek();
    if (endToken.value != "endwhile")
    {
        outputFile << "ERROR IN PARSER: Expected 'endwhile' to close while loop but found \"" << endToken.value << "\" instead." << endl;
        outputFile.close();
        exit(1);
    }
    // Consume "endwhile"
    tokens.get();

    return make_shared<ASTnode>("while", "WHILE-LOOP", condition, body);
}

shared_ptr<ASTnode> parseExpression(TokenStream &tokens, ofstream &outputFile)
{
    if (tokens.peek().value == ")")
    {
        outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
        outputFile.close();
        exit(1);
    }

    auto node = parseTerm(tokens, outputFile);
    while (tokens.peek().value == "+")
    {
        tokens.get();

        if (tokens.peek().value == ")")
        {
            outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
            outputFile.close();
            exit(1);
        }
        node = make_shared<ASTnode>("+", "SYMBOL", node, parseTerm(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseTerm(TokenStream &tokens, ofstream &outputFile)
{
    if (tokens.peek().value == ")")
    {
        outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
        outputFile.close();
        exit(1);
    }

    auto node = parseFactor(tokens, outputFile);
    while (tokens.peek().value == "-")
    {
        tokens.get();

        if (tokens.peek().value == ")")
        {
            outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
            outputFile.close();
            exit(1);
        }

        node = make_shared<ASTnode>("-", "SYMBOL", node, parseFactor(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseFactor(TokenStream &tokens, ofstream &outputFile)
{
    if (tokens.peek().value == ")")
    {
        outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
        outputFile.close();
        exit(1);
    }

    auto node = parsePiece(tokens, outputFile);
    while (tokens.peek().value == "/")
    {
        tokens.get();

        if (tokens.peek().value == ")")
        {
            outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
            outputFile.close();
            exit(1);
        }

        node = make_shared<ASTnode>("/", "SYMBOL", node, parsePiece(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parsePiece(TokenStream &tokens, ofstream &outputFile)
{
    if (tokens.peek().value == ")")
    {
        outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
        outputFile.close();
        exit(1);
    }

    auto node = parseElement(tokens, outputFile);
    while (tokens.peek().value == "*")
    {
        tokens.get();

        if (tokens.peek().value == ")")
        {
            outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
            outputFile.close();
            exit(1);
        }

        node = make_shared<ASTnode>("*", "SYMBOL", node, parseElement(tokens, outputFile));
    }
    return node;
}

shared_ptr<ASTnode> parseElement(TokenStream &tokens, ofstream &outputFile)
{
    Token token = tokens.get();
    if (token.type == "NUMBER" || token.type == "IDENTIFIER")
    {
        return make_shared<ASTnode>(token.value, token.type);
    }
    else if (token.value == "(")
    {
        auto node = parseExpression(tokens, outputFile);

        token = tokens.get();
        if (token.value != ")")
        {
            outputFile << "ERROR IN PARSER: Expected closing parenthesis but only found: " << token.value << endl;
            outputFile.close();
            exit(1);
        }
        return node;
    }
    else if (token.value == ")")
    {
        outputFile << "ERROR IN PARSER: Unexpected closing parenthesis with no matching opening parenthesis" << endl;
        outputFile.close();
        exit(1);
    }

    outputFile << "ERROR IN PARSER: Unexpected token: " << token.value << endl;
    outputFile.close();
    exit(1);
}

void printAST(const shared_ptr<ASTnode> &node, ofstream &outputFile, int depth = 0)
{
    if (!node)
        return;

    // Special case for the dummy node in IF-STATEMENT
    if (node->type == "" && node->value == "")
    {
        // This is the dummy node holding the then and else branches
        // Print both branches at the current depth
        printAST(node->left, outputFile, depth);
        printAST(node->right, outputFile, depth);
        return;
    }
    if (node->value == "if")
    {
        outputFile << string(depth * 4, ' ') << node->type << endl;
        printAST(node->left, outputFile, depth + 1); // Print condition

        // Check if right child is our special dummy node
        if (node->right && node->right->type == "" && node->right->value == "")
        {
            // Print the then and else branches directly at the correct depth
            printAST(node->right->left, outputFile, depth + 1);
            printAST(node->right->right, outputFile, depth + 1);
            return; // Skip the normal right child processing
        }
    }
    else if (node->value == "while")
    {
        outputFile << string(depth * 4, ' ') << node->type << endl;
        printAST(node->left, outputFile, depth + 1);
        printAST(node->right, outputFile, depth + 1);
        return;
    }
    else
    {
        // General case: print "TYPE VALUE"
        outputFile << string(depth * 4, ' ') << node->type << " " << node->value << endl;
        printAST(node->left, outputFile, depth + 1);
        printAST(node->right, outputFile, depth + 1);
    }
}

/*
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        
        Ensure that the program does not crash
        when required agruments are missing
        
        cout << "Type: ./LimpParser <input_file> <output_file>" << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    string line;
    string fullInput;

    ifstream inputFile(inputFilePath);
    ofstream outputFile(outputFilePath);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        cerr << "ERROR OPENING FILE" << endl;
        return 1;
    }

    outputFile << "Tokens: " << endl;

    while (getline(inputFile, line))
    {
        if (isOnlyWhiteSpace(line))
        {
            continue;
        }
        vector<Token> tokens = scanLine(line);

        for (const Token &token : tokens)
        {
            if (token.type == "ERROR READING")
            {
                outputFile << "ERROR READING: \"" + token.value + "\"" << endl;
            }
            else
            {
                outputFile << token.value << ": " << token.type << endl;
            }
        }
        fullInput += line + " ";
    }

    vector<Token> tokens = scanLine(fullInput);
    TokenStream ts(tokens);
    shared_ptr<ASTnode> root = parseStatement(ts, outputFile);
    Token nextToken = ts.peek();
    if (nextToken.type != "End of File")
    {
        outputFile << "ERROR IN PARSER: Unexpected token: \"" << nextToken.value << "\" after expression"<< endl;
        outputFile << endl;
        outputFile.close();
        exit(1);
    }

    outputFile << endl;
    outputFile << "AST:" << endl;
    printAST(root, outputFile);
    outputFile << endl;

    inputFile.close();
    outputFile.close();

    return 0;
}
*/