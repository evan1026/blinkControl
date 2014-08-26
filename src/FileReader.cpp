#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "FileReader.hpp"

Logger fileLogger;

std::vector<Pattern *> FileReader::getPatterns(){
    return FileReader::getPatterns("$HOME/.blink/patterns/"); //Will be using bash on linux, so might as well set the default to this
}

std::vector<Pattern *> FileReader::getPatterns(std::string path){
    std::vector<Pattern *> output;

    path = executeGetOutput(Logger::makeString("echo ", path)); //Expands path with bash variables in it
    replaceAll(path, "\n", "");

    std::vector<std::string> fileNames;

    std::string files = executeGetOutput(Logger::makeString("ls -1 ", path, " 2>/dev/null"));
    if (files == ""){
        fileLogger.log(Logger::LogType::Error, "Pattern directory empty or does not exist. This program is useless without patterns, so go fix it.");
        exit(1);
    }

    std::stringstream ss(files);
    std::string temp;
    while (std::getline(ss, temp, '\n')){
        std::string temp2 = temp;
        fileNames.push_back(temp2);
    }

    for (std::string file : fileNames){
        std::ifstream instream(path + file);
        std::string   line;

        if (instream.is_open()){
            std::vector<PatternPart> currParts;
            int lineNum = 0;
            while (std::getline(instream, line)){
                lineNum++;
                replaceAll(line, " ", "");                     //Remove spaces
                if (line[0] == '#' || line.empty()) continue;  //Ignore comments and empty lines

                std::stringstream lineReader(line);
                std::string redString,
                            greenString,
                            blueString,
                            timeString;

                if (std::count(line.begin(), line.end(), ',') != 3){
                    fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                    exit(2);
                }

                std::getline(lineReader, redString, ',');
                std::getline(lineReader, greenString, ',');
                std::getline(lineReader, blueString, ',');
                std::getline(lineReader, timeString, ',');

                short r, g, b;
                int   time;

                if (redString[0] == '0'){ //Reading in binary, octal, or hex (if it's just 0, it doesn't really matter)
                    if (redString.size() < 2) { //Must just be the zero
                        r = 0;
                    } else {
                        if (redString[1] == 'x'){
                            r = std::stoi(redString, nullptr, 16);
                        } else if (redString[1] == 'b') {
                            r = std::stoi(redString.substr(2), nullptr, 2);
                        } else {
                            fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                            exit(2);
                        }
                    }
                } else {
                    r = std::stoi(redString);
                }

                if (greenString[0] == '0'){ //Reading in binary, octal, or hex (if it's just 0, it doesn't really matter)
                    if (greenString.size() < 2) { //Must just be the zero
                        g = 0;
                    } else {
                        if (greenString[1] == 'x'){
                            g = std::stoi(greenString, nullptr, 16);
                        } else if (greenString[1] == 'b') {
                            g = std::stoi(greenString.substr(2), nullptr, 2);
                        } else {
                            fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                            exit(2);
                        }
                    }
                } else {
                    g = std::stoi(greenString);
                }

                if (blueString[0] == '0'){ //Reading in binary, octal, or hex (if it's just 0, it doesn't really matter)
                    if (blueString.size() < 2) { //Must just be the zero
                        b = 0;
                    } else {
                        if (blueString[1] == 'x'){
                            b = std::stoi(blueString, nullptr, 16);
                        } else if (blueString[1] == 'b') {
                            b = std::stoi(blueString.substr(2), nullptr, 2);
                        } else {
                            fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                            exit(2);
                        }
                    }
                } else {
                    b = std::stoi(blueString);
                }

                time = std::stoi(timeString);

                currParts.push_back(PatternPart(r, g, b, time));
            }

            Pattern * curr = new Pattern(currParts, file);

            output.push_back(curr);
        } else {
            fileLogger.log(Logger::LogType::Error, "Could not open file \"", path + file, "\" for reading. Please check that the file exists and the permissions and make sure it's readable.");
        }
    }

    return output;
}

void FileReader::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    }
