#include <everest/logging.hpp>

#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest::log example");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("logconf", po::value<std::string>(), "The path to a custom logging.ini");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    // initialize logging as early as possible
    std::string logging_config = "logging.ini";
    if (vm.count("logconf") != 0) {
        logging_config = vm["logconf"].as<std::string>();
    }
    Everest::Logging::init(logging_config, "hello there");

    EVLOG(debug) << "logging_config was set to " << logging_config;

    EVLOG(debug) << "This is a DEBUG message.";
    EVLOG(info) << "This is a INFO message.";
    EVLOG(warning) << "This is a WARNING message.";
    EVLOG(error) << "This is a ERROR message.";
    EVLOG(critical) << "This is a CRITICAL message.";

    return 0;
}
