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

MFRC522 rfid(RFID_SS_SDA_PIN, RFID_RST_PIN); 
MFRC522::MIFARE_Key key;

void rfidMessageQueueInit(){
    rfidMessageQueue = xQueueCreate(2, sizeof( struct rfidMessage ));
}

void rfidTask( void * pvParameters ){
    rfidMessage rfMesg;
    rfid.PCD_Init();
    for(;;){
        if(!rfidReadEnable) continue;
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
    }
}

#endif