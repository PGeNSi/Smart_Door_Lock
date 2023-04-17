#include <Arduino.h>
#include <wifiManagerHandler.h>
#include <wire.h>
#include <lcd.h>
#include <relay.h>
#include <buzzer.h>
#include <modeTakeOverMutex.h>
#include <keypadHandler.h>
#include <rfidHandler.h>
#include <twowiresMutex.h>
#include <rtc.h>
#include <sysConfig.h>
#include <modeAdmin.h>
#include <modeNormal.h>
#include <fileSystemHandler.h>
#include <buttonInterrupt.h>
#include <adminModeTriggeringSystem.h>

TaskHandle_t taskHandle[8];

void setup(){
    Serial.println("> Initializing All System...");
    pinMode(WIFI_RESET_BUTTON_PIN, INPUT_PULLUP);
    pinMode(INTERNAL_DOOR_UNLOCK_BUTTON_PIN, INPUT_PULLUP);
    Serial.begin(115200);
    Wire.begin();
    SPI.begin(); 
    fileSystemInit();
    wifiInit();
    rtcInit();
    buzzerInit();
    modeTakeOverMutexInit();
    twoWireMutexInit();
    lcdQueueInit();
    keypadMessageQueueMutexInit();
    rfidMessageQueueInit();
    relayInit();
    Serial.println("> All System Initialized");

    // Initialize All Task
    Serial.println("> Creating Tasks");
    xTaskCreate(rtcAutoCalibrationTask, "rtcTask", TASK_RTC_STACK_DEPTH, NULL, TASK_RTC_PRIORITY, &taskHandle[0]);
    xTaskCreate(lcdTask, "lcdTask", TASK_LCD_STACK_DEPTH, NULL, TASK_LCD_PRIORITY, &taskHandle[1]);
    xTaskCreate(keypadTask, "keypadTask", TASK_KEYPAD_STACK_DEPTH, NULL, TASK_KEYPAD_PRIORITY, &taskHandle[2]);
    xTaskCreate(rfidTask, "rfidTask", TASK_RFID_STACK_DEPTH, NULL, TASK_RFID_PRIORITY, &taskHandle[3]);
    xTaskCreate(modeNormalTask, "modeNormalTask", TASK_MODENORMAL_STACK_DEPTH, NULL, TASK_MODENORMAL_PRIORITY, &taskHandle[4]);
    xTaskCreate(modeAdminTask, "modeAdminTask", TASK_MODEADMIN_STACK_DEPTH, NULL, TASK_MODEADMIN_PRIORITY, &taskHandle[5]);
    xTaskCreate(relayTask, "relayTask", TASK_RELAY_STACK_DEPTH, NULL, TASK_RELAY_PRIORITY, &taskHandle[6]);
    xTaskCreate(adminModeTriggeringSystemTask, "AMTSTask", TASK_ADMINMODETRIGGERER_STACK_DEPTH, NULL, TASK_ADMINMODETRIGGERER_PRIORITY, &taskHandle[7]);
    Serial.println("> Tasks Created");

    // Initialize Interrupt
    attachInterrupt(digitalPinToInterrupt(INTERNAL_DOOR_UNLOCK_BUTTON_PIN),buttonUnlockedISR,CHANGE);
}

void loop(){}