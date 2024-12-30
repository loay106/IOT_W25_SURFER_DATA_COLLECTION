#ifndef SAMPLING_UNIT_H 
#define SAMPLING_UNIT_H

using namespace std;
#include <vector>
#include <string>
#include <src/Status/SystemStatus.h>

class SamplingUnitRep{
    private:
        SamplingUnitStatus status;

    public:
        SamplingUnitStatus getStatus();
        void updateStatus(SamplingUnitStatus status);
};



#endif /* SAMPLING_UNIT_H */