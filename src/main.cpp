#include <Arduino.h>
#include <ledBar.h>
#include <speakerController.h>

////////////////////////
// LED BAR DEFINITION //
#define LED_DATA_PIN 23
#define LED_CLK_PIN  18
#define LED_CS_PIN   15
#define NUM_MODULES  4
LedBar *ledBar;
////////////////////////

////////////////////////
// SPEAKER DEFINITION //
#define SPEAKER_PIN 25
SpeakerController *speaker;
////////////////////////


void setup() 
{
  // put your setup code here, to run once:
  ledBar = new LedBar(LED_CS_PIN, LED_CLK_PIN, LED_DATA_PIN, NUM_MODULES);
  ledBar->begin();

  speaker = new SpeakerController(SPEAKER_PIN);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  //ledBar->update(100, 50, 25);
  speaker->playVictoryTune();
  delay(1000);
  speaker->playAskTune();
  delay(1000);
  speaker->playErrorTune();
  delay(1000);
}

