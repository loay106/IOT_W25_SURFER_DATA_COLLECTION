#include "HX711.h"
#include <Arduino.h>
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/IO/SDCardHandler.h"
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/IO/Logger.h"
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/Sampler.h"
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/Sensors/Force_HX711.h"
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/Sensors/Force_FAKE.h"
#include "libraries/IOT_TECHNION_SURFBOARD/src/Components/Sensors/SensorBase.h"

uint8_t SDCardChipSelectPin = 5;
const int serialBaudRate = 115200;
const int calibration_factor = 430;
unsigned long previousMillis = 0; // Store the last time the message was printed
const long interval = 15000;
//unsigned long startMillis = 0;
unsigned long timestamp = millis();
const int HX711_rate=80;
int delay_time=0;
const int doutPin=12;
const int sckPin=13;
bool sampling = true;
Logger* logger;
SDCardHandler* sdCardHandler;
Sampler* sampler;


void setup() {
  logger = Logger::getInstance();
  sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
  Force_FAKE* fake_force_1 = new Force_FAKE(logger,sdCardHandler);
  Force_FAKE* fake_force_2 = new Force_FAKE(logger,sdCardHandler);
  Force_FAKE* fake_force_3 = new Force_FAKE(logger,sdCardHandler);
  Force_HX711* real_force = new Force_HX711(logger,sdCardHandler,calibration_factor,doutPin,sckPin);
  sampler = new Sampler(logger,sdCardHandler);
  try{
      // don't change the order of the init
      logger->init(serialBaudRate);    
      sdCardHandler->init();
      sampler->init();
      real_force->init();

  }
  catch(InitError& err){
      while(true){
          // don't proceed, there's a wiring error!
          logger->error("Init error! Check your wiring!");
          delay(100);
      }
  }
    
  sampler->addSensor(fake_force_1);
  sampler->addSensor(fake_force_2);
  sampler->addSensor(fake_force_3);
  sampler->addSensor(real_force);
  // Start sampling (timestamp = 123456 and IMU rate = 10)
  sampler->startSampling(timestamp, HX711_rate);
  delay_time = fake_force_1->getDelayTime();
  // Wait for a few seconds to simulate the sampling
  delay(5000);  // Sample for 5 seconds
  logger->info("Ready to start sampling");
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && sampling) {
    logger->info("15 seconds have passed. Stopping sampling...");
    unsigned long sampling_time = currentMillis - previousMillis;
    sampler->printAcutalRates(sampling_time);
    sampler->stopSampling();
    logger->info("Sampling stopped.");
   
    sampling =false;
  }
  if(sampling)
  {
    sampler->writeSensorsData();
    delayMicroseconds(delay_time);
  }
 
}



