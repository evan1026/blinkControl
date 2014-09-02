#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <vector>
#include <mutex>
#include <thread>
#include "blink1-lib/blink1-lib.h"
#include "PatternPart.hpp"

class Pattern {

    bool        playing = false;
    bool        debug   = false;
    std::mutex  m;
    std::string name;

    void doPlay(blink1_device * blink);
    long getTime();
    std::thread playingThread;

    public:
        std::vector<PatternPart> parts;

        Pattern();
        Pattern(std::vector<PatternPart>& _parts, std::string _name);

        void play(blink1_device * blink);
        void stop();
        bool isPlaying();

        std::string getName();

};

#endif
