#ifndef RTCHANDLER_HEADER
#define  RTCHANDLER_HEADER

#include <RTClib.h>
#include <time.h>
#include <twowiresMutex.h>

const char ntp_server1[20] = "pool.ntp.org";
const char ntp_server2[20] = "time.nist.gov";
const char ntp_server3[20] = "time.uni.net.th";

RTC_DS3231 rtc;
struct tm timeinfo;

void rtcInit(){
    Serial.println("--> Initializing RTC");
    configTime(RTC_GMT_UTC_OFFSET_HOUR*3600, RTC_DST_OFFSET_SECOND, ntp_server1, ntp_server2, ntp_server3);
    while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(RTC_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
    rtc.begin();
    while (!getLocalTime(&timeinfo)) {
        vTaskDelay(pdMS_TO_TICKS(1000*RTC_WAIT_FOR_LOCAL_TIME_LOOP_SEC));
    }
    if (rtc.lostPower()) rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    xSemaphoreGive(twoWireMutex);
    Serial.println("--> RTC Initialized");
}

void rtcAutoCalibrationTask( void * pvParameters ){
    TickType_t xLastWakeTime;
    const TickType_t xWaitDuration = pdMS_TO_TICKS(1000*60*60*((RTC_CALIBRATION_PERIOD_HOUR > 0) ? RTC_CALIBRATION_PERIOD_HOUR : 24));
    for( ;; ){
        if (!getLocalTime(&timeinfo)) {
            vTaskDelay(pdMS_TO_TICKS(1000*RTC_WAIT_FOR_LOCAL_TIME_LOOP_SEC));
            continue;
        }
        while(!(xSemaphoreTake( twoWireMutex, pdMS_TO_TICKS(RTC_TWOWIRES_MUTEX_WAIT_LOOP_MS) ) == pdTRUE)){}
        rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
        xSemaphoreGive(twoWireMutex);
        xTaskDelayUntil(&xLastWakeTime,xWaitDuration);
    }
}

#endif