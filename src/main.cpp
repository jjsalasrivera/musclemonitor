#include <Arduino.h>
#include <ledBar.h>
#include <speakerController.h>
#include <KeypadController.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <vector>

#define STORAGE_NAMESPACE "storage"
#define STORAGE_KEY "int_" 

////////////////////////
// LED BAR DEFINITION //
#define LED_DATA_PIN 15
#define LED_CLK_PIN  18
#define LED_CS_PIN   2
#define NUM_MODULES  4
LedBar *ledBar = NULL;
////////////////////////

////////////////////////
// SPEAKER DEFINITION //
#define SPEAKER_PIN 13
SpeakerController *speaker = NULL;
////////////////////////

////////////////////////
// KEYBOARD DEFINITION //
#define ROWS 4
#define COLS 3
uint8_t colPins[COLS] = { 25, 33, 32 };
uint8_t rowPins[ROWS] = { 12, 14, 27, 26 };
KeypadController *keypadController = NULL;
////////////////////////

////////////////////////
// EMG DEFINITION //
#define EMG_PIN 36
const int EMG_BACKGORUND_SCALE = 205;
const int EMG_MIN_SCALE = 0;
const int EMG_MAX_SCALE = 4095;
////////////////////////

void keyPressedManager(char key);
void deleteAllNVS();
void deleteParameterFromNVS(int programLoaded);
int loadParameterFromNVS(int programLoaded);
void beginProgram();
int getAverage();
void saveValue(int programLoaded, int value);
bool isNumber(String str);

String keySequence = "";

bool valueLoaded = false;
bool justLoaded = false;
int savedValue = 0;
int programLoaded = 0;

#define TIME_TO_AVERAGE 15000
unsigned long beginTime = 0;
unsigned long currentTime = 0;

std::vector<int> emgValues;
int average = 0;

void setup() 
{
    Serial.begin(9600);
    // put your setup code here, to run once:
    ledBar = new LedBar(LED_CS_PIN, LED_CLK_PIN, LED_DATA_PIN, NUM_MODULES);
    ledBar->begin();

    speaker = new SpeakerController(SPEAKER_PIN);
    keypadController = new KeypadController(rowPins, colPins);

    // Secuencia de inicio
    Serial.println("Bienvenido al programa de EMG!");
    speaker->playShortBeep();
    ledBar->setFull();
    delay(250);
    ledBar->setEmpty();
    delay(250);
    ledBar->setFull();
    delay(250);
    ledBar->setEmpty();
    delay(250);
    ledBar->setFull();
    delay(250);
    ledBar->setEmpty();
    speaker->playAskTune();
}

void loop() 
{
    if( valueLoaded )
    {
        if( !justLoaded )
        {
            justLoaded = true;
            beginProgram();
        }
        // Leer valor de EMG
        int emgValue = analogRead(EMG_PIN);
        //Serial.println("Valor EMG: " + String(emgValue));
        Serial.print(">emgValue:");
        Serial.println(emgValue);
    
        // restar fondo de escala
        if( emgValue <= EMG_BACKGORUND_SCALE )
            emgValue = 0;

        // Escalar valor
        int normalizedValue = map(emgValue, EMG_MIN_SCALE, EMG_MAX_SCALE, 0, 100);
        Serial.print(">normalizedValue:");
        //Serial.println("Valor Normalizado: " + String(normalizedValue));
        Serial.println(normalizedValue);

        if(normalizedValue > 0)
            emgValues.push_back(normalizedValue);

        if( millis() - currentTime > TIME_TO_AVERAGE )
        {
            Serial.println("Cantidad de valores: " + String(emgValues.size())); 

            currentTime = millis();
            average = getAverage();  
            //Serial.println("Promedio: " + String(average));
            Serial.print(">average:");
            Serial.println(average);
        
            if( average > savedValue )
            {
                savedValue = average;
                saveValue(programLoaded, savedValue);
                speaker->playVictoryTune();
                Serial.print(">savedValue:");
                Serial.println(savedValue);
            }
        }
        ledBar->update(normalizedValue, average, savedValue);
    }

    /////////////////////////
    char key = keypadController->getKey();

    if( key )
    {
        Serial.println("Tecla pulsada: " + String(key));
        speaker->playShortBeep();
        keyPressedManager(key); 
    }

    delay(100);
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
    average = 0;
    emgValues.clear();
}

