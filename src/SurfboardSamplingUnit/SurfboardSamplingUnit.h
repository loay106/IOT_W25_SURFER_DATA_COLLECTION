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
        int lastStatusReportTime;
    public:
        SurfboardSamplingUnit(){};
        SurfboardSamplingUnit(Logger logger, uint8_t controlUnitMac[]): logger(logger){
            syncManager = ControllerSyncManager(logger, controlUnitMac);
            sampler = Sampler(logger);
        };

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
            // command handling
            CommandMessage* command = syncManager.getNextCommand();
            if(command){
                switch(command->command){
                    case ControlUnitCommand::START_SAMPLING:
                        try{
                            int timestamp = stoi(command->params[TIMESTAMP]);
                            int imuRate = stoi(command->params[IMU_RATE]);
                            sampler.startSampling(timestamp, imuRate);
                        }catch(const exception& ex){
                            logger.error("Invalid command params");
                            return;
                        }    
                    case ControlUnitCommand::STOP_SAMPLING:
                        sampler.stopSampling();
                    case ControlUnitCommand::UPLOAD_SAMPLE_FILES:
                        try{
                            // todo: get wifi params here....
                            sampler.uploadSampleFiles();
                        }catch(exception& err){
                            // todo: complete handling here....
                        }
                }
                delete command;
            }

            // status report
            int elapsedSeconds = (millis() - lastStatusReportTime) / 1000; 
            if(elapsedSeconds >= STATUS_REPORT_DELAY_SECONDS){
                syncManager.reportStatus(sampler.getStatus());
                lastStatusReportTime = millis();
            }
        }
};

#endif // SURFBOARD_SAMPLING_UNIT_H