Lexp Parser - Phase 2.1
=======================
Author: Hoang Mai Han Dang, Yazi Zhang

Compiler Information:
---------------------
This program was tested using the GNU Compiler Collection (G++).
Compiler version: g++ (MinGW-w64 or GCC on Linux/Mac)

Build Instructions:
-------------------
To compile the program, use the following command in the terminal:

    g++ -std=c++11 LexpScanner.cpp LexpParser.cpp -o LexpParser

This will generate an executable named "LexpParser".

Run Instructions:
-----------------
To execute the program, provide an input file and an output file:

    ./LexpParser input_file output_file

Example:
--------
If you have an input file `sample.txt`, run:

    ./LexpParser sample.txt output.txt

This will process the input file, generate tokens, build an AST, and write the output to output.txt.

The output file will include:
    - The list of tokens in order, one per line, reporting the token type and value.
    - The generated AST (preorder traverse)

Error Handling:
---------------
If the scanner encounters an invalid character, it will print an error message in the output file and terminate the program.

If the parser encounters a syntax error, it will also write an error message in the output file and terminate the program.

Common errors include:

    - Invalid characters in input

    - Missing closing parentheses

    - Unexpected tokens

Notes:
---------------
Ensure that your input file contains valid Lexp expressions.

Review the output file to verify both tokens and AST are correctly generated.

In case of errors, the output file will provide details of what went wrong.


End of Document