void keyPressedManager(char key)
{
    // Codes '#XXXX#' where XXXX is the code with no length limit will load XXXX from memory
    // Code '*XXXX*' will delete NVS memory
    // Code '*#*' Will delete all NVS memory
    // Code '***" will stop program
    // Otherwise the code is wrong and will be ignored and play error tone

    keySequence += key;

    // Verifica los comandos ingresados
    if (keySequence.startsWith("#") && keySequence.endsWith("#") && keySequence.length() > 2)
    {
        // Comando "#<num>#": Cargar valor guardado
        String num = keySequence.substring(1, keySequence.length() - 1);
        // Verifica que <num> no esté vacío
        if (num.length() > 0 && isNumber(num)) 
        {  
            Serial.println("Cargar valor guardado de: " + num);
            savedValue = loadParameterFromNVS(num.toInt());
            programLoaded = num.toInt();
            valueLoaded = true;
        } 
        else 
        {
            speaker->playErrorTune();
            valueLoaded = false;
            Serial.println("Error: Comando inválido, falta <num>");
        }
        keySequence.clear();
    }
    else if (keySequence.startsWith("*") && keySequence.endsWith("*") && keySequence.length() > 2)
    {
        if (keySequence.equals("*#*")) 
        {
            deleteAllNVS();
        } 
        else if (keySequence.equals("***")) 
        {
            valueLoaded = false;
            speaker->playOKTune();
            ledBar->setEmpty();
        } 
        else 
        {
            // Comando "*<num>*": Borrar valor guardado
            String num = keySequence.substring(1, keySequence.length() - 1);
            // Verifica que <num> no esté vacío
            if (num.length() > 0 && isNumber(num)) 
            {
                try
                {
                    deleteParameterFromNVS(num.toInt());
                    speaker->playOKTune();
                }
                catch(const std::exception& e)
                {
                    Serial.println("Error: " + String(e.what()));
                    speaker->playErrorTune();
                }
            } 
            else 
            {
                speaker->playErrorTune();
            }
            
        }
        valueLoaded = false;
        keySequence.clear();
    }
    /*else if (keySequence.length() >= 3) 
    {
        // Longitud mínima de un comando válido ya alcanzada
        if (!keySequence.equals("*") && !keySequence.equals("#")) 
        {
            Serial.println("Error: Comando no reconocido: " + keySequence);
            speaker->playErrorTune();
            keySequence.clear(); // Reinicia la entrada en caso de error
        }
    }*/
}

void deleteParameterFromNVS(int programLoaded)
{
    Serial.println("Borrando parámetro " + String(programLoaded) + " de la memoria NVS...");
    
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
        Serial.println("Error ("+ String(ret) + ") opening NVS handle!\n");
        speaker->playErrorTune();
    } 
    else 
    {
        char key[16];
        snprintf(key, sizeof(key), "%s%d", STORAGE_KEY, programLoaded);
        // Borrar el parámetro
        ret = nvs_erase_key(my_handle, key);
        if (ret != ESP_OK) 
        {
            Serial.println("Error (" + String(ret) + ") erasing key!\n");
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

int loadParameterFromNVS(int programLoaded)
{
    int res = 0;
    Serial.println("Cargando parámetro " + String(STORAGE_KEY) + String(programLoaded)+ " de la memoria NVS...");

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
        Serial.println("Error (" + String(ret) + ") opening NVS handle!\n");
        speaker->playErrorTune();
    } 
    else 
    {
        char key[16];
        snprintf(key, sizeof(key), "%s%d", STORAGE_KEY, programLoaded);
        // Leer el parámetro
        int value = 0;
        ret = nvs_get_i32(my_handle, key, &value);
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
                Serial.println("Error ("+ String(ret) + ") reading!\n");
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
        Serial.println("Error (" + String(ret) + ") opening NVS handle!\n");
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

int getAverage()
{
    int sum = 0;
    int size = emgValues.size();
    
    if( size == 0 )
        return 0;

    for(int i = 0; i < emgValues.size(); i++)
        sum += emgValues[i];

    int res = sum / emgValues.size();
    emgValues.clear();

    return res;
}

void saveValue(int programLoaded, int value)
{
    Serial.println("Salvando " + String(STORAGE_KEY) + String(programLoaded) + " : " + String(value) + " en la memoria NVS...");

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
        // Escribir el parámetro
        char key[16];
        snprintf(key, sizeof(key), "%s%d", STORAGE_KEY, programLoaded);

        ret = nvs_set_i32(my_handle, key, value);
        if (ret != ESP_OK) 
        {
            Serial.println("Error (%s) writing!\n");
            speaker->playErrorTune();
        }
        else 
        {
            ret = nvs_commit(my_handle);
            if (ret != ESP_OK) 
            {
                Serial.println("Error (%s) committing!\n");
                speaker->playErrorTune();
            }
            Serial.println("Valor salvado correctamente!");
            speaker->playOKTune();
        }
    }

    nvs_close(my_handle);
}

bool isNumber(String str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if( !isDigit(str[i]) )
            return false;
    }

    return true;
}