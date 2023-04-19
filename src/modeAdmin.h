#ifndef MODEADMIN_HEADER
#define  MODEADMIN_HEADER

// PERSISTID 0xa0

#include <Arduino.h>
#include <modeTakeOverMutex.h>
#include <SignalingVariables.h>
#include <keypadHandler.h>
#include <rfidHandler.h>
#include <lcd.h>
#include <authenticationHandler.h>
#include <relay.h>

const String lcdTopTextAdmin = "-> Admin";

void modeAdminTask( void * pvParameters ){
    struct lcdMessageObject textLCD;
    // 0x00: Main Menu
    // 0x01: Add Password - Index
    // 0x02: Add Password - Password
    // 0x03: Remove Password - Index
    // 0x04: Add RFID - Index
    // 0x05: Add RFID - READ RFID
    // 0x06: Remove RFID - Index
    uint8_t currentSubMode = 0x00;
    lcdTopTextAdmin.toCharArray(textLCD.messagerow1, 16);
    textLCD.taskPersistID = 0xa0;
    textLCD.taskPersistState = false;
    textLCD.duration = MODE_ADMIN_LCD_NON_PERSIST_DEFAULT_DURATION_MS;
    bool lcdUpdateSignal = 0;
    for(;;){
        if(adminMode){
            while(!(xSemaphoreTake( modeTakeOverMutex, pdMS_TO_TICKS(MODE_ADMIN_MODETAKEOVER_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
            keypadReadEnable = 1;
            rfidReadEnable = 1;
            textLCD.taskPersistState = true;
            String("INPUT MODE NUM").toCharArray(textLCD.messagerow2,16);
            int selectedIndex = 0;
            lcdUpdateSignal = 1;
            while(adminMode){
                vTaskDelay(pdMS_TO_TICKS(MODE_ADMIN_INTERNAL_LOOP_DELAY_MS));
                if(lcdUpdateSignal){
                    String finalizedLCDTopText = "-> Admin 0x0";
                    finalizedLCDTopText.concat(String(currentSubMode,HEX));
                    finalizedLCDTopText.toCharArray(textLCD.messagerow1, 16);
                    xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_ADMIN_LCD_QUEUE_SEND_WAIT_TICK );
                    lcdUpdateSignal = 0;
                }
                struct keypadMessage kpMesgRecv;
                struct rfidMessage rfidMesgRecv;
                if( xQueueReceive( rfidMessageQueue, &( rfidMesgRecv ), ( TickType_t ) MODE_ADMIN_RFID_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ) {
                    if(currentSubMode == 0x05){
                        if(!addRFIDToSlot(selectedIndex,rfidMesgRecv.rfidTag)){
                            currentSubMode = 0x00;
                            String("RFID ADD FAILED").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(ADMIN_MODE_BUZZER_SUCCESS_ADD_DURATION_MS));
                            buzzerOff();
                            xSemaphoreGive(buzzerMutex);
                        }
                        currentSubMode = 0x00;
                        String("RFID ADDED").toCharArray(textLCD.messagerow2,16);
                        lcdUpdateSignal = 1;
                        continue;
                    }
                    if(authRFID(rfidMesgRecv.rfidTag)){
                        xQueueSend(relayQueue, (void *) &relayTRUE, ( TickType_t ) MODE_ADMIN_QUEUE_RELAY_SEND_WAIT_TICK );
                        if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_SOUND_DURATION_MS));
                            buzzerOff();
                            xSemaphoreGive(buzzerMutex);
                        }
                        continue;
                    }
                    if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                        for(int i = 0; i < BUZZER_UNLOCK_FAILED_SOUND_REPEAT; i++){
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_FAILED_SOUND_DURATION_MS));
                            buzzerOff();
                            vTaskDelay(pdMS_TO_TICKS(BUZZER_UNLOCK_FAILED_SOUND_DURATION_MS));
                        }
                        xSemaphoreGive(buzzerMutex);
                    }
                    continue;
                }
                if( xQueueReceive( keypadMessageQueue, &( kpMesgRecv ), ( TickType_t ) MODE_ADMIN_KEYPAD_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ) {
                    if(!kpMesgRecv.returnPress){
                        if(currentSubMode == 0x05){
                            xQueueSend(keypadClearQueue, (void *) &keypadClearTRUE, ( TickType_t ) MODE_ADMIN_QUEUE_KEYPAD_CLEAR_SEND_WAIT_TICK );
                            continue;
                        }
                        memcpy(textLCD.messagerow2, kpMesgRecv.message, 16);
                        lcdUpdateSignal = 1;
                        continue;
                    }
                    if((String(kpMesgRecv.message) == "") && (currentSubMode != 0x00)){
                        currentSubMode = 0x00;
                        String("").toCharArray(textLCD.messagerow2,16);
                        lcdUpdateSignal = 1;
                        continue;
                    }
                    if(currentSubMode == 0x00){
                        // Add Password/PIN
                        if(String(kpMesgRecv.message) == "A1"){
                            currentSubMode = 0x01;
                            String("INPUT INDEX").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Add RFID
                        if(String(kpMesgRecv.message) == "A2"){
                            currentSubMode = 0x04;
                            String("INPUT INDEX").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Delete Password/PIN
                        if(String(kpMesgRecv.message) == "D1"){
                            currentSubMode = 0x03;
                            String("INPUT INDEX").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Delete RFID
                        if(String(kpMesgRecv.message) == "D2"){
                            currentSubMode = 0x06;
                            String("INPUT INDEX").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        String("INVALID INPUT").toCharArray(textLCD.messagerow2,16);
                        lcdUpdateSignal = 1;
                        continue;
                    }
                    if(currentSubMode == 0x02){
                        if(!addPasswdToSlot(selectedIndex,kpMesgRecv.message)){
                            currentSubMode = 0x00;
                            String("PIN ADD FAILED").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        if(xSemaphoreTake( buzzerMutex, pdMS_TO_TICKS(KEYPAD_BUZZER_MUTEX_WAIT_MS) ) == pdTRUE) {
                            buzzerOn();
                            vTaskDelay(pdMS_TO_TICKS(ADMIN_MODE_BUZZER_SUCCESS_ADD_DURATION_MS));
                            buzzerOff();
                            xSemaphoreGive(buzzerMutex);
                        }
                        currentSubMode = 0x00;
                        String("PIN ADDED").toCharArray(textLCD.messagerow2,16);
                        lcdUpdateSignal = 1;
                        continue;
                    }
                    if(currentSubMode == 0x01 || currentSubMode == 0x04 || currentSubMode == 0x03 || currentSubMode == 0x06){
                        String kpInput = kpMesgRecv.message;
                        int kpInputIndex = kpInput.toInt();
                        if(kpInputIndex < 1 || kpInputIndex > 10){
                            currentSubMode = 0x00;
                            String("INVALID INDEX").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        selectedIndex = kpInputIndex - 1;
                        // Add Password/PIN
                        if(currentSubMode == 0x01){
                            if(isPasswdSlotAssigned(selectedIndex)){
                                currentSubMode = 0x00;
                                String("INDEX OCCUPIED").toCharArray(textLCD.messagerow2,16);
                                lcdUpdateSignal = 1;
                                continue;
                            }
                            currentSubMode = 0x02;
                            String("INPUT PIN").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Add RFID
                        if(currentSubMode == 0x04){
                            if(isRFIDSlotAssigned(selectedIndex)){
                                currentSubMode = 0x00;
                                String("INDEX OCCUPIED").toCharArray(textLCD.messagerow2,16);
                                lcdUpdateSignal = 1;
                                continue;
                            }
                            currentSubMode = 0x05;
                            xQueueReset(rfidMessageQueue);
                            String("SCAN RFID").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Delete Password/PIN
                        if(currentSubMode == 0x03){
                            currentSubMode = 0x00;
                            clearPasswdSlot(selectedIndex);
                            String("PIN CLEARED").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                        // Delete RFID
                        if(currentSubMode == 0x06){
                            currentSubMode = 0x00;
                            clearRFIDSlot(selectedIndex);
                            String("RFID CLEARED").toCharArray(textLCD.messagerow2,16);
                            lcdUpdateSignal = 1;
                            continue;
                        }
                    }
                }
            }
            keypadReadEnable = 0;
            rfidReadEnable = 0;
            textLCD.taskPersistState = false;
            lcdTopTextAdmin.toCharArray(textLCD.messagerow1, 16);
            String(">Exit Admin..").toCharArray(textLCD.messagerow2,16);
            while(!xQueueSend(lcdQueue,( void * ) &textLCD,( TickType_t ) MODE_ADMIN_LCD_QUEUE_SEND_WAIT_TICK ) == pdTRUE){}
            currentSubMode = 0x00;
            xQueueSend(keypadClearQueue, (void *) &keypadClearTRUE, ( TickType_t ) MODE_ADMIN_QUEUE_KEYPAD_CLEAR_SEND_WAIT_TICK );
            xQueueReset(rfidMessageQueue);
            xSemaphoreGive(modeTakeOverMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(MODE_ADMIN_WAIT_FOR_MODE_UPDATE_MS));
    }
}

#endif