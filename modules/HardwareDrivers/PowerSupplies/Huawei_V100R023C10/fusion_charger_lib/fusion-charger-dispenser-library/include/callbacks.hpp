// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <fusion_charger/modbus/registers/connector.hpp>

using ElectronicLockStatus =
    fusion_charger::modbus_driver::raw_registers::CollectedConnectorRegisters::ElectronicLockStatus;
using ContactorStatus = fusion_charger::modbus_driver::raw_registers::CollectedConnectorRegisters::ContactorStatus;

struct ConnectorCallbacks {
    std::function<float()> connector_upstream_voltage;
    std::function<float()> output_voltage;
    std::function<float()> output_current;
    std::function<ContactorStatus()> contactor_status;
    std::function<ElectronicLockStatus()> electronic_lock_status;
};
