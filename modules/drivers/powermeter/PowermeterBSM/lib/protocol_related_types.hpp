// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_BASE_TYPES_HPP
#define POWERMETER_BSM_BASE_TYPES_HPP

#include <cstdint>
#include <vector>

namespace transport {
using DataVector = std::vector<std::uint8_t>;
}

namespace protocol_related_types {

using SunspecDataModelOffset = std::uint16_t;
using SunspecRegisterCount = std::uint16_t;
using SunspecModelId = std::uint16_t;
using ModbusUnitId = std::uint16_t;

struct ModbusRegisterAddress;

struct SunspecDataModelAddress {

    std::uint16_t val;

    explicit SunspecDataModelAddress(const ModbusRegisterAddress&);

    explicit SunspecDataModelAddress(std::uint16_t v) : val(v) {
    }

    SunspecDataModelAddress& operator=(const ModbusRegisterAddress&);
};

struct ModbusRegisterAddress {

    std::uint16_t val;

    explicit ModbusRegisterAddress(const SunspecDataModelAddress& sma) : val(sma.val - 1) {
    }

    explicit ModbusRegisterAddress(std::uint16_t v) : val(v) {
    }

    ModbusRegisterAddress operator=(const SunspecDataModelAddress& sma);
};

} // namespace protocol_related_types

protocol_related_types::SunspecDataModelAddress operator"" _sma(unsigned long long int v);
protocol_related_types::ModbusRegisterAddress operator"" _mra(unsigned long long int v);

protocol_related_types::SunspecDataModelAddress operator+(const protocol_related_types::SunspecDataModelAddress& s0,
                                                          const protocol_related_types::SunspecDataModelAddress& s1);
protocol_related_types::ModbusRegisterAddress operator+(const protocol_related_types::ModbusRegisterAddress& m0,
                                                        const protocol_related_types::ModbusRegisterAddress& m1);
protocol_related_types::ModbusRegisterAddress operator+(const protocol_related_types::ModbusRegisterAddress& m0,
                                                        const protocol_related_types::SunspecDataModelAddress& s1);

#endif // POWERMETER_BSM_BASE_TYPES_HPP
