#include <chrono>
#include <MiscUtils/MiscUtils.hpp>
#include <Logger/Logger.hpp>
#include <gtkmm.h>
#include "TrayIcon.hpp"
#include "FileReader.hpp"
#include "GTK.hpp"

Glib::RefPtr<Gtk::Application> app;

Logger GTKLogger;

int GTK::startGTK(int argc, char* argv[]){
    app = Gtk::Application::create(argc, argv, "org.evan1026.blink_control");

    TrayIcon tray;

    Glib::Threads::Thread* iconThread = Glib::Threads::Thread::create(sigc::ptr_fun(backgroundThread));
    MarkUnused(iconThread);

    app->hold();
    return app->run();
}

void GTK::backgroundThread(){
    std::vector<Pattern *> patterns = FileReader::getPatterns();

    int patternIndex = 0;

    patterns[patternIndex]->play();
    GTKLogger.log("Playing \"", patterns[patternIndex]->getName(), "\"");

    while(true){
        if (!patterns[patternIndex]->isPlaying()){
            GTKLogger.log("Finished");
            patternIndex++;
            if (patternIndex == patterns.size()) patternIndex = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            patterns[patternIndex]->play();
            GTKLogger.log("Playing \"", patterns[patternIndex]->getName(), "\"");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
