#ifndef LEDTASKHANDLER_HEADER
#define  LEDTASKHANDLER_HEADER

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <sysConfig.h>
#include <twowiresMutex.h>

struct lcdMessageObject {
    char messagerow1[17] = "";
    char messagerow2[17] = "";
    uint8_t taskPersistID = 0x00;
    bool taskPersistState = 0;
    int duration = LCD_MESSAGE_OBJECT_DEFAULT_DURATION_MS;
};

QueueHandle_t lcdQueue;

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);

void lcdQueueInit(){
    lcdQueue = xQueueCreate(LCD_MESSAGE_QUEUE_LENGTH, sizeof( struct lcdMessageObject ));
}

void lcdTask( void * pvParameters ) {
    while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(LCD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
    lcd.begin();
    lcd.backlight(); 
    lcd.clear();
    xSemaphoreGive(twoWireMutex);
    struct lcdMessageObject receivedMessage;
    bool isSetPersist = 0;
    uint8_t persistID = 0x0;

    for( ;; ) {
        if( xQueueReceive( lcdQueue, &( receivedMessage ), ( TickType_t ) LCD_QUEUE_RECEIVE_WAIT_TICK ) == pdPASS ) {
            if(isSetPersist && !(receivedMessage.taskPersistID == persistID)) continue;
            String row1 = receivedMessage.messagerow1;
            if(row1.length() > 16) row1.remove(16, row1.length()-16);
            String row2 = receivedMessage.messagerow2;
            if(row2.length() > 16) row2.remove(16, row2.length()-16);
            while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(LCD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(row1);
            lcd.setCursor(0,1);
            lcd.print(row2);
            xSemaphoreGive(twoWireMutex);
            if(isSetPersist){
                if(!(receivedMessage.taskPersistState)) {
                    isSetPersist = false;
                    if(receivedMessage.duration){
                        int delay = receivedMessage.duration;
                        if(delay <= 0) delay = LCD_DEFAULT_VALUE_ON_INVALID_DURATION_MS;
                        vTaskDelay(pdMS_TO_TICKS(delay));
                        continue;
                    }
                }
                continue;
            }
            if(receivedMessage.taskPersistState) {
                isSetPersist = true;
                persistID = receivedMessage.taskPersistID;
            }
            int delay = receivedMessage.duration;
            if(delay <= 0) delay = LCD_DEFAULT_VALUE_ON_INVALID_DURATION_MS;
            vTaskDelay(pdMS_TO_TICKS(delay));
            continue;
        }
        if(!isSetPersist){
            while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(LCD_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("XXXXXXXXXXXXXXXX");
            lcd.setCursor(0,1);
            lcd.print("XXXXXXXXXXXXXXXX");
            xSemaphoreGive(twoWireMutex);
            vTaskDelay(pdMS_TO_TICKS(LCD_DEFAULT_MODE_TASK_DELAY_MS));
        }
    }
}

#endif