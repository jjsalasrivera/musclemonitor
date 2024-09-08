# Muscle Monitor
A muscle intensity sensor with ESP32, speaker and a MAX7219 led bar

## ESP32
![ESP-WROOM32-30PIN-DEVBOARD.webp](Images/ESP-WROOM32-30PIN-DEVBOARD.webp)

## Barra Led MAX7219
| ESP32 | MAX7219 |
| ----- | ------- |
| 3V3   | VCC     |
| GND   | GND     |
| D15   | DIN     |
| D2    | CS      |
| D18   | CLK     |

## Altavoz
| ESP32 | Speaker |
| ----- | ------- |
| D13   | Red     |
| Gnd   | Black   |
## Keypad

| ESP32 | Keypad    |
| ----- | --------- |
| D25   | COL PIN 1 |
| D33   | COL PIN 2 |
| D32   | COL PIN 3 |
| D12   | ROW PIN 1 |
| D14   | ROW PIN 2 |
| D27   | ROW PIN 3 |
| D26   | ROW PIN 4 |

 ## Keypad Codes
 * **"#\<num\>#"**: Load saved value from <num>.
 * **"\*\<num\>\*"**: Delete saved value from <num>.
 * **"\*#\*"**: Delete all saved values.
 * **"\*\*\*"**: End program loaded

## EMG Sensor

| ESP32 | EMG   | 
| ----- | ----- | 
| D34   | S Out | 
| GND   | GND   | 