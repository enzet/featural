#include <sstream>
#include <string>
#include <vector>

#include "util.hpp"

/*
 * Split string by delimiter, ignore empty elements.
 */
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (not token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}
