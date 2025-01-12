#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include "../Sampler/Sampler.h"
#include "Sync/ControllerSyncManager.h"


class SurfboardSamplingUnit {
    private:
        Sampler sampler;
        ControllerSyncManager syncManager;
        Logger logger;
    public:
        SurfboardSamplingUnit(){};
        SurfboardSamplingUnit(Logger logger, uint8_t controlUnitMac[]): logger(logger){
            syncManager = ControllerSyncManager(logger, controlUnitMac);
            sampler = Sampler(logger);
        }};

        void init(){
            syncManager.init();
        }

        void addSensor(SensorBase sensor){
            try{
                sampler.addSensor(sensor);
            }catch(InitError& err){
                logger.error("Failed to add sensor");
                // todo: report error status here...
            }
            
        }

        void updateSystem(){
            ControlUnitCommand command = syncManager.getNextCommand();
            // todo: change and update status here accordingly
            switch(command){
                case ControlUnitCommand::NO_COMMAND:
                    return;
                case ControlUnitCommand::START_SAMPLING:
                    sampler.startSampling(); // todo: get timestamp as a parameter here
                case ControlUnitCommand::STOP_SAMPLING:
                    sampler.stopSampling();
                case ControlUnitCommand::UPLOAD_FILES:
                    sampler.uploadSampleFiles();
            }

        }
};

#endif // SURFBOARD_SAMPLING_UNIT_H