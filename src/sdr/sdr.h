#ifndef _SDR_H_
#define _SDR_H_

#include <string>
#include <thread>

#include "../utils.h"

class SDR {

    public:
        SDR() {};
        // abstract methods
        virtual uint64_t getHardwareFrequency() = 0;
        virtual uint32_t getSampleRate() = 0;

        // fills array with samples of size FFT_WINDOW_SIZE (constants.h) 
        void poll(std::complex<double> *samples); 
        void start();


    protected:
        // abstract methods
        virtual void run() = 0;

        Buffer buffer;
    
    private:
        std::thread sdrTh;

};

class SDRFactory {
    public:
        static SDR* newDevice(std::string source, uint64_t frequency, uint32_t sampleRate);
};

#endif