// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

enum class DispenserPsuCommunicationState {
    UNINITIALIZED = 0,
    INITIALIZING = 1,
    READY = 2,
    FAILED = 3,
};
