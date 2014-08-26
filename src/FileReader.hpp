#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <vector>
#include <string>
#include "Pattern.hpp"

namespace FileReader {
    std::vector<Pattern *> getPatterns();
    std::vector<Pattern *> getPatterns(std::string path); //Path to folder holding files

    void replaceAll(std::string& str, const std::string& from, const std::string& to);
}

#endif
