// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is a simple wrapper for accessing GPIOs via the Linux kernel's
 GPIO Character Device Userspace API (v1). This API was introduced in Linux
 kernel version 4.8.
 This wrapper attempts to provide a simple interface to that API, for uses
 in EVerest where simply setting or reading of a single GPIO is required.
*/

#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>

namespace Everest {

struct GpioSettings {
    std::string chip_name;
    int line_number{0};
    bool inverted{false};
};

class Gpio {
public:
    // open GPIO line
    bool open(const std::string& chip_name, int line_number, bool inverted = false);
    bool open(const GpioSettings& settings);

    // free GPIO line
    void close_all();

    // Update the pin value in output mode. Do nothing in input mode.
    void set(bool value);

    // Set output mode with an initial value.
    bool set_output(bool initial_value);

    // Normally true means high, false means low. By setting this to true all outputs and inputs are inverted.
    void invert_pin(bool inverted);

    // Set to input mode.
    bool set_input();

    // Returns current pin value
    bool read();

    // indicated whether GPIO is ready to be used
    bool is_ready();

private:
    int fd{0};
    int line_fd{0};
    int line_number{0};

    // flag to indicate wether GPIO is ready to be used
    bool ready{false};

    // set to true to invert both input and output values
    bool inverted{false};
};
} // namespace Everest

#endif
