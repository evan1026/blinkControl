#ifndef PATTERN_PART_HPP
#define PATTERN_PART_HPP

#include <stdint.h>

struct PatternPart {

    short r,
          g,
          b;
    int   time; //Time to get to this color
    short led;

    PatternPart(short _r, short _g, short _b, int _time, short _led) : r(_r), g(_g), b(_b), time(_time), led(_led) {}

};

#endif
