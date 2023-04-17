#ifndef FILESYSTEMHANDLER_HEADER
#define  FILESYSTEMHANDLER_HEADER

#include <authenticationHandler.h>
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <sysConfig.h>

void fileSystemInit(){
    Serial.println("--> Initializing and Mounting File System");
    if (SPIFFS.begin(false) || SPIFFS.begin(true)){
        Serial.println("----> File System Mounted and Initialized");
        if (SPIFFS.exists(AUTH_JSON_FILE_PATH)){
            Serial.println("----> Loading Authentication File");
            loadAuthDB();
            Serial.println("----> Authentication File Loaded");
        } else {
            Serial.println("----> Authentication File Not Found, Creating New One.");
            bool result = saveAuthDB();
            if(!result){
                Serial.println("ERR----> Failed to Create New Authentication File, Falling Back To Temporary Object");
            }
            Serial.println("----> New Authentication File Created");
        }
        return;
    }
    Serial.println("ERR--> Failed to Initialize and Mount File System, Falling Back To Temporary Object");
}

#endif