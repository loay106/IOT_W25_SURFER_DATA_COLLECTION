#include "HX711.h"
#include <SD.h>
#include <SPI.h>

// Define pins for the HX711 load cell
const int LOADCELL_DOUT_PIN = 12;  // Data pin for HX711
const int LOADCELL_SCK_PIN = 13;   // Clock pin for HX711
HX711 scale;  // HX711 scale object

long knownWeight = 500;  // A known weight in grams for calibration (adjust as per your setup)
long calibration_factor = 438;  // Replace this with your calibration factor


void appendData(const char* file, String message){
    File dataFile = SD.open(file, FILE_APPEND);
    if (dataFile) {
      dataFile.println(message);
      dataFile.close();
    } else {
      Serial.println("Error opening IMU_data.csv for appending.");
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


/*void CalibrationFactorSample()
{

 if (scale.is_ready()) 
 {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else 
  {
    Serial.println("HX711 not found.");
  }
  delay(100);
}*/


void CalibrationFactorSample()
{
    // Log the start of the calibration
    writeData("/calibration_log.csv", "Calibration Started");

    if (scale.is_ready()) {
        scale.set_scale();
        appendData("/calibration_log.csv", "Taring... Please remove any weights from the scale.");
        delay(5000);
        scale.tare();
        appendData("/calibration_log.csv", "Tare done...");
        
        appendData("/calibration_log.csv", "Place a known weight on the scale...");
        delay(5000); // Give the user time to place the known weight

        long reading = scale.get_units(10); // Read average of 10 readings
        appendData("/calibration_log.csv", "Raw reading with known weight: " + String(reading));

    } else {
        appendData("/calibration_log.csv", "HX711 not found.");
    }
    delay(100);
}


void ForceWeightSample()
{
  float reading = scale.get_units();  // Get the current weight reading

  // Get the average of 10 readings
  float average = scale.get_units(10);  // Get average of 10 readings

  // Create a message string with the reading and average
  String message = "one reading:\t" + String(reading, 1) + "\t| average:\t" + String(average, 5);
  // Write the message to the SD card in the ForceWeightSample.txt file
  appendData("/ForceWeightSample.txt", message);  // Write to file

  // Power down the scale to save energy
  scale.power_down();  
  delay(5000);  // Wait for 5 seconds

  scale.power_up();
}

void setup() {
  // Start the serial communication for debugging
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  SPI.begin(19, 21, 18, 5); // SCK, MISO, MOSI, CS
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    return;
  }
  // Initialize the HX711 scale
  delay(100);
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  // by the SCALE parameter (not set yet)
  scale.set_scale(calibration_factor); //!!!!!!!!!!!!!!!!!!!!!!!!insert your calibration factor!!!!!!!!!!!!!!!!!!!!!!!!!!!
  scale.tare();               // reset the scale to 0
  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale
}



void loop() {
  //CalibrationFactorSample();
  scale.set_scale(calibration_factor);
  //scale.tare();
  ForceWeightSample();
  //delay(400);

}