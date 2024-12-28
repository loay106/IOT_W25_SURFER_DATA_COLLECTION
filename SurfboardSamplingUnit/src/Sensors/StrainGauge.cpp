#include "StrainGauge.h"

StrainGuage::StrainGuage(ForceModel Model): ForceBase(Model),force_calibration_enabled(false),
 force_weight_enabled(false) {}


void StrainGuage::enableForceCalibrationMeasurement()
{
    force_calibration_enabled = true;
}

void StrainGuage::disableForceCalibrationMeasurement()
{
    force_calibration_enabled = false;
}

void StrainGuage::enableForceWeightMeasurement()
{
    force_weight_enabled = true;
}

void StrainGuage::disableForceWeightMeasurement()
{
    force_weight_enabled = false;
}

string StrainGuage::getForceCalibrationSample()
{
    
    String result = "";  // To store the result as a string
    if (sensor.is_ready()) {
        sensor.set_scale();    
        Serial.println("Tare... remove any weights from the scale.");
        delay(3000);  // Wait for 3 seconds to remove any weights

        sensor.tare();  // Zero the scale
        Serial.println("Tare done...");

        Serial.print("Place a known weight on the scale...");
        delay(3000);  // Wait for 3 seconds to place the known weight

        long reading = sensor.get_units(10);  // Get the average reading from 10 samples

        Serial.print("Result: ");
        Serial.println(reading);

        // Store the result in the string
        result = String(reading);
    } 
    
    else {
        Serial.println("HX711 not found.");
        result = "HX711 not found.";  // Error message if sensor is not ready
    }

    delay(100);  // Short delay before the function ends

    return result;  // Return the result as a string 
}


string StrainGuage::getForceWeightSample()
{
    float reading = scale.get_units();  // Get the current weight reading
    float average = scale.get_units(10);  // Get average of 10 readings
    // Create a message string with the reading and average
    String message = "one reading:\t" + String(reading, 1) + "\t| average:\t" + String(average, 5);
    // Power down the scale to save energy
    scale.power_down();  
    delay(5000);  // Wait for 5 seconds
    scale.power_up();
    return message;
}


string StrainGuage::getSamples()
{
    if(force_calibration_enabled)
    {
        return getForceCalibrationSample();
    }

    else if(force_weight_enabled)
    {
        return getForceWeightSample();
    }

    else{
        status = ERROR;
        Serial.println("No Samples Available...");
        return "error";
    }
}


void  StrainGuage::stopSampling()
{
    force_calibration_enabled = false;
    force_weight_enabled = false;
    status = STANDBY;
}