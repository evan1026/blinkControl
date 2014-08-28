#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <chrono>
#include <iomanip>
#include "Pattern.hpp"

Logger patternLogger;

Pattern::Pattern() {
    PatternPart defaultPart(0x00, 0x00, 0x00, 300); //By default, turn off in 300 ms
    parts.push_back(defaultPart);
    name = "Off";
}

Pattern::Pattern(std::vector<PatternPart>& _parts, std::string _name) {
    parts = _parts;
    name  = _name;
}

void Pattern::play() {
    if (!playing){
        playing = true;
        playingThread = std::thread(&Pattern::doPlay, this);
        playingThread.detach();
    } else {
        patternLogger.log(Logger::LogType::Error, "Pattern already running!");
    }
}

void Pattern::stop(){
    m.lock();
        playing = false;
    m.unlock();
}

bool Pattern::isPlaying(){
    m.lock();
        bool isPlaying = playing;
    m.unlock();
    return isPlaying;
}

void Pattern::doPlay(){
    bool isPlaying = true;
    int  index = -1; //Getting time from the next value, so starting here will get the time of index 0 first

    double redStep,
           greenStep,
           blueStep;

    std::string output = MiscUtils::executeGetOutput("blink1-tool --rgbread");

    double r, g, b;

    int32_t prevColor;

    r = std::stoi(output.substr(19, 4), nullptr, 16);
    g = std::stoi(output.substr(24, 4), nullptr, 16);
    b = std::stoi(output.substr(29, 4), nullptr, 16);

    m.lock();
        long endTime = getTime() + parts[index + 1].time;

        redStep   = (parts[index + 1].r - r) / parts[index + 1].time;
        greenStep = (parts[index + 1].g - g) / parts[index + 1].time;
        blueStep  = (parts[index + 1].b - b) / parts[index + 1].time;
    m.unlock();

    long prevTime = getTime();
    while(isPlaying){

        long time = getTime();
        long elapsed = time - prevTime;
        prevTime = time;

        r += redStep   * elapsed;
        g += greenStep * elapsed;
        b += blueStep  * elapsed;

        if (r > 255) r = 255;
        if (r < 0)   r =   0;

        if (b > 255) b = 255;
        if (b < 0)   b =   0;

        if (g > 255) g = 255;
        if (g < 0)   g =   0;

        int32_t color = (int)r * 0x010000 +
                        (int)g * 0x000100 +
                        (int)b * 0x000001; //Redundant, I know, but it looks nice

        if (color != prevColor){
            if (debug) std::cout << std::hex << std::setfill('0') << std::setw(6) << color << std::endl;
            MiscUtils::execute(Logger::makeString("blink1-tool -m 0 --rgb=",r,",",g,",",b));
            prevColor = color;
        }

        if (getTime() >= endTime){
            index++;

            m.lock();
                endTime = getTime() + parts[index + 1].time;

                r = parts[index].r;
                g = parts[index].g;
                b = parts[index].b;

                redStep   = (parts[index + 1].r - r) / parts[index + 1].time;
                greenStep = (parts[index + 1].g - g) / parts[index + 1].time;
                blueStep  = (parts[index + 1].b - b) / parts[index + 1].time;

            m.unlock();
        }

        m.lock();

            isPlaying = playing;

            if (index == parts.size() - 1){
                isPlaying = false;
            }
        m.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    m.lock();
        playing = false;
    m.unlock();
}

long Pattern::getTime(){
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
    return millis.count();
}

std::string Pattern::getName(){
    return name;
}
