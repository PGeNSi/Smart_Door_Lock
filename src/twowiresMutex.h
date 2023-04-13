#ifndef TWOWIREMUTEX_HEADER
#define  TWOWIREMUTEX_HEADER

#include <Arduino.h>
#include <sysConfig.h>

SemaphoreHandle_t twoWireMutex;

void twoWireMutexInit(){
    twoWireMutex = xSemaphoreCreateMutex();
}

#endif