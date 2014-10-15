#include <gtkmm.h>
#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <time.h>
#include "Pattern.hpp"
#include "FileReader.hpp"
#include "Row.hpp"
#include "PatternMaker.hpp"

PatternMaker::PatternMaker(Glib::RefPtr<Gtk::Builder> builder, std::mutex * _lock){
    holder    = Glib::RefPtr<Gtk::Fixed>:: cast_dynamic(builder->get_object("PatternPartHolder"));
    nameEntry = Glib::RefPtr<Gtk::Entry>:: cast_dynamic(builder->get_object("NameEntry"));
    window    = Glib::RefPtr<Gtk::Window>::cast_dynamic(builder->get_object("PatternMaker"));

    window->signal_delete_event().connect(sigc::mem_fun(this, &PatternMaker::onWindowClose));

    Glib::RefPtr<Gtk::Button>::cast_dynamic(builder->get_object("SubmitButton"))->signal_clicked().connect(sigc::mem_fun(this, &PatternMaker::addPattern), false);

    addButton = new Gtk::Button("Add part");
    addButton->signal_clicked().connect(sigc::mem_fun(this, static_cast<void (PatternMaker::*)(void)>(&PatternMaker::addPart)));
    addButton->show();
    addButton->set_size_request(50, 25);

    holder->put(*addButton, 0, 0);

    lock = _lock;

    pattern = nullptr;
}

void PatternMaker::addPart() {
    PatternPart defaultPart(0, 0, 0, 1000, 0); //r=0, g=0, b=0, time=1000ms, led=0
    addPart(defaultPart);
}

void PatternMaker::addPart(PatternPart & part){ //returns new row
    std::vector<Gtk::Widget *> children = holder->get_children();

    int highest = -1; //Makes row names start at 0
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

    Row * newRow = new Row(part, current);
    rows.push_back(newRow);

    sigc::connection deleteButtonConnection =
        newRow->deleteButton->signal_clicked().connect(sigc::bind(sigc::mem_fun<int>(this, &PatternMaker::deleteRow), current));
    newRow->deleteButtonConnection = deleteButtonConnection;

    holder->move(*addButton, 0, (current + 1) * ROW_HEIGHT);
    holder->put(*newRow->holder, 0, current * ROW_HEIGHT);
}

void PatternMaker::deleteRow(int rowNum){

    //Move the rows ahead of the deleted one backwards 1
    for (int i = rowNum + 1; i < rows.size(); ++i) {
        holder->move(*rows[i]->holder, 0, (i - 1) * ROW_HEIGHT);
        rows[i]->holder->set_name(Logger::makeString("Row ", i - 1));

        rows[i]->deleteButtonConnection.disconnect();

        sigc::connection deleteButtonConnection =
            rows[i]->deleteButton->signal_clicked().connect(sigc::bind(sigc::mem_fun<int>(this, &PatternMaker::deleteRow), i - 1));

        rows[i]->deleteButtonConnection = deleteButtonConnection;
    }

    //Kill the row and remove it from the list
    rows[rowNum]->suicide();
    rows.erase(rows.begin() + rowNum);

    //Lastly, move the add button to match the new location of the rows
    holder->move(*addButton, 0, rows.size() * ROW_HEIGHT);

}

void PatternMaker::addPattern(){

    //Create a pattern first. By doing the processing first and then copying the values over
    //I can hopefully save the other thread from blocking for as long
    std::vector<PatternPart> newPattern;

    //Go through each row and extract values from it to the pattern
    for (int i = 0; i < rows.size(); ++i){
        int r,
            g,
            b,
            time;
        short led;

        Gdk::Color color = rows[i]->colorButton->get_color();
        r = color.get_red()   & 0xff;
        g = color.get_green() & 0xff;
        b = color.get_blue()  & 0xff;

        time = rows[i]->timeSpinButton->get_value_as_int();

        if      (rows[i]->radioButton1->get_active()) led = 1;
        else if (rows[i]->radioButton2->get_active()) led = 2;
        else                                          led = 0; //Because of radio button groups, radioButtonBoth must be active if the other two are not

        newPattern.push_back(PatternPart(r, g, b, time, led));
    }

    Glib::Threads::Thread* saveThread = Glib::Threads::Thread::create(sigc::bind(sigc::mem_fun<std::vector<PatternPart> &>(this, &PatternMaker::savePattern), newPattern));
    MarkUnused(saveThread);

    hideWindow();
}

void PatternMaker::savePattern(std::vector<PatternPart> & newPattern) {
    //Create a dialog in case we have to wait
    Gtk::Dialog notificationDialog("Waiting...");
    Gtk::Label notificationLabel("A pattern is currently playing. Waiting until it finishes...");
    notificationLabel.show();
    notificationDialog.get_content_area()->pack_start(notificationLabel);

    //Once this exits, we've successfully locked
    while (!lock->try_lock()) {
        notificationDialog.show();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
        notificationDialog.hide();
        pattern->clear();
        for (int i = 0; i < newPattern.size(); ++i) {
            pattern->parts.push_back(newPattern[i]);
        }
        logger.log("Added ", nameEntry->get_text());
    lock->unlock();

    //Make sure to save it to the file
    FileReader::savePattern(*pattern);
    logger.log("Saved ", nameEntry->get_text());
}

void PatternMaker::show(Pattern * _pattern) {
    window->show();

    pattern = _pattern;

    if (!pattern){
        logger.log(Logger::LogType::Error, "Pattern \"", _pattern, "\" does not exist.");
        return;
    }

    nameEntry->set_text(pattern->getName());

    for (int i = 0; i < pattern->parts.size(); ++i) addPart(pattern->parts[i]);
}

bool PatternMaker::onWindowClose(GdkEventAny * event) {
    hideWindow();
    return true;
}

void PatternMaker::hideWindow(){
    window->hide();

    while (rows.size() > 0) deleteRow(0);

}
