#ifndef LIMP_SCANNER_H
#define LIMP_SCANNER_H

#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string value;
};

bool isOnlyWhiteSpace(const std::string& line);

std::vector<Token> scanLine(const std::string& line);

#endif 