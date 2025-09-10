// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/extensions/unsolicitated_registers.hpp>

namespace fusion_charger::modbus_extensions {

std::optional<std::vector<uint8_t>>
unsolicitated_report_helper(modbus::registers::complex_registers::ComplexRegisterUntyped* reg) {
    modbus::registers::data_providers::DataProviderUntyped* prov = reg->get_data_provider();
    if (auto ext = dynamic_cast<DataProviderExtUnsolicitated*>(prov)) {
        if (ext->should_uncolicitated_report()) {
            return reg->on_read();
        }
    }

    return std::nullopt;
}

}; // namespace fusion_charger::modbus_extensions
