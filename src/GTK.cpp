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
#include "GTK.hpp"

Glib::RefPtr<Gtk::Application> app;

Logger GTKLogger;

blink1_device * blink;

std::string blinkID;

time_t prevTime = time(NULL);

int GTK::startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID){
    app = Gtk::Application::create(argc, argv, "org.evan1026.blink_control");

    TrayIcon tray;

    Glib::Threads::Thread* iconThread = Glib::Threads::Thread::create(sigc::ptr_fun(backgroundThread));
    MarkUnused(iconThread);

    blink   = _blink;
    blinkID = _blinkID;

    app->hold();
    return app->run();
}

void GTK::backgroundThread(){
    std::vector<Pattern *> patterns = FileReader::getPatterns();

    int patternIndex = -1;

    while(true){
        if (patternIndex != -1 && !patterns[patternIndex]->isPlaying()){
            GTKLogger.log("Finished");
            patternIndex = -1;
        }
        std::string eventJSON = IFTTT::getEvents(blinkID);
        std::vector<Event> events = IFTTT::processEvents(eventJSON);

        for (Event s : events){
            if (s.date > prevTime){
                GTKLogger.log("BlinkID(", s.blink_id, ") Date(", s.date, ") Name(", s.name, ") Source(", s.source, ")");
                prevTime = s.date;
                for (int i = 0; i < patterns.size(); ++i){
                    if (patterns[i]->getName() == s.name){
                        patterns[i]->play(blink);
                        patternIndex = i;
                        break;
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
