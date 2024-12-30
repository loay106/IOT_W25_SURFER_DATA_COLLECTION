#ifndef LOGGER_H 
#define LOGGER_H

using namespace std;
#include <cstdint>
#include <string>

class Logger{
    public:
        static void error(string message);
        static void info(string message);
};


#endif /* LOGGER_H */