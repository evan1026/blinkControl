#ifndef PATTERN_PART_HPP
#define PATTERN_PART_HPP

#include <stdint.h>

struct PatternPart {

    short r,
          g,
          b;
    int   time; //Time to get to this color

    PatternPart(short _r, short _g, short _b, int _time) : r(_r), g(_g), b(_b), time(_time) {}

};

#endif
