#include "src/DataLogger/DataLogger.h"
#include "src/TimeManager/TimeManager.h"
#include "src/UnitManager/UnitManager.h"

// constants
int SDCardPin = 5;
int IMUSamplingRatio = 50; // todo: change this to a parameter later on?

// global objects
UnitManager unitManager;


void setup() {
  DataLogger dataLogger = DataLogger(SDCardPin);
  TimeManager timeManager = TimeManager(100); // todo: get current time stamp from the control unit later on....
  unitManager = UnitManager(dataLogger, timeManager);

  // add imu sensors here
  unitManager.addIMUSensor(SupportedIMUModels::BNO085, IMUSamplingRatio);
}

void loop() {
  UnitManagerStatus status = unitManager.getStatus();
  switch (status)
  {
    case UnitManagerStatus::STANDBY:
      // todo: wait for instructions from the control unit to start sampling...
      // todo: add support for ESP now between the two boxes
      delay(50);
      break;
  
    case UnitManagerStatus::SAMPLING:
      unitManager.logSamples();
      delay(unitManager.getSamplingDelayTime());
      break;

    case UnitManagerStatus::SYNCING:
      delay(50);
      break;
  
    case UnitManagerStatus::ERROR:
      delay(50);
      break;

    default:
      break;
  }

}

