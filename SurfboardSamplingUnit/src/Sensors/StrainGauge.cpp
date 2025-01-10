#include "StrainGauge.h"


StrainGuage::StrainGuage(ForceModel Model): ForceBase(Model),mode(FORCE_IN_NEWTONS) {}


void StrainGuage::enableSensor()
{
    sensorEnabled = true;
}

void StrainGuage::disableSensor()
{
    sensorEnabled = false;
}

bool StrainGuage::getSensorStatus()
{
    return sensorEnabled;
}

void StrainGuage::setup()
{
    Serial.begin(115200);
    Serial.println("HX711 Force Measurement Test");
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare(); // Reset scale to zero
    Serial.println("Place known mass on scale to calculate force");
    delay(5000);
}


string StrainGuage::getSamples()
{
    if(mode==FORCE_IN_NEWTONS)
    {
        if(sensorEnabled)
        {
            status = SAMPLING;
            if (scale.is_ready()) 
            {
                float mass_kg = scale.get_units() / 1000;

                float force_N = mass_kg * GRAVITY;
                Serial.print("Mass: ");
                Serial.print(mass_kg, 3);
                Serial.print(" kg, Force: ");
                Serial.print(force_N, 2);
                Serial.println(" N");
                String force_str = String(force_N, 2);
                return force_str;

            } 
            else 
            {
                Serial.println("HX711 not found.");
                return "Error";
            }
            //delay(13);
        }
        else{
            Serial.println("No Samples Available...");
            return "Error";
        }
    }
    
    else
    {
        status = ERROR;
        Serial.println("Mode Unsupported...");
        return "Error";
    }
}


int StrainGuage::calculateDelayTime(int sampling_rate);
{
    double delay_time = 1000.0/sampling_rate;
    return ceil(delay_time);
}

/*void  StrainGuage::stopSampling()
{
    sensorEnabled = false;
    status = STANDBY;
}*/
