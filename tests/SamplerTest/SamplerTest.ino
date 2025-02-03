#include <IOT_TECHNION_SURFBOARD.h>

uint8_t SDCardChipSelectPin = 5;
const int serialBaudRate = 57600;

Logger* logger;
SDCardHandler* sdCardHandler;
Sampler* sampler;

const long samplingIntervalMillis = 5000;
int startTimeMillis;
bool isSampling;


void setup() {
  logger = Logger::getInstance();
  logger->init(serialBaudRate);
  logger->setLogLevel(LogLevel::DEBUG);
  sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
  sdCardHandler->init();

  IMU_BNO080* IMU0 = new IMU_BNO080(logger,sdCardHandler, 100, 4);

  sampler = new Sampler(logger,sdCardHandler, nullptr);
  sampler->init();
  IMU0->init();
  sampler->addSensor(IMU0);

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



