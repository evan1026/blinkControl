#include <Logger/Logger.hpp>
#include <MiscUtils/MiscUtils.hpp>
#include <chrono>
#include <iomanip>
#include "blink1-lib/blink1-lib.h"
#include "Pattern.hpp"

Logger patternLogger;

Pattern::Pattern() {
    PatternPart defaultPart(0x00, 0x00, 0x00, 300, 0); //By default, turn off in 300 ms
    parts.push_back(defaultPart);
    name = "Off";
}

Pattern::Pattern(std::vector<PatternPart>& _parts, std::string _name) {
    parts = _parts;
    name  = _name;
}

void Pattern::play(blink1_device * blink) {
    if (!playing){
        playing = true;
        playingThread = std::thread(&Pattern::doPlay, this, blink);
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

void Pattern::doPlay(blink1_device * blink){

    if (blink == NULL) {
        patternLogger.log(Logger::LogType::Error, "Why u no leave blink(1) plugged in?");
        exit(4);
    }

    bool isPlaying = true;
    int  index = -1; //Getting time from the next value, so starting here will get the time of index 0 first

    double red1Step,   red2Step,
           green1Step, green2Step,
           blue1Step,  blue2Step;

    double r1, r2, g1, g2, b1, b2;
    short  led;
    int32_t prevColor1,
            prevColor2;

    uint8_t  readRed                = 0;
    uint8_t  readGreen              = 0;
    uint8_t  readBlue               = 0;
    uint16_t INeedThisToPreventSegV = 0; //The function needs a place to store the time the device was last sent

    blink1_readRGB(blink, &INeedThisToPreventSegV, &readRed, &readGreen, &readBlue, 1);
    r1 = readRed;
    g1 = readGreen;
    b1 = readBlue;

    blink1_readRGB(blink, &INeedThisToPreventSegV, &readRed, &readGreen, &readBlue, 2);
    r2 = readRed;
    g2 = readGreen;
    b2 = readBlue;

    prevColor1 = (int)r1 * 0x010000 +
                 (int)g1 * 0x000100 +
                 (int)b1 * 0x000001; //Redundant, I know, but it looks nice

    prevColor2 = (int)r2 * 0x010000 +
                 (int)g2 * 0x000100 +
                 (int)b2 * 0x000001; //Redundant, I know, but it looks nice

    m.lock();

        led = parts[index + 1].led;

        long endTime = getTime() + parts[index + 1].time;

        if (led == 0){
            red1Step   = (parts[index + 1].r - r1) / parts[index + 1].time;
            green1Step = (parts[index + 1].g - g1) / parts[index + 1].time;
            blue1Step  = (parts[index + 1].b - b1) / parts[index + 1].time;

            red2Step   = (parts[index + 1].r - r2) / parts[index + 1].time;
            green2Step = (parts[index + 1].g - g2) / parts[index + 1].time;
            blue2Step  = (parts[index + 1].b - b2) / parts[index + 1].time;
        } else if (led == 1){
            red1Step   = (parts[index + 1].r - r1) / parts[index + 1].time;
            green1Step = (parts[index + 1].g - g1) / parts[index + 1].time;
            blue1Step  = (parts[index + 1].b - b1) / parts[index + 1].time;

            red2Step   = 0;
            green2Step = 0;
            blue2Step  = 0;
        } else if (led == 2) {
            red2Step   = (parts[index + 1].r - r2) / parts[index + 1].time;
            green2Step = (parts[index + 1].g - g2) / parts[index + 1].time;
            blue2Step  = (parts[index + 1].b - b2) / parts[index + 1].time;

            red1Step   = 0;
            green1Step = 0;
            blue1Step  = 0;
        }

    m.unlock();

    long prevTime = getTime();
    while(isPlaying){

        long time = getTime();
        long elapsed = time - prevTime;
        prevTime = time;

        r1 += red1Step   * elapsed;
        g1 += green1Step * elapsed;
        b1 += blue1Step  * elapsed;

        r2 += red2Step   * elapsed;
        g2 += green2Step * elapsed;
        b2 += blue2Step  * elapsed;

        if (r1 > 255) r1 = 255;
        if (r1 < 0)   r1 =   0;

        if (b1 > 255) b1 = 255;
        if (b1 < 0)   b1 =   0;

        if (g1 > 255) g1 = 255;
        if (g1 < 0)   g1 =   0;

        if (r2 > 255) r2 = 255;
        if (r2 < 0)   r2 =   0;

        if (b2 > 255) b2 = 255;
        if (b2 < 0)   b2 =   0;

        if (g2 > 255) g2 = 255;
        if (g2 < 0)   g2 =   0;


        int32_t color1 = (int)r1 * 0x010000 +
                         (int)g1 * 0x000100 +
                         (int)b1 * 0x000001; //Redundant, I know, but it looks nice

        int32_t color2 = (int)r2 * 0x010000 +
                         (int)g2 * 0x000100 +
                         (int)b2 * 0x000001; //Redundant, I know, but it looks nice


        if (color1 != prevColor1){
            if (debug)  std::cout << led << " " << std::hex << std::setfill('0') << std::setw(6) << color1 << std::endl;
            if (!debug) blink1_fadeToRGBN(blink, 0, (uint8_t)r1, (uint8_t)g1, (uint8_t)b1, 1); //Had to use fade because set doesn't all for individual control
            prevColor1 = color1;
        }

        if (color2 != prevColor2){
            if (debug)  std::cout << led << " " << std::hex << std::setfill('0') << std::setw(6) << color2 << std::endl;
            if (!debug) blink1_fadeToRGBN(blink, 0, (uint8_t)r2, (uint8_t)g2, (uint8_t)b2, 2); //Had to use fade because set doesn't all for individual control

            prevColor2 = color2;
        }

        if (getTime() >= endTime){
            index++;

            m.lock();

                led = parts[index + 1].led;

                endTime = getTime() + parts[index + 1].time;

                if (led == 0){
                    red1Step   = (parts[index + 1].r - r1) / parts[index + 1].time;
                    green1Step = (parts[index + 1].g - g1) / parts[index + 1].time;
                    blue1Step  = (parts[index + 1].b - b1) / parts[index + 1].time;

                    red2Step   = (parts[index + 1].r - r2) / parts[index + 1].time;
                    green2Step = (parts[index + 1].g - g2) / parts[index + 1].time;
                    blue2Step  = (parts[index + 1].b - b2) / parts[index + 1].time;
                } else if (led == 1){
                    red1Step   = (parts[index + 1].r - r1) / parts[index + 1].time;
                    green1Step = (parts[index + 1].g - g1) / parts[index + 1].time;
                    blue1Step  = (parts[index + 1].b - b1) / parts[index + 1].time;

                    red2Step   = 0;
                    green2Step = 0;
                    blue2Step  = 0;
                } else if (led == 2) {
                    red2Step   = (parts[index + 1].r - r2) / parts[index + 1].time;
                    green2Step = (parts[index + 1].g - g2) / parts[index + 1].time;
                    blue2Step  = (parts[index + 1].b - b2) / parts[index + 1].time;

                    red1Step   = 0;
                    green1Step = 0;
                    blue1Step  = 0;
                }

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
