#include <thread>

#include "sdr.h"
#include "../constants.h"

void SDR::poll(std::complex<double> *samples) {
    this->buffer.pop(samples, FFT_WINDOW_SIZE);
}

void SDR::start() {
    this->sdrTh = std::thread(&SDR::run, this);
}