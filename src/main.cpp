#include <Arduino.h>

#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h> 
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#include <RTClib.h>
#include <time.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define RFID_ADDR 0x38  //set addr rfid

#define Reset_wifi 27
#define Relay 32
#define Button 33
#define Buzzer 2

#define RST_PIN  4  //set RST pin rfid
#define SS_PIN  5   //set SS(SDA) pin rfid

bool status_relay = LOW;

MFRC522 rfid(SS_PIN, RST_PIN);  // ตั้งค่าฟังก์ชัน
MFRC522::MIFARE_Key key;        // ตั้งค่ารูปแบบ Tag
RTC_DS3231 rtc;

char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

const long gmtOffset_sec = 7 * 3600; // Thailand's timezone offset in seconds (GMT+7)
const int   daylightOffset_sec = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String datestr, timestr;

const byte ROWS = 4;  // กำหนดจำนวนของ Rows
const byte COLS = 4;  // กำหนดจำนวนของ Columns
// กำหนด Key ที่ใช้งาน (4x4)
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// กำหนด Pin ที่ใช้งาน (4x4)
byte rowPins[ROWS] = {0, 1, 2, 3}; // เชื่อมต่อกับ Pin แถวของปุ่มกด
byte colPins[COLS] = {4, 5, 6, 7}; // เชื่อมต่อกับ Pin คอลัมน์ของปุ่มกด

// makeKeymap(keys) : กำหนด Keymap
// rowPins : กำหนด Pin แถวของปุ่มกด
// colPins : กำหนด Pin คอลัมน์ของปุ่มกด
// ROWS : กำหนดจำนวนของ Rows
// COLS : กำหนดจำนวนของ Columns
// I2CADDR : กำหนด Address ขอ i2C
// PCF8574 : กำหนดเบอร์ IC
Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, RFID_ADDR, PCF8574 );

void setup() {
  Serial.begin(115200);
  lcd.begin();           // ใช้หน้าจอ LCD
  lcd.backlight();       // เปิด backlight

  SPI.begin();          // เริ่มต้นใช้งานการสื่อสารแบบ SPI
  rfid.PCD_Init();      // ตั้งค่าเริ่มต้นของ RFID

  Wire.begin();  // initialize I2C
  keypad.begin( makeKeymap(keys) );  // call use keypad

  pinMode(Relay, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Relay, status_relay);
  pinMode(Button, INPUT_PULLUP);

  // setup WiFi Manager
  pinMode(Reset_wifi, INPUT_PULLUP); //input pullup มี logic "1" เวลาใช้ในต่อกับ GND เลยไม่ต้องใช้ R
  WiFiManager wm;
  if (digitalRead(Reset_wifi) == LOW) {
    Serial.println("Reset WiFi");
    wm.resetSettings();
  }
  bool res;
  res = wm.autoConnect("smart_doorlock", "1234567890");
  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    Serial.println("connected...yeey :)");
  }

  //init and get the time (NTP)
  configTime(gmtOffset_sec, daylightOffset_sec, ntp_server1, ntp_server2, ntp_server3);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Set RTC time
  rtc.begin();
  if (rtc.lostPower()) { // use compilation time to set date and time for RTC
    // rtc.adjust(DateTime(year, month, day, hour(am), min, sec));
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    // Note!(.tm_year) The first year of counting was 1900.
    // Note! (.tm_mon) January starts at 0.
  }

}

void buzzer() {
  //  tone(Buzzer, 2000, 20);
  digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);
  delay(100);
}

String Read_RFID() {
  //  buzzer();
  // ตรวจสอบการอ่าน Tag อันใหม่
  //  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  //    return;
  // Serial.print("PICC type: ");
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));

  // ตรวจสอบชนิดของ Tag ต้องตรงตามที่กำหนดเท่านั้น
  if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||
      piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
      piccType == MFRC522::PICC_TYPE_MIFARE_4K)
  {
    String strID = "";                // ล้างค่า ID
    for (byte i = 0; i < 4; i++)      // วนลูปเพื่อรับค่า UID Tag
    {
      //      strID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +      // เก็บค่า UID Tag
      //      String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : "");

      strID.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
      strID.concat(String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : ""));
    }
    strID.toUpperCase();              // ปรับตัวอักษรเป็นตัวพิมพ์ใหญ่

    // เมื่อถึงส่วนนี้ ตัวแปร strID จะเก็บค่า UID ของแท็กไว้แล้ว
    Serial.print("Tap card key: ");   // แสดงข้อตวาม ทาง Serial Monitor
    Serial.println(strID);              // แสดงค่า UID ทาง Serial Monitor

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();           // ปิดระบบความปลอดภัยการอ่านข้อมูล
    return strID;
  }
  else {
    Serial.println("Your tag is not of type MIFARE Classic.");
  }
}

void sh_time() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void loop() {
  // check rfid
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println(Read_RFID());
  }

  // check buzzer
  //  buzzer();

  // check keypad
  char key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด
  if (key) { // ถ้าหากตัวแปร key มีอักขระ
    Serial.println(key);
  }

  // check LED
  lcd.setCursor(0, 0); // ไปที่ตัวอักษรที่ 0 แถวที่ 1
  lcd.print("Welcome To");
  lcd.setCursor(7, 1); // ไปที่ตัวอักษรที่ 7 แถวที่ 2
  lcd.print("Cybertice");

  // check RTC & temperature
  //  sh_time();
  //  DateTime time = rtc.now();
  //  datestr = time.timestamp(DateTime::TIMESTAMP_DATE);
  //  timestr = time.timestamp(DateTime::TIMESTAMP_TIME);
  //  Serial.print(daysOfTheWeek[time.dayOfTheWeek()]);
  //  Serial.println(datestr);
  //  Serial.println(timestr);
  //  Serial.print("Temperature: ");
  //  Serial.print(rtc.getTemperature());
  //  Serial.println(" C");
  //  delay(1000);

  // check switch & relay
  if (digitalRead(Button) == LOW) {
    delay(70);
    if (digitalRead(Button) == LOW) {
      Serial.println("switch active");
      Serial.println(status_relay);
      digitalWrite(Relay, status_relay);
      status_relay = !status_relay;
      Serial.println(status_relay);
      delay(100);
    }
  }
}