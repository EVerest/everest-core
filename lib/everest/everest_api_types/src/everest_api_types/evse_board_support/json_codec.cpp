// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/json_codec.hpp"
#include "evse_board_support/API.hpp"
#include "evse_board_support/codec.hpp"
#include "nlohmann/json.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::evse_board_support {

void to_json(json& j, Event const& k) noexcept {
    switch (k) {
    case Event::A:
        j = "A";
        return;
    case Event::B:
        j = "B";
        return;
    case Event::C:
        j = "C";
        return;
    case Event::D:
        j = "D";
        return;
    case Event::E:
        j = "E";
        return;
    case Event::F:
        j = "F";
        return;
    case Event::PowerOn:
        j = "PowerOn";
        return;
    case Event::PowerOff:
        j = "PowerOff";
        return;
    case Event::EvseReplugStarted:
        j = "EvseReplugStarted";
        return;
    case Event::EvseReplugFinished:
        j = "EvseReplugFinished";
        return;
    case Event::Disconnected:
        j = "Disconnected";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_board_support::Event";
}

void from_json(json const& j, Event& k) {
    std::string s = j;
    if (s == "A") {
        k = Event::A;
        return;
    }
    if (s == "B") {
        k = Event::B;
        return;
    }
    if (s == "C") {
        k = Event::C;
        return;
    }
    if (s == "D") {
        k = Event::D;
        return;
    }
    if (s == "E") {
        k = Event::E;
        return;
    }
    if (s == "F") {
        k = Event::F;
        return;
    }
    if (s == "PowerOn") {
        k = Event::PowerOn;
        return;
    }
    if (s == "PowerOff") {
        k = Event::PowerOff;
        return;
    }
    if (s == "EvseReplugStarted") {
        k = Event::EvseReplugStarted;
        return;
    }
    if (s == "EvseReplugFinished") {
        k = Event::EvseReplugFinished;
        return;
    }
    if (s == "Disconnected") {
        k = Event::Disconnected;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::evse_board_support::Event");
}

void to_json(json& j, BspEvent const& k) noexcept {
    j = json{
        {"event", k.event},
    };
}

void from_json(const json& j, BspEvent& k) {
    k.event = j.at("event");
}

void to_json(json& j, ErrorEnum const& k) noexcept {
    switch (k) {
    case ErrorEnum::DiodeFault:
        j = "DiodeFault";
        return;
    case ErrorEnum::VentilationNotAvailable:
        j = "VentilationNotAvailable";
        return;
    case ErrorEnum::BrownOut:
        j = "BrownOut";
        return;
    case ErrorEnum::EnergyManagement:
        j = "EnergyManagement";
        return;
    case ErrorEnum::PermanentFault:
        j = "PermanentFault";
        return;
    case ErrorEnum::MREC2GroundFailure:
        j = "MREC2GroundFailure";
        return;
    case ErrorEnum::MREC3HighTemperature:
        j = "MREC3HighTemperature";
        return;
    case ErrorEnum::MREC4OverCurrentFailure:
        j = "MREC4OverCurrentFailure";
        return;
    case ErrorEnum::MREC5OverVoltage:
        j = "MREC5OverVoltage";
        return;
    case ErrorEnum::MREC6UnderVoltage:
        j = "MREC6UnderVoltage";
        return;
    case ErrorEnum::MREC8EmergencyStop:
        j = "MREC8EmergencyStop";
        return;
    case ErrorEnum::MREC10InvalidVehicleMode:
        j = "MREC10InvalidVehicleMode";
        return;
    case ErrorEnum::MREC14PilotFault:
        j = "MREC14PilotFault";
        return;
    case ErrorEnum::MREC15PowerLoss:
        j = "MREC15PowerLoss";
        return;
    case ErrorEnum::MREC17EVSEContactorFault:
        j = "MREC17EVSEContactorFault";
        return;
    case ErrorEnum::MREC18CableOverTempDerate:
        j = "MREC18CableOverTempDerate";
        return;
    case ErrorEnum::MREC19CableOverTempStop:
        j = "MREC19CableOverTempStop";
        return;
    case ErrorEnum::MREC20PartialInsertion:
        j = "MREC20PartialInsertion";
        return;
    case ErrorEnum::MREC23ProximityFault:
        j = "MREC23ProximityFault";
        return;
    case ErrorEnum::MREC24ConnectorVoltageHigh:
        j = "MREC24ConnectorVoltageHigh";
        return;
    case ErrorEnum::MREC25BrokenLatch:
        j = "MREC25BrokenLatch";
        return;
    case ErrorEnum::MREC26CutCable:
        j = "MREC26CutCable";
        return;
    case ErrorEnum::ConnectorLockCapNotCharged:
        j = "ConnectorLockCapNotCharged";
        return;
    case ErrorEnum::ConnectorLockUnexpectedOpen:
        j = "ConnectorLockUnexpectedOpen";
        return;
    case ErrorEnum::ConnectorLockUnexpectedClose:
        j = "ConnectorLockUnexpectedClose";
        return;
    case ErrorEnum::ConnectorLockFailedLock:
        j = "ConnectorLockFailedLock";
        return;
    case ErrorEnum::ConnectorLockFailedUnlock:
        j = "ConnectorLockFailedUnlock";
        return;
    case ErrorEnum::MREC1ConnectorLockFailure:
        j = "MREC1ConnectorLockFailure";
        return;
    case ErrorEnum::Selftest:
        j = "Selftest";
        return;
    case ErrorEnum::DC:
        j = "DC";
        return;
    case ErrorEnum::AC:
        j = "AC";
        return;
    case ErrorEnum::TiltDetected:
        j = "TiltDetected";
        return;
    case ErrorEnum::WaterIngressDetected:
        j = "WaterIngressDetected";
        return;
    case ErrorEnum::EnclosureOpen:
        j = "EnclosureOpen";
        return;
    case ErrorEnum::VendorError:
        j = "VendorError";
        return;
    case ErrorEnum::VendorWarning:
        j = "VendorWarning";
        return;
    case ErrorEnum::CommunicationFault:
        j = "CommunicationFault";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_board_support::ErrorEnum";
}

void from_json(json const& j, ErrorEnum& k) {
    std::string s = j;
    if (s == "DiodeFault") {
        k = ErrorEnum::DiodeFault;
        return;
    }
    if (s == "VentilationNotAvailable") {
        k = ErrorEnum::VentilationNotAvailable;
        return;
    }
    if (s == "BrownOut") {
        k = ErrorEnum::BrownOut;
        return;
    }
    if (s == "EnergyManagement") {
        k = ErrorEnum::EnergyManagement;
        return;
    }
    if (s == "PermanentFault") {
        k = ErrorEnum::PermanentFault;
        return;
    }
    if (s == "MREC2GroundFailure") {
        k = ErrorEnum::MREC2GroundFailure;
        return;
    }
    if (s == "MREC3HighTemperature") {
        k = ErrorEnum::MREC3HighTemperature;
        return;
    }
    if (s == "MREC4OverCurrentFailure") {
        k = ErrorEnum::MREC4OverCurrentFailure;
        return;
    }
    if (s == "MREC5OverVoltage") {
        k = ErrorEnum::MREC5OverVoltage;
        return;
    }
    if (s == "MREC6UnderVoltage") {
        k = ErrorEnum::MREC6UnderVoltage;
        return;
    }
    if (s == "MREC8EmergencyStop") {
        k = ErrorEnum::MREC8EmergencyStop;
        return;
    }
    if (s == "MREC10InvalidVehicleMode") {
        k = ErrorEnum::MREC10InvalidVehicleMode;
        return;
    }
    if (s == "MREC14PilotFault") {
        k = ErrorEnum::MREC14PilotFault;
        return;
    }
    if (s == "MREC15PowerLoss") {
        k = ErrorEnum::MREC15PowerLoss;
        return;
    }
    if (s == "MREC17EVSEContactorFault") {
        k = ErrorEnum::MREC17EVSEContactorFault;
        return;
    }
    if (s == "MREC18CableOverTempDerate") {
        k = ErrorEnum::MREC18CableOverTempDerate;
        return;
    }
    if (s == "MREC19CableOverTempStop") {
        k = ErrorEnum::MREC19CableOverTempStop;
        return;
    }
    if (s == "MREC20PartialInsertion") {
        k = ErrorEnum::MREC20PartialInsertion;
        return;
    }
    if (s == "MREC23ProximityFault") {
        k = ErrorEnum::MREC23ProximityFault;
        return;
    }
    if (s == "MREC24ConnectorVoltageHigh") {
        k = ErrorEnum::MREC24ConnectorVoltageHigh;
        return;
    }
    if (s == "MREC25BrokenLatch") {
        k = ErrorEnum::MREC25BrokenLatch;
        return;
    }
    if (s == "MREC26CutCable") {
        k = ErrorEnum::MREC26CutCable;
        return;
    }
    if (s == "ConnectorLockCapNotCharged") {
        k = ErrorEnum::ConnectorLockCapNotCharged;
        return;
    }
    if (s == "ConnectorLockUnexpectedOpen") {
        k = ErrorEnum::ConnectorLockUnexpectedOpen;
        return;
    }
    if (s == "ConnectorLockUnexpectedClose") {
        k = ErrorEnum::ConnectorLockUnexpectedClose;
        return;
    }
    if (s == "ConnectorLockFailedLock") {
        k = ErrorEnum::ConnectorLockFailedLock;
        return;
    }
    if (s == "ConnectorLockFailedUnlock") {
        k = ErrorEnum::ConnectorLockFailedUnlock;
        return;
    }
    if (s == "MREC1ConnectorLockFailure") {
        k = ErrorEnum::MREC1ConnectorLockFailure;
        return;
    }
    if (s == "Selftest") {
        k = ErrorEnum::Selftest;
        return;
    }
    if (s == "DC") {
        k = ErrorEnum::DC;
        return;
    }
    if (s == "AC") {
        k = ErrorEnum::AC;
        return;
    }
    if (s == "TiltDetected") {
        k = ErrorEnum::TiltDetected;
        return;
    }
    if (s == "WaterIngressDetected") {
        k = ErrorEnum::WaterIngressDetected;
        return;
    }
    if (s == "EnclosureOpen") {
        k = ErrorEnum::EnclosureOpen;
        return;
    }
    if (s == "VendorError") {
        k = ErrorEnum::VendorError;
        return;
    }
    if (s == "VendorWarning") {
        k = ErrorEnum::VendorWarning;
        return;
    }
    if (s == "CommunicationFault") {
        k = ErrorEnum::CommunicationFault;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::evse_board_support::ErrorEnum");
}

void to_json(json& j, const Error& k) noexcept {
    j = json{
        {"type", k.type},
    };
    if (k.sub_type) {
        j["sub_type"] = k.sub_type.value();
    }
    if (k.message) {
        j["message"] = k.message.value();
    };
}

void from_json(const json& j, Error& k) {
    k.type = j.at("type");
    if (j.contains("sub_type")) {
        k.sub_type.emplace(j.at("sub_type"));
    }
    if (j.contains("message")) {
        k.message.emplace(j.at("message"));
    }
}

void to_json(json& j, Connector_type const& k) noexcept {
    switch (k) {
    case Connector_type::IEC62196Type2Cable:
        j = "IEC62196Type2Cable";
        return;
    case Connector_type::IEC62196Type2Socket:
        j = "IEC62196Type2Socket";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_board_support::Connector_type";
}

void from_json(json const& j, Connector_type& k) {
    std::string s = j;
    if (s == "IEC62196Type2Cable") {
        k = Connector_type::IEC62196Type2Cable;
        return;
    }
    if (s == "IEC62196Type2Socket") {
        k = Connector_type::IEC62196Type2Socket;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::evse_board_support::Connector_type");
}

void to_json(json& j, HardwareCapabilities const& k) noexcept {
    j = json{
        {"max_current_A_import", k.max_current_A_import},
        {"min_current_A_import", k.min_current_A_import},
        {"max_phase_count_import", k.max_phase_count_import},
        {"min_phase_count_import", k.min_phase_count_import},
        {"max_current_A_export", k.max_current_A_export},
        {"min_current_A_export", k.min_current_A_export},
        {"max_phase_count_export", k.max_phase_count_export},
        {"min_phase_count_export", k.min_phase_count_export},
        {"supports_changing_phases_during_charging", k.supports_changing_phases_during_charging},
        {"connector_type", k.connector_type},
    };
    if (k.max_plug_temperature_C) {
        j["max_plug_temperature_C"] = k.max_plug_temperature_C.value();
    }
}

void from_json(json const& j, HardwareCapabilities& k) {
    k.max_current_A_import = j.at("max_current_A_import");
    k.min_current_A_import = j.at("min_current_A_import");
    k.max_phase_count_import = j.at("max_phase_count_import");
    k.min_phase_count_import = j.at("min_phase_count_import");
    k.max_current_A_export = j.at("max_current_A_export");
    k.min_current_A_export = j.at("min_current_A_export");
    k.max_phase_count_export = j.at("max_phase_count_export");
    k.min_phase_count_export = j.at("min_phase_count_export");
    k.supports_changing_phases_during_charging = j.at("supports_changing_phases_during_charging");
    k.connector_type = j.at("connector_type");

    if (j.contains("max_plug_temperature_C")) {
        k.max_plug_temperature_C.emplace(j.at("max_plug_temperature_C"));
    }
}

void to_json(json& j, Reason const& k) noexcept {
    switch (k) {
    case Reason::DCCableCheck:
        j = "DCCableCheck";
        return;
    case Reason::DCPreCharge:
        j = "DCPreCharge";
        return;
    case Reason::FullPowerCharging:
        j = "FullPowerCharging";
        return;
    case Reason::PowerOff:
        j = "PowerOff";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_board_support::Reason";
}

void from_json(json const& j, Reason& k) {
    std::string s = j;
    if (s == "DCCableCheck") {
        k = Reason::DCCableCheck;
        return;
    }
    if (s == "DCPreCharge") {
        k = Reason::DCPreCharge;
        return;
    }
    if (s == "FullPowerCharging") {
        k = Reason::FullPowerCharging;
        return;
    }
    if (s == "PowerOff") {
        k = Reason::PowerOff;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::evse_board_support::Reason");
}

void to_json(json& j, PowerOnOff const& k) noexcept {
    j = json{
        {"allow_power_on", k.allow_power_on},
        {"reason", k.reason},
    };
}

void from_json(json const& j, PowerOnOff& k) {
    k.allow_power_on = j.at("allow_power_on");
    k.reason = j.at("reason");
}

void to_json(json& j, Ampacity const& k) noexcept {
    switch (k) {
    case Ampacity::None:
        j = "None";
        return;
    case Ampacity::A_13:
        j = "A_13";
        return;
    case Ampacity::A_20:
        j = "A_20";
        return;
    case Ampacity::A_32:
        j = "A_32";
        return;
    case Ampacity::A_63_3ph_70_1ph:
        j = "A_63_3ph_70_1ph";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_board_support::Ampacity";
}

void from_json(json const& j, Ampacity& k) {
    std::string s = j;
    if (s == "None") {
        k = Ampacity::None;
        return;
    }
    if (s == "A_13") {
        k = Ampacity::A_13;
        return;
    }
    if (s == "A_20") {
        k = Ampacity::A_20;
        return;
    }
    if (s == "A_32") {
        k = Ampacity::A_32;
        return;
    }
    if (s == "A_63_3ph_70_1ph") {
        k = Ampacity::A_63_3ph_70_1ph;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::evse_board_support::Ampacity");
}

void to_json(json& j, ProximityPilot const& k) noexcept {
    j = json{
        {"ampacity", k.ampacity},
    };
}

void from_json(json const& j, ProximityPilot& k) {
    k.ampacity = j.at("ampacity");
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
