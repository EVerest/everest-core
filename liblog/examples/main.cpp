// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest::log example");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("logconf", po::value<std::string>(), "The path to a custom logging.ini");
    desc.add_options()("logconfreinit", po::value<std::string>(), "The path to a custom logging.ini");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    // initialize logging as early as possible
    Everest::Logging::init();
    // since we initialized without any config the following messages should not logged at all:
    EVLOG_verbose << "This is a VERBOSE message.";
    EVLOG_debug << "This is a DEBUG message.";
    EVLOG_info << "This is a INFO message.";
    EVLOG_warning << "This is a WARNING message.";
    EVLOG_error << "This is a ERROR message.";
    EVLOG_critical << "This is a CRITICAL message.";

    std::string logging_config = "logging.ini";
    if (vm.count("logconf") != 0) {
        logging_config = vm["logconf"].as<std::string>();
    }
    Everest::Logging::init(logging_config, "hello there");

    EVLOG_debug << "logging_config was set to " << logging_config;

    EVLOG_verbose << "This is a VERBOSE message.";
    EVLOG_debug << "This is a DEBUG message.";
    EVLOG_info << "This is a INFO message.";
    EVLOG_warning << "This is a WARNING message.";
    EVLOG_error << "This is a ERROR message.";
    EVLOG_critical << "This is a CRITICAL message.";

    std::string logging_config_reinit = "logging_reinit.ini";
    if (vm.count("logconfreinit") != 0) {
        logging_config_reinit = vm["logconfreinit"].as<std::string>();
    }
    Everest::Logging::init(logging_config_reinit, "hello reinit");

    EVLOG_verbose << "This is a VERBOSE message after reinit.";
    EVLOG_debug << "This is a DEBUG message after reinit.";
    EVLOG_info << "This is a INFO message after reinit.";
    EVLOG_warning << "This is a WARNING message after reinit.";
    EVLOG_error << "This is a ERROR message after reinit.";
    EVLOG_critical << "This is a CRITICAL message after reinit.";

    return 0;
}
