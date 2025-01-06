

#include "src/UnitManager/UnitManager.h"
#include "src/ControlUnitSync/ESPNowControlUnitSyncManager.h"
#include "src/Sensors/IMU_BNO080.h"
#include <WiFi.h>
#include <esp_now.h>

uint8_t CONTROL_UNIT_DEVICE_MAC[] = {0xA8, 0x42, 0xE3, 0x45, 0x94, 0x68}; // todo: change to control unit device's MAC..

// global objects
int SamplingDelayTime = 800;
UnitManager unitManager;
ESPNowControlUnitSyncManager syncManager;
IMU_BNO080 sensor_1(SamplingModes::ACCELEROMETER,SamplingDelayTime);

void setup() {
    Serial.begin(115200);
    syncManager.initialize(CONTROL_UNIT_DEVICE_MAC);
    unitManager = UnitManager(&syncManager);
    // add sensors here
    unitManager.addIMUSensor(&sensor_1);
}

void loop() {
     UnitManagerStatus status = unitManager.status;
    ControlUnitCommand command = syncManager.getNextCommand();
    switch (status)
    {
        case UnitManagerStatus::STANDBY:
            if(command == ControlUnitCommand::START_SAMPLING){
                unitManager.status = UnitManagerStatus::SAMPLING;
                syncManager.reportStatus(unitManager.status);
            }
            delay(SamplingDelayTime);
            break;

        case UnitManagerStatus::SAMPLING:
            if(command == ControlUnitCommand::STOP_SAMPLING){
                unitManager.status = UnitManagerStatus::STANDBY;
                syncManager.reportStatus(unitManager.status);
            }
            else{
                unitManager.startSampling();
            }
            delay(SamplingDelayTime);
            break;

        case UnitManagerStatus::ERROR:
            syncManager.reportStatus(unitManager.status);
            while(1);
            break;

        default:
            break;
    }
}

// #include "src/Sensors/IMU_BNO080.h"
// #include "src/UnitManager/UnitManager.h"
// #include <WiFi.h>
// #include <esp_now.h>

//  uint8_t CONTROL_UNIT_DEVICE_MAC[] = {0xA8, 0x42, 0xE3, 0x46, 0xE0, 0x64};
// UnitManager unitManager;
// ESPNowControlUnitSyncManager syncManager;

// std::string messageToSend="STATUS_UPDATE|STAND_BY";

// void setup() {
//   Serial.begin(115200);
//   syncManager.initialize(CONTROL_UNIT_DEVICE_MAC);
//    unitManager = UnitManager(&syncManager);
// }

// void loop(){
//   unitManager.syncManager->sendData((uint8_t *) messageToSend.c_str(),sizeof(messageToSend));
//   delay(500);

// }



