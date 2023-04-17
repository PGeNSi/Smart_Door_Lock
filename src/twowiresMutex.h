#ifndef TWOWIREMUTEX_HEADER
#define  TWOWIREMUTEX_HEADER

#include <Arduino.h>
#include <sysConfig.h>

SemaphoreHandle_t twoWireMutex;

void twoWireMutexInit(){
    Serial.println("--> Initializing Two Wire Mutex Object");
    twoWireMutex = xSemaphoreCreateMutex();
    if(twoWireMutex == NULL){
        Serial.println("ERR--> Failed to Initialize Two Wire Mutex Object");
        ESP.restart();
    }
    Serial.println("--> Two Wire Mutex Object Initialized");
}

#endif