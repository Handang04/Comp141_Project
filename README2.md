Limp Scanner - Phase 1.2
=======================
Author: Hoang Mai Han Dang, Yazi Zhang

Compiler Information:
---------------------
This program was tested using the GNU Compiler Collection (G++).
Compiler version: g++ (MinGW-w64 or GCC on Linux/Mac)

Build Instructions:
-------------------
To compile the program, use the following command in the terminal:

    g++ -o LimpScanner LimpScanner.cpp

This will generate an executable named "LimpScanner".

Run Instructions:
-----------------
To execute the program, provide an input file and an output file:

    ./LimpScanner test_input.txt test_output.txt

Example:
--------
If you have an input file `sample.txt`, run:

    ./LimpScanner sample.txt output.txt

This will scan the input file and write tokenized output to `output.txt`.

Error Handling:
---------------
- The program gracefully handles invalid tokens by printing an error message.
- If the input file does not exist, the program will display an error message.
- If no command-line arguments are provided, it will display usage instructions.

Notes:
------
- The program reads the input file line by line.
- Lines containing only whitespace are skipped.
- If an unexpected character is encountered, an "ERROR READING" token is generated and scanning for that line stops immediately.
- The output file will list each scanned line and its corresponding tokens.

End of Document