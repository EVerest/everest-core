// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/slac/io.hpp>

#include <stdexcept>
#include <thread>

void SlacIO::init(const std::string& if_name) {
    if (!slac_channel.open(if_name)) {
        throw std::runtime_error(slac_channel.get_error());
    }
}

void SlacIO::run(std::function<InputHandlerFnType> callback) {
    input_handler = callback;

    running = true;

    loop_thread = std::thread(&SlacIO::loop, this);
}

void SlacIO::quit() {
    if (!running) {
        return;
    }

    running = false;

    loop_thread.join();
}

void SlacIO::loop() {

    while (running) {
        if (slac_channel.read(incoming_msg, 10)) {
            input_handler(incoming_msg);
        }
    }
}

void SlacIO::send(slac::messages::HomeplugMessage& msg) {
    // FIXME (aw): handle errors
    slac_channel.write(msg, 1);
}

const uint8_t* SlacIO::get_mac_addr() /* const */ {
    return slac_channel.get_mac_addr();
}
