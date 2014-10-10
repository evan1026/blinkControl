#ifndef GTK_HPP
#define GTK_HPP

#include <Logger/Logger.hpp>
#include <chrono>
#include <gtkmm.h>
#include <time.h>
#include "blink1-lib/blink1-lib.h"
#include "Pattern.hpp"
#include "PatternMaker.hpp"
#include "FileReader.hpp"

class GTK {
    Glib::RefPtr<Gtk::Application> app;

    Logger logger;

    blink1_device * blink;
    std::string     blinkID;

    time_t prevTime = time(NULL);
    time_t nextRun  = time(NULL);

    std::mutex mutex;

    std::vector<Pattern *> patterns = FileReader::getPatterns();

    Glib::RefPtr<Gtk::Window>  mainWindow;
    Glib::RefPtr<Gtk::Fixed>   patternHolder;
    Glib::RefPtr<Gtk::Builder> builder;

    PatternMaker * patternMaker;

    public:
        void backgroundThread();
        int  startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID);
        void addPart();
        void deleteRow(int row);
        void addPattern();
        void initMainWindow(Glib::RefPtr<Gtk::Builder> builder);
        void showPatternMaker(std::string patternName);
        void updateMainWindow();
};

#endif
