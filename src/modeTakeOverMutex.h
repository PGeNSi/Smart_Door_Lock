#ifndef MODETAKEOVERMUTEX_HEADER
#define  MODETAKEOVERMUTEX_HEADER

#include <Arduino.h>

SemaphoreHandle_t modeTakeOverMutex;

void modeTakeOverMutexInit(){
    modeTakeOverMutex = xSemaphoreCreateMutex();
}

#endif