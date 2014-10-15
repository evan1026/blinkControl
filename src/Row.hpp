#ifndef ROW_HPP
#define ROW_HPP

#include <gtkmm.h>
#include "PatternPart.hpp"

#define ROW_HEIGHT 30

//I really only have this class because the current structure of the code has all of the contained
//components declared as heap variables, and keeping track and deleting them has become a pain
//without a container, but creating this struct makes it easy without having to rewrite the code
//to make these non-heap variables (which, honestly, I can't even think of a way to do right now.
struct Row {

    //defined in order of appearance in the window
    Gtk::Fixed       * holder;
    Gtk::Image       * deleteImage;
    Gtk::Button      * deleteButton;
    Gtk::Label       * colorLabel;
    Gdk::Color       * color;
    Gtk::ColorButton * colorButton;
    Gtk::Label       * timeLabel;
    Gtk::SpinButton  * timeSpinButton;
    Gtk::Label       * ledLabel;
    Gtk::RadioButton * radioButton1;
    Gtk::RadioButton * radioButton2;
    Gtk::RadioButton * radioButtonBoth;

    //Used to reconnect deleteButtons to new functions
    sigc::connection deleteButtonConnection;

    Row(PatternPart & part, int rowNum);

    void suicide();
};

#endif
