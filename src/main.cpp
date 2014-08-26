#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <vector>
#include "PatternPart.hpp"
#include "Pattern.hpp"
#include "FileReader.hpp"
#include "main.hpp"

Logger mainLogger;

int main(int argc, char* argv[]){
   // std::vector<PatternPart> parts;

    /*parts.push_back(PatternPart(0x00, 0x00, 0x00, 300));
    parts.push_back(PatternPart(0xff, 0xff, 0xff, 5000));
    parts.push_back(PatternPart(0x00, 0xff, 0x00, 30000));
    parts.push_back(PatternPart(0xff, 0x00, 0x00, 30000));
    parts.push_back(PatternPart(0x00, 0x00, 0x00, 3000));

    Pattern pattern(parts);
    pattern.play();
    std::this_thread::sleep_for(std::chrono::milliseconds(100000));*/

    std::vector<Pattern *> patterns = FileReader::getPatterns();

    mainLogger.log("Playing \"", patterns[0]->getName(), "\"");
    patterns[0]->play();
    std::this_thread::sleep_for(std::chrono::milliseconds(100000));
}
