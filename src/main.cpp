#include <Arduino.h>
#include <ledBar.h>
#include <speakerController.h>
#include <KeypadController.h>
#include <nvs_flash.h>
#include <nvs.h>

#define STORAGE_NAMESPACE "storage"

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
#define SPEAKER_PIN 35
SpeakerController *speaker = NULL;
////////////////////////

////////////////////////
// KEYBOARD DEFINITION //
#define ROWS 4
#define COLS 3
uint8_t colPins[COLS] = { 26, 25, 33 };
uint8_t rowPins[ROWS] = { 13, 12, 14, 27 };
KeypadController *keypadController = NULL;
////////////////////////

void keyPressedManager(char key);
void deleteAllNVS();
void deleteParameterFromNVS(int number);
int loadParameterFromNVS(int number);
void beginProgram();

bool firstTime = true;
String keySequence = "";

bool valueLoaded = false;
bool justLoaded = false;
int savedValue = 0;

#define TIME_TO_AVERAGE 15000
unsigned long beginTime = 0;
unsigned long currentTime = 0;

void setup() 
{
    // put your setup code here, to run once:
    ledBar = new LedBar(LED_CS_PIN, LED_CLK_PIN, LED_DATA_PIN, NUM_MODULES);
    ledBar->begin();

    speaker = new SpeakerController(SPEAKER_PIN);
    keypadController = new KeypadController(rowPins, colPins);
}

void loop() 
{
    if( firstTime )
    {
        firstTime = false;
        ledBar->update(100, 100, 100);
        delay(1000);
        ledBar->update(0, 0, 0);
        speaker->playAskTune();
    }

    if( valueLoaded )
    {
        if( !justLoaded )
        {
            justLoaded = true;
            beginProgram();
        }
        // Leer valor de EMG
        int emgValue = analogRead(34);
        Serial.println("Valor EMG: " + String(emgValue));

        if( millis() - currentTime > TIME_TO_AVERAGE )
        {
            currentTime = millis();
            
            speaker->playVictoryTune();
            ledBar->setFull();
            delay(1000);
            ledBar->setEmpty();
            valueLoaded = false;
            justLoaded = false;
        }

        // Actualizar barra de LEDs
        int percent = map(emgValue, 0, 4095, 0, 100);
        ledBar->update(percent, percent, percent);
    }

    char key = keypadController->getKey();

    if( key )
    {
        Serial.println("Tecla pulsada: " + String(key));
        speaker->playShortBeep();
        keyPressedManager(key); 
    }
}

void beginProgram()
{
    ledBar->setFull();
    speaker->playMediumTune();
    delay(1000);
    ledBar->setEmpty();
    delay(500);
    ledBar->setFull();
    speaker->playMediumTune();
    delay(1000);
    ledBar->setEmpty();
    delay(500);
    ledBar->setFull();
    speaker->playLongTune();
    delay(1000);
    ledBar->setEmpty();
    beginTime = millis();
    currentTime = beginTime;
}

void keyPressedManager(char key)
{
    // Codes '#XXXX#' where XXXX is the code with no length limit will load XXXX from memory
    // Code '*XXXX*' will delete NVS memory
    // Code '*#*' Will delete all NVS memory
    // Code '***" will stop program
    // Otherwise the code is wrong and will be ignored and play error tone

    if (key == '#' || key == '*') 
    {
        if (keySequence.isEmpty()) 
        {
            keySequence += key;
        } 
        else if (keySequence == "*#*") 
        {
            deleteAllNVS();
            keySequence.clear();
            valueLoaded = false;
        }
        else if(keySequence == "***")
        {
            speaker->playErrorTune();
            keySequence.clear();
            valueLoaded = false;
        }
        else if (keySequence[0] == '#' && key == '#' && keySequence.length() > 2) 
        {
            try
            {
                int number = keySequence.substring(1, keySequence.length() - 2).toInt();
                savedValue = loadParameterFromNVS(number);
                valueLoaded = true;
            }
            catch(const std::exception& e)
            {
                Serial.println("Error: " + String(e.what()));
                speaker->playErrorTune();
                valueLoaded = false;
            }
            
            keySequence.clear();
        } 
        else if (keySequence[0] == '*' && key == '*' && keySequence.length() > 2) 
        {
            try
            {
                int number = keySequence.substring(1, keySequence.length() - 2).toInt();
                deleteParameterFromNVS(number);            
            }
            catch(const std::exception& e)
            {
                Serial.println("Error: " + String(e.what()));
                speaker->playErrorTune();
            }

            valueLoaded = false;
            keySequence.clear();
        }
        if(keySequence.length() > 2)
        {
            speaker->playErrorTune();
            keySequence.clear();
            valueLoaded = false;
        }
        if( (keySequence[0] == '#' && key == '*') || (keySequence[0] == '*' && key == '#'))
        {
            speaker->playErrorTune();
            keySequence.clear();
            valueLoaded = false;
        }
        else 
        {
            keySequence += key;
        }
    } 
    else if (key >= '0' && key <= '9' && keySequence.length() > 0)
    {
        keySequence += key;
    } 
    else 
    {
        speaker->playErrorTune();
        keySequence.clear();
        valueLoaded = false;
    }
}

void deleteParameterFromNVS(int number)
{
    Serial.println("Borrando parámetro " + String(number) + " de la memoria NVS...");
    
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Abrir espacio de almacenamiento
    nvs_handle_t my_handle;
    ret = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if(ret != ESP_OK) 
    {
        Serial.println("Error (%s) opening NVS handle!\n");
        speaker->playErrorTune();
    } 
    else 
    {
        // Borrar el parámetro
        ret = nvs_erase_key(my_handle, String(number).c_str());
        if (ret != ESP_OK) 
        {
            Serial.println("Error (%s) erasing key!\n");
            speaker->playErrorTune();
        } 
        else 
        {
            Serial.println("Parámetro borrado correctamente!");
            speaker->playOKTune();
        }
    }

    // Cerrar espacio de almacenamiento
    nvs_close(my_handle);
}

int loadParameterFromNVS(int number)
{
    int res = 0;
    Serial.println("Cargando parámetro " + String(number) + " de la memoria NVS...");

    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Abrir espacio de almacenamiento
    nvs_handle_t my_handle;
    ret = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if(ret != ESP_OK) 
    {
        Serial.println("Error (%s) opening NVS handle!\n");
        speaker->playErrorTune();
    } 
    else 
    {
        // Leer el parámetro
        int value = 0;
        ret = nvs_get_i32(my_handle, String(number).c_str(), &value);
        switch (ret) 
        {
            case ESP_OK:
                Serial.println("Valor cargado correctamente: " + String(value));
                speaker->playOKTune();
                res = value;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                Serial.println("El parámetro no existe!");
                speaker->playOKTune();
                break;
            default:
                Serial.println("Error (%s) reading!\n");
                speaker->playErrorTune();
        }
    }

    nvs_close(my_handle);

    return res;
}

void deleteAllNVS()
{
    Serial.println("Borrando todos los parámetros de la memoria NVS...");

    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Abrir espacio de almacenamiento
    nvs_handle_t my_handle;
    ret = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if(ret != ESP_OK) 
    {
        Serial.println("Error (%s) opening NVS handle!\n");
        speaker->playErrorTune();
    } 
    else 
    {
        // Borrar todos los parámetros
        nvs_erase_all(my_handle);
        Serial.println("Todos los parámetros han sido borrados correctamente!");
        speaker->playOKTune();
    }

    nvs_close(my_handle);
}
