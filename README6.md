Limp Interpreter - Phase 3.1
=======================
Author: Hoang Mai Han Dang, Yazi Zhang

Compiler Information:
---------------------
This program was tested using the GNU Compiler Collection (G++).
Compiler version: g++ (MinGW-w64 or GCC on Linux/Mac)

Build Instructions:
-------------------
To compile the program, use the following command in the terminal:

    g++ -std=c++11 LimpScanner.cpp LimpParser.cpp LimpInterpreter.cpp -o LimpInterpreter

This will generate an executable named "LimpInterpreter".

Run Instructions:
-----------------
To execute the program, provide an input file and an output file:

    ./LimpInterpreter input_file output_file

Example:
--------
If you have an input file `sample.txt`, run:

    ./LimpInterpreter sample.txt output.txt

This will process the input file, generate tokens, build an AST, evaluate the program statements, and write the output to output.txt.

The output file will include:
    - The list of tokens in order, one per line, reporting the token type and value
    - The generated Abstract Syntax Tree (AST)
    - The final values of all variables after execution completes

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

Language Features:
---------------
The Limp language supports:
    - Variable assignment (:=)
    - Arithmetic operations (+, -, *, /)
    - Control flow statements:
    - If-then-else conditional execution
    - While loops
    - Integer arithmetic (no floating-point support)
    - Non-negative integers only (subtraction resulting in a negative value returns 0)

Notes:
---------------
Ensure that your input file contains valid Limp statements.

The evaluator uses a recursive approach to evaluate the AST, maintaining a memory store for variable values.

The interpreter evaluates arithmetic expressions fully before using their values in assignments or control flow decisions.

For assignments, the value of the right-hand expression is stored in the variable on the left-hand side.

Integer division is used (e.g., 3/2 evaluates to 1).

Subtraction operations that would result in negative numbers instead return 0, as the language does not support negative values.

If statements execute the "then" branch if the condition evaluates to a value greater than 0, otherwise the "else" branch is executed.

While loops continue execution as long as the condition evaluates to a value greater than 0.

Review the output file to verify tokens, AST, and final variable values are correctly generated.

In case of errors, the output file will provide details of what went wrong.

End of Document
