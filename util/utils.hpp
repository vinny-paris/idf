#ifndef ENERGYPLUSPARSER_UTILS_HPP
#define ENERGYPLUSPARSER_UTILS_HPP

#include <vector>
#include <string>

std::vector<std::string> splitString(std::string const &string, char delimiter);

std::vector<int> splitStringToInt(std::string const &string, char delimiter);

#endif //ENERGYPLUSPARSER_UTILS_HPP
