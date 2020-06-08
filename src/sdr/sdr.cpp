#include <thread>

#include "sdr.h"
#include "bladeRF.h"
#include "iqrecord.h"
#include "../constants.h"

void SDR::poll(std::complex<double> *samples) {
    this->buffer.pop(samples, FFT_WINDOW_SIZE);
}

void SDR::start() {
    this->sdrTh = std::thread(&SDR::run, this);
    this->sdrTh.detach();
}

SDR* SDRFactory::newDevice(std::string source, uint64_t frequency, uint32_t sampleRate) {
    if (!source.compare("bladeRF")) {
        return new BladeRF(frequency, sampleRate);
    }

    return new IQRecord(frequency, sampleRate, source);
}