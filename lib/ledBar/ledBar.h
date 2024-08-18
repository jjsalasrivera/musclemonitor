#ifndef LEDBAR_H
#define LEDBAR_H

#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define DEFAULT_BRIGHTNESS 15


class LedBar {
public:
    LedBar(int csPin, int clkPin, int DataPin, int numModules);
    void begin();
    void clear();
    void update(int percent1, int percent2, int percent3);

    ~LedBar();

private:
    int csPin;
    int numModules;
    int clkPin;
    int dataPin;
    int brightness;
    MD_MAX72XX *mx;

    void setBrightness(int brightness);
};

#endif