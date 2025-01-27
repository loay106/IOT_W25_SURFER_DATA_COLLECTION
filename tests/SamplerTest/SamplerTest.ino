#include <IOT_TECHNION_SURFBOARD.h>

uint8_t SDCardChipSelectPin = 5;
const int serialBaudRate = 57600;

Logger* logger;
SDCardHandler* sdCardHandler;
Sampler* sampler;

const long samplingIntervalMillis = 60000;
int startTimeMillis;
bool isSampling;


void setup() {
  logger = Logger::getInstance();
  logger->init(serialBaudRate);
  logger->setLogLevel(LogLevel::DEBUG);
  sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
  sdCardHandler->init();

  Force_FAKE* fake_force_0 = new Force_FAKE(logger,sdCardHandler);
  Force_FAKE* fake_force_1 = new Force_FAKE(logger,sdCardHandler);
  Force_FAKE* fake_force_2 = new Force_FAKE(logger,sdCardHandler);
  sampler = new Sampler(logger,sdCardHandler, nullptr, "-", "-");
  sampler->init();
  sampler->addSensor(fake_force_0);
  sampler->addSensor(fake_force_1);
  sampler->addSensor(fake_force_2);

  startTimeMillis = millis();
  sampler->startSampling(rand());
  isSampling = true;
}

void loop() {
  if(millis() - startTimeMillis < samplingIntervalMillis){
    sampler->writeSensorsData();
    delay(3);
  }else if(isSampling){
    sampler->stopSampling();
    isSampling=false;
  }else{
    // simulation stopped
    delay(1000);
  }
}



