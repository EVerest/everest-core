// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <chrono>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include <nxpnfcfrontend.hpp>

bool stop = false;

void handleSigInt(int sig) {
    std::cout << "Caught signal " << sig << " (Ctrl+C)" << std::endl;
    stop = true;
}

int main(void) {
    std::signal(SIGINT, handleSigInt);

    auto callback = [](const std::pair<std::string, std::vector<std::uint8_t>>& reply) {
        auto& [protocol, nfcid] = reply;
        std::cout << "Detected Card\n  Type : '" << protocol << "'\n";
        std::cout << "  Length: " << nfcid.size() << "\n";
        std::cout << "  UID   : ";
        for (std::uint16_t byte : nfcid) {
            std::cout << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << byte;
        }
        std::cout << std::endl;
    };

    auto error_callback = [](const std::string& error_message) {
        std::cerr << "ERROR: " << error_message << std::endl;
    };

    std::cout << "NFC client:" << std::endl;
    NxpNfcFrontendWrapper::NxpNfcFrontend frontend{};
    frontend.setDetectionCallback(callback);
    frontend.setErrorLogCallback(error_callback);
    std::cout << "Starting the loop" << std::endl;
    frontend.run();
    while (!stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    // destructor of NxpNfcFrontend will stop the internal threads

    return 0;
}
