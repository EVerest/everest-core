// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <csignal>
#include <iomanip>
#include <iostream>
#include <namedPipeDataSource.hpp>
#include <sstream>
#include <thread>

static std::atomic<bool> stopped{false};

std::string nfcidToString(const std::vector<std::uint8_t>& nfcid) {
    std::ostringstream oss;

    for (std::uint16_t byte : nfcid) {
        oss << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << byte;
    }

    return oss.str();
}

int main() {

    std::signal(SIGINT, [](int sig) {
        std::cout << "Caught signal " << sig << " (^C)" << std::endl;
        stopped = true;
    });

    NamedPipeDataSource x("/tmp/myFifo");

    x.setDetectionCallback([](const std::pair<std::string, std::vector<std::uint8_t>>& reply) {
        auto& [protocol, nfcid] = reply;
        std::cout << "Received: " << protocol << ": " << nfcidToString(nfcid) << std::endl;
    });
    x.setErrorLogCallback([](const std::string& str) { std::cout << "ERROR!" << std::endl; });
    x.run();

    while (not stopped) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
