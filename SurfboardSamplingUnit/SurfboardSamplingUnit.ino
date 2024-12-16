//////////////////////////////////////////////// libraries///////////////////////////////////////////////////////

#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "SparkFun_BNO080_Arduino_Library.h"

//////////////////////////////////////////////// defenetions///////////////////////////////////////////////////////

BNO080 myIMU;

// Define SD card chip select pin
const int chipSelect = 5;
//////////////////////////////////////////////// writing to SD///////////////////////////////////////////////////////
void appendData(const char* file, String message){
      File dataFile = SD.open(file, FILE_APPEND);
    if (dataFile) {
      dataFile.println(message);
      dataFile.close();
    } else {
      Serial.println("Error opening IMU_data.csv for writing.");
    }
}
void writeData(const char* file, String message){
      File dataFile = SD.open(file, FILE_WRITE);
    if (dataFile) {
      dataFile.println(message);
      dataFile.close();
    } else {
      Serial.println("Error opening IMU_data.csv for writing.");
    }
}
//////////////////////////////////////////////// setups ///////////////////////////////////////////////////////

void rotationVectorSetup(){
  myIMU.enableRotationVector(50); // Send data update every 50ms

  Serial.println(F("Rotation vector enabled"));
  Serial.println(F("Output in form i, j, k, real, accuracy"));

  // Write the header line in FILE_WRITE mode
  String header = "quatI,quatJ,quatK,quatReal,quatRadianAccuracy";
  writeData("/IMU_RV_data.csv",header);
}

void accelerometerSetup(){
  myIMU.enableAccelerometer(50); // Enable accelerometer data every 50ms

  Serial.println(F("Accelerometer enabled"));
  Serial.println(F("Output in form AccX, AccY, AccZ"));
  // Write the header line in FILE_WRITE mode
  String header = "AccX,AccY,AccZ";
  writeData("/IMU_acc_data.csv",header);
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("BNO080 Read Example with SD card output");

  // Initialize SPI and SD card
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, CS
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed. Check connections.");
    while (1);
  }
  Serial.println("SD card initialized.");
  Wire.begin(21, 22); // SDA, SCL for ESP32

  if (myIMU.begin() == false) {
    Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }

  Wire.setClock(400000); // Increase I2C data rate to 400kHz
// sensor setup
  accelerometerSetup();

}
//////////////////////////////////////////////// samples ///////////////////////////////////////////////////////

void rotationVectorSample(){
  if (myIMU.dataAvailable() == true) {
    float quatI = myIMU.getQuatI();
    float quatJ = myIMU.getQuatJ();
    float quatK = myIMU.getQuatK();
    float quatReal = myIMU.getQuatReal();
    float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();

    // Create a string to store the data
    String dataString = String(quatI, 2) + "," + 
                        String(quatJ, 2) + "," + 
                        String(quatK, 2) + "," + 
                        String(quatReal, 2) + "," + 
                        String(quatRadianAccuracy, 2);
    Serial.println(dataString);
    // Write to SD card in append mode
    appendData("/IMU_RV_data.csv",dataString);
  }
}

void accelerometerSample(){
   if (myIMU.dataAvailable() == true) {
    float accX = myIMU.getAccelX();
    float accY = myIMU.getAccelY();
    float accZ = myIMU.getAccelZ();

    // Create a string to store the data
    String dataString = String(accX, 2) + "," + 
                        String(accY, 2) + "," + 
                        String(accZ, 2);
    Serial.println(dataString);
    // Write to SD card in append mode
    appendData("/IMU_acc_data.csv",dataString);
  }
}
//////////////////////////////////////////////// loop ///////////////////////////////////////////////////////

void loop() {

accelerometerSample();
delay(50);

}

