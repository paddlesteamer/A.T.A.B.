#ifndef _BLADERF_H_
#define _BLADERF_H_

#include <exception>
#include <libbladeRF.h>

#include "sdr.h"

class BladeRF: public SDR {
    public:
        BladeRF(uint64_t frequency, uint32_t sampleRate);
        
        uint64_t getHardwareFrequency();
        uint32_t getSampleRate();

    protected:
        void run();
    
    private:
        static const bladerf_bandwidth BANDWIDTH    = 1000000; // 1M 
        static const bladerf_channel CHANNEL        = BLADERF_CHANNEL_RX(0); // RX0
        static const bladerf_gain GAIN              = 0; // no gain

        static const uint NUM_BUFFERS    = 16;
        static const uint BUFFER_SIZE    = 8192;
        static const uint NUM_TRANSFERS  = 8;
        static const uint STREAM_TIMEOUT = 1000;

        static constexpr double MAX_SIGNAL_VALUE = 2048.0;

        struct bladerf *device;
        struct bladerf_sample_rate;
};

class BladeRFException: public std::exception {
    public:
        BladeRFException(int status);
        
        virtual const char* what() const throw();
    
    private:
        int status;
};

#endif