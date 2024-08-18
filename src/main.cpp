#include <Arduino.h>
#include <ledBar.h>

////////////////////////
// LED BAR DEFINITION //
#define LED_DATA_PIN 23
#define LED_CLK_PIN  18
#define LED_CS_PIN   15
#define NUM_MODULES  4
LedBar *ledBar;
////////////////////////


void setup() {
  // put your setup code here, to run once:
  ledBar = new LedBar(LED_CS_PIN, LED_CLK_PIN, LED_DATA_PIN, NUM_MODULES);
  ledBar->begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  ledBar->update(100, 50, 25);
}

