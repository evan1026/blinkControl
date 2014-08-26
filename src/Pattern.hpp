#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <vector>
#include <mutex>
#include <thread>
#include "PatternPart.hpp"

class Pattern {

    bool        playing = false;
    std::mutex  m;
    std::string name;

    void doPlay();
    long getTime();
    std::thread playingThread = std::thread(&Pattern::doPlay, this);

    public:
        std::vector<PatternPart> parts;

        Pattern();
        Pattern(std::vector<PatternPart>& _parts, std::string _name);

        void play();
        void stop();
        bool isPlaying();

        std::string getName();

};

#endif
