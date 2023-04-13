#ifndef BUZZERHANDLER_HEADER
#define  BUZZERHANDLER_HEADER

#include <Arduino.h>
#include <sysConfig.h>

SemaphoreHandle_t buzzerMutex;

void buzzerInit(){
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerMutex = xSemaphoreCreateMutex();
}

void buzzerOn(){
    digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff(){
    digitalWrite(BUZZER_PIN, LOW);
}

#endif