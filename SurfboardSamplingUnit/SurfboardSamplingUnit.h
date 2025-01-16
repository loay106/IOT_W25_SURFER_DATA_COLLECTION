#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include <IOT_TECHNION_SURFBOARD.h>


class SurfboardSamplingUnit {
    private:
        Sampler sampler;
        SamplingUnitSyncManager syncManager;
        Logger* logger;
        SDCardHandler sdCardHandler;
        int lastStatusReportTime;
    public:
        SurfboardSamplingUnit(){};
        SurfboardSamplingUnit(uint8_t controlUnitMac[], int SDCardChipSelectPin);

        void init();

        void addSensor(SensorBase* sensor);
        void updateSystem();

#endif // SURFBOARD_SAMPLING_UNIT_H