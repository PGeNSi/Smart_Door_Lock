#ifndef ADMINMODETRIGGERER_HEADER
#define  ADMINMODETRIGGERER_HEADER

#include <Arduino.h>
#include <sysConfig.h>
#include <SignalingVariables.h>

const int maxAMTTick = ((ADMIN_MODE_TRIGGER_LONG_PRESS_DURATION_MS > ADMIN_MODE_TRIGGERER_TICK_DELAY_MS) ? ADMIN_MODE_TRIGGER_LONG_PRESS_DURATION_MS:ADMIN_MODE_TRIGGERER_TICK_DELAY_MS)/ADMIN_MODE_TRIGGERER_TICK_DELAY_MS;
int currentAMTTick = maxAMTTick;
int buttonReleased = 1;

void adminModeTriggeringSystemTask( void * pvParameters ){
    for(;;){
        while(doorUnlockedButtonPressed){
            if(currentAMTTick <= 0) {
                if(!buttonReleased) continue;
                adminMode = !adminMode;
                buttonReleased = 0;
                continue;
            }
            currentAMTTick--;
            vTaskDelay(pdMS_TO_TICKS(ADMIN_MODE_TRIGGERER_TICK_DELAY_MS));
        }
        buttonReleased = 1;
        currentAMTTick = maxAMTTick;
        vTaskDelay(pdMS_TO_TICKS(ADMIN_MODE_TRIGGERER_LOOP_DELAY_MS));
    }
}

#endif