#include "keypadController.h"

KeypadController::KeypadController(uint8_t rowPins[ROWS], uint8_t colPins[COLS]) 
{
    // Initialize the keypad with the specified row and column pins
    // Copy the row and column pin values to the member variables
    for (int i = 0; i < ROWS; i++) 
    {
        _rowPins[i] = rowPins[i];
    }

    for (int i = 0; i < COLS; i++) 
    {
        _colPins[i] = colPins[i];
    }

    keypad = new Keypad( makeKeymap(_keys), _rowPins, _colPins, ROWS, COLS );
}

char KeypadController::getKey() 
{
    return keypad->getKey();
}