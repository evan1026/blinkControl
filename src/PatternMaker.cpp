#include <gtkmm.h>
#include <Logger/Logger.hpp>
#include "Pattern.hpp"
#include "FileReader.hpp"
#include "PatternMaker.hpp"

Logger makerLogger;

Glib::RefPtr<Gtk::Fixed>  holder;
Glib::RefPtr<Gtk::Entry>  nameEntry;
Glib::RefPtr<Gtk::Window> window;

Gtk::Button * addButton;

std::mutex * lock;

std::vector<Pattern *> * patterns;

void PatternMaker::initParts(Glib::RefPtr<Gtk::Builder> builder, std::mutex * _lock, std::vector<Pattern *> * _patterns){
    holder    = Glib::RefPtr<Gtk::Fixed>:: cast_dynamic(builder->get_object("PatternPartHolder"));
    nameEntry = Glib::RefPtr<Gtk::Entry>:: cast_dynamic(builder->get_object("NameEntry"));
    window    = Glib::RefPtr<Gtk::Window>::cast_dynamic(builder->get_object("PatternMaker"));

    Glib::RefPtr<Gtk::Button>::cast_dynamic(builder->get_object("SubmitButton"))->signal_clicked().connect(sigc::ptr_fun(addPattern));

    addButton = new Gtk::Button("Add part");
    addButton->signal_clicked().connect(sigc::ptr_fun(addPart));
    addButton->show();
    addButton->set_size_request(50, 25);

    holder->put(*addButton, 0, 0);

    lock = _lock;

    patterns = _patterns;
}
void PatternMaker::addPart(){
    std::vector<Gtk::Widget *> children = holder->get_children();

    int highest = 0;
    for (Gtk::Widget * child : children) {
        Gtk::Fixed * testrow = dynamic_cast<Gtk::Fixed*>(child);
        if (testrow){
            int num;
            std::string name = testrow->get_name();
            name.replace(0,4,""); //Turns "Row n" into "n"

            std::stringstream ss(name);
            ss >> num;
            if (num > highest) highest = num;
        }
    }

    int current = highest + 1;

    std::stringstream ss;
    ss << current;

    Gtk::Fixed* newRowHolder = new Gtk::Fixed();
    newRowHolder->set_size_request(-1, PATTERN_MAKER_ROW_HEIGHT);
    newRowHolder->set_name("Row " + ss.str());
    newRowHolder->show();

    Gtk::Image* deleteImage = new Gtk::Image(Gtk::Stock::STOP, Gtk::BuiltinIconSize::ICON_SIZE_SMALL_TOOLBAR);

    Gtk::Button* newDeleteButton = new Gtk::Button();
    newDeleteButton->set_name("Delete");
    newDeleteButton->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newDeleteButton->set_image(*deleteImage);
    newDeleteButton->show();
    newDeleteButton->signal_clicked().connect(sigc::bind(sigc::ptr_fun<int>(deleteRow), current));

    Gtk::Label* newColorLabel = new Gtk::Label("Color: ");
    newColorLabel->show();

    Gtk::ColorButton* newColorButton = new Gtk::ColorButton();
    newColorButton->set_size_request(50, PATTERN_MAKER_ROW_HEIGHT);
    newColorButton->set_use_alpha(false);
    newColorButton->show();

    Gtk::Label* newTimeLabel = new Gtk::Label("Time (ms): ");
    newTimeLabel->show();

    Gtk::SpinButton* newSpinButton = new Gtk::SpinButton(1);
    newSpinButton->set_size_request(50, PATTERN_MAKER_ROW_HEIGHT);
    newSpinButton->show();
    newSpinButton->get_adjustment()->set_lower(0);
    newSpinButton->get_adjustment()->set_upper(std::numeric_limits<int>::max());
    newSpinButton->get_adjustment()->set_step_increment(100);

    Gtk::Label* newLedLabel = new Gtk::Label("Led: ");
    newLedLabel->show();

    Gtk::RadioButton* newRadioButton1 = new Gtk::RadioButton("1");
    newRadioButton1->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButton1->show();

    Gtk::RadioButton* newRadioButton2 = new Gtk::RadioButton("2");
    newRadioButton2->set_size_request(30, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButton2->show();
    newRadioButton2->join_group(*newRadioButton1);

    Gtk::RadioButton* newRadioButtonBoth = new Gtk::RadioButton("Both");
    newRadioButtonBoth->set_size_request(40, PATTERN_MAKER_ROW_HEIGHT);
    newRadioButtonBoth->show();
    newRadioButtonBoth->join_group(*newRadioButton1);

    holder->move(*addButton, 0, current * PATTERN_MAKER_ROW_HEIGHT);

    newRowHolder->put(*newDeleteButton,      0, 0);
    newRowHolder->put(*newColorLabel,       50, 6);
    newRowHolder->put(*newColorButton,      90, 0);
    newRowHolder->put(*newTimeLabel,       160, 6);
    newRowHolder->put(*newSpinButton,      230, 0);
    newRowHolder->put(*newLedLabel,        380, 6);
    newRowHolder->put(*newRadioButton1,    410, 0);
    newRowHolder->put(*newRadioButton2,    445, 0);
    newRowHolder->put(*newRadioButtonBoth, 480, 0);

    holder->put(*newRowHolder, 0, highest * PATTERN_MAKER_ROW_HEIGHT);
}

void PatternMaker::deleteRow(int row){
    std::vector<Gtk::Widget *> children = holder->get_children();
    int rowNum;
    for (int i = 0; i < children.size(); ++i) {
        Gtk::Fixed * testrow = dynamic_cast<Gtk::Fixed*>(children[i]);
        if (testrow){
            std::stringstream ss(testrow->get_name().replace(0,4,""));
            ss >> rowNum;
            if(rowNum == row){
                holder->remove(*testrow);
            } else if (rowNum > row) {
                holder->move(*testrow, 0, (rowNum - 2) * PATTERN_MAKER_ROW_HEIGHT);
                testrow->set_name(Logger::makeString("Row ", rowNum - 1));
                std::vector<Gtk::Widget *> rowChildren = testrow->get_children();
                for (Gtk::Widget * child : rowChildren){
                    Gtk::Button * deleteButton = dynamic_cast<Gtk::Button *>(child);
                    if (deleteButton && deleteButton->get_name() == "Delete"){
                        deleteButton->signal_clicked().connect(sigc::bind(sigc::ptr_fun<int>(deleteRow), rowNum - 1));
                    }
                }
            }
        }
    }

    holder->move(*addButton, 0, (rowNum - 1) * PATTERN_MAKER_ROW_HEIGHT);
}

void PatternMaker::addPattern(){
    std::vector<PatternPart> newPattern;
    std::vector<Gtk::Widget *> children = holder->get_children();
    for (int i = 0; i < children.size(); ++i){
        Gtk::Fixed * testrow = dynamic_cast<Gtk::Fixed *>(children[i]);
        if (testrow) {
            int r,
                g,
                b,
                time;
            short led;

            std::vector<Gtk::Widget *> rowChildren = testrow->get_children();
            for (Gtk::Widget * child : rowChildren) {
                Gtk::ColorButton * testColorButton = dynamic_cast<Gtk::ColorButton *>(child);
                Gtk::SpinButton  * testSpinButton  = dynamic_cast<Gtk::SpinButton  *>(child);
                Gtk::RadioButton * testRadioButton = dynamic_cast<Gtk::RadioButton *>(child);

                if (testColorButton) {
                    Gdk::Color color = testColorButton->get_color();
                    r = color.get_red()   & 0xff;
                    g = color.get_green() & 0xff;
                    b = color.get_blue()  & 0xff;
                } else if (testSpinButton) {
                    time = testSpinButton->get_value_as_int();
                } else if (testRadioButton && testRadioButton->get_active()){
                    if (testRadioButton->get_label() == "1"){
                        led = 1;
                    } else if (testRadioButton->get_label() == "2"){
                        led = 2;
                    } else if (testRadioButton->get_label() == "Both"){
                        led = 0;
                    }
                }
            }
//            GTKLogger.log("Row(", testrow->get_name(), ") r(", r, ") g(", g, ") b(", b, ") time(", time, ") led(", led, ")");
            newPattern.push_back(PatternPart(r,g,b,time,led));
        }
    }

    lock->lock();
        Pattern * patternToAdd = new Pattern(newPattern, nameEntry->get_text());
        for (int i = 0; i < patterns->size(); ++i){
            if ((*patterns)[i]->getName() == patternToAdd->getName()){
                patterns->erase(patterns->begin() + i);
                break;
            }
        }
        patterns->push_back(patternToAdd);
        makerLogger.log("Added ", nameEntry->get_text());
    lock->unlock();

    FileReader::savePattern(*patternToAdd);
    makerLogger.log("Saved ", nameEntry->get_text());
}
