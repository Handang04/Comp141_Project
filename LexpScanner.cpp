#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

const regex IDENTIFIER_REGEX("^[a-zA-Z][a-zA-Z0-9]*");
const regex NUMBER_REGEX("^[0-9]+");
const regex SYMBOL_REGEX("^[+\\-*/()]");

vector<string> scanLine(const string& line)
{
    vector<string> result;
    size_t index = 0;

    while (index < line.length())
    {
        char currChar = line[index];
        if (isspace(currChar))
        {
            index++;
            continue;
        }

        string remaining = line.substr(index);
        smatch match;

        if (regex_search(remaining, match, IDENTIFIER_REGEX))
        {
            string token = match.str();
            result.push_back(token + ": IDENTIFIER");
            index += token.length();
        }
        else if (regex_search(remaining, match, NUMBER_REGEX))
        {
            string token = match.str();
            result.push_back(token + ": NUMBER");
            index += token.length();
        }
        else if (regex_search(remaining, match, SYMBOL_REGEX))
        {
            string token = match.str();
            result.push_back(token + ": SYMBOL");
            index += token.length();
        }
        else
        {
            result.push_back("ERROR READING: \"" + string(1, currChar) + "\"");
            index++;
        }
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
