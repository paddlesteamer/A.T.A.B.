#include "utils.h"

void Buffer::append(std::complex<double>* in, uint32_t size) {
    this->mtx.lock();

    this->buffer.insert(this->buffer.end(), in, in + size);

    this->mtx.unlock();
}
        
void Buffer::pop(std::complex<double> *out, uint32_t size) {
    while (true) {
        this->mtx.lock();

        if (this->buffer.size() < size) {
            this->mtx.unlock();
            continue;
        }

        for (uint32_t i = 0; i < size; i++) {
            out[i] = this->buffer.at(i);
        }

        this->buffer.erase(this->buffer.begin(), this->buffer.begin() + size);

        this->mtx.unlock();

        return;
    }
}