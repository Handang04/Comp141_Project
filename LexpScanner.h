#ifndef LEXP_SCANNER_H
#define LEXP_SCANNER_H

#include <vector>
#include <string>

struct Token {
    std::string type;
    std::string value;
};

bool isOnlyWhiteSpace(const std::string& line);

std::vector<Token> scanLine(const std::string& line);

#endif 
