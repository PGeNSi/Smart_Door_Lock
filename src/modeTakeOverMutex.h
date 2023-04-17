#ifndef MODETAKEOVERMUTEX_HEADER
#define  MODETAKEOVERMUTEX_HEADER

#include <Arduino.h>

SemaphoreHandle_t modeTakeOverMutex;

void modeTakeOverMutexInit(){
    Serial.println("--> Initializing Mode Take Over Mutex Object");
    modeTakeOverMutex = xSemaphoreCreateMutex();
    if(modeTakeOverMutex == NULL){
        Serial.println("ERR--> Failed to Initialize Mode Take Over Mutex Object");
        ESP.restart();
    }
    Serial.println("--> Mode Take Over Mutex Object Initialiated");
}

#endif