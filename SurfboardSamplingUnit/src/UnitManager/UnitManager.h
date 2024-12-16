#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include <../Sensors/IMUBase.h>
#include <../TimeManager/TimeManager.h>
#include <string>
#include <list>

class UnitManager {
    private:
        DataLogger dataLogger;
        UnitManagerStatus status;
        TimeManager timeManager;
        int samplingDelayTime; // sampling delay time is calculated as the GCD of the sampling ratios of all the sensors
    
    public:
        std::list<IMUBase> imuSensors;

        UnitManager(DataLogger dataLogger, TimeManager timeManager);

        void addIMUSensor(SupportedIMUModels model, int samplingRatio);
        UnitManagerStatus getStatus();

        void startSampling();
        int getSamplingDelayTime();
        void logSamples();
        void stopSampling();

};

enum UnitManagerStatus{
    STANDBY,
    SAMPLING,
    SYNCING, // sycning samples data to the control unit
    ERROR,
};

enum SupportedIMUModels{
    BNO085,
};

#endif // UNIT_MANAGER_H