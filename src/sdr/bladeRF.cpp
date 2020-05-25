#include <libbladeRF.h>

#include "bladeRF.h"
#include "sdr.h"

BladeRF::BladeRF(uint64_t frequency) {
    int status = bladerf_open(&(this->device), "");

    
}

void BladeRF::run() {

}

uint64_t BladeRF::getHardwareFrequency() {
    return 0;
}

uint32_t BladeRF:: getSampleRate() {
    return 0;
}
