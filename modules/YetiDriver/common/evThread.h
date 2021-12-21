// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <thread>
#include <future>
#include <chrono>

class evThread {
    public:
        evThread();
        ~evThread();

        bool shouldExit();
        std::thread handle;

    private:
        std::promise<void> exitSignal;
        std::future<void> exitFuture;
};
