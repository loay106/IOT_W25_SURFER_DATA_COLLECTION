#ifndef SYSTEM_STATUS_H 
#define SYSTEM_STATUS_H

enum SystemStatus{
    STARTING,
    INITILAZING,
    STAND_BY,
    SAMPLING,
    ERROR
};

enum SamplingUnitStatus{
    STAND_BY,
    SAMPLING,
    DISCONNECTED,
    ERROR  
};

#endif /* SYSTEM_STATUS_H */