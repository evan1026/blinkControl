#include <chrono>
#include <MiscUtils/MiscUtils.hpp>
#include <Logger/Logger.hpp>
#include <gtkmm.h>
#include <time.h>
#include "blink1-lib/blink1-lib.h"
#include "TrayIcon.hpp"
#include "FileReader.hpp"
#include "IFTTT.hpp"
#include "Event.hpp"
#include "PatternMaker.hpp"
#include "GTK.hpp"

Glib::RefPtr<Gtk::Application> app;

Logger GTKLogger;

blink1_device * blink;

std::string blinkID;

time_t prevTime = time(NULL);

std::mutex mutex;

std::vector<Pattern *> thePatterns = FileReader::getPatterns();

int GTK::startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID){
    app = Gtk::Application::create(argc, argv, "org.evan1026.blink_control");
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("../blinkControl.glade");

    TrayIcon tray;

    Glib::Threads::Thread* backThread = Glib::Threads::Thread::create(sigc::ptr_fun(backgroundThread));
    MarkUnused(backThread);

    blink   = _blink;
    blinkID = _blinkID;

    PatternMaker::initParts(builder, &mutex, &thePatterns);

    app->hold();
    return app->run();
}

void GTK::backgroundThread(){
    int patternIndex = -1;

    while(true){
        mutex.lock();
            if (patternIndex != -1 && !thePatterns[patternIndex]->isPlaying()){
                GTKLogger.log("Finished");
                patternIndex = -1;
            }
        mutex.unlock();

        std::string eventJSON = IFTTT::getEvents(blinkID);
        std::vector<Event> events = IFTTT::processEvents(eventJSON);

        for (Event s : events){
            if (s.date > prevTime){
                GTKLogger.log("BlinkID(", s.blink_id, ") Date(", s.date, ") Name(", s.name, ") Source(", s.source, ")");
                prevTime = s.date;
                mutex.lock();
                    for (int i = 0; i < thePatterns.size(); ++i){
                        if (thePatterns[i]->getName() == s.name){
                            thePatterns[i]->play(blink);
                            patternIndex = i;
                            break;
                        }
                    }
                mutex.unlock();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
