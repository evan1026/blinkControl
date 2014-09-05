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

#define PATTERN_MAKER_ROW_HEIGHT 30

Glib::RefPtr<Gtk::Application> app;
Glib::RefPtr<Gtk::Fixed>       holder;

Gtk::Button * addButton;

Logger GTKLogger;

blink1_device * blink;

std::string blinkID;

time_t prevTime = time(NULL);

int GTK::startGTK(int argc, char* argv[], blink1_device * _blink, std::string _blinkID){
    app = Gtk::Application::create(argc, argv, "org.evan1026.blink_control");
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("../blinkControl.glade");

    TrayIcon tray;

    Glib::Threads::Thread* backThread = Glib::Threads::Thread::create(sigc::ptr_fun(backgroundThread));
    MarkUnused(backThread);

    blink   = _blink;
    blinkID = _blinkID;

    Gtk::Window* window;
    builder->get_widget("PatternMaker", window);
    window->show();

    holder = Glib::RefPtr<Gtk::Fixed>::cast_dynamic(builder->get_object("PatternPartHolder"));

    addButton = new Gtk::Button("Add part");
    addButton->signal_clicked().connect(sigc::ptr_fun(addPart));
    addButton->show();
    addButton->set_size_request(50, 25);

    holder->put(*addButton, 0, 0);

    app->hold();
    return app->run(*window);
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

void GTK::addPart(){
    std::vector<Gtk::Widget *> children = holder->get_children();

    int highest = 0;
    for (Gtk::Widget * child : children) {
        Gtk::ColorButton * testColorButton = dynamic_cast<Gtk::ColorButton*>(child);
        if (testColorButton){
            int num;
            std::string name = testColorButton->get_name();
            name.replace(0,12,""); //Turns "ColorButton n" into "n"

            std::stringstream ss(name);
            ss >> num;
            if (num > highest) highest = num;
        }
    }

    int current = highest + 1;

    std::stringstream ss;
    ss << current;

    Gtk::Image* deleteImage = new Gtk::Image(Gtk::Stock::STOP, Gtk::BuiltinIconSize::ICON_SIZE_SMALL_TOOLBAR);

    Gtk::Button* newDeleteButton = new Gtk::Button();
    newDeleteButton->set_name("Delete Button " + ss.str());
    newDeleteButton->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newDeleteButton->set_image(*deleteImage);
    newDeleteButton->show();

    Gtk::Label* newColorLabel = new Gtk::Label("Color: ");
    newColorLabel->set_name("Color Label " + ss.str());
    newColorLabel->show();

    Gtk::ColorButton* newColorButton = new Gtk::ColorButton();
    newColorButton->set_name("ColorButton " + ss.str());
    newColorButton->set_size_request(50, PATTERN_MAKER_ROW_HEIGHT);
    newColorButton->show();

    Gtk::Label* newTimeLabel = new Gtk::Label("Time (ms): ");
    newTimeLabel->set_name("Time Label " + ss.str());
    newTimeLabel->show();

    Gtk::SpinButton* newSpinButton = new Gtk::SpinButton(1);
    newSpinButton->set_name("Spin Button " + ss.str());
    newSpinButton->set_size_request(50, PATTERN_MAKER_ROW_HEIGHT);
    newSpinButton->show();
    newSpinButton->get_adjustment()->set_lower(0);
    newSpinButton->get_adjustment()->set_upper(std::numeric_limits<int>::max());
    newSpinButton->get_adjustment()->set_step_increment(100);

    Gtk::Label* newLedLabel = new Gtk::Label("Led: ");
    newLedLabel->set_name("Led Label " + ss.str());
    newLedLabel->show();

    Gtk::RadioButton* newRadioButton1 = new Gtk::RadioButton("1");
    newRadioButton1->set_name("Radio Button 1 " + ss.str());
    newRadioButton1->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButton1->show();

    Gtk::RadioButton* newRadioButton2 = new Gtk::RadioButton("2");
    newRadioButton2->set_name("Radio Button 2 " + ss.str());
    newRadioButton2->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButton2->show();
    newRadioButton2->join_group(*newRadioButton1);

    Gtk::RadioButton* newRadioButtonBoth = new Gtk::RadioButton("Both");
    newRadioButtonBoth->set_name("Radio Button both " + ss.str());
    newRadioButtonBoth->set_size_request(40, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButtonBoth->show();
    newRadioButtonBoth->join_group(*newRadioButton1);

    holder->move(*addButton, 0, current * PATTERN_MAKER_ROW_HEIGHT);

    holder->put(*newDeleteButton,      0, highest * PATTERN_MAKER_ROW_HEIGHT);
    holder->put(*newColorLabel,       50, highest * PATTERN_MAKER_ROW_HEIGHT + 6);
    holder->put(*newColorButton,      90, highest * PATTERN_MAKER_ROW_HEIGHT);
    holder->put(*newTimeLabel,       160, highest * PATTERN_MAKER_ROW_HEIGHT + 6);
    holder->put(*newSpinButton,      230, highest * PATTERN_MAKER_ROW_HEIGHT);
    holder->put(*newLedLabel,        380, highest * PATTERN_MAKER_ROW_HEIGHT + 6);
    holder->put(*newRadioButton1,    410, highest * PATTERN_MAKER_ROW_HEIGHT);
    holder->put(*newRadioButton2,    445, highest * PATTERN_MAKER_ROW_HEIGHT);
    holder->put(*newRadioButtonBoth, 480, highest * PATTERN_MAKER_ROW_HEIGHT);
}
