#include "IMUBase.h"
#include <../DataLogger/DataLogger.h>

IMUBase::IMUBase(const std::string id, const std::string model, DataLogger dataLogger, int samplingRatio){
    this->id=id;
    this->model = model;
    this->dataLogger = dataLogger;
    accelerometerDataFile = NULL;
    rotationVectorDataFile = NULL;
    this->samplingRatio = samplingRatio;
    status = IMUStatus::STANDBY;
}

void IMUBase::startSampling(int currentTimestamp,bool accelerometereSampling, bool rotationVectorSampling){
    if(status != IMUStatus::STANDBY){
        // already sampling or on error mode...
        // todo: log accordingly
        return;
    }
    if(!accelerometereSampling && !rotationVectorSampling){
        // nothing to sample...
        // todo: add warning?
        return;
    }else if(accelerometereSampling){
        // todo: create unique file names with timestamps/id etc.....
        accelerometerDataFile = &dataLogger.createSampleLogFile("test1234", "quatI,quatJ,quatK,quatReal,quatRadianAccuracy");
        // todo: change to parameter
        enableAccelerometer();
        accelerometerEnabled = true;
    }else if(rotationVectorSampling){
        // todo: create unique file names with timestamps/id etc.....
        rotationVectorDataFile = &dataLogger.createSampleLogFile("test1234", "quatI,quatJ,quatK,quatReal,quatRadianAccuracy");
        // todo: change to parameter
        enableRotationVector();
        rotationVectorEnabled = true;
    }
    status = IMUStatus::SAMPLING;
}

void IMUBase::logSamples(int currentTimestamp){
    if(rotationVectorEnabled){
        std::string sample = getgRotationVectorSample();
        dataLogger.logSampleToFile(*rotationVectorDataFile, sample, currentTimestamp);
        
    }
    if(accelerometerEnabled){
        std::string sample = getgAccelerometerSample();
        dataLogger.logSampleToFile(*accelerometerDataFile, sample, currentTimestamp);
    }
}

void IMUBase::stopSampling(int currentTimestamp){
    if(rotationVectorEnabled){
        disableRotationVector();
        // todo: close rotation vector file...
        rotationVectorDataFile = NULL;
        rotationVectorEnabled = false;
    }
    if(accelerometerEnabled){
        disableAccelerometer();
        // todo: close accelerometer file...
        accelerometerDataFile = NULL;
        accelerometerEnabled = false;
    }
    
    status = IMUStatus::STANDBY;
}