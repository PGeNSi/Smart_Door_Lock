#ifndef AUTHENTICATIONHANDLER_HEADER
#define  AUTHENTICATIONHANDLER_HEADER

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <sysConfig.h>
#include <ArduinoJson.h>

struct authList_t{
    char passwd[10][17] = {""};
    char rfid[10][12] = {""};
} authList;

void loadAuthDB() {
    File authDB = SPIFFS.open(AUTH_JSON_FILE_PATH, "r");
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, authDB);
    authDB.close();
    if (error) return;
    JsonArray passwd = doc["passwd"];
    JsonArray rfid = doc["rfid"];
    for(int i = 0; i < 10; i++){
        const char* passwdData = passwd[i];
        const char* rfidData = rfid[i];
        memcpy(authList.passwd[i],passwdData,16);
        memcpy(authList.rfid[i],rfidData,11);
    }
}

bool saveAuthDB() {
    StaticJsonDocument<768> doc;
    JsonArray passwd = doc.createNestedArray("passwd");
    JsonArray rfid = doc.createNestedArray("rfid");
    for(int i = 0; i < 10; i++){
        passwd.add(authList.passwd[i]);
        rfid.add(authList.rfid[i]);
    }
    File authDB = SPIFFS.open(AUTH_JSON_FILE_PATH, "w");
    if (!authDB) return 0;
    if (serializeJson(doc, authDB) == 0){
        authDB.close();
        return 0;
    }
    authDB.close();
    return 1;
}

bool authPasswd(char *inputPasswd){
    String inPass = inputPasswd;
    for(int i = 0; i < 10; i++){
        if(inPass == String(authList.passwd[i])) return 1;
    }
    return 0;
}

bool authRFID(char *inputUID){
    String inUID = inputUID;
    for(int i = 0; i < 10; i++){
        if(inUID == String(authList.rfid[i])) return 1;
    }
    return 0;
}

bool isPasswdSlotAssigned(int index){
    if(index < 0 || index > 9) return 1;
    String passwordSlotValue = authList.passwd[index];
    if(passwordSlotValue == "") return 0;
    return 1;
}

bool isRFIDSlotAssigned(int index){
    if(index < 0 || index > 9) return 1;
    String rfidSlotValue = authList.rfid[index];
    if(rfidSlotValue == "") return 0;
    return 1;
}

void clearPasswdSlot(int index){
    if(index < 0 || index > 9) return;
    String("").toCharArray(authList.passwd[index],16);
    saveAuthDB();
}

void clearRFIDSlot(int index){
    if(index < 0 || index > 9) return;
    String("").toCharArray(authList.rfid[index],16);
    saveAuthDB();
}

bool addPasswdToSlot(int index, char *inputPasswd){
    if(index < 0 || index > 9) return 0;
    if(isPasswdSlotAssigned(index)) return 0;
    memcpy(authList.passwd[index],inputPasswd,16);
    saveAuthDB();
    return 1;
}

bool addRFIDToSlot(int index, char *inputUID){
    if(index < 0 || index > 9) return 0;
    if(isRFIDSlotAssigned(index)) return 0;
    memcpy(authList.rfid[index],inputUID,11);
    saveAuthDB();
    return 1;
}


#endif