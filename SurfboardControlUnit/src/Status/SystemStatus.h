#ifndef SYSTEM_STATUS_H 
#define SYSTEM_STATUS_H

enum SystemStatus {
    SYSTEM_STARTING,
    SYSTEM_INITIALIZING,
    SYSTEM_STAND_BY,
    SYSTEM_SAMPLING,
    SYSTEM_ERROR
};

enum SamplingUnitStatus {
    UNIT_STAND_BY,
    UNIT_SAMPLING,
    UNIT_DISCONNECTED,
    UNIT_ERROR  
};

#endif /* SYSTEM_STATUS_H */