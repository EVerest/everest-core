// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_ERROR_MAPPING_HPP
#define OCPP_ERROR_MAPPING_HPP

#include <unordered_map>

#include <ocpp/v16/enums.hpp>

namespace module {

const std::string CHARGE_X_MREC_VENDOR_ID = "https://chargex.inl.gov";

// Error type mappings
const std::unordered_map<std::string, std::pair<ocpp::v16::ChargePointErrorCode, std::string>> MREC_ERROR_MAP = {
    {"connector_lock/MREC1ConnectorLockFailure", {ocpp::v16::ChargePointErrorCode::ConnectorLockFailure, "CX001"}},
    {"evse_board_support/MREC2GroundFailure", {ocpp::v16::ChargePointErrorCode::GroundFailure, "CX002"}},
    {"evse_board_support/MREC3HighTemperature", {ocpp::v16::ChargePointErrorCode::HighTemperature, "CX003"}},
    {"evse_board_support/MREC4OverCurrentFailure", {ocpp::v16::ChargePointErrorCode::OverCurrentFailure, "CX004"}},
    {"evse_board_support/MREC5OverVoltage", {ocpp::v16::ChargePointErrorCode::OverVoltage, "CX005"}},
    {"evse_board_support/MREC6UnderVoltage", {ocpp::v16::ChargePointErrorCode::UnderVoltage, "CX006"}},
    {"evse_board_support/MREC8EmergencyStop", {ocpp::v16::ChargePointErrorCode::OtherError, "CX008"}},
    {"evse_board_support/MREC10InvalidVehicleMode", {ocpp::v16::ChargePointErrorCode::OtherError, "CX010"}},
    {"evse_board_support/MREC14PilotFault", {ocpp::v16::ChargePointErrorCode::OtherError, "CX014"}},
    {"evse_board_support/MREC15PowerLoss", {ocpp::v16::ChargePointErrorCode::OtherError, "CX015"}},
    {"evse_board_support/MREC17EVSEContactorFault", {ocpp::v16::ChargePointErrorCode::OtherError, "CX017"}},
    {"evse_board_support/MREC18CableOverTempDerate", {ocpp::v16::ChargePointErrorCode::OtherError, "CX018"}},
    {"evse_board_support/MREC19CableOverTempStop", {ocpp::v16::ChargePointErrorCode::OtherError, "CX019"}},
    {"evse_board_support/MREC20PartialInsertion", {ocpp::v16::ChargePointErrorCode::OtherError, "CX020"}},
    {"evse_board_support/MREC23ProximityFault", {ocpp::v16::ChargePointErrorCode::OtherError, "CX023"}},
    {"evse_board_support/MREC24ConnectorVoltageHigh", {ocpp::v16::ChargePointErrorCode::OtherError, "CX024"}},
    {"evse_board_support/MREC25BrokenLatch", {ocpp::v16::ChargePointErrorCode::OtherError, "CX025"}},
    {"evse_board_support/MREC26CutCable", {ocpp::v16::ChargePointErrorCode::OtherError, "CX026"}},
    {"evse_manager/MREC4OverCurrentFailure", {ocpp::v16::ChargePointErrorCode::OverCurrentFailure, "CX004"}},
    {"ac_rcd/MREC2GroundFailure", {ocpp::v16::ChargePointErrorCode::GroundFailure, "CX002"}},
};

} // namespace module

#endif
