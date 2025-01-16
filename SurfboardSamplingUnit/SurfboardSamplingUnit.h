#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include <IOT_TECHNION_SURFBOARD.h>

const string CONFIG_FILE_NAME = "sampling_unit.config";

class SurfboardSamplingUnit {
    /*  
        todo: add functionalities:
            1. read sensors params from config file
    */
    private:
        Sampler* sampler;
        SamplingUnitSyncManager* syncManager;
        Logger* logger;
        SDCardHandler* sdCardHandler;
        int lastStatusReportTime;
    public:
        SurfboardSamplingUnit(SamplingUnitSyncManager* syncManager, SDCardHandler* sdCardHandler, Sampler* sampler, Logger* logger);
        void addSensor(SensorBase* sensor);
        void updateSystem();
};
#endif // SURFBOARD_SAMPLING_UNIT_H