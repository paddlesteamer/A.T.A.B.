#include <fstream>

#include "iqrecord.h"
#include "../constants.h"

IQRecord::IQRecord(uint64_t frequency, uint32_t sampleRate, std::string filePath) {
    this->frequency = frequency;
    this->sampleRate = sampleRate;
    this->filePath = filePath;
}

uint64_t IQRecord::getHardwareFrequency() {
    return this->frequency;
}

uint32_t IQRecord::getSampleRate() {
    return this->sampleRate;
}

void IQRecord::run() {
    std::ifstream in(this->filePath, std::ios::binary);

    float i,q;
    std::complex<double> samples[FFT_WINDOW_SIZE];
    int idx=0;
    while (!in.eof()) {
        in.read((char *) &i, sizeof(float));
        in.read((char *) &q, sizeof(float));

        samples[idx] = std::complex<double>(i,q);

        if (idx == FFT_WINDOW_SIZE - 1) {
            this->buffer.append(samples, FFT_WINDOW_SIZE);
            idx = 0;    
        } else {
            idx++;
        }
    }

    in.close();
}