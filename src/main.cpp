#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <fftw3.h>

#include "constants.h"
#include "sdr/sdr.h"
#include "sdr/bladeRF.h"
#include "utils.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    uint64_t frequency;   

    try {
        po::options_description desc("Options");
        desc.add_options()
            ("frequency,f", po::value<uint64_t>(), "Radio frequency to listen");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("frequency") == 0) {
            std::cerr << "[-] Frequency must be set\n";
            return -1;
        }

        frequency = vm["frequency"].as<uint64_t>();

    } catch (std::exception& e) {
        std::cerr << "[-] Error in command line arguments: " << e.what() << "\n";
        return -1;
    }

    std::cout << "[+] Frequency: " << frequency << "\n";

    SDR *sdr;
    try {
        sdr = new BladeRF(frequency);

    } catch (std::exception& e) {
        std::cerr << "[-] BladeRF Error: " << e.what() << "\n";
        return -1;
    }

    uint32_t samplerate = sdr->getSampleRate();

    std::cout << "[+] Initializing FFT plan\n";
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    
    p = fftw_plan_dft_1d(FFT_WINDOW_SIZE, in, out, FFTW_FORWARD, FFTW_MEASURE);

    double window[FFT_WINDOW_SIZE];
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        double p = sin(i * M_PI / FFT_WINDOW_SIZE);
        window[i] = p * p;
    }

    double iirFft[FFT_WINDOW_SIZE];
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        iirFft[i] = -140.0;
    }

    std::cout << "[+] Starting...\n";
    sdr->start();

    double cutoff = -15.0;
    double max = -10000.0;
    int maxIdx = 0;
    double pwr_scale = 1.0 / ((double) (FFT_WINDOW_SIZE * FFT_WINDOW_SIZE));
    std::complex<double> samples[FFT_WINDOW_SIZE];
    while (true) {
        sdr->poll(samples);

        for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
            samples[i] *= window[i];
            memcpy(in+i, samples+i, sizeof(fftw_complex));
        }

        fftw_execute(p);

        for (int i = 0, idx = 0; i < FFT_WINDOW_SIZE; i++) {
            if (i < FFT_WINDOW_SIZE / 2) {
                idx = FFT_WINDOW_SIZE / 2 + i;
            } else {
                idx = i - FFT_WINDOW_SIZE / 2;
            }
            
            double pwr = pwr_scale * (out[idx][0] * out[idx][0] + out[idx][1] * out[idx][1]); 
            double dB = 10.0 * log10(pwr + 1.0e-20);

            iirFft[i] += dB - iirFft[i];

            dB = iirFft[i];

            if (dB < cutoff) {
                if (maxIdx > 0) {
                    uint64_t trgfreq = (frequency - samplerate / 2) 
                        + ((uint64_t) ((samplerate / FFT_WINDOW_SIZE) * (maxIdx)));

                    std::cout << "[+] Peak! pwr: " << max << " idx: " << maxIdx 
                        << " freq: " << trgfreq << "\n";

                    max = -10000.0;
                    maxIdx = 0;
                }
            } else if (dB > max) {
                max = dB;
                maxIdx = i;
            }

        }
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}