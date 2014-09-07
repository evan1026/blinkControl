#ifndef GTK_HPP
#define GTK_HPP

#include <string>
#include "blink1-lib/blink1-lib.h"

namespace GTK {
    void backgroundThread();
    int  startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID);
    void addPart();
    void deleteRow(int row);
    void addPattern();
}

#endif
