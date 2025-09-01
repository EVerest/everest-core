// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include <everest/gpio/gpio.hpp>

#include <fcntl.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>

namespace Everest {

bool Gpio::open(const std::string& chip_name, int _line_number, bool _inverted) {
    inverted = _inverted;
    line_number = _line_number;
    ready = false;

    if (chip_name != "") {
        fd = ::open(("/dev/" + chip_name).c_str(), O_RDONLY);
        if (fd > 0) {
            ready = true;
        }
    }

    return ready;
}

bool Gpio::open(const GpioSettings& settings) {
    return open(settings.chip_name, settings.line_number, settings.inverted);
}

void Gpio::close_all() {
    if (ready) {
        close(fd);
        close(line_fd);
    }
}

bool Gpio::set_output(bool initial_value) {
    if (ready) {
        if (inverted) {
            initial_value = !initial_value;
        }

        struct gpiohandle_request rq;
        rq.lineoffsets[0] = line_number;
        rq.lines = 1;
        rq.flags = GPIOHANDLE_REQUEST_OUTPUT;

        int ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
        line_fd = rq.fd;
        if (ret < 0) {
            ready = false;
        }

        set(initial_value);
    }

    return ready;
}

bool Gpio::set_input() {
    if (ready) {
        struct gpiohandle_request rq;
        rq.lineoffsets[0] = line_number;
        rq.lines = 1;
        rq.flags = GPIOHANDLE_REQUEST_INPUT;

        int ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
        line_fd = rq.fd;
        if (ret < 0)
            ready = false;
    }
    return ready;
}

void Gpio::set(bool value) {
    if (ready) {
        if (inverted) {
            value = !value;
        }
        struct gpiohandle_data data;
        data.values[0] = (value ? 1 : 0);
        int ret = ioctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
        if (ret < 0) {
            ready = false;
        }
    }
}

bool Gpio::read() {
    bool value = false;

    if (ready) {
        struct gpiohandle_data data;
        int ret = ioctl(line_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if (ret < 0)
            ready = false;

        value = (data.values[0] == 1);

        if (inverted) {
            value = !value;
        }
    }

    return value;
}

void Gpio::invert_pin(bool _inverted) {
    inverted = _inverted;
}

bool Gpio::is_ready() {
    return ready;
}

} // namespace Everest
