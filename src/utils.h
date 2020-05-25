#ifndef _UTILS_H_
#define _UTILS_H_

#include <complex>
#include <mutex>
#include <vector>

class Buffer {
    public:
        void append(std::complex<float> *in, uint32_t size);
        void pop(std::complex<float> *out, uint32_t size);

    private:
        std::mutex mtx;
        std::vector< std::complex<float> > buffer;
        
};

#endif