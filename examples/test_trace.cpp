// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <iostream>

class TraceTest {
public:
    static void do_trace(bool) {
        std::cout << Everest::Logging::trace();
    }
};

void bax() {
    std::cout << Everest::Logging::trace();
}

bool baz(const char* str) {
    auto lambda = [&str](int& i) {
        i++;

        bax();
    };

    int test = 41;

    lambda(test);
    return true;
}

extern "C" {
void bar(int) {
    baz("Please trace ...");
}
}

void foo() {
    bar(85);
}

int main(int argc, char* argv[]) {
    TraceTest::do_trace(true);

    foo();

    return 0;
}
