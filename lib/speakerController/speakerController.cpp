#include "SpeakerController.h"
#include <Arduino.h>

SpeakerController::SpeakerController(int pin) 
{
    _pin = pin;
    ledcSetup(0, 1000, 8);
    ledcAttachPin(_pin, 0);
}

SpeakerController::~SpeakerController() 
{
}

void SpeakerController::playVictoryTune() 
{
    playTone(1000, 200);  // Tono 1 (1000 Hz) durante 200 ms
    playTone(1200, 200);  // Tono 2 (1200 Hz) durante 200 ms
    playTone(1400, 200);  // Tono 3 (1400 Hz) durante 200 ms
    playTone(1600, 300);  // Tono 4 (1600 Hz) durante 300 ms
    playTone(2000, 500);  // Tono final (2000 Hz) durante 500 ms
}

void SpeakerController::playAskTune() 
{
    playTone(800, 100);  // Tono corto (800 Hz) durante 100 ms
    delay(150);          // Pausa de 150 ms
    playTone(800, 100);  // Repetir el tono (800 Hz) durante 100 ms
}

void SpeakerController::playErrorTune() 
{
    playTone(400, 300);   // Tono 1 (400 Hz) durante 300 ms
    playTone(300, 300);   // Tono 2 (300 Hz) durante 300 ms
    playTone(200, 500);   // Tono final (200 Hz) durante 500 ms
}

void SpeakerController::playTone(int frequency, int duration) {
  ledcWriteTone(0, frequency);  // Generar el tono con la frecuencia especificada
  delay(duration);              // Mantener el tono durante la duración especificada
  ledcWriteTone(0, 0);          // Detener el tono
  delay(50);                    // Pausa entre notas
}