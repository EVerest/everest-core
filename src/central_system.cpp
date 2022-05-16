// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <iostream>
#include <sys/prctl.h>
#include <thread>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <ocpp1_6/BootNotification.hpp>
#include <ocpp1_6/schemas.hpp>
#include <ocpp1_6/types.hpp>

#include <central_system.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("OCPP central system");

    desc.add_options()("help,h", "produce help message");
    desc.add_options()("maindir", po::value<std::string>(), "set main dir in which the schemas folder resides");
    desc.add_options()("ip", po::value<std::string>(), "ip address on which the websocket should listen on");
    desc.add_options()("port", po::value<std::string>(), "port on which the websocket should listen on");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string maindir = ".";
    if (vm.count("maindir") != 0) {
        maindir = vm["maindir"].as<std::string>();
    }

    std::string ip = "0.0.0.0";
    if (vm.count("ip") != 0) {
        ip = vm["ip"].as<std::string>();
    }

    std::string port = "8428";
    if (vm.count("port") != 0) {
        port = vm["port"].as<std::string>();
    }

    ocpp1_6::Schemas schemas = ocpp1_6::Schemas(maindir);

    CentralSystem central_system = CentralSystem(ip, port, schemas);

    return 0;
}
