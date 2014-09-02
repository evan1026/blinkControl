#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <vector>
#include <string>
#include "Pattern.hpp"

namespace FileReader {
    std::vector<Pattern *> getPatterns();
    std::vector<Pattern *> getPatterns(std::string path); //Path to folder holding files

    std::string getBlinkID(std::string serial);
    std::string getBlinkID(std::string serial, std::string path);

    void saveBlinkID(std::string serial, std::string id);
    void saveBlinkID(std::string serial, std::string id, std::string path);

    std::string generateBlinkID(std::string serial);
}

#endif
