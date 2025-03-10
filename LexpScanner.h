#ifndef LEXP_SCANNER_H
#define LEXP_SCANNER_H

#include <vector>
#include <string>

struct Token {
    std::string type;
    std::string value;
};

std::vector<Token> scanLine(const std::string& line);

#endif 
