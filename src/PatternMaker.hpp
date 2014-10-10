#ifndef PATTERN_MAKER_HPP
#define PATTERN_MAKER_HPP

#include <Logger/Logger.hpp>
#include <gtkmm.h>
#include "Pattern.hpp"

#define PATTERN_MAKER_ROW_HEIGHT 30

class PatternMaker {
    Logger logger;

    Glib::RefPtr<Gtk::Fixed>  holder;
    Glib::RefPtr<Gtk::Entry>  nameEntry;
    Glib::RefPtr<Gtk::Window> window;

    Gtk::Button * addButton;

    std::mutex * lock;

    std::string pattern;
    std::vector<Pattern *> * patterns;

    bool mainWindowShouldUpdate = false;

    public:
        PatternMaker(Glib::RefPtr<Gtk::Builder> builder, std::mutex * _lock, std::vector<Pattern *> * patterns);

        void addPattern();
        void addPart(PatternPart * part);
        void addPart();
        void deleteRow(int row);
        void show(std::string _pattern);

        bool onWindowClose(GdkEventAny * event);
        void hideWindow();

        bool mainWindowNeedsUpdate();
        void mainWindowUpdated();

};

#endif
