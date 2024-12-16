#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

class TimeManager {
    private:
        int receivedTimestamp;
        long syncMillis;
        bool isConfigured;

    public:
        TimeManager();
        void adjust(int currentTimeStamp);
        int getCurrentTimeStamp();
};

enum UnitManagerStatus{
    STANDBY,
    SAMPLING,
    SYNCING, // sycning samples data to the control unit
    ERROR,
};

enum SupportedIMUModels{
    BNO085,
};

#endif // TIME_MANAGER_H