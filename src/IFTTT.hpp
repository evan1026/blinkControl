#ifndef IFTTT_HPP
#define IFTTT_HPP

#include <string>
#include <vector>
#include "Event.hpp"
#include "blink1-lib/blink1-lib.h"

namespace IFTTT {
    std::string getEvents(std::string blinkID);
    std::vector<Event> processEvents(std::string eventJSON);
    int writer(char *data, size_t size, size_t nmemb, std::string *buffer_in);
}

#endif
