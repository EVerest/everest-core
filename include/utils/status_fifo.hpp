// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef STATUS_FIFO_HPP
#define STATUS_FIFO_HPP

#include <string>

namespace Everest {

class StatusFifo {
public:
    // defined messages
    static constexpr auto ALL_MODULES_STARTED = "ALL_MODULES_STARTED\n";
    static constexpr auto WAITING_FOR_STANDALONE_MODULES = "WAITING_FOR_STANDALONE_MODULES\n";

    static StatusFifo create_from_path(const std::string&);
    void update(const std::string&);

    StatusFifo(StatusFifo const&) = delete;
    StatusFifo& operator=(StatusFifo const&) = delete;
    // NOTE (aw): the move constructor could be implementented, but we don't need it for now
    StatusFifo(StatusFifo&&) = delete;
    StatusFifo& operator=(StatusFifo&&) = delete;
    ~StatusFifo();

private:
    StatusFifo() = default;
    explicit StatusFifo(int fd_) : fd(fd_), disabled(false), opened(true){};

    int fd{-1};
    bool disabled{true};
    bool opened{false};
};

} // namespace Everest

#endif // STATUS_FIFO_HPP
