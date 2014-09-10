#ifndef PATTERN_MAKER_HPP
#define PATTERN_MAKER_HPP

#include <gtkmm.h>

#define PATTERN_MAKER_ROW_HEIGHT 30

namespace PatternMaker {

    void initParts(Glib::RefPtr<Gtk::Builder> builder, std::mutex * _lock, std::vector<Pattern *> * _patterns);
    void addPattern();
    void addPart();

    void addPart();
    void deleteRow(int row);
    void addPattern();

};

#endif
