#ifndef SURFBOARD_SAMPLING_UNIT_H
#define SURFBOARD_SAMPLING_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include "../Components/Sampler.h"
#include "../Sync/SamplingUnitSyncManager.h"


class SurfboardSamplingUnit {
    private:
        Sampler sampler;
        SamplingUnitSyncManager syncManager;
        Logger logger;
        int lastStatusReportTime;
    public:
        SurfboardSamplingUnit(){};
        SurfboardSamplingUnit(uint8_t controlUnitMac[]){
            logger = Logger();
            syncManager = SamplingUnitSyncManager(logger, controlUnitMac);
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
                sampler.enterErrorState();
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
                    case ControlUnitCommand::UPLOAD_SAMPLE_FILES:
                        try{
                            sampler.stopSampling();
                            sampler.uploadSampleFiles(command->params[WIFI_SSID], command->params[WIFI_PASSWORD]);
                        }catch(exception& err){
                            logger.error("Invalid command params");
                            return;
                        }
                }
                delete command;
            }

            // status report - update every STATUS_REPORT_DELAY_MILLIS
            int current = millis();
            if((current - lastStatusReportTime) >= STATUS_REPORT_DELAY_MILLIS){
                syncManager.reportStatus(sampler.getStatus());
                lastStatusReportTime = current;
            }
        }
};

#endif // SURFBOARD_SAMPLING_UNIT_H