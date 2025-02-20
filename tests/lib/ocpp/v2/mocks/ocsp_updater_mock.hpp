// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include "gmock/gmock.h"

#include <ocpp/v201/ocsp_updater.hpp>

namespace ocpp::v201 {
class OcspUpdaterMock : public OcspUpdaterInterface {
public:
    virtual ~OcspUpdaterMock() {
    }
    MOCK_METHOD(void, start, ());
    MOCK_METHOD(void, stop, ());
    MOCK_METHOD(void, trigger_ocsp_cache_update, ());
};
} // namespace ocpp::v201
