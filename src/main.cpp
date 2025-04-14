// https://circuits4you.com/2018/10/03/rfid-reader-rc522-interface-with-nodemcu-using-arduino-ide/


#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <SPI.h>
#include <MFRC522.h>

/* pinout
VL53L0X:
SDA - D3  - 0
SCL - D4  - 2
RFID-RC522:
SDA - D2  - 4
SCL - D5  - 14
MOSI  - D7  - 13
MISO  - D6  - 12
RST - D1  - 5
*/

constexpr uint8_t RFID_RST_PIN = 5;
constexpr uint8_t RFID_SS_PIN = 4;
constexpr uint8_t vl53l0xSDA_PIN = 0;
constexpr uint8_t vl53l0xSCL_PIN = 2;

VL53L0X sensor;
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN); 
MFRC522::MIFARE_Key key; 

byte nuidPICC[4];

void vl53l0xInit(){
  unsigned long now_time = millis();
  unsigned long time_delta = 1000;
  while (!sensor.init()) {
    Serial.println("Try to initialize VL53L0X");
    if (millis() > now_time + time_delta){
      now_time = millis();
      Wire.begin(vl53l0xSDA_PIN, vl53l0xSCL_PIN); 
      sensor.setTimeout(500);
    }
  }
  Serial.println("Sucess initialization VL53L0X");
  sensor.startContinuous();
}

void checkVl53l0x(){
  Serial.println(sensor.readRangeContinuousMillimeters());

  if (sensor.timeoutOccurred()) {
    Serial.println("timeout error");
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void rfidInit(){
  SPI.begin(); 
  rfid.PCD_Init(); 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void checkRfid(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
 
  if ( ! rfid.PICC_ReadCardSerial())
    return;
 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
 
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    printHex(rfid.uid.uidByte, rfid.uid.size);
    printDec(rfid.uid.uidByte, rfid.uid.size);
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void setup() {
  Serial.begin(9600);
  vl53l0xInit();
  rfidInit();
}

void loop() {
  checkRfid();
}
