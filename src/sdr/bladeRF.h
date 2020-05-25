#ifndef _BLADERF_H_
#define _BLADERF_H_

#include <libbladeRF.h>

#include "sdr.h"

class BladeRF: public SDR {
    public:
        BladeRF(uint64_t frequency);

        
        uint64_t getHardwareFrequency();
        uint32_t getSampleRate();

    protected:
        void run();
    
    private:
        static const bladerf_bandwidth BANDWIDTH    = 1000000; // 1M 
        static const bladerf_sample_rate SAMPLERATE = 5000000; // 5M
        static const bladerf_gain GAIN              = 0; // no gain

        struct bladerf *device;

};

#endif