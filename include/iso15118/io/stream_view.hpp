// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <cstdint>

namespace iso15118::io {

struct StreamInputView {
    uint8_t const* payload;
    size_t payload_len;
};

struct StreamOutputView {
    uint8_t* payload;
    size_t payload_len;

    operator StreamInputView() const {
        return {payload, payload_len};
    }
};

} // namespace iso15118::io
