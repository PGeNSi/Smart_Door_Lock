#ifndef SIGNALINGVARIABLES_HEADER
#define  SIGNALINGVARIABLES_HEADER

#include <Arduino.h>

volatile bool adminMode = 0;
volatile bool keypadReadEnable = 1;
volatile bool rfidReadEnable = 1;
volatile bool doorUnlockedButtonPressed = 0;

#endif