#ifndef SD_CARD_SAMPLE_LOGGER_H 
#define SD_CARD_SAMPLE_LOGGER_H

using namespace std;
#include <cstdint>
#include <string>

// todo: add SD card connection here

class SDCardSampleLogger{
    public:
        /*
            Log samples to a file in an SD card.
            Constraints:
                1. file name is unique that includes the timestamp in which the sampling started at
                2. file csv header is shown in the example
                3. sample coordinates are seperated by a comma ','

                example file:
                    ----------------------------------------------------------------------------------------------------------------
                    |        sample_data          |          sample_units                           | sampling_unit_id | sensor_id | 
                    ----------------------------------------------------------------------------------------------------------------
                    |(-0.13,0.67,-0.71,0.13,3.14) | (quatI,quatJ,quatK,quatReal,quatRadianAccuracy) |         123      |   1547    | 
                    ----------------------------------------------------------------------------------------------------------------
                    |(0.23,-9.77,-1.57)           | (AccX,AccY,AccZ)                                |         547      |   1747    | 
                    ----------------------------------------------------------------------------------------------------------------

        */
        static FILE createCSVSampleFile(int timestamp); // automatically creates the above header in the file
        static void logSample(FILE logFile, string samplingUnitID, string sensorID, string sampleData, string sampleUnits);
};


#endif /* SD_CARD_SAMPLE_LOGGER_H */