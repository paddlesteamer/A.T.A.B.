#include <boost/program_options.hpp>
#include <exception>
#include <iostream>

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
}