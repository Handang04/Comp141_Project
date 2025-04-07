/*
Name: Hoang Mai Han Dang, Yazi Zhang  
Phase 3.1: Interpreter for Lexp  
Description: This program implements an evaluator module for Lexp.  
             The evaluator receives the Abstract Syntax Tree (AST) from the parser  
             and performs a pre-order traversal to evaluate expressions.  
             Using a stack-based approach, it pushes nodes onto a stack and evaluates  
             when possible (when the top three elements are two numbers followed by an operator).  
             The final result of evaluation is returned as a single numeric value.  
*/

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

/*
// THIS IS NOT STACK YET
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
*/

int evaluateAST(shared_ptr<ASTnode> root) {
    stack<shared_ptr<ASTnode>> evalStack; // Stack for evaluation
    stack<shared_ptr<ASTnode>> traversalStack;// Stack for traversal (to implement pre-order traversal iteratively)
    
    // Push the root to start traversal
    if (root) {
        traversalStack.push(root);
    }
    
    // Pre-order traversal
    while (!traversalStack.empty()) {
        // Get the next node in pre-order
        shared_ptr<ASTnode> current = traversalStack.top();
        traversalStack.pop();
        
        // Push right child first
        // so it's processed after the left child
        if (current->right) {
            traversalStack.push(current->right);
        }
        if (current->left) {
            traversalStack.push(current->left);
        }
        
        // Push current node to evaluation stack
        evalStack.push(current);
        
        // Check if we can evaluate the top three elements
        while (evalStack.size() >= 3) {
            // Get the top three elements without popping
            shared_ptr<ASTnode> top1 = evalStack.top();
            evalStack.pop();
            shared_ptr<ASTnode> top2 = evalStack.top();
            evalStack.pop();
            shared_ptr<ASTnode> top3 = evalStack.top();
            evalStack.pop();
            
            // Check if we can evaluate (top two are numbers, third is an operator)
            bool canEvaluate = false;
            if ((top1->type == "NUMBER" || top1->type == "EVALUATED") && 
                (top2->type == "NUMBER" || top2->type == "EVALUATED") &&
                top3->type == "SYMBOL") {
                
                // Convert string values to integers
                int num1 = stoi(top1->value);
                int num2 = stoi(top2->value);
                int result = 0;
                
                // Apply the operator
                if (top3->value == "+") {
                    result = num2 + num1;  // Note: stack order reverses operands
                } else if (top3->value == "-") {
                    result = num2 - num1;
                } else if (top3->value == "*") {
                    result = num2 * num1;
                } else if (top3->value == "/") {
                    if (num1 == 0) {
                        throw runtime_error("Division by zero");
                    }
                    result = num2 / num1;
                } else {
                    throw runtime_error("Unknown operator");
                }
                
                // Create a new node with the result and push it back
                evalStack.push(make_shared<ASTnode>(to_string(result), "EVALUATED"));
                canEvaluate = true;
            }
            
            // If we couldn't evaluate, push the elements back in reverse order
            if (!canEvaluate) {
                evalStack.push(top3);
                evalStack.push(top2);
                evalStack.push(top1);
                break;
            }
        }
    }
    
    // the stack should have exactly one element with the final result
    if (evalStack.size() != 1) {
        throw runtime_error("Invalid expression: evaluation did not result in a single value");
    }
    
    // return the final result
    return stoi(evalStack.top()->value);
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
        if (isOnlyWhiteSpace(line))
        {
            continue;
        }
        vector<Token> tokens = scanLine(line);
        outputFile << "Tokens:" << endl;
        for (const Token &token : tokens)
        {
            if (token.type == "ERROR READING")
            {
                outputFile << "ERROR READING: \"" + token.value + "\""  << endl;
                exit(1);
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
        
        try {
            int result = evaluateAST(root);
            outputFile << "Result: " << result << endl;
            outputFile << endl;
        } catch (const exception &e) {
            outputFile << "Evaluation Error: " << e.what() << endl;
            exit(1);
        }
    }
    
    inputFile.close();
    outputFile.close();
    return 0;
}
