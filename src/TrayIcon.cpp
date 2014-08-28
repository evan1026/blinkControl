#include "TrayIcon.hpp"

TrayIcon::TrayIcon() {
    set(Gtk::Stock::OK);
    set_visible(true);
    set_tooltip_text("Doesn't do anything yet. I'm working on it.");
}
