// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <utils/status_fifo.hpp>

#include <stdexcept>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace Everest {

StatusFifo StatusFifo::create_from_path(const std::string& fifo_path) {
    if (fifo_path.length() == 0) {
        return StatusFifo();
    }

    // try to open the file
    auto fd = open(fifo_path.c_str(), O_WRONLY | O_NONBLOCK);

    // note: for fifo files, opening for write only in non-blocking mode will fail with ENXIO if the other end hasn't
    // opened the file yet
    if (fd == -1) {
        if (errno == ENXIO) {
            auto msg = std::string("Failed to open status fifo at ") + fifo_path + " (fifo not opened for read?)";
            throw std::runtime_error(msg);
        } else {
            auto msg =
                std::string("Failed to open status fifo at ") + fifo_path + " (fifo file not created with mkfifo?)";
            throw std::runtime_error(msg);
        }
    }

    return StatusFifo(fd);
}

void StatusFifo::update(const std::string& message) {
    if (disabled) {
        return;
    }

    const auto ret = write(fd, message.c_str(), message.length());
    if (ret == -1) {
        // NOTE (aw): if we fail to write, we might assume, that the reader of the fifo is not interested in us anymore
        // so we won't send any further messages
        disabled = true;
    }
}

StatusFifo::~StatusFifo() {
    if (opened) {
        close(fd);
    }
}
} // namespace Everest
