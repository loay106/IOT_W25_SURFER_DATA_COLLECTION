#ifndef TIME_MANAGER_H 
#define TIME_MANAGER_H

class TimeManager{
    public:
        TimeManager();
        int getCurrentTimestamp();
        void adjust(int timestamp);
};


#endif /* TIME_MANAGER_H */