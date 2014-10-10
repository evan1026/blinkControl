#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <vector>
#include <gtkmm.h>
#include "blink1-lib/blink1-lib.h"
#include "PatternPart.hpp"
#include "Pattern.hpp"
#include "FileReader.hpp"
#include "TrayIcon.hpp"
#include "GTK.hpp"
#include "main.hpp"

Logger mainLogger;

int main(int argc, char* argv[]){

    blink1_device * blink = blink1_open();

    if (blink == NULL) {
        mainLogger.log(Logger::LogType::Error, "No blink(1) device found.");
        exit(3);
    }

    GTK program;

    return program.startGTK(argc, argv, blink, FileReader::getBlinkID(blink1_getSerialForDev(blink)));
}
