#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>

struct Event {

    std::string blink_id,
                name,
                source;

    int date;

    Event(std::string _blink_id, int _date, std::string _name, std::string _source) : blink_id(_blink_id), name(_name), source(_source), date(_date) {}
};

#endif
