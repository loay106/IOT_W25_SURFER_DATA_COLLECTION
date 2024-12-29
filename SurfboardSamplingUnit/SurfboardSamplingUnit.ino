#include "src/UnitManager/UnitManager.h"
#include "src/ControlUnitSync/ControlUnitSyncManager.h"
#include "src/Sensors/IMU_BNO080.h"

uint8_t CONTROL_UNIT_DEVICE_MAC[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}; // todo: change to control unit device's MAC..

// global objects
UnitManager unitManager;
ControlUnitSyncManager syncManager;
int SamplingDelayTime = 80;

void setup() {
    Serial.begin(9600);
    syncManager = ControlUnitSyncManager(CONTROL_UNIT_DEVICE_MAC);
    unitManager = UnitManager();

    // add imu sensors here
    IMU_BNO080 sensor_1 = IMU_BNO080(SamplingModes::ACCELEROMETER,SamplingDelayTime);
    sensor.setup();
    unitManager.addIMUSensor(sensor_1);
}

void loop() {
    UnitManagerStatus status = unitManager.status;
    switch (status)
    {
        case UnitManagerStatus::STANDBY:
            if(syncManager.getNextCommand() == ControlUnitCommand::START_SAMPLING){
                unitManager.status = UnitManagerStatus::SAMPLING;
                syncManager.reportStatus(unitManager.status);

            }
            else{
                unitManager.status = UnitManagerStatus::ERROR;
            }
            delay(SamplingDelayTime);
            break;

        case UnitManagerStatus::SAMPLING:
            if(syncManager.getNextCommand() == ControlUnitCommand::STOP_SAMPLING){
                unitManager.stopSampling();
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
            while(1);// don't know what to do
            break;

        default:
            break;
    }
}

