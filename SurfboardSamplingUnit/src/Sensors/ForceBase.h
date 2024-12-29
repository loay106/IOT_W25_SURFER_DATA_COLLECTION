#ifndef FORCE_BASE_H
#define FORCE_BASE_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

class ForceBase {
    protected:
        string id;
        ForceModel model;
        ForceStatus status;
        //int samplingRate;
    
    public:
        ForceBase(ForceModel Model) : model(Model), status(STANDBY)
        {
            srand(time(0));
            id = to_string(rand());
        }
       // void startSampling();
        virtual string getSamples()=0;
        virtual void stopSampling()=0;
        virtual ~ForceBase() = default;
};

enum ForceStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};


enum ForceModel{
    STRAIN_GUAGE
};

#endif // FORCE_BASE_H