#include "src/Data/SamplingDataWriter.h"
#include "src/Data/Logger.h"

using namespace std;
#include <string>;

SamplingDataWriter dataWriter;
string fileName = "";


void setup() {
    Logger logger = Logger(57600);
    logger.initialize();
    dataWriter = SamplingDataWriter(5, logger);
    dataWriter.initialize();
    fileName = dataWriter.createSamplingFile(1736083807);
};


void loop() {
    vector<string> data = {"(-0.13 0.67 -0.71 0.13 3.14)", "(-0.13 0.67 -0.71 0.13 3.14)"};
    dataWriter.writeSamples(fileName, "samplingUnit123", "sensor123", data, "(quatI,quatJ,quatK,quatReal,quatRadianAccuracy)");

    //Delay 5s
    delay(5000);
};

