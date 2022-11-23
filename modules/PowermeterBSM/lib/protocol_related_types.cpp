// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "protocol_related_types.hpp"

namespace protocol_related_types {

ModbusRegisterAddress ModbusRegisterAddress::operator=(const SunspecDataModelAddress& sma) {
    val = sma.val - 1;
    return *this;
}
SunspecDataModelAddress::SunspecDataModelAddress(const ModbusRegisterAddress& mra) : val(mra.val + 1) {
}

SunspecDataModelAddress& SunspecDataModelAddress::operator=(const ModbusRegisterAddress& mra) {
    val = mra.val + 1;
    return *this;
}

} // namespace protocol_related_types

protocol_related_types::SunspecDataModelAddress operator"" _sma(unsigned long long int v) {

    return protocol_related_types::SunspecDataModelAddress{static_cast<std::uint16_t>(v)};
}

protocol_related_types::ModbusRegisterAddress operator"" _mra(unsigned long long int v) {

    return protocol_related_types::ModbusRegisterAddress{static_cast<std::uint16_t>(v)};
}

protocol_related_types::SunspecDataModelAddress operator+(const protocol_related_types::SunspecDataModelAddress& s0,
                                                          const protocol_related_types::SunspecDataModelAddress& s1) {

    return protocol_related_types::SunspecDataModelAddress(s0.val + s1.val);
}

protocol_related_types::ModbusRegisterAddress operator+(const protocol_related_types::ModbusRegisterAddress& m0,
                                                        const protocol_related_types::ModbusRegisterAddress& m1) {

    return protocol_related_types::ModbusRegisterAddress(m0.val + m1.val);
}

protocol_related_types::ModbusRegisterAddress operator+(const protocol_related_types::ModbusRegisterAddress& m0,
                                                        const protocol_related_types::SunspecDataModelAddress& s1) {

    return protocol_related_types::ModbusRegisterAddress(m0.val + s1.val - 1);
}
