#ifndef RELAYHANDLER_HEADER
#define  RELAYHANDLER_HEADER

#include <Arduino.h>
#include <sysConfig.h>

QueueHandle_t relayQueue;

const int maxRelayTick = ((RELAY_ON_MS > RELAY_TICK_DELAY_MS) ? RELAY_ON_MS : RELAY_TICK_DELAY_MS)/RELAY_TICK_DELAY_MS;
const bool relayTRUE = 1;

int currentRelayTick = 0;
bool relayIsOn = false;

void relayInit(){
    pinMode(RELAY_PIN, OUTPUT);
    relayQueue = xQueueCreate(10, sizeof(bool));
}

void relayTask( void * pvParameters ){
    bool queueResult;
    for(;;){
        if( xQueueReceive( relayQueue, &( queueResult ), ( TickType_t ) RELAY_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ){
            currentRelayTick = maxRelayTick;
        }
        if(currentRelayTick<=0){
            if(!relayIsOn) continue;
            digitalWrite(RELAY_PIN,0);
            relayIsOn = false;
            continue;
        }
        if(!relayIsOn){
            digitalWrite(RELAY_PIN,1);
            relayIsOn = true;
        }
        currentRelayTick --;
        vTaskDelay(pdMS_TO_TICKS(RELAY_TICK_DELAY_MS));
    }
}

#endif