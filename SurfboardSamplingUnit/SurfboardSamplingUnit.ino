
/*#include "HX711.h"

#define DOUT_PIN 12
#define CLK_PIN 13

HX711 scale;
float data_loss_percentage = 0;
static int total = 0;
static int data_loss = 0;

// Calibration factor (adjust this based on your calibration)
float calibration_factor = 438;

// Gravity constant (m/s^2)
const float GRAVITY = 9.81;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Force Measurement Test");

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Reset scale to zero

  delay(5000);
}

void loop() {
  
  unsigned long startTime = millis();
  unsigned long currentTime;
  int sampleCount = 0; 
  // Count samples for exactly one second
  do {
    if (scale.is_ready()) {
    sampleCount++;
    float mass_kg = scale.get_units() / 1000;

    /*float force_N = mass_kg * GRAVITY;
    Serial.print("Mass: ");
    Serial.print(mass_kg, 3);
    Serial.print(" kg, Force: ");
    Serial.print(force_N, 2);
    Serial.println(" N");
  } 
  else 
  {
    Serial.println("HX711 not found.");
  }

    delay(13);
    currentTime = millis();
  } while (currentTime - startTime < 1000);  // Run for 1 second
  
  // The sampling rate is equal to the number of samples taken in one second
  float samplingRate = sampleCount;
  if (scale.is_ready()) {
    float mass_kg = scale.get_units() / 1000;

    float force_N = mass_kg * GRAVITY;
    Serial.print("Mass: ");
    Serial.print(mass_kg, 3);
    Serial.print(" kg, Force: ");
    Serial.print(force_N, 2);
    Serial.println(" N");
  } 
  else {
    Serial.println("HX711 not found.");
  }
  Serial.print("Sample Count : ");
  Serial.println(sampleCount);
  //delay(20);
}*/


#include "HX711.h"

#define DOUT_PIN 12
#define CLK_PIN 13

HX711 scale;

// Make these static to maintain their values across loop iterations
static int total = 0;
static int data_loss = 0;

// Calibration factor (adjust this based on your calibration)
float calibration_factor = 438;

// Gravity constant (m/s^2)
const float GRAVITY = 9.81;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Force Measurement Test");

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Reset scale to zero

  Serial.println("Place known mass on scale to calculate force");
  delay(5000);
}

void loop() {
  unsigned long startTime = millis();
  unsigned long currentTime;
  int sampleCount = 0;

  // Count samples for exactly one second
  do {
    if (scale.is_ready()) {
      sampleCount++;
      // Your measurement code here
    } else {
      Serial.println("HX711 not found.");
      data_loss++;
    }
    total++;
    delay(13);
    currentTime = millis();
  } while (currentTime - startTime < 1000);  // Run for 1 second

  // Calculate and print data loss percentage every 100 samples
  if (total >= 100) {
    float data_loss_percentage = (float)data_loss / total * 100;
    Serial.print("Data loss percentage: ");
    Serial.print(data_loss_percentage);
    Serial.println("%");

    // Reset counters
    total = 0;
    data_loss = 0;
  }

  Serial.print("Sample Count: ");
  Serial.println(sampleCount);
}
