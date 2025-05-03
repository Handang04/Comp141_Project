#include "LimpParser.h"
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
#include <map>
#include <stdexcept>

using namespace std;

class Evaluator {
    private:
    shared_ptr<ASTnode> ast;
    // The memory is implemented as a map that associates variable names (strings) with their integer values
    map<string, int> memory;

    int evaluateExpression(shared_ptr<ASTnode> node) {
        stack<int> s;
        evaluateExpressionHelper(node, s);

        if (s.empty()) {
            throw runtime_error("Expression evaluation resulted in empty stack");
        }
        
        return s.top();
    }

    void evaluateExpressionHelper(shared_ptr<ASTnode> node, stack<int>& s) {
        if (!node) {
            return;
        }

        if (node->type == "NUMBER") {
            s.push(stoi(node->value));
        }
        else if (node->type == "IDENTIFIER") {
            if (memory.find(node->value) == memory.end()) {
                // If the element isn't found, it returns memory.end()
                // It will return true if the variable doesn't exist in memory
                // (the search reached the end without finding it)
                throw runtime_error("Undefined variable: " + node->value);
            }
            s.push(memory[node->value]);
        }
        else if (node->type == "SYMBOL") {
            evaluateExpressionHelper(node->left, s);
            evaluateExpressionHelper(node->right, s);

            int right;
            int left;
            right = s.top(); 
            s.pop();
            left = s.top();
            s.pop();

            if (node->value == "+") {
                s.push(left + right);
            }
            else if (node->value == "-") {
                s.push(left - right);
            }
            else if (node->value == "*") {
                s.push(left * right);
            }
            else if (node->value == "/") {
                if (right == 0) {
                    throw runtime_error("Division by zero");
                }
                s.push(left / right);
            }
            else {
                throw runtime_error("Unknown operator: " + node->value);
            }
        }
        else {
            throw runtime_error("Invalid node type in expression: " + node->type);
        }
    }

    shared_ptr<ASTnode> evaluateStatement(shared_ptr<ASTnode> node) {
        if (!node) {
            return nullptr;
        }

        if (node->type == "SYMBOL" && node->value == ":=") {
            string identifier = node->left->value;
            int value = evaluateExpression(node->right);

            // The [] operation on a map does 2 things:
            // If identifier already exists as a key in the map, it returns a reference to its corresponding value
            // If identifier doesn't exist yet, it creates a new key-value pair with a default-initialized value (0 for integers)
            // The = operator then assigns the new value to the map entry for that key
            memory[identifier] = value;
            return nullptr;
        }
        else if (node->type == "IF-STATEMENT") {
            /*
            The structure of IF-STATEMENT:
            ifNode->left = condition
            ifNode-> right = dummyNode
            dummyNode->left = thenBranch
            dummyNode->right = elseBranch
            */
            int condition = evaluateExpression(node->left);
            if (condition > 0) {
                return node->right->left->clone(); // True condition, replace with then branch
            } else {
                return node->right->right->clone(); // False condition, replace with else branch
            }
        }
        else if (node->type == "WHILE-LOOP") {
            /*
            The structure of WHILE-STATEMENT:
            left = condition
            right = body
            */
            int condition = evaluateExpression(node->left);
            if (condition > 0) {
                return make_shared<ASTnode>(";", "SYMBOL", node->right->clone(), node->clone());
                /*
                First executes the body of the loop (the node->right->clone() part)
                Then comes back to evaluate the entire while loop again (the node->clone() part)
                This continues until the condition becomes false
                */
            } else {
                return nullptr;
            }
        }
        else if (node->type == "KEYWORD" && node->value == "skip") {
            // Skip statement does nothing
            return nullptr;
        }
        else if (node->type == "SYMBOL" && node->value == ";") {
            auto newLeft = evaluateStatement(node->left);
            if (newLeft) {
                // Left branch transformed to a new subtree
                return make_shared<ASTnode>(";", "SYMBOL", newLeft, node->right->clone());
            } else {
                // Left branch was completely evaluated (removed)
                return node->right->clone();
            }
        }
        else {
            throw runtime_error("Invalid statement type: " + node->type + " " + node->value);
        }
    }

    public:
        Evaluator(shared_ptr<ASTnode> tree) : ast(tree) {}

        void evaluate() {
            while (ast) {
                auto newAst = evaluateStatement(ast);
                ast = newAst;
            }
        }

        map<string, int> getMemory() const {
            return memory;
        }
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./LimpInterpreter <input_file> <output_file>" << endl;
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

    try {
        Evaluator evaluator(root);
        evaluator.evaluate();
        
        // Output the final memory state
        outputFile << "Output:" << endl;
        map<string, int> memory = evaluator.getMemory();
        for (const auto& [var, val] : memory) {
            outputFile << var << " = " << val << endl;
        }
    } catch (const exception &e) {
        outputFile << "Evaluation Error: " << e.what() << endl;
        exit(1);
    }

    
    inputFile.close();
    outputFile.close();
    return 0;
}