// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include <generated/interfaces/auth_token_provider/Implementation.hpp>

class NfcHandler {
public:
    enum class Protocol {
        UNKNOWN,
        MIFARE,
        ISO_IEC_15693,
    };

    using Callback = std::function<void(char* uid, size_t length, Protocol)>;

    NfcHandler();
    ~NfcHandler();

    bool start(Callback);

private:
    void on_tag_arrival(void* pTagInfo);
    void on_tag_departure();

    Callback callback{nullptr};
};
