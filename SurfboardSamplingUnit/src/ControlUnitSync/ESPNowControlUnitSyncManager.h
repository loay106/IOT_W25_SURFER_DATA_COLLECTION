#include <cstdint>
#include <src/UnitManager/UnitManager.h>
#ifndef CONTROL_UNIT_SYNC_MANAGER_H
#define CONTROL_UNIT_SYNC_MANAGER_H

class ESPNowControlUnitSyncManager {
    private:
        // todo: handle messaging between this device and the control unit...
        // save in a buffer, or an array etc....
        // different messages have differnet formats and data! handle this too!
    public:
        ESPNowControlUnitSyncManager(uint8_t controlUnitDeviceMac[]);
        void reportStatus(UnitManagerStatus status);
        void sendSamples(); // todo: add params (see format of how they're sent in ControlUnit!)
        ControlUnitCommand getNextCommand();
};

#endif // CONTROL_UNIT_SYNC_MANAGER_H