#ifndef WIFICONFIGURATION_HEADER
#define  WIFICONFIGURATION_HEADER

#include <WiFiManager.h> 
#include <sysConfig.h>

void wifiInit(){
    WiFiManager wm;
    if (digitalRead(WIFI_RESET_BUTTON_PIN) == LOW) {
        Serial.println("Reset WiFi");
        wm.resetSettings();
    }
    bool res = wm.autoConnect(WIFI_MANAGER_SSID, WIFI_MANAGER_PASSWD);
    if (!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    }
    else {
        Serial.println("WiFi Connected");
    }
}

#endif