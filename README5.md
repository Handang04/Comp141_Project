Limp Parser - Phase 3.1
=======================
Author: Hoang Mai Han Dang, Yazi Zhang

Compiler Information:
---------------------
This program was tested using the GNU Compiler Collection (G++).
Compiler version: g++ (MinGW-w64 or GCC on Linux/Mac)

Build Instructions:
-------------------
To compile the program, use the following command in the terminal:

    g++ -std=c++11 LexpScanner.cpp LexpParser.cpp LexpInterpreter.cpp -o LexpInterpreter

This will generate an executable named "LexpInterpreter".

Run Instructions:
-----------------
To execute the program, provide an input file and an output file:

    ./LexpInterpreter input_file output_file

Example:
--------
If you have an input file `sample.txt`, run:

    ./LexpInterpreter sample.txt output.txt

This will process the input file, generate tokens, build an AST, evaluate the expressions, and write the output to output.txt.

The output file will include:
    - The list of tokens in order, one per line, reporting the token type and value
    - The generated Abstract Syntax Tree (AST) in preorder traversal
    - The result of evaluating the expression

Error Handling:
---------------
The interpreter handles various types of errors:
    - Lexical Errors: If the scanner encounters an invalid character, it will print an error message in the output file.
    - Syntax Errors: If the parser encounters a syntax error, it will write an error message in the output file.
    - Runtime Errors: If the evaluator encounters errors during expression evaluation (such as division by zero), it will report these in the output file.

Common errors include:
    - Invalid characters in input
    - Missing closing parentheses
    - Unexpected tokens
    - Division by zero
    - Invalid operations

Notes:
---------------
Ensure that your input file contains valid Lexp expressions.

The evaluator uses a stack-based approach with pre-order traversal of the AST.

When evaluating expressions, if the top three elements of the stack are two numbers followed by an operator, they are popped and the result is pushed back.

Integer division is used (e.g., 3/2 evaluates to 1).

Review the output file to verify tokens, AST, and evaluation results are correctly generated.

In case of errors, the output file will provide details of what went wrong.


End of Document
