#ifndef BUZZERHANDLER_HEADER
#define  BUZZERHANDLER_HEADER

#include <Arduino.h>
#include <sysConfig.h>

SemaphoreHandle_t buzzerMutex;

void buzzerInit(){
    Serial.println("--> Initializing Buzzer pin and Mutex Object");
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerMutex = xSemaphoreCreateMutex();
    if(buzzerMutex == NULL){
        Serial.println("ERR--> Failed to Initialize Buzzer Mutex Object");
        ESP.restart();
    }
    Serial.println("--> Buzzer pin and Mutex Object Initialized");
}

void buzzerOn(){
    digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff(){
    digitalWrite(BUZZER_PIN, LOW);
}

#endif