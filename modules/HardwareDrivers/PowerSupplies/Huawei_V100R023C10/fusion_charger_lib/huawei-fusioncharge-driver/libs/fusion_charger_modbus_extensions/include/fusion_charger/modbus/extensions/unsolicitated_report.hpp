// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef FUSION_CHARGER_MODBUS_EXTENSIONS__DUMMY_HPP
#define FUSION_CHARGER_MODBUS_EXTENSIONS__DUMMY_HPP

#include <modbus-server/frames.hpp>

namespace fusion_charger::modbus_extensions {

struct UnsolicitatedReportRequest : public modbus_server::pdu::SpecificPDU {
    // Subtype defs:
    struct Segment {
        std::uint16_t registers_start;
        std::uint16_t registers_count;       // todo: can be derived!
        std::vector<std::uint8_t> registers; //! 2 bytes per register

        std::vector<std::uint8_t> to_vec() const;
    };
    struct Device {
        std::uint16_t location;
        std::vector<Segment> segments;

        std::vector<std::uint8_t> to_vec() const;

        // defragment segments
        void defragment();
    };

    // data members:
    bool response_required;
    std::vector<Device> devices;

    UnsolicitatedReportRequest() = default;

    void from_generic(const modbus_server::pdu::GenericPDU& generic) override;
    modbus_server::pdu::GenericPDU to_generic() const override;

    // defragment segments
    void defragment();
};

struct UnsolicitatedReportResponse : public modbus_server::pdu::SpecificPDU {
    bool success;
    void from_generic(const modbus_server::pdu::GenericPDU& generic) override;
    modbus_server::pdu::GenericPDU to_generic() const override;
};

} // namespace fusion_charger::modbus_extensions

#endif
