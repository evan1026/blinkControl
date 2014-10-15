#ifndef PATTERN_MAKER_HPP
#define PATTERN_MAKER_HPP

#include <Logger/Logger.hpp>
#include <gtkmm.h>
#include "Row.hpp"
#include "Pattern.hpp"

class PatternMaker {
    Logger logger;

    Glib::RefPtr<Gtk::Fixed>  holder;
    Glib::RefPtr<Gtk::Entry>  nameEntry;
    Glib::RefPtr<Gtk::Window> window;

    Gtk::Button * addButton;

    std::mutex * lock;

    Pattern * pattern;

    std::vector<Row *> rows;

    public:
        PatternMaker(Glib::RefPtr<Gtk::Builder> builder, std::mutex * _lock);

        void addPattern();
        void addPart(PatternPart & part);
        void addPart();
        void deleteRow(int row);
        void show(Pattern * _pattern);
        void savePattern(std::vector<PatternPart> & newPattern);

        bool onWindowClose(GdkEventAny * event);
        void hideWindow();

};

#endif
