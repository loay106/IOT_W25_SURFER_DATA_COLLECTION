#include <cstdint>
#ifndef CONTROL_UNIT_SYNC_MANAGER_H
#define CONTROL_UNIT_SYNC_MANAGER_H

class ControlUnitSyncManager {
    public:
        ControlUnitSyncManager(uint8_t controlUnitDeviceMac[]);
        void requestCurrentTimeStamp(int* timeStampBuffer); // fill timeStampBuffer with the received result from control unit
        void reportStatus(); // todo: add params and so on...
        void awaitNextCommand(); // todo: declare commands between sampling unit and control unit
        // commands such as: START_SAMPLING, STOP_SAMPLING, SYNC_SAMPLE_DATA
        // todo: add more methods as needed
};

#endif // CONTROL_UNIT_SYNC_MANAGER_H