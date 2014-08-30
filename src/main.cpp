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

    //I need to detect if it's plugged in immediately, but if it is, another thread needs to open it, so I'll close it here
    blink1_close(blink);

    return GTK::startGTK(argc, argv);
}
