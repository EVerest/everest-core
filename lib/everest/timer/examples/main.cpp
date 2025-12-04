// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <iostream>
#include <thread>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <date/date.h>

#include <everest/timer.hpp>

namespace po = boost::program_options;

using date::operator<<;
using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest::time example");
    desc.add_options()("help,h", "produce help message");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    boost::asio::io_service io_service;

    std::cout << "start time: " << date::utc_clock::now() << std::endl;

    int count_t1 = 0;
    int count_t2 = 0;
    Everest::SteadyTimer* t0 = new Everest::SteadyTimer(&io_service);
    t0->timeout([]() { std::cout << "Goodbye after 25s" << std::endl; }, 25s);
    Everest::SteadyTimer* t1 = new Everest::SteadyTimer(&io_service);
    t1->at(
        [&]() {
            std::cout << "t1 (asio) after 5s: " << date::utc_clock::now() << std::endl;
            t1->interval(
                [&]() {
                    std::cout << "t1 (asio) interval (1s): " << date::utc_clock::now() << std::endl;
                    count_t1++;
                    if (count_t1 > 3) {
                        t1->timeout(
                            []() { std::cout << "t1 (asio) timeout (3s): " << date::utc_clock::now() << std::endl; },
                            3s);
                    }
                },
                1s);
        },
        date::utc_clock::now() + 5s);

    Everest::SteadyTimer* t2 = new Everest::SteadyTimer();
    t2->at(
        [&]() {
            std::cout << "t2 (thread) after 12s: " << date::utc_clock::now() << std::endl;
            t2->interval(
                [&]() {
                    std::cout << "t2 (thread) interval (1s): " << date::utc_clock::now() << std::endl;
                    count_t2++;
                    if (count_t2 > 3) {
                        t2->timeout(
                            []() { std::cout << "t2 (thread) timeout (3s): " << date::utc_clock::now() << std::endl; },
                            3s);
                    }
                },
                1s);
        },
        date::utc_clock::now() + 12s);

    io_service.run();

    return 0;
}
