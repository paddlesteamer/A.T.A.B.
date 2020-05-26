#ifndef _UTILS_H_
#define _UTILS_H_

#include <complex>
#include <mutex>
#include <vector>

class Buffer {
    public:
        void append(std::complex<double> *in, uint32_t size);
        void pop(std::complex<double> *out, uint32_t size);

    private:
        std::mutex mtx;
        std::vector< std::complex<double> > buffer;
        
};

#endif