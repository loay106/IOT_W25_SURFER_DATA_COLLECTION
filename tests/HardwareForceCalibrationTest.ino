/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/arduino-load-cell-hx711/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Calibrating the load cell
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;
float calibration_factor = 0;
float weight=0; //in grams

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {
  
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Enter a known weight and Place it on the scale...");
    while (!Serial.available()) {
    // Wait for user input
    }
    if (Serial.available() > 0) {
      String userInput = Serial.readString();
      userInput.trim(); // Remove any whitespace
      weight = userInput.toFloat();
    }
    //delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Raw Result: ");
    Serial.println(reading);
    calibration_factor = reading / weight;
    Serial.print("Calibration Factor: ");
    Serial.println(calibration_factor);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(100);
}

//calibration factor will be the (reading)/(known weight)
