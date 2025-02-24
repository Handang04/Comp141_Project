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

struct Token {
    string type;
    string value;
};

bool isOnlyWhiteSpace(const string& line)
{
    return all_of(line.begin(), line.end(), [](char c) { return isspace(c); });
}

vector<Token> scanLine(const string& line)
{
    vector<Token> tokens;
    size_t index = 0;

    while (index < line.length())
    {
        string token;
        string remaining = line.substr(index);
        smatch match;
        if (regex_search(remaining, match, WHITESPACE_REGEX) && match.position() == 0)
        {
            index += match.length();
            continue;
        }
        else if (regex_search(remaining, match, KEYWORD_REGEX) && match.position() == 0)
        {
            token = match.str();
            tokens.push_back({"KEYWORD", token});
        }
        else if (regex_search(remaining, match, IDENTIFIER_REGEX) && match.position() == 0)
        {
            token = match.str();
            tokens.push_back({"IDENTIFIER", token});
        }
        else if (regex_search(remaining, match, NUMBER_REGEX) && match.position() == 0)
        {
            token = match.str();
            tokens.push_back({"NUMBER", token});
        }
        else if (regex_search(remaining, match, SYMBOL_REGEX) && match.position() == 0)
        {
            token = match.str();
            tokens.push_back({"SYMBOL", token});
        }
        else
        {
            string error(1, line[index]);
            tokens.push_back({"ERROR READING", error});
            break;
        }
        /*
        when nothing matches, the length of match is 0
        ensure index always move forward
        */
        index += match.length() > 0 ? match.length() : 1;
    }

    return tokens;
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        /*
        Ensure that the program does not crash
        when required agruments are missing
        */
        cout << "Type: ./LimpScanner <input_file> <output_file>" << endl;
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
        if (isOnlyWhiteSpace(line))
        {
            continue;
        }

        outputFile << "Line: " << line << endl;
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

        outputFile << endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
