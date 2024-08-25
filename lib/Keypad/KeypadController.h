#ifndef KEYPADCONTROLLER_H
#define KEYPADCONTROLLER_H

#include <Keypad.h>

// define numero de filas
const uint8_t ROWS = 4;
// define numero de columnas
const uint8_t COLS = 3; 

class KeypadController
{
public:
    KeypadController(uint8_t rowPins[ROWS], uint8_t colPins[COLS]);
    char getKey();

private:
    Keypad *keypad;

    uint8_t _rowPins[ROWS];
    uint8_t _colPins[COLS];

    char _keys[ROWS][COLS] = 
    {
        { '1', '2', '3' },
        { '4', '5', '6' },
        { '7', '8', '9' },
        { '*', '0', '#' }
    };
};

#endif // KEYPAD_H