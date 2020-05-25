#include <thread>

#include "sdr.h"
#include "../constants.h"

void SDR::poll(std::complex<float> *samples) {
    this->buffer.pop(samples, FFT_WINDOW_SIZE);
}

void SDR::start() {
    this->sdrTh = std::thread(&SDR::run, this);
}