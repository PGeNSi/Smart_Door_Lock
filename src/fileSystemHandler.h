#ifndef FILESYSTEMHANDLER_HEADER
#define  FILESYSTEMHANDLER_HEADER

#include <authenticationHandler.h>
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <sysConfig.h>

void fileSystemInit(){
    if (SPIFFS.begin(false) || SPIFFS.begin(true)){
        if (SPIFFS.exists(AUTH_JSON_FILE_PATH)){
            loadAuthDB();
        } else {
            bool result = saveAuthDB();
        }
    }
}

#endif