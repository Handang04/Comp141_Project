Lexp Scanner - Phase 1
=======================
Author: Hoang Mai Han Dang, Yazi Zhang

Compiler Information:
---------------------
This program was tested using the GNU Compiler Collection (G++).
Compiler version: g++ (MinGW-w64 or GCC on Linux/Mac)

Build Instructions:
-------------------
To compile the program, use the following command in the terminal:

    g++ -o LexpScanner LexpScanner.cpp

This will generate an executable named "LexpScanner".

Run Instructions:
-----------------
To execute the program, provide an input file and an output file:

    ./LexpScanner test_input.txt test_output.txt

Example:
--------
If you have an input file `sample.txt`, run:

    ./LexpScanner sample.txt output.txt

This will scan the input file and write tokenized output to `output.txt`.

Error Handling:
---------------
- The program gracefully handles invalid tokens by printing an error message.
- If the input file does not exist, the program will display an error message.
- If no command-line arguments are provided, it will display usage instructions.

End of Document