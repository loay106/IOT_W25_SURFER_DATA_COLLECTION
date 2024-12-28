#include "src/UnitManager/UnitManager.h"
#include "src/ControlUnitSync/ControlUnitSyncManager.h"

uint8_t CONTROL_UNIT_DEVICE_MAC[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}; // todo: change to control unit device's MAC..

// global objects
UnitManager unitManager;
ControlUnitSyncManager syncManager;

void setup() {
    syncManager = ControlUnitSyncManager(CONTROL_UNIT_DEVICE_MAC);
    unitManager = UnitManager();

    // add imu sensors here
    unitManager.addIMUSensor(SupportedIMUModels::BNO085, IMUSamplingRatio);
}

void loop() {
    UnitManagerStatus status = unitManager.getStatus();
    switch (status)
    {
        case UnitManagerStatus::CONFIGURING:
            if(receivedTimeStamp == NULL){
                delay(5);
            }else{
                unitManager.configure(receivedTimeStamp);
            }
            break;

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

