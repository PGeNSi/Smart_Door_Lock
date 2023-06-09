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
QueueHandle_t keypadClearQueue;

const bool keypadClearTRUE = 1;

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

void keypadMessageQueueAndKeypadClearQueueInit(){
    Serial.println("--> Initializing Keypad Message Queue and Keypad Clear Queue Object");
    keypadMessageQueue = xQueueCreate(KEYPAD_MESSAGE_QUEUE_LENGTH, sizeof( struct keypadMessage ));
    keypadClearQueue = xQueueCreate(KEYPAD_CLEAR_QUEUE_LENGTH, sizeof(bool));
    if(keypadMessageQueue == NULL){
        Serial.println("ERR--> Failed to Initialize Keypad Message Queue Object");
        ESP.restart();  
    }
    if(keypadClearQueue == NULL){
        Serial.println("ERR--> Failed to Initialize Keypad Clear Queue Object");
        ESP.restart();  
    }
    Serial.println("--> Keypad Message Queue and Keypad Clear Queue Object Initialized");
}

void keypadTask( void * pvParameters ){
    while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(KEYPAD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
    keypad.begin( makeKeymap(keys) );
    xSemaphoreGive(twoWireMutex);
    bool queueResult;
    char keypadInput[16] = "";
    int keypadCurrentIndex = 0;
    for (;;){
        if( xQueueReceive( keypadClearQueue, &( queueResult ), ( TickType_t ) KEYPAD_CLEAR_QUEUE_WAIT_TICK ) == pdPASS ){
            xQueueReset(keypadMessageQueue);
            for(int kpindx = 0; kpindx < 16; kpindx++){
                keypadInput[kpindx] = 0;
            }
            keypadCurrentIndex = 0;
        }
        if(!keypadReadEnable) {
            vTaskDelay(pdMS_TO_TICKS(KEYPAD_WAIT_FOR_ENABLE_MS));
            continue;
        }
        while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(KEYPAD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
        char key = keypad.getKey(); 
        xSemaphoreGive(twoWireMutex);
        if (key) {
            if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                buzzerOn();
                vTaskDelay(pdMS_TO_TICKS(BUZZER_KEYPAD_PRESS_DURATION_MS));
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
                for(int kpindx = 0; kpindx < 16; kpindx++){
                    keypadInput[kpindx] = 0;
                }
                continue;
            }
            keypadInput[keypadCurrentIndex] = key;
            keypadCurrentIndex++;
            memcpy(keypadMesg.message, keypadInput, 16);
            xQueueSend(keypadMessageQueue, (void *) &keypadMesg, pdMS_TO_TICKS(KEYPAD_QUEUE_SEND_WAIT_MS));
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(KEYPAD_LOOP_DELAY_MS));
    }
}

#endif