#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include <IOT_TECHNION_SURFBOARD.h>

const string CONFIG_FILE_NAME = "sampling_unit.config";
const int REPORT_STATUS_INTERVAL_MILLIS = 1000;

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
        SamplerStatus lastStatusReported;
    public:
        SurfboardSamplingUnit(SamplingUnitSyncManager* syncManager, SDCardHandler* sdCardHandler, Sampler* sampler, Logger* logger);
        void addSensor(SensorBase* sensor);
        SamplerStatus getStatus();
        void handleNextCommand();
        void loopSampling();
        void loopFileUpload();
        void reportStatus();
};
#endif // SURFBOARD_SAMPLING_UNIT_H