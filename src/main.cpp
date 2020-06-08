#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <fftw3.h>

#include "constants.h"
#include "sdr/sdr.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    uint64_t frequency; 
    uint32_t samplerate;
    std::string source;

    try {
        po::options_description desc("Options");
        desc.add_options()
            ("source,s", po::value<std::string>(), "Signal source bladeRF or GQRX raw file")
            ("frequency,f", po::value<uint64_t>(), "Radio frequency to listen")
            ("samplerate,r", po::value<uint32_t>(), "Sample rate");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("source") == 0) {
            std::cerr << "[-] Source must be set\n";
            return -1;
        }
        source = vm["source"].as<std::string>();

        if (vm.count("frequency") == 0) {
            std::cerr << "[-] Frequency must be set\n";
            return -1;
        }
        frequency = vm["frequency"].as<uint64_t>();

        if (vm.count("samplerate") == 0) {
            std::cerr << "[-] Sample rate must be set\n";
            return -1;
        }
        samplerate = vm["samplerate"].as<uint32_t>();

    } catch (std::exception& e) {
        std::cerr << "[-] Error in command line arguments: " << e.what() << "\n";
        return -1;
    }

    std::cout << "[+] Initializing device with frequency " << frequency
        << " Hz and with sample rate " << samplerate << " Hz";

    SDR *sdr;
    try {
        sdr = SDRFactory::newDevice(source, frequency, samplerate);
    } catch (std::exception& e) {
        std::cerr << "[-] SDR Error: " << e.what() << "\n";
        return -1;
    }

    samplerate = sdr->getSampleRate();
    frequency = sdr->getHardwareFrequency();

    std::cout << "[+] Initializing FFT plan\n";
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    
    p = fftw_plan_dft_1d(FFT_WINDOW_SIZE, in, out, FFTW_FORWARD, FFTW_MEASURE);

    double window[FFT_WINDOW_SIZE]; // Hann window
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        double p = sin(i * M_PI / FFT_WINDOW_SIZE);
        window[i] = p * p;
    }

    std::cout << "[+] Starting with frequency set to " << frequency 
        << "Hz and with sample rate " << samplerate << "Hz...\n";
    sdr->start();

    double norm = 1.0 / ((double) (FFT_WINDOW_SIZE * FFT_WINDOW_SIZE));
    std::complex<double> samples[FFT_WINDOW_SIZE];
    double shiftedFFT[FFT_WINDOW_SIZE];
    while (true) {
        sdr->poll(samples);

        for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
            samples[i] *= window[i];
            memcpy(in+i, samples+i, sizeof(fftw_complex));
        }

        fftw_execute(p);

        double sum = 0.0;
        double sqrsum = 0.0;
        for (int i = 0, idx = 0; i < FFT_WINDOW_SIZE; i++) {
            if (i < FFT_WINDOW_SIZE / 2) {
                idx = FFT_WINDOW_SIZE / 2 + i;
            } else {
                idx = i - FFT_WINDOW_SIZE / 2;
            }
            
            double pwr = norm * (out[idx][0] * out[idx][0] + out[idx][1] * out[idx][1]); 
            double dB = 10.0 * log10(pwr);

            sum += dB;
            sqrsum += dB * dB;

            shiftedFFT[i] = dB;
        }
        double mean = sum / FFT_WINDOW_SIZE;
        double stdev = sqrt(sqrsum / FFT_WINDOW_SIZE - mean * mean);
        double cutoffdB = mean + stdev * PEAK_CUTOFF_SD_COUNT;
        
        shiftedFFT[FFT_WINDOW_SIZE/2] = mean;
        
        bool found    = false;
        double peakdB = -1000.0;
        int peakIdx   = -1;
        for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
            if (shiftedFFT[i] < cutoffdB) {
                if (peakIdx >= 0) {
                    std::cout << "[+] Peak: dB = " << peakdB << ", idx = " << peakIdx << " freq = " 
                        << (frequency - samplerate / 2) + ((uint64_t) (samplerate / FFT_WINDOW_SIZE * peakIdx))
                        << "\n";

                    found = true;
                    peakdB = -1000.0;
                    peakIdx = -1;
                }
            } else if (shiftedFFT[i] > peakdB) {
                peakIdx = i;
                peakdB = shiftedFFT[i];
            }
        }

        if (peakIdx >= 0) {
            std::cout << "[+] Peak: dB = " << peakdB << ", idx = " << peakIdx << " freq = " 
                << (frequency - samplerate / 2) + ((uint64_t) (samplerate / FFT_WINDOW_SIZE * peakIdx))
                << "\n";

            found = true;
        }

        if (found) {
            std::cout << "\n";
        }
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}