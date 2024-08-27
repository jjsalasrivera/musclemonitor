#include "ledBar.h"

LedBar::LedBar(int csPin, int clkPin, int dataPin, int numModules) 
{
    csPin = csPin;
    clkPin = clkPin;
    dataPin = dataPin;
    numModules = numModules;
    brightness = 0;
    mx = new MD_MAX72XX(HARDWARE_TYPE, dataPin, clkPin, csPin, numModules);
}

void LedBar::begin() 
{
    mx->begin();
    setBrightness(DEFAULT_BRIGHTNESS);
}

void LedBar::setFull() 
{
    mx->setRow(0, 0b11111111);
    mx->setRow(1, 0b11111111);
    mx->setRow(2, 0b11111111);
    mx->setRow(3, 0b11111111);
    mx->setRow(4, 0b11111111);
    mx->setRow(5, 0b11111111);
    mx->setRow(6, 0b11111111);
    mx->setRow(7, 0b11111111);
}

void LedBar::setEmpty() 
{
    mx->setRow(0, 0b00000000);
    mx->setRow(1, 0b00000000);
    mx->setRow(2, 0b00000000);
    mx->setRow(3, 0b00000000);
    mx->setRow(4, 0b00000000);
    mx->setRow(5, 0b00000000);
    mx->setRow(6, 0b00000000);
    mx->setRow(7, 0b00000000);
}

void LedBar::update(int percent1, int percent2, int percent3) 
{
    int porcentajes[] = {percent1, percent2, percent3};
  
    clear();
  
    for (int barra = 0; barra < 3; barra++) 
    {
        int porcentaje = constrain(porcentajes[barra], 0, 100);
        int columnasEncendidas = map(porcentaje, 0, 100, 0, 32);
    
        for (int col = 0; col < columnasEncendidas; col++) 
        {
            mx->setPoint(barra * 3, col, true);
            mx->setPoint(barra * 3 + 1, col, true);
        }
    }
}

void LedBar::clear() 
{
    mx->clear();
}

void LedBar::setBrightness(int brightness) 
{
    mx->control(MD_MAX72XX::INTENSITY, brightness);
}

LedBar::~LedBar() 
{
    delete mx;
}