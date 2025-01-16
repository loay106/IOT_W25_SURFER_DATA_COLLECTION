#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include <IOT_TECHNION_SURFBOARD.h>


class SurfboardSamplingUnit {
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

#endif // SURFBOARD_SAMPLING_UNIT_H