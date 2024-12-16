#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include <string>
#include <list>

class UnitManager {
    protected:
        std::list<IMUBase> imuSensors;
    
    public:
        UnitManager();
        void addIMUSensor(IMUBase sensor);
};

enum UnitManagerStatus{
    STANDBY,
    SAMPLING,
    SYNCING, // sycning samples data to the control unit
    ERROR,
};

#endif // UNIT_MANAGER_H