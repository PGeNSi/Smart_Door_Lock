#ifndef RFIDHANDLER_HEADER
#define  RFIDHANDLER_HEADER

#include <Arduino.h>
#include <MFRC522.h>
#include <sysConfig.h>
#include <SignalingVariables.h>

struct rfidMessage {
    char rfidTag[12];
};

QueueHandle_t rfidMessageQueue;

void rfidMessageQueueInit(){
    Serial.println("--> Initializing RFID Message Queue Object");
    rfidMessageQueue = xQueueCreate(RFID_MESSAGE_QUEUE_LENGTH, sizeof( struct rfidMessage ));
    if(rfidMessageQueue == NULL){
        Serial.println("ERR--> Failed to Initialize RFID Message Queue Object");
        ESP.restart();
    }
    Serial.println("--> RFID Message Queue Object Initialized");
}

void rfidTask( void * pvParameters ){
    MFRC522 rfid(RFID_SS_SDA_PIN, RFID_RST_PIN); 
    MFRC522::MIFARE_Key key;
    rfidMessage rfMesg;
    for(;;){
        if(!rfidReadEnable) {
            vTaskDelay(pdMS_TO_TICKS(RFID_WAIT_FOR_ENABLE_MS));
            continue;
        }
        rfid.PCD_Init();
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
            MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
            if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||
                piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
                piccType == MFRC522::PICC_TYPE_MIFARE_4K)
            {
                String strID = ""; 
                for (byte i = 0; i < 4; i++) {
                    strID.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
                    strID.concat(String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : ""));
                }
                strID.toUpperCase();
                rfid.PICC_HaltA();
                rfid.PCD_StopCrypto1();
                strID.toCharArray(rfMesg.rfidTag,11);
                xQueueSend(rfidMessageQueue, (void *) &rfMesg, pdMS_TO_TICKS(RFID_QUEUE_SEND_WAIT_MS));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(RFID_LOOP_DELAY_MS));
    }
}

#endif