#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <vector>
#include <gtkmm.h>
#include "PatternPart.hpp"
#include "Pattern.hpp"
#include "FileReader.hpp"
#include "TrayIcon.hpp"
#include "GTK.hpp"
#include "main.hpp"

Logger mainLogger;

int main(int argc, char* argv[]){
    if (MiscUtils::replaceAllGiveString(MiscUtils::executeGetOutput("blink1-tool --rgbread"), "\n", "") == "no blink(1) devices found") {
        mainLogger.log(Logger::LogType::Error, "No blink(1) device found.");
        exit(3);
    }

    return GTK::startGTK(argc, argv);
}
