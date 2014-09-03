#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "FileReader.hpp"

Logger fileLogger;

std::vector<Pattern *> FileReader::getPatterns(){
    return FileReader::getPatterns(MiscUtils::getLinuxHomeFolder() + ".blink/patterns/"); //Will be using bash on linux, so might as well set the default to this
}

std::vector<Pattern *> FileReader::getPatterns(std::string path){
    std::vector<Pattern *> output;

    std::vector<std::string> fileNames;

    std::string files = MiscUtils::executeGetOutput(Logger::makeString("ls -1 ", path, " 2>/dev/null"));
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
                MiscUtils::replaceAll(line, " ", "");          //Remove spaces
                if (line[0] == '#' || line.empty()) continue;  //Ignore comments and empty lines

                std::stringstream lineReader(line);
                std::string redString,
                            greenString,
                            blueString,
                            timeString,
                            ledString;

                if (std::count(line.begin(), line.end(), ',') != 4){
                    fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                    exit(2);
                }

                std::getline(lineReader, redString, ',');
                std::getline(lineReader, greenString, ',');
                std::getline(lineReader, blueString, ',');
                std::getline(lineReader, timeString, ',');
                std::getline(lineReader, ledString, ',');

                short r, g, b;
                short led;
                int   time;

                led = std::stoi(ledString);
                if (!(led == 0 || led == 1 || led == 2)){
                    fileLogger.log(Logger::LogType::Error, "Line ", lineNum, " in \"", file, "\" is malformed. Exiting...");
                    exit(2);
                }

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
                currParts.push_back(PatternPart(r, g, b, time, led));
            }

            Pattern * curr = new Pattern(currParts, file);

            output.push_back(curr);
        } else {
            fileLogger.log(Logger::LogType::Error, "Could not open file \"", path + file, "\" for reading. Please check that the file exists and the permissions and make sure it's readable.");
        }
    }

    return output;
}

std::string FileReader::getBlinkID(std::string serial) {
    return FileReader::getBlinkID(serial, MiscUtils::getLinuxHomeFolder() + ".blink/ids/");
}

std::string FileReader::getBlinkID(std::string serial, std::string path) {
    std::string blinkID;

    std::ifstream idFile(path + serial);
    if (idFile.is_open()) {
        getline(idFile, blinkID);
        idFile.close();
    } else {
        blinkID = generateBlinkID(serial);
        saveBlinkID(serial, blinkID);
    }

    return blinkID;
}

std::string FileReader::generateBlinkID(std::string serial) {
    uint32_t randomBits;

    //Generates a 32-bit random number, or 8 hex chars
    srand (time(NULL));
    randomBits  = rand() & 0xff;
    randomBits |= (rand() & 0xff) << 8;
    randomBits |= (rand() & 0xff) << 16;
    randomBits |= (rand() & 0xff) << 24;

    std::stringstream ss;
    ss << std::hex << std::setw(8) << randomBits;

    return ss.str() + serial;
}

void FileReader::saveBlinkID(std::string serial, std::string id) {
    saveBlinkID(serial, id, MiscUtils::getLinuxHomeFolder() + ".blink/ids/");
}

void FileReader::saveBlinkID(std::string serial, std::string id, std::string path){
    if (!MiscUtils::dirExists(path)){
        MiscUtils::execute("mkdir -p " + path);
    }

    std::ofstream file(path + serial, std::ios::out | std::ios::trunc);

    if (file.is_open()){
        file << id;
        file.close();
    } else {
        fileLogger.log(Logger::LogType::Error, "Could not open file \"", path + serial, "\" for writing. Please check your permission to write to this location.");
        exit(5);
    }
}

void FileReader::savePattern(Pattern & pattern) {
    savePattern(pattern, MiscUtils::getLinuxHomeFolder() + ".blink/patterns/");
}

void FileReader::savePattern(Pattern & pattern, std::string path) {
    std::ofstream file(path + pattern.getName(), std::ios::out | std::ios::trunc);

    if (!file.is_open()){
        fileLogger.log(Logger::LogType::Error, "Could not open file \"", path + pattern.getName(), "\" for writing. Please check your permission to write to this location.");
        exit(5);
    }

    file << "#This pattern file was automatically generated." << std::endl;

    for (int i = 0; i < pattern.parts.size(); ++i) {
        file << pattern.parts[i].r << "," << pattern.parts[i].g << "," << pattern.parts[i].b << "," << pattern.parts[i].time << "," << pattern.parts[i].led << std::endl;
    }

    file.close();
}
