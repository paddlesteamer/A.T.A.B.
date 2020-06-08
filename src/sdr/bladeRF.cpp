#include <libbladeRF.h>

#include "bladeRF.h"
#include "sdr.h"

// BladeRF implementations
BladeRF::BladeRF(uint64_t frequency, uint32_t sampleRate) {
    int status = bladerf_open(&(this->device), "");
    if (status != 0) {
        throw BladeRFException(status);
    }

    status = bladerf_set_frequency(this->device, BladeRF::CHANNEL, frequency);
    if (status != 0) {
        throw BladeRFException(status);
    }

    status = bladerf_set_bandwidth(this->device, BladeRF::CHANNEL, BladeRF::BANDWIDTH, NULL);
    if (status != 0) {
        throw BladeRFException(status);
    }

    status = bladerf_set_sample_rate(this->device, BladeRF::CHANNEL, sampleRate, NULL);
    if (status != 0) {
        throw BladeRFException(status);
    }

    status = bladerf_set_gain(this->device, BladeRF::CHANNEL, BladeRF::GAIN);
    if (status != 0) {
        throw BladeRFException(status);
    }

    status = bladerf_sync_config(this->device, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11,
        BladeRF::NUM_BUFFERS, BladeRF::BUFFER_SIZE, BladeRF::NUM_TRANSFERS, BladeRF::STREAM_TIMEOUT);
    if (status != 0) {
        throw BladeRFException(status);
    }
}

void BladeRF::run() {
    // TODO: add error handling
    bladerf_enable_module(this->device, BladeRF::CHANNEL, true);

    // TODO:
    // - check if this is fast enough (maybe create another thread to read samples?)
    // - test with different configuration and buffer sizes 
    // - add exit condition
    int16_t rawSamples[2 * BladeRF::BUFFER_SIZE];
    std::complex<double> samples[BladeRF::BUFFER_SIZE];
    while (true) {
        // TODO: choose a reasonable timeout and move it to class constants
        bladerf_sync_rx(this->device, rawSamples, BladeRF::BUFFER_SIZE, NULL, 50);

        for (int j = 0; j < 2 * BladeRF::BUFFER_SIZE; j += 2) {
            double i = ((double) rawSamples[j]) / BladeRF::MAX_SIGNAL_VALUE; 
            double q = ((double) rawSamples[j+1]) / BladeRF::MAX_SIGNAL_VALUE; 

            samples[j/2] = std::complex<double>(i, q);
        }

        this->buffer.append(samples, BladeRF::BUFFER_SIZE);
    }
}

uint64_t BladeRF::getHardwareFrequency() {
    uint64_t frequency = 0;

    int status = bladerf_get_frequency(this->device, BladeRF::CHANNEL, &frequency);
    if (status != 0) {
        throw BladeRFException(status);
    }

    return frequency;
}

uint32_t BladeRF:: getSampleRate() {
    uint32_t samplerate = 0;

    int status = bladerf_get_sample_rate(this->device, BladeRF::CHANNEL, &samplerate);
    if (status != 0) {
        throw BladeRFException(status);
    }

    return samplerate;
}

// BladeRFException implementations
BladeRFException::BladeRFException(int status): status(status) {}

const char* BladeRFException::what() const throw() {
    return bladerf_strerror(this->status);
}