#include <Logger/Logger.hpp>
#include <gtkmm.h>
#include "PatternPart.hpp"
#include "Row.hpp"

Row::Row(PatternPart & part, int rowNum) {

    holder = new Gtk::Fixed();
    holder->set_size_request(-1, ROW_HEIGHT);
    holder->set_name(Logger::makeString("Row ", rowNum));
    holder->show();

    deleteImage = new Gtk::Image(Gtk::Stock::STOP, Gtk::BuiltinIconSize::ICON_SIZE_SMALL_TOOLBAR);

    deleteButton = new Gtk::Button();
    deleteButton->set_name("Delete");
    deleteButton->set_size_request(30, ROW_HEIGHT);
    deleteButton->set_image(*deleteImage);
    deleteButton->show();

    colorLabel = new Gtk::Label("Color: ");
    colorLabel->show();

    color = new Gdk::Color();
    color->set_red  (part.r + part.r * 0x0100); //Basically, each color is represented twice in the variable
    color->set_green(part.g + part.g * 0x0100); //Not sure why, but that's what I got when I output it
    color->set_blue (part.b + part.b * 0x0100); //So I'll just leave it that way

    colorButton = new Gtk::ColorButton();
    colorButton->set_size_request(50, ROW_HEIGHT);
    colorButton->set_use_alpha(false);
    colorButton->set_color(*color);
    colorButton->show();

    timeLabel = new Gtk::Label("Time (ms): ");
    timeLabel->show();

    timeSpinButton = new Gtk::SpinButton(1);
    timeSpinButton->set_size_request(50, ROW_HEIGHT);
    timeSpinButton->get_adjustment()->set_lower(0);
    timeSpinButton->get_adjustment()->set_upper(std::numeric_limits<int>::max());
    timeSpinButton->get_adjustment()->set_step_increment(100);
    timeSpinButton->set_value(part.time);
    timeSpinButton->show();

    ledLabel = new Gtk::Label("Led: ");
    ledLabel->show();

    radioButton1 = new Gtk::RadioButton("1");
    radioButton1->set_size_request(30, ROW_HEIGHT);
    if (part.led == 1) radioButton1->set_active(true);
    radioButton1->show();

    radioButton2 = new Gtk::RadioButton("2");
    radioButton2->set_size_request(30, ROW_HEIGHT);
    radioButton2->join_group(*radioButton1);
    if (part.led == 2) radioButton2->set_active(true);
    radioButton2->show();

    radioButtonBoth = new Gtk::RadioButton("Both");
    radioButtonBoth->set_size_request(40, ROW_HEIGHT);
    radioButtonBoth->join_group(*radioButton1);
    if (part.led == 0) radioButtonBoth->set_active(true);
    radioButtonBoth->show();

    holder->put(*deleteButton,      0, 0);
    holder->put(*colorLabel,       50, 6);
    holder->put(*colorButton,      90, 0);
    holder->put(*timeLabel,       160, 6);
    holder->put(*timeSpinButton,      230, 0);
    holder->put(*ledLabel,        380, 6);
    holder->put(*radioButton1,    410, 0);
    holder->put(*radioButton2,    445, 0);
    holder->put(*radioButtonBoth, 480, 0);

}

void Row::suicide() {
    delete holder;
    delete deleteImage;
    delete deleteButton;
    delete colorLabel;
    delete color;
    delete colorButton;
    delete timeLabel;
    delete timeSpinButton;
    delete ledLabel;
    delete radioButton1;
    delete radioButton2;
    delete radioButtonBoth;
    delete this;
}
