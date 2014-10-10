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

#define GTK_ROW_HEIGHT 30

int GTK::startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID){
    app = Gtk::Application::create(argc, argv, "org.evan1026.blink_control");
    builder = Gtk::Builder::create_from_file("../blinkControl.glade");

    TrayIcon tray;

    Glib::Threads::Thread* backThread = Glib::Threads::Thread::create(sigc::mem_fun(this, &GTK::backgroundThread));
    MarkUnused(backThread);

    blink   = _blink;
    blinkID = _blinkID;

    initMainWindow(builder);

    mainWindow->show();

    patternMaker = new PatternMaker(builder, &mutex, &patterns);

    app->hold();
    return app->run();
}

void GTK::backgroundThread(){
    int patternIndex = -1;

    while(true){
        mutex.lock();
            if (patternIndex != -1 && !patterns[patternIndex]->isPlaying()){
                logger.log("Finished");
                patternIndex = -1;
            }
        mutex.unlock();

        if (time(NULL) >= nextRun){
            std::string eventJSON = IFTTT::getEvents(blinkID);
            std::vector<Event> events = IFTTT::processEvents(eventJSON);

            for (Event s : events){
                if (s.date > prevTime){
                    logger.log("BlinkID(", s.blink_id, ") Date(", s.date, ") Name(", s.name, ") Source(", s.source, ")");
                    prevTime = s.date;
                    mutex.lock();
                        for (int i = 0; i < patterns.size(); ++i){
                            if (patterns[i]->getName() == s.name){
                                patterns[i]->play(blink);
                                patternIndex = i;
                                break;
                            }
                        }
                    mutex.unlock();
                }
            }
            nextRun = time(NULL) + 1;
        }
        if (patternMaker->mainWindowNeedsUpdate()){
            updateMainWindow();
            patternMaker->mainWindowUpdated();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void GTK::initMainWindow(Glib::RefPtr<Gtk::Builder> builder) {
    mainWindow    = Glib::RefPtr<Gtk::Window>::cast_dynamic(builder->get_object("MainWindow"));
    patternHolder = Glib::RefPtr<Gtk::Fixed>::cast_dynamic(builder->get_object("PatternHolder"));
    updateMainWindow();
}

void GTK::updateMainWindow(){
    std::vector<Gtk::Widget *> children = patternHolder->get_children();
    for (Gtk::Widget * child : children) {
        child->hide();
        patternHolder->remove(*child);
    }
    for (int i = 0; i < patterns.size(); ++i) {
        Gtk::Label * label = new Gtk::Label(patterns[i]->getName());
        label->show();

        Gtk::Image* deleteImage = new Gtk::Image(Gtk::Stock::STOP, Gtk::BuiltinIconSize::ICON_SIZE_SMALL_TOOLBAR);

        Gtk::Button* newDeleteButton = new Gtk::Button();
        newDeleteButton->set_name("Delete");
        newDeleteButton->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
        newDeleteButton->set_image(*deleteImage);
        newDeleteButton->show();

        Gtk::Button * newEditButton = new Gtk::Button("Edit");
        newEditButton->set_name("EditButton");
        newEditButton->set_size_request(-1, PATTERN_MAKER_ROW_HEIGHT);
        newEditButton->show();
        newEditButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &GTK::showPatternMaker), patterns[i]->getName()));

        patternHolder->put(*newDeleteButton,  0, GTK_ROW_HEIGHT * i);
        patternHolder->put(*newEditButton,   35, GTK_ROW_HEIGHT * i);
        patternHolder->put(*label,           75, GTK_ROW_HEIGHT * i + 6);
    }
}

void GTK::showPatternMaker(std::string patternName) {
    int i = 0;
    for ( ; i < patterns.size(); ++i){
        if (patterns[i]->getName() == patternName) break;
    }
    patternMaker->show(patterns[i]->getName());
}
