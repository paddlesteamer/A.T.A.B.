#ifndef _IQRECORD_H_
#define _IQRECORD_H_

#include <string>

#include "sdr.h"

class IQRecord: public SDR {
    public:
        IQRecord(uint64_t frequency, uint32_t sampleRate, std::string filePath);

        uint64_t getHardwareFrequency();
        uint32_t getSampleRate();

    protected:
        void run();

    private:
        uint64_t frequency;
        uint32_t sampleRate;

        std::string filePath;
};

#endif 