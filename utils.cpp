#include "utils.hpp"
#include <sstream>

std::vector<std::string> splitString(std::string const &string, char delimiter) {
    std::vector<std::string> results;
    if (!string.empty()) { // Only do work if there is work to do
        std::stringstream stream(string);
        std::string substring;
        while (std::getline(stream, substring, delimiter)) { // Loop and fill the results vector
            results.emplace_back(substring);
        }
        if (*(string.end() - 1) == ',') { // Add an empty string if the last char is the delimiter
            results.emplace_back(std::string());
        }
    }
    return results;
}

std::vector<int> splitStringToInt(std::string const &string, char delimiter) {
    std::vector<int> results;
    if (!string.empty()) { // Only do work if there is work to do
        std::stringstream stream(string);
        std::string substring;
        while (std::getline(stream, substring, delimiter)) { // Loop and fill the results vector
            results.emplace_back(std::stoi(substring));
        }
        if (*(string.end() - 1) == ',') { // Add an empty string if the last char is the delimiter
            results.emplace_back(0);
        }
    }
    return results;
}
