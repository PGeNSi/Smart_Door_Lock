#ifndef MODENORMAL_HEADER
#define MODENORMAL_HEADER

// PERSISTID 0xb0

#include <Arduino.h>
#include <modeTakeOverMutex.h>
#include <SignalingVariables.h>
#include <keypadHandler.h>
#include <rfidHandler.h>
#include <lcd.h>
#include <authenticationHandler.h>

const String lcdTopTextNormal = "-> Authenticate";

const int MaxWaitTick = ((MODE_NORMAL_SHOW_BEFORE_RESET_MS > MODE_NORMAL_TICK_DELAY_MS) ? MODE_NORMAL_SHOW_BEFORE_RESET_MS:MODE_NORMAL_TICK_DELAY_MS)/MODE_NORMAL_TICK_DELAY_MS;
int currentTick = 0;
bool lcdShow = 0;


void modeNormalTask( void * pvParameters ){
    struct lcdMessageObject textLCD;
    lcdTopTextNormal.toCharArray(textLCD.messagerow1, 16);
    textLCD.taskPersistID = 0xb0;
    textLCD.taskPersistState = false;
    textLCD.duration = MODE_NORMAL_LCD_NON_PERSIST_DEFAULT_DURATION_MS;
    for(;;){
        if(!adminMode){
            while(!(xSemaphoreTake( modeTakeOverMutex, pdMS_TO_TICKS(MODE_NORMAL_MODETAKEOVER_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
            keypadReadEnable = 1;
            rfidReadEnable = 1;
            String("").toCharArray(textLCD.messagerow2,16);
            while(!adminMode){
                String("").toCharArray(textLCD.messagerow2,16);
                struct keypadMessage kpMesgRecv;
                if( xQueueReceive( keypadMessageQueue, &( kpMesgRecv ), ( TickType_t ) MODE_NORMAL_KEYPAD_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ) {
                    textLCD.taskPersistState = true;
                    lcdShow = true;
                    currentTick = MaxWaitTick;
                    if(!kpMesgRecv.returnPress){
                        int currentTick = MaxWaitTick;
                        memcpy(textLCD.messagerow2, kpMesgRecv.message, 16);
                        xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK );
                        continue;
                    }
                    if(String(kpMesgRecv.message) == "") continue;
                    textLCD.taskPersistState = false;
                    lcdShow = false;
                    currentTick = 0;
                    if(authPasswd(kpMesgRecv.message)){
                        String("PIN - VALID").toCharArray(textLCD.messagerow2,16);
                        while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
                        xQueueSend(relayQueue, (void *) &relayTRUE, ( TickType_t ) MODE_NORMAL_QUEUE_RELAY_SEND_WAIT_TICK );
                        if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_SOUND_DURATION_MS));
                            buzzerOff();
                            xSemaphoreGive(buzzerMutex);
                        }
                    }
                    String("PIN - INVALID").toCharArray(textLCD.messagerow2,16);
                    while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
                    if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                        for(int i = 0; i < BUZZER_UNLOCK_FAILED_SOUND_REPEAT; i++){
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_FAILED_SOUND_DURATION_MS));
                            buzzerOff();
                        }
                        xSemaphoreGive(buzzerMutex);
                    }
                    continue;
                }
                struct rfidMessage rfidMesgRecv;
                if( xQueueReceive( rfidMessageQueue, &( rfidMesgRecv ), ( TickType_t ) MODE_NORMAL_RFID_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ) {
                    textLCD.taskPersistState = false;
                    lcdShow = false;
                    currentTick = 0;
                    resetKeypadQueue();
                    if(authRFID(rfidMesgRecv.rfidTag)){
                        String("RFID - VALID").toCharArray(textLCD.messagerow2,16);
                        while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
                        xQueueSend(relayQueue, (void *) &relayTRUE, ( TickType_t ) MODE_NORMAL_QUEUE_RELAY_SEND_WAIT_TICK );
                        if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_SOUND_DURATION_MS));
                            buzzerOff();
                            xSemaphoreGive(buzzerMutex);
                        }
                        continue;
                    }
                    String("RFID - INVALID").toCharArray(textLCD.messagerow2,16);
                    while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
                    if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                        for(int i = 0; i < BUZZER_UNLOCK_FAILED_SOUND_REPEAT; i++){
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_FAILED_SOUND_DURATION_MS));
                            buzzerOff();
                        }
                        xSemaphoreGive(buzzerMutex);
                    }
                    continue;
                }
                vTaskDelay(pdMS_TO_TICKS(1));
                if(currentTick > 0) currentTick--;
                else{
                    if(!lcdShow) continue;
                    resetKeypadQueue();
                    textLCD.taskPersistState = false;
                    xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK );
                    lcdShow = false;
                }
            }
            keypadReadEnable = 0;
            rfidReadEnable = 0;
            textLCD.taskPersistState = false;
            String(">Enter Admin..").toCharArray(textLCD.messagerow2,16);
            while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
            resetKeypadQueue();
            xQueueReset(rfidMessageQueue);
            xSemaphoreGive(modeTakeOverMutex);
        }
    }
}

#endif