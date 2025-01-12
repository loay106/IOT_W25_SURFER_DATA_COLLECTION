#include "HX711.h"

const int LOADCELL_DOUT_PIN = 12;  // Data pin for HX711
const int LOADCELL_SCK_PIN = 13; 
HX711 scale;

void setup() {
  Serial.begin(115200);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  Serial.println("HX711 initialized");
  Serial.println("Calculating sampling rate...");
}

void loop() {
  unsigned long startTime = millis();
  unsigned long currentTime;
  int sampleCount = 0;
  
  // Count samples for exactly one second
  do {
    if (scale.is_ready()) {
      long reading = scale.read();
      sampleCount++;
      
      // Optionally print the reading (note: this will slow down sampling)
      // Serial.print("Sample ");
      // Serial.print(sampleCount);
      // Serial.print(": ");
      // Serial.println(reading);
    }
    currentTime = millis();
  } while (currentTime - startTime < 1000);  // Run for 1 second
  
  // The sampling rate is equal to the number of samples taken in one second
  float samplingRate = sampleCount;
  
  Serial.print("Sampling rate: ");
  Serial.print(samplingRate);
  Serial.println(" Hz");
  
  delay(5000); // Wait 5 seconds before next measurement
}
