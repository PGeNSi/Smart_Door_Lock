#ifndef BUTTONINTERRUPT_HEADER
#define  BUTTONINTERRUPT_HEADER

#include <Arduino.h>
#include <relay.h>
#include <sysConfig.h>
#include <SignalingVariables.h>

void buttonUnlockedISR(){
    if(digitalRead(INTERNAL_DOOR_UNLOCK_BUTTON_PIN) == LOW){
        xQueueSendFromISR(relayQueue, (void *) &relayTRUE,NULL);
        doorUnlockedButtonPressed = 1;
        return;
    }
    doorUnlockedButtonPressed = 0;
}

#endif