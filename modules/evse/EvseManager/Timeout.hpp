// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TIMEOUT_HPP
#define TIMEOUT_HPP

#include <chrono>
using namespace std::chrono;

/*
 Simple helper class for a timeout
*/

class Timeout {
public:
    explicit Timeout(milliseconds _t) : t(_t), start(steady_clock::now()) {
    }

    bool reached() {
        if ((steady_clock::now() - start) > t)
            return true;
        else
            return false;
    }

private:
    milliseconds t;
    time_point<steady_clock> start;
};

#endif
