#ifndef SYSCONFIG_HEADER
#define SYSCONFIG_HEADER

#define LCD_I2C_ADDR 0x27
#define KEYPAD_I2C_ADDR 0x38

#define WIFI_MANAGER_SSID "smart_doorlock"
#define WIFI_MANAGER_PASSWD "1234567890"

#define INTERNAL_DOOR_UNLOCK_BUTTON_PIN 33
#define WIFI_RESET_BUTTON_PIN 27
#define BUZZER_PIN 2
#define RELAY_PIN 32
#define RFID_RST_PIN  4
#define RFID_SS_SDA_PIN  5

#define MODE_NORMAL_SHOW_BEFORE_RESET_MS 4500
#define RELAY_ON_MS 5000
#define RTC_CALIBRATION_PERIOD_HOUR 24
#define RTC_GMT_UTC_OFFSET_HOUR 7
#define RTC_DST_OFFSET_SECOND 0

#define BUZZER_KEYPAD_PRESS_DURATION_MS 100
#define BUZZER_UNLOCK_SOUND_DURATION_MS 500
#define BUZZER_UNLOCK_FAILED_SOUND_DURATION_MS 200
#define BUZZER_UNLOCK_FAILED_SOUND_REPEAT 3
#define ADMIN_MODE_TRIGGER_LONG_PRESS_DURATION_MS 5000

// ADVANCED CONFIGURABLE VARIABLE

#define AUTH_JSON_FILE_PATH "/authDB.json"
#define ADMIN_MODE_TRIGGERER_BUTTON_RELEASE_LOOP_DELAY_MS 50
#define ADMIN_MODE_TRIGGERER_TICK_DELAY_MS 100
#define ADMIN_MODE_TRIGGERER_LOOP_DELAY_MS 50
#define KEYPAD_MESSAGE_QUEUE_LENGTH 5
#define KEYPAD_CLEAR_QUEUE_LENGTH 2
#define KEYPAD_CLEAR_QUEUE_WAIT_TICK 1
#define KEYPAD_MESSAGE_MUTEX_RESET_QUEUE_FUNCTION_WAIT_TICK 0
#define KEYPAD_MESSAGE_MUTEX_WAIT_LOOP_MS 1
#define KEYPAD_TWOWIRES_MUTEX_WAIT_LOOP_MS 1
#define KEYPAD_BUZZER_MUTEX_WAIT_MS 1
#define KEYPAD_QUEUE_SEND_WAIT_MS 10
#define KEYPAD_WAIT_FOR_ENABLE_MS 10
#define KEYPAD_LOOP_DELAY_MS 10
#define LCD_MESSAGE_QUEUE_LENGTH 10
#define LCD_TWOWIRES_MUTEX_WAIT_LOOP_MS 1
#define LCD_QUEUE_RECEIVE_WAIT_TICK 1
#define LCD_DEFAULT_MODE_TASK_DELAY_MS 1500
#define LCD_DEFAULT_VALUE_ON_INVALID_DURATION_MS 50
#define LCD_MESSAGE_OBJECT_DEFAULT_DURATION_MS 500
#define MODE_ADMIN_QUEUE_RELAY_SEND_WAIT_TICK 1
#define MODE_ADMIN_QUEUE_KEYPAD_CLEAR_SEND_WAIT_TICK 1
#define MODE_ADMIN_LCD_NON_PERSIST_DEFAULT_DURATION_MS 500
#define MODE_ADMIN_MODETAKEOVER_MUTEX_WAIT_LOOP_MS 1
#define MODE_ADMIN_RFID_QUEUE_RECEIVE_WAIT_TICK 1
#define MODE_ADMIN_KEYPAD_QUEUE_RECEIVE_WAIT_TICK 1
#define MODE_ADMIN_LCD_QUEUE_SEND_WAIT_TICK 1
#define MODE_ADMIN_WAIT_FOR_MODE_UPDATE_MS 10
#define MODE_ADMIN_INTERNAL_LOOP_DELAY_MS 2
#define MODE_NORMAL_LCD_NON_PERSIST_DEFAULT_DURATION_MS 500
#define MODE_NORMAL_MODETAKEOVER_MUTEX_WAIT_LOOP_MS 1
#define MODE_NORMAL_RFID_QUEUE_RECEIVE_WAIT_TICK 1
#define MODE_NORMAL_RFID_BUZZER_MUTEX_WAIT_MS 5
#define MODE_NORMAL_KEYPAD_QUEUE_RECEIVE_WAIT_TICK 1
#define MODE_NORMAL_LCD_QUEUE_SEND_WAIT_TICK 1
#define MODE_NORMAL_TICK_DELAY_MS 1
#define MODE_NORMAL_QUEUE_RELAY_SEND_WAIT_TICK 1
#define MODE_NORMAL_QUEUE_KEYPAD_CLEAR_SEND_WAIT_TICK 1
#define MODE_NORMAL_WAIT_FOR_MODE_UPDATE_MS 10
#define MODE_NORMAL_INTERNAL_LOOP_DELAY_MS 2
#define RELAY_QUEUE_LENGTH 10
#define RELAY_TICK_DELAY_MS 50
#define RELAY_WAIT_FOR_QUEUE_DELAY_MS 10
#define RELAY_QUEUE_RECEIVE_WAIT_TICK 0
#define RFID_MESSAGE_QUEUE_LENGTH 2
#define RFID_QUEUE_SEND_WAIT_MS 10
#define RFID_WAIT_FOR_ENABLE_MS 10
#define RFID_LOOP_DELAY_MS 10
#define RTC_TWOWIRES_MUTEX_WAIT_LOOP_MS 1
#define RTC_WAIT_FOR_LOCAL_TIME_LOOP_SEC 2

// TASK CONFIGURABLE VARIABLE
#define TASK_RTC_STACK_DEPTH 2500
#define TASK_RTC_PRIORITY 1
#define TASK_LCD_STACK_DEPTH 2500
#define TASK_LCD_PRIORITY 2
#define TASK_KEYPAD_STACK_DEPTH 2500
#define TASK_KEYPAD_PRIORITY 3
#define TASK_RFID_STACK_DEPTH 10000
#define TASK_RFID_PRIORITY 3
#define TASK_MODENORMAL_STACK_DEPTH 1000
#define TASK_MODENORMAL_PRIORITY 4
#define TASK_MODEADMIN_STACK_DEPTH 1000
#define TASK_MODEADMIN_PRIORITY 4
#define TASK_RELAY_STACK_DEPTH 1000
#define TASK_RELAY_PRIORITY 3
#define TASK_ADMINMODETRIGGERER_STACK_DEPTH 1000
#define TASK_ADMINMODETRIGGERER_PRIORITY 3

#endif