/*
Name: Hoang Mai Han Dang, Yazi Zhang
Phase 1.1: Scanner for Lexp
Description: This program implements a lexical scanner for Lexp,
            identifying identifiers, numbers, and symbols.
*/

#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

const regex IDENTIFIER_REGEX("^[a-zA-Z][a-zA-Z0-9]*");
const regex NUMBER_REGEX("^[0-9]+");
const regex SYMBOL_REGEX("^[+\\-*/()]");
const regex WHITESPACE_REGEX(R"(\s)");

vector<string> scanLine(const string& line)
{
    vector<string> result;
    size_t index = 0;
    string token;

    while (index < line.length())
    {
        char currChar = line[index];
        string remaining = line.substr(index);
        string token;
        smatch match;

        if (regex_search(remaining, match, WHITESPACE_REGEX) && match.position() == 0)
        {
            index += match.length();
            continue;
        }
        else if (regex_search(remaining, match, IDENTIFIER_REGEX))
        {
            token = match.str();
            result.push_back(token + ": IDENTIFIER");
        }
        else if (regex_search(remaining, match, NUMBER_REGEX))
        {
            token = match.str();
            result.push_back(token + ": NUMBER");
        }
        else if (regex_search(remaining, match, SYMBOL_REGEX))
        {
            token = match.str();
            result.push_back(token + ": SYMBOL");
        }
        else
        {
            result.push_back("ERROR READING: \"" + string(1, currChar) + "\"");
        }
        /*
        when nothing matches, the length of match is 0
        ensure index always move forward
        */
       index += match.length() > 0 ? match.length() : 1;
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        /*
        Ensure that the program does not crash
        when required agruments are missing
        */
        cout << "Type: ./LexpScanner <input_file> <output_file>" << endl;
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
        outputFile << "Line: " << line << endl;
        vector<string> tokens = scanLine(line);

        for (const string &token : tokens)
        {
            outputFile << token << endl;
        }

        outputFile << endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
