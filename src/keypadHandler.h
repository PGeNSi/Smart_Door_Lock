#ifndef KEYPADHANDLER_HEADER
#define  KEYPADHANDLER_HEADER

#include <Arduino.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <sysConfig.h>
#include <twowiresMutex.h>
#include <buzzer.h>
#include <SignalingVariables.h>

struct keypadMessage {
    char message[17] = "";
    bool returnPress = 0;
};

QueueHandle_t keypadMessageQueue;
SemaphoreHandle_t keypadMessageMutex;

const byte ROWS = 4; // Row Count for keypad
const byte COLS = 4; // Coulmn Count for keypad

// Keypad Button structure
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3}; // Keypad Row Map
byte colPins[COLS] = {4, 5, 6, 7}; // Keypad Column Map

Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, KEYPAD_I2C_ADDR, PCF8574 );

char keypadInput[16] = "";
int keypadCurrentIndex = 0;

void keypadMessageQueueMutexInit(){
    Serial.println("--> Initializing Keypad Message Queue and Mutex Object");
    keypadMessageQueue = xQueueCreate(5, sizeof( struct keypadMessage ));
    keypadMessageMutex = xSemaphoreCreateMutex();
    if(keypadMessageQueue == NULL){
        Serial.println("ERR--> Failed to Initialize Keypad Message Queue Object");
        ESP.restart();  
    }
    if(keypadMessageMutex == NULL){
        Serial.println("ERR--> Failed to Initialize Keypad Message Mutex Object");
        ESP.restart();
    }
    Serial.println("--> Keypad Message Queue and Mutex Object Initialized");
}

void resetKeypadQueue(){
    while(!(xSemaphoreTake( keypadMessageMutex, ( TickType_t ) KEYPAD_MESSAGE_MUTEX_RESET_QUEUE_FUNCTION_WAIT_TICK ) == pdTRUE)){}
    xQueueReset(keypadMessageQueue);
    String("").toCharArray(keypadInput,16);
    keypadCurrentIndex = 0;
    xSemaphoreGive(keypadMessageMutex);
}

void keypadTask( void * pvParameters ){
    while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(KEYPAD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
    keypad.begin( makeKeymap(keys) );
    xSemaphoreGive(twoWireMutex);
    for (;;){
        while(!(xSemaphoreTake( keypadMessageMutex, pdMS_TO_TICKS(KEYPAD_MESSAGE_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
        if(!keypadReadEnable) continue;
        while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(KEYPAD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
        char key = keypad.getKey(); 
        xSemaphoreGive(twoWireMutex);
        if (key) {
            if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                buzzerOn();
                vTaskDelay(pdMS_TO_TICKS(100));
                buzzerOff();
                xSemaphoreGive(buzzerMutex);
            }
            if(keypadCurrentIndex > 16) continue;
            struct keypadMessage keypadMesg;
            if(key == '*'){
                if(keypadCurrentIndex == 0) continue;
                keypadCurrentIndex--;
                keypadInput[keypadCurrentIndex] = 0;
                memcpy(keypadMesg.message, keypadInput, 16);
                xQueueSend(keypadMessageQueue, (void *) &keypadMesg, pdMS_TO_TICKS(KEYPAD_QUEUE_SEND_WAIT_MS));
                continue;
            }
            if(key == '#'){
                keypadCurrentIndex = 0;
                keypadMesg.returnPress = 1;
                memcpy(keypadMesg.message, keypadInput, 16);
                while(!(xQueueSend(keypadMessageQueue, (void *) &keypadMesg, pdMS_TO_TICKS(KEYPAD_QUEUE_SEND_WAIT_MS))) == pdTRUE){}
                String("").toCharArray(keypadInput,16);
                continue;
            }
            keypadInput[keypadCurrentIndex] = key;
            keypadCurrentIndex++;
            memcpy(keypadMesg.message, keypadInput, 16);
            xQueueSend(keypadMessageQueue, (void *) &keypadMesg, pdMS_TO_TICKS(KEYPAD_QUEUE_SEND_WAIT_MS));
        }
        xSemaphoreGive(keypadMessageMutex);
    }
}

#endif