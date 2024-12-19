#ifndef SAMPLING_UNIT_H 
#define SAMPLING_UNIT_H

using namespace std;
#include <vector>
#include <string>

class SamplingUnit{
    private:
        SamplingUnitStatus status;

    public:
        SamplingUnitStatus getStatus();
        void updateStatus(SamplingUnitStatus status);
};


enum SamplingUnitStatus{
    STAND_BY,
    SAMPLING,
    DISCONNECTED,
    ERROR  
};


#endif /* SAMPLING_UNIT_H */