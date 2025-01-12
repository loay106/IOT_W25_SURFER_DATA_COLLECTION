#ifndef FORCE_BASE_H
#define FORCE_BASE_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

enum ForceSamplingModes{
    FORCE_IN_NEWTONS
};

enum ForceStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};


enum ForceModel{
    STRAIN_GUAGE
};

class ForceBase {
    protected:
        string id;
        ForceModel model;
        ForceStatus status;
        bool sensorEnabled;
        ForceSamplingModes mode;
    
    public:
        ForceBase(ForceModel Model) : model(Model), status(STANDBY) , sensorEnabled(false)
        {
            srand(time(0));
            id = to_string(rand());
        }
        virtual void setup()=0;
        virtual string getSamples()=0;
        virtual void stopSampling()=0;
        virtual void enableSensor()=0;
        virtual void disableSensor()=0;
        virtual bool getSensorStatus()=0;
        virtual ~ForceBase() = default;
};


#endif // FORCE_BASE_H