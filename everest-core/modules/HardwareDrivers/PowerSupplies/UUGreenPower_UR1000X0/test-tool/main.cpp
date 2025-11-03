// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "UUGreenCanDevice.hpp"
#include <iostream>
#include <unistd.h>

#include <everest/logging.hpp>

int main(int argc, char** argv) {
    UUGreenCanDevice can;

    if (!can.open_device("can0")) {
        return 1;
    }

    EVLOG_debug << "CAN device started.";

    can.switch_on(true);
    can.set_voltage_current(200, 10);

    while (true) {
        usleep(50000);
    }

    can.close_device();
    return 0;
}
