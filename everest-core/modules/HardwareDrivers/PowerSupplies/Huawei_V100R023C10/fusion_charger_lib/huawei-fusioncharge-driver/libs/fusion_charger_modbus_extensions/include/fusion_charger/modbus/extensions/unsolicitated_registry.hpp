// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <modbus-registers/registry.hpp>

#include "unsolicitated_registers.hpp"
#include "unsolicitated_report.hpp"

namespace fusion_charger::modbus_extensions {

class UnsolicitatedRegistry : public modbus::registers::registry::ComplexRegisterRegistry {
public:
    UnsolicitatedRegistry() : modbus::registers::registry::ComplexRegisterRegistry() {
    }

    std::optional<fusion_charger::modbus_extensions::UnsolicitatedReportRequest> unsolicitated_report() {
        fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Device req_device;
        req_device.location = 0x0000;

        for (auto& reg : this->get_all_registers()) {
            if (auto res = unsolicitated_report_helper(reg)) {
                fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Segment req_segment;
                req_segment.registers_start = reg->get_start_address();
                req_segment.registers_count = reg->get_size();
                req_segment.registers = *res;

                req_device.segments.push_back(req_segment);
            }
        }

        if (req_device.segments.empty()) {
            return std::nullopt;
        }

        fusion_charger::modbus_extensions::UnsolicitatedReportRequest req;
        req.response_required = false;
        req.devices.push_back(req_device);

        req.defragment();

        return req;
    }
};

}; // namespace fusion_charger::modbus_extensions
