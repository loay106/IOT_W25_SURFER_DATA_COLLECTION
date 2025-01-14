#ifndef SYSTEM_STATUS_H 
#define SYSTEM_STATUS_H

enum SystemStatus {
    SYSTEM_STARTING, // NO COLOR
    SYSTEM_STAND_BY, // GREEN
    SYSTEM_SAMPLING, // GREEN flickering
    SYSTEM_SAMPLING_PARTIAL_ERROR, // GREEN/RED flickering
    SYSTEM_SAMPLE_FILE_UPLOAD, // BLUE flickering
    SYSTEM_ERROR // RED
};

enum SamplerStatus {
    UNIT_STAND_BY,
    UNIT_SAMPLING,
    UNIT_ERROR,
    UNIT_SAMPLE_FILES_UPLOAD
};

#endif /* SYSTEM_STATUS_H */