// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_manager/json_codec.hpp"
#include "auth/API.hpp"
#include "auth/codec.hpp"
#include "auth/json_codec.hpp"
#include "evse_manager/API.hpp"
#include "evse_manager/codec.hpp"
#include "nlohmann/json.hpp"
#include "powermeter/API.hpp"
#include "powermeter/codec.hpp"
#include "powermeter/json_codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::evse_manager {

void to_json(json& j, StopTransactionReason const& k) noexcept {
    switch (k) {
    case StopTransactionReason::EmergencyStop:
        j = "EmergencyStop";
        return;
    case StopTransactionReason::EVDisconnected:
        j = "EVDisconnected";
        return;
    case StopTransactionReason::HardReset:
        j = "HardReset";
        return;
    case StopTransactionReason::Local:
        j = "Local";
        return;
    case StopTransactionReason::Other:
        j = "Other";
        return;
    case StopTransactionReason::PowerLoss:
        j = "PowerLoss";
        return;
    case StopTransactionReason::Reboot:
        j = "Reboot";
        return;
    case StopTransactionReason::Remote:
        j = "Remote";
        return;
    case StopTransactionReason::SoftReset:
        j = "SoftReset";
        return;
    case StopTransactionReason::UnlockCommand:
        j = "UnlockCommand";
        return;
    case StopTransactionReason::DeAuthorized:
        j = "DeAuthorized";
        return;
    case StopTransactionReason::EnergyLimitReached:
        j = "EnergyLimitReached";
        return;
    case StopTransactionReason::GroundFault:
        j = "GroundFault";
        return;
    case StopTransactionReason::LocalOutOfCredit:
        j = "LocalOutOfCredit";
        return;
    case StopTransactionReason::MasterPass:
        j = "MasterPass";
        return;
    case StopTransactionReason::OvercurrentFault:
        j = "OvercurrentFault";
        return;
    case StopTransactionReason::PowerQuality:
        j = "PowerQuality";
        return;
    case StopTransactionReason::SOCLimitReached:
        j = "SOCLimitReached";
        return;
    case StopTransactionReason::StoppedByEV:
        j = "StoppedByEV";
        return;
    case StopTransactionReason::TimeLimitReached:
        j = "TimeLimitReached";
        return;
    case StopTransactionReason::Timeout:
        j = "Timeout";
        return;
    case StopTransactionReason::ReqEnergyTransferRejected:
        j = "ReqEnergyTransferRejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::StopTransactionReason";
}

void from_json(json const& j, StopTransactionReason& k) {
    std::string s = j;
    if (s == "EmergencyStop") {
        k = StopTransactionReason::EmergencyStop;
        return;
    }
    if (s == "EVDisconnected") {
        k = StopTransactionReason::EVDisconnected;
        return;
    }
    if (s == "HardReset") {
        k = StopTransactionReason::HardReset;
        return;
    }
    if (s == "Local") {
        k = StopTransactionReason::Local;
        return;
    }
    if (s == "Other") {
        k = StopTransactionReason::Other;
        return;
    }
    if (s == "PowerLoss") {
        k = StopTransactionReason::PowerLoss;
        return;
    }
    if (s == "Reboot") {
        k = StopTransactionReason::Reboot;
        return;
    }
    if (s == "Remote") {
        k = StopTransactionReason::Remote;
        return;
    }
    if (s == "SoftReset") {
        k = StopTransactionReason::SoftReset;
        return;
    }
    if (s == "UnlockCommand") {
        k = StopTransactionReason::UnlockCommand;
        return;
    }
    if (s == "DeAuthorized") {
        k = StopTransactionReason::DeAuthorized;
        return;
    }
    if (s == "EnergyLimitReached") {
        k = StopTransactionReason::EnergyLimitReached;
        return;
    }
    if (s == "GroundFault") {
        k = StopTransactionReason::GroundFault;
        return;
    }
    if (s == "LocalOutOfCredit") {
        k = StopTransactionReason::LocalOutOfCredit;
        return;
    }
    if (s == "MasterPass") {
        k = StopTransactionReason::MasterPass;
        return;
    }
    if (s == "OvercurrentFault") {
        k = StopTransactionReason::OvercurrentFault;
        return;
    }
    if (s == "PowerQuality") {
        k = StopTransactionReason::PowerQuality;
        return;
    }
    if (s == "SOCLimitReached") {
        k = StopTransactionReason::SOCLimitReached;
        return;
    }
    if (s == "StoppedByEV") {
        k = StopTransactionReason::StoppedByEV;
        return;
    }
    if (s == "TimeLimitReached") {
        k = StopTransactionReason::TimeLimitReached;
        return;
    }
    if (s == "Timeout") {
        k = StopTransactionReason::Timeout;
        return;
    }
    if (s == "ReqEnergyTransferRejected") {
        k = StopTransactionReason::ReqEnergyTransferRejected;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_StopTransactionReason");
}

void to_json(json& j, StopTransactionRequest const& k) noexcept {
    j = json{{"reason", k.reason}};
    if (k.id_tag) {
        j["id_tag"] = k.id_tag.value();
    }
}

void from_json(json const& j, StopTransactionRequest& k) {
    k.reason = j.at("reason");
    if (j.contains("id_tag")) {
        k.id_tag.emplace(j.at("id_tag"));
    }
}

void to_json(json& j, StartSessionReason const& k) noexcept {
    switch (k) {
    case StartSessionReason::EVConnected:
        j = "EVConnected";
        return;
    case StartSessionReason::Authorized:
        j = "Authorized";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::StartSessionReason";
}

void from_json(json const& j, StartSessionReason& k) {
    std::string s = j;
    if (s == "EVConnected") {
        k = StartSessionReason::EVConnected;
        return;
    }
    if (s == "Authorized") {
        k = StartSessionReason::Authorized;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_StartSessionReason");
}

void to_json(json& j, SessionEventEnum const& k) noexcept {
    switch (k) {
    case SessionEventEnum::Authorized:
        j = "Authorized";
        return;
    case SessionEventEnum::Deauthorized:
        j = "Deauthorized";
        return;
    case SessionEventEnum::Enabled:
        j = "Enabled";
        return;
    case SessionEventEnum::Disabled:
        j = "Disabled";
        return;
    case SessionEventEnum::SessionStarted:
        j = "SessionStarted";
        return;
    case SessionEventEnum::AuthRequired:
        j = "AuthRequired";
        return;
    case SessionEventEnum::TransactionStarted:
        j = "TransactionStarted";
        return;
    case SessionEventEnum::PrepareCharging:
        j = "PrepareCharging";
        return;
    case SessionEventEnum::ChargingStarted:
        j = "ChargingStarted";
        return;
    case SessionEventEnum::ChargingPausedEV:
        j = "ChargingPausedEV";
        return;
    case SessionEventEnum::ChargingPausedEVSE:
        j = "ChargingPausedEVSE";
        return;
    case SessionEventEnum::WaitingForEnergy:
        j = "WaitingForEnergy";
        return;
    case SessionEventEnum::ChargingResumed:
        j = "ChargingResumed";
        return;
    case SessionEventEnum::StoppingCharging:
        j = "StoppingCharging";
        return;
    case SessionEventEnum::ChargingFinished:
        j = "ChargingFinished";
        return;
    case SessionEventEnum::TransactionFinished:
        j = "TransactionFinished";
        return;
    case SessionEventEnum::SessionFinished:
        j = "SessionFinished";
        return;
    case SessionEventEnum::ReservationStart:
        j = "ReservationStart";
        return;
    case SessionEventEnum::ReservationEnd:
        j = "ReservationEnd";
        return;
    case SessionEventEnum::ReplugStarted:
        j = "ReplugStarted";
        return;
    case SessionEventEnum::ReplugFinished:
        j = "ReplugFinished";
        return;
    case SessionEventEnum::PluginTimeout:
        j = "PluginTimeout";
        return;
    case SessionEventEnum::SwitchingPhases:
        j = "SwitchingPhases";
        return;
    case SessionEventEnum::SessionResumed:
        j = "SessionResumed:";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::SessionEventEnum";
}

void from_json(json const& j, SessionEventEnum& k) {
    std::string s = j;
    if (s == "Authorized") {
        k = SessionEventEnum::Authorized;
        return;
    }
    if (s == "Deauthorized") {
        k = SessionEventEnum::Deauthorized;
        return;
    }
    if (s == "Enabled") {
        k = SessionEventEnum::Enabled;
        return;
    }
    if (s == "Disabled") {
        k = SessionEventEnum::Disabled;
        return;
    }
    if (s == "SessionStarted") {
        k = SessionEventEnum::SessionStarted;
        return;
    }
    if (s == "AuthRequired") {
        k = SessionEventEnum::AuthRequired;
        return;
    }
    if (s == "TransactionStarted") {
        k = SessionEventEnum::TransactionStarted;
        return;
    }
    if (s == "PrepareCharging") {
        k = SessionEventEnum::PrepareCharging;
        return;
    }
    if (s == "ChargingStarted") {
        k = SessionEventEnum::ChargingStarted;
        return;
    }
    if (s == "ChargingPausedEV") {
        k = SessionEventEnum::ChargingPausedEV;
        return;
    }
    if (s == "ChargingPausedEVSE") {
        k = SessionEventEnum::ChargingPausedEVSE;
        return;
    }
    if (s == "WaitingForEnergy") {
        k = SessionEventEnum::WaitingForEnergy;
        return;
    }
    if (s == "ChargingResumed") {
        k = SessionEventEnum::ChargingResumed;
        return;
    }
    if (s == "StoppingCharging") {
        k = SessionEventEnum::StoppingCharging;
        return;
    }
    if (s == "ChargingFinished") {
        k = SessionEventEnum::ChargingFinished;
        return;
    }
    if (s == "TransactionFinished") {
        k = SessionEventEnum::TransactionFinished;
        return;
    }
    if (s == "SessionFinished") {
        k = SessionEventEnum::SessionFinished;
        return;
    }
    if (s == "ReservationStart") {
        k = SessionEventEnum::ReservationStart;
        return;
    }
    if (s == "ReservationEnd") {
        k = SessionEventEnum::ReservationEnd;
        return;
    }
    if (s == "ReplugStarted") {
        k = SessionEventEnum::ReplugStarted;
        return;
    }
    if (s == "ReplugFinished") {
        k = SessionEventEnum::ReplugFinished;
        return;
    }
    if (s == "PluginTimeout") {
        k = SessionEventEnum::PluginTimeout;
        return;
    }
    if (s == "SwitchingPhases") {
        k = SessionEventEnum::SwitchingPhases;
        return;
    }
    if (s == "SessionResumed:") {
        k = SessionEventEnum::SessionResumed;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_SessionEventEnum");
}

void to_json(json& j, SessionEvent const& k) noexcept {
    j = json{
        {"uuid", k.uuid},
        {"timestamp", k.timestamp},
        {"event", k.event},
    };
    if (k.connector_id) {
        j["connector_id"] = k.connector_id.value();
    }
    if (k.session_started) {
        j["session_started"] = k.session_started.value();
    }
    if (k.session_finished) {
        j["session_finished"] = k.session_finished.value();
    }
    if (k.transaction_started) {
        j["transaction_started"] = k.transaction_started.value();
    }
    if (k.transaction_finished) {
        j["transaction_finished"] = k.transaction_finished.value();
    }
    if (k.charging_state_changed_event) {
        j["charging_state_changed_event"] = k.charging_state_changed_event.value();
    }
    if (k.authorization_event) {
        j["authorization_event"] = k.authorization_event.value();
    }
    if (k.source) {
        j["source"] = k.source.value();
    }
}

void from_json(json const& j, SessionEvent& k) {
    k.uuid = j.at("uuid");
    k.timestamp = j.at("timestamp");
    k.event = j.at("event");
    if (j.contains("connector_id")) {
        k.connector_id.emplace(j.at("connector_id"));
    }
    if (j.contains("session_started")) {
        k.session_started.emplace(j.at("session_started"));
    }
    if (j.contains("session_finished")) {
        k.session_finished.emplace(j.at("session_finished"));
    }
    if (j.contains("transaction_started")) {
        k.transaction_started.emplace(j.at("transaction_started"));
    }
    if (j.contains("transaction_finished")) {
        k.transaction_finished.emplace(j.at("transaction_finished"));
    }
    if (j.contains("charging_state_changed_event")) {
        k.charging_state_changed_event.emplace(j.at("charging_state_changed_event"));
    }
    if (j.contains("authorization_event")) {
        k.authorization_event.emplace(j.at("authorization_event"));
    }
    if (j.contains("source")) {
        k.source.emplace(j.at("source"));
    }
}

void to_json(json& j, Limits const& k) noexcept {
    j = json{
        {"max_current", k.max_current},
        {"nr_of_phases_available", k.nr_of_phases_available},
    };
    if (k.uuid) {
        j["uuid"] = k.uuid.value();
    }
}

void from_json(json const& j, Limits& k) {
    k.max_current = j.at("max_current");
    k.nr_of_phases_available = j.at("nr_of_phases_available");

    if (j.contains("uuid")) {
        k.uuid.emplace(j.at("uuid"));
    }
}

void to_json(json& j, EVInfo const& k) noexcept {
    j = json({});
    if (k.soc) {
        j["soc"] = k.soc.value();
    }
    if (k.present_voltage) {
        j["present_voltage"] = k.present_voltage.value();
    }
    if (k.present_current) {
        j["present_current"] = k.present_current.value();
    }
    if (k.target_voltage) {
        j["target_voltage"] = k.target_voltage.value();
    }
    if (k.target_current) {
        j["target_current"] = k.target_current.value();
    }
    if (k.maximum_current_limit) {
        j["maximum_current_limit"] = k.maximum_current_limit.value();
    }
    if (k.minimum_current_limit) {
        j["minimum_current_limit"] = k.minimum_current_limit.value();
    }
    if (k.maximum_voltage_limit) {
        j["maximum_voltage_limit"] = k.maximum_voltage_limit.value();
    }
    if (k.maximum_power_limit) {
        j["maximum_power_limit"] = k.maximum_power_limit.value();
    }
    if (k.estimated_time_full) {
        j["estimated_time_full"] = k.estimated_time_full.value();
    }
    if (k.departure_time) {
        j["departure_time"] = k.departure_time.value();
    }
    if (k.estimated_time_bulk) {
        j["estimated_time_bulk"] = k.estimated_time_bulk.value();
    }
    if (k.evcc_id) {
        j["evcc_id"] = k.evcc_id.value();
    }
    if (k.remaining_energy_needed) {
        j["remaining_energy_needed"] = k.remaining_energy_needed.value();
    }
    if (k.battery_capacity) {
        j["battery_capacity"] = k.battery_capacity.value();
    }
    if (k.battery_full_soc) {
        j["battery_full_soc"] = k.battery_full_soc.value();
    }
    if (k.battery_bulk_soc) {
        j["battery_bulk_soc"] = k.battery_bulk_soc.value();
    }
}

void from_json(json const& j, EVInfo& k) {
    if (j.contains("soc")) {
        k.soc.emplace(j.at("soc"));
    }
    if (j.contains("present_voltage")) {
        k.present_voltage.emplace(j.at("present_voltage"));
    }
    if (j.contains("present_current")) {
        k.present_current.emplace(j.at("present_current"));
    }
    if (j.contains("target_voltage")) {
        k.target_voltage.emplace(j.at("target_voltage"));
    }
    if (j.contains("target_current")) {
        k.target_current.emplace(j.at("target_current"));
    }
    if (j.contains("maximum_current_limit")) {
        k.maximum_current_limit.emplace(j.at("maximum_current_limit"));
    }
    if (j.contains("minimum_current_limit")) {
        k.minimum_current_limit.emplace(j.at("minimum_current_limit"));
    }
    if (j.contains("maximum_voltage_limit")) {
        k.maximum_voltage_limit.emplace(j.at("maximum_voltage_limit"));
    }
    if (j.contains("maximum_power_limit")) {
        k.maximum_power_limit.emplace(j.at("maximum_power_limit"));
    }
    if (j.contains("estimated_time_full")) {
        k.estimated_time_full.emplace(j.at("estimated_time_full"));
    }
    if (j.contains("departure_time")) {
        k.departure_time.emplace(j.at("departure_time"));
    }
    if (j.contains("estimated_time_bulk")) {
        k.estimated_time_bulk.emplace(j.at("estimated_time_bulk"));
    }
    if (j.contains("evcc_id")) {
        k.evcc_id.emplace(j.at("evcc_id"));
    }
    if (j.contains("remaining_energy_needed")) {
        k.remaining_energy_needed.emplace(j.at("remaining_energy_needed"));
    }
    if (j.contains("battery_capacity")) {
        k.battery_capacity.emplace(j.at("battery_capacity"));
    }
    if (j.contains("battery_full_soc")) {
        k.battery_full_soc.emplace(j.at("battery_full_soc"));
    }
    if (j.contains("battery_bulk_soc")) {
        k.battery_bulk_soc.emplace(j.at("battery_bulk_soc"));
    }
}

void to_json(json& j, CarManufacturer const& k) noexcept {
    switch (k) {
    case CarManufacturer::VolkswagenGroup:
        j = "VolkswagenGroup";
        return;
    case CarManufacturer::Tesla:
        j = "Tesla";
        return;
    case CarManufacturer::Unknown:
        j = "Unknown";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::CarManufacturer";
}

void from_json(json const& j, CarManufacturer& k) {
    std::string s = j;
    if (s == "VolkswagenGroup") {
        k = CarManufacturer::VolkswagenGroup;
        return;
    }
    if (s == "Tesla") {
        k = CarManufacturer::Tesla;
        return;
    }
    if (s == "Unknown") {
        k = CarManufacturer::Unknown;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_CarManufacturer");
}

void to_json(json& j, SessionStarted const& k) noexcept {
    j = json{
        {"reason", k.reason},
        {"meter_value", k.meter_value},
    };
    if (k.id_tag) {
        j["id_tag"] = k.id_tag.value();
    }
    if (k.signed_meter_value) {
        j["signed_meter_value"] = k.signed_meter_value.value();
    }
    if (k.reservation_id) {
        j["reservation_id"] = k.reservation_id.value();
    }
    if (k.logging_path) {
        j["logging_path"] = k.logging_path.value();
    }
}

void from_json(json const& j, SessionStarted& k) {
    k.reason = j.at("reason");
    k.meter_value = j.at("meter_value");
    if (j.contains("id_tag")) {
        k.id_tag.emplace(j.at("id_tag"));
    }
    if (j.contains("signed_meter_value")) {
        k.signed_meter_value.emplace(j.at("signed_meter_value"));
    }
    if (j.contains("reservation_id")) {
        k.reservation_id.emplace(j.at("reservation_id"));
    }
    if (j.contains("logging_path")) {
        k.logging_path.emplace(j.at("logging_path"));
    }
}

void to_json(json& j, SessionFinished const& k) noexcept {
    j = json{
        {"meter_value", k.meter_value},
    };
}

void from_json(json const& j, SessionFinished& k) {
    k.meter_value = j.at("meter_value");
}

void to_json(json& j, TransactionStarted const& k) noexcept {
    j = json{
        {"id_tag", k.id_tag},
        {"meter_value", k.meter_value},
    };
    if (k.signed_meter_value) {
        j["signed_meter_value"] = k.signed_meter_value.value();
    }
    if (k.reservation_id) {
        j["reservation_id"] = k.reservation_id.value();
    }
}

void from_json(json const& j, TransactionStarted& k) {
    k.meter_value = j.at("meter_value");
    k.id_tag = j.at("id_tag");
    if (j.contains("signed_meter_value")) {
        k.signed_meter_value.emplace(j.at("signed_meter_value"));
    }
    if (j.contains("reservation_id")) {
        k.reservation_id.emplace(j.at("reservation_id"));
    }
}

void to_json(json& j, TransactionFinished const& k) noexcept {
    j = json{
        {"meter_value", k.meter_value},
    };
    if (k.start_signed_meter_value) {
        j["start_signed_meter_value"] = k.start_signed_meter_value.value();
    }
    if (k.signed_meter_value) {
        j["signed_meter_value"] = k.signed_meter_value.value();
    }
    if (k.reason) {
        j["reason"] = k.reason.value();
    }
    if (k.id_tag) {
        j["id_tag"] = k.id_tag.value();
    }
}

void from_json(json const& j, TransactionFinished& k) {
    k.meter_value = j.at("meter_value");
    if (j.contains("start_signed_meter_value")) {
        k.start_signed_meter_value.emplace(j.at("start_signed_meter_value"));
    }
    if (j.contains("signed_meter_value")) {
        k.signed_meter_value.emplace(j.at("signed_meter_value"));
    }
    if (j.contains("reason")) {
        k.reason.emplace(j.at("reason"));
    }
    if (j.contains("id_tag")) {
        k.id_tag.emplace(j.at("id_tag"));
    }
}

void to_json(json& j, ChargingStateChangedEvent const& k) noexcept {
    j = json{
        {"meter_value", k.meter_value},
    };
}

void from_json(json const& j, ChargingStateChangedEvent& k) {
    k.meter_value = j.at("meter_value");
}

void to_json(json& j, AuthorizationEvent const& k) noexcept {
    j = json{
        {"meter_value", k.meter_value},
    };
}

void from_json(json const& j, AuthorizationEvent& k) {
    k.meter_value = j.at("meter_value");
}

void to_json(json& j, ErrorSeverity const& k) noexcept {
    switch (k) {
    case ErrorSeverity::High:
        j = "High";
        return;
    case ErrorSeverity::Medium:
        j = "Medium";
        return;
    case ErrorSeverity::Low:
        j = "Low";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::ErrorSeverity";
}

void from_json(json const& j, ErrorSeverity& k) {
    std::string s = j;
    if (s == "High") {
        k = ErrorSeverity::High;
        return;
    }
    if (s == "Medium") {
        k = ErrorSeverity::Medium;
        return;
    }
    if (s == "Low") {
        k = ErrorSeverity::Low;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type API_V1_0_ErrorSeverity");
}

void to_json(json& j, ErrorState const& k) noexcept {
    switch (k) {
    case ErrorState::Active:
        j = "Active";
        return;
    case ErrorState::ClearedByModule:
        j = "ClearedByModule";
        return;
    case ErrorState::ClearedByReboot:
        j = "ClearedByReboot";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::ErrorState";
}

void from_json(json const& j, ErrorState& k) {
    std::string s = j;
    if (s == "Active") {
        k = ErrorState::Active;
        return;
    }
    if (s == "ClearedByModule") {
        k = ErrorState::ClearedByModule;
        return;
    }
    if (s == "ClearedByReboot") {
        k = ErrorState::ClearedByReboot;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type API_V1_0_ErrorState");
}

void to_json(json& j, ErrorOrigin const& k) noexcept {
    j = json{
        {"module_id", k.module_id},
        {"implementation_id", k.implementation_id},
    };
}

void from_json(json const& j, ErrorOrigin& k) {
    k.module_id = j.at("module_id");
    k.implementation_id = j.at("implementation_id");
}

void to_json(json& j, Error const& k) noexcept {
    j = json{
        {"type", k.type},           {"sub_type", k.sub_type}, {"description", k.description},
        {"message", k.message},     {"severity", k.severity}, {"origin", k.origin},
        {"timestamp", k.timestamp}, {"uuid", k.uuid},         {"state", k.state},
    };
}

void from_json(json const& j, Error& k) {
    k.type = j.at("type");
    k.sub_type = j.at("sub_type");
    k.description = j.at("description");
    k.message = j.at("message");
    k.severity = j.at("severity");
    k.origin = j.at("origin");
    k.timestamp = j.at("timestamp");
    k.uuid = j.at("uuid");
    k.state = j.at("state");
}

void to_json(json& j, ConnectorTypeEnum const& k) noexcept {
    switch (k) {
    case ConnectorTypeEnum::cCCS1:
        j = "cCCS1";
        return;
    case ConnectorTypeEnum::cCCS2:
        j = "cCCS2";
        return;
    case ConnectorTypeEnum::cG105:
        j = "cG105";
        return;
    case ConnectorTypeEnum::cMCS:
        j = "cMCS";
        return;
    case ConnectorTypeEnum::cTesla:
        j = "cTesla";
        return;
    case ConnectorTypeEnum::cType1:
        j = "cType1";
        return;
    case ConnectorTypeEnum::cType2:
        j = "cType2";
        return;
    case ConnectorTypeEnum::s309_1P_16A:
        j = "s309_1P_16A";
        return;
    case ConnectorTypeEnum::s309_1P_32A:
        j = "s309_1P_32A";
        return;
    case ConnectorTypeEnum::s309_3P_16A:
        j = "s309_3P_16A";
        return;
    case ConnectorTypeEnum::s309_3P_32A:
        j = "s309_3P_32A";
        return;
    case ConnectorTypeEnum::sBS1361:
        j = "sBS1361";
        return;
    case ConnectorTypeEnum::sCEE_7_7:
        j = "sCEE_7_7";
        return;
    case ConnectorTypeEnum::sType2:
        j = "sType2";
        return;
    case ConnectorTypeEnum::sType3:
        j = "sType3";
        return;
    case ConnectorTypeEnum::Other1PhMax16A:
        j = "Other1PhMax16A";
        return;
    case ConnectorTypeEnum::Other1PhOver16A:
        j = "Other1PhOver16A";
        return;
    case ConnectorTypeEnum::Other3Ph:
        j = "Other3Ph";
        return;
    case ConnectorTypeEnum::Pan:
        j = "Pan";
        return;
    case ConnectorTypeEnum::wInductive:
        j = "wInductive";
        return;
    case ConnectorTypeEnum::wResonant:
        j = "wResonant";
        return;
    case ConnectorTypeEnum::Undetermined:
        j = "Undetermined";
        return;
    case ConnectorTypeEnum::Unknown:
        j = "Unknown";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::ConnectorTypeEnum";
}

void from_json(json const& j, ConnectorTypeEnum& k) {
    std::string s = j;
    if (s == "cCCS1") {
        k = ConnectorTypeEnum::cCCS1;
        return;
    }
    if (s == "cCCS2") {
        k = ConnectorTypeEnum::cCCS2;
        return;
    }
    if (s == "cG105") {
        k = ConnectorTypeEnum::cG105;
        return;
    }
    if (s == "cMCS") {
        k = ConnectorTypeEnum::cMCS;
        return;
    }
    if (s == "cTesla") {
        k = ConnectorTypeEnum::cTesla;
        return;
    }
    if (s == "cType1") {
        k = ConnectorTypeEnum::cType1;
        return;
    }
    if (s == "cType2") {
        k = ConnectorTypeEnum::cType2;
        return;
    }
    if (s == "s309_1P_16A") {
        k = ConnectorTypeEnum::s309_1P_16A;
        return;
    }
    if (s == "s309_1P_32A") {
        k = ConnectorTypeEnum::s309_1P_32A;
        return;
    }
    if (s == "s309_3P_16A") {
        k = ConnectorTypeEnum::s309_3P_16A;
        return;
    }
    if (s == "s309_3P_32A") {
        k = ConnectorTypeEnum::s309_3P_32A;
        return;
    }
    if (s == "sBS1361") {
        k = ConnectorTypeEnum::sBS1361;
        return;
    }
    if (s == "sCEE_7_7") {
        k = ConnectorTypeEnum::sCEE_7_7;
        return;
    }
    if (s == "sType2") {
        k = ConnectorTypeEnum::sType2;
        return;
    }
    if (s == "sType3") {
        k = ConnectorTypeEnum::sType3;
        return;
    }
    if (s == "Other1PhMax16A") {
        k = ConnectorTypeEnum::Other1PhMax16A;
        return;
    }
    if (s == "Other1PhOver16A") {
        k = ConnectorTypeEnum::Other1PhOver16A;
        return;
    }
    if (s == "Other3Ph") {
        k = ConnectorTypeEnum::Other3Ph;
        return;
    }
    if (s == "Pan") {
        k = ConnectorTypeEnum::Pan;
        return;
    }
    if (s == "wInductive") {
        k = ConnectorTypeEnum::wInductive;
        return;
    }
    if (s == "wResonant") {
        k = ConnectorTypeEnum::wResonant;
        return;
    }
    if (s == "Undetermined") {
        k = ConnectorTypeEnum::Undetermined;
        return;
    }
    if (s == "Unknown") {
        k = ConnectorTypeEnum::Unknown;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_EVSE_MANAGER_ConnectorTypeEnum");
}

void to_json(json& j, Connector const& k) noexcept {
    j = json{
        {"id", k.id},
    };
    if (k.type) {
        j["type"] = k.type.value();
    }
}

void from_json(json const& j, Connector& k) {
    k.id = j.at("id");

    if (j.contains("type")) {
        k.type.emplace(j.at("type"));
    }
}

void to_json(json& j, Evse const& k) noexcept {
    j = json{
        {"id", k.id},
        {"connectors", k.connectors},
    };
}

void from_json(json const& j, Evse& k) {
    k.id = j.at("id");
    for (auto val : j.at("connectors")) {
        k.connectors.push_back(val);
    }
}

void to_json(json& j, EnableSourceEnum const& k) noexcept {
    switch (k) {
    case EnableSourceEnum::Unspecified:
        j = "Unspecified";
        return;
    case EnableSourceEnum::LocalAPI:
        j = "LocalAPI";
        return;
    case EnableSourceEnum::LocalKeyLock:
        j = "LocalKeyLock";
        return;
    case EnableSourceEnum::ServiceTechnician:
        j = "ServiceTechnician";
        return;
    case EnableSourceEnum::RemoteKeyLock:
        j = "RemoteKeyLock";
        return;
    case EnableSourceEnum::MobileApp:
        j = "MobileApp";
        return;
    case EnableSourceEnum::FirmwareUpdate:
        j = "FirmwareUpdate";
        return;
    case EnableSourceEnum::CSMS:
        j = "CSMS";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::EnableSourceEnum";
}

void from_json(json const& j, EnableSourceEnum& k) {
    std::string s = j;
    if (s == "Unspecified") {
        k = EnableSourceEnum::Unspecified;
        return;
    }
    if (s == "LocalAPI") {
        k = EnableSourceEnum::LocalAPI;
        return;
    }
    if (s == "LocalKeyLock") {
        k = EnableSourceEnum::LocalKeyLock;
        return;
    }
    if (s == "ServiceTechnician") {
        k = EnableSourceEnum::ServiceTechnician;
        return;
    }
    if (s == "RemoteKeyLock") {
        k = EnableSourceEnum::RemoteKeyLock;
        return;
    }
    if (s == "MobileApp") {
        k = EnableSourceEnum::MobileApp;
        return;
    }
    if (s == "FirmwareUpdate") {
        k = EnableSourceEnum::FirmwareUpdate;
        return;
    }
    if (s == "CSMS") {
        k = EnableSourceEnum::CSMS;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_EVSE_MANAGER_EnableSourceEnum");
}

void to_json(json& j, EnableStateEnum const& k) noexcept {
    switch (k) {
    case EnableStateEnum::Unassigned:
        j = "Unassigned";
        return;
    case EnableStateEnum::Disable:
        j = "Disable";
        return;
    case EnableStateEnum::Enable:
        j = "Enable";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::EnableStateEnum";
}

void from_json(json const& j, EnableStateEnum& k) {
    std::string s = j;
    if (s == "Unassigned") {
        k = EnableStateEnum::Unassigned;
        return;
    }
    if (s == "Disable") {
        k = EnableStateEnum::Disable;
        return;
    }
    if (s == "Enable") {
        k = EnableStateEnum::Enable;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_EVSE_MANAGER_EnableStateEnum");
}

void to_json(json& j, EnableDisableSource const& k) noexcept {
    j = json{
        {"enable_source", k.enable_source},
        {"enable_state", k.enable_state},
        {"enable_priority", k.enable_priority},
    };
}

void from_json(json const& j, EnableDisableSource& k) {
    k.enable_source = j.at("enable_source");
    k.enable_state = j.at("enable_state");
    k.enable_priority = j.at("enable_priority");
}

void to_json(json& j, EnableDisableRequest const& k) noexcept {
    j = json{
        {"connector_id", k.connector_id},
        {"source", k.source},
    };
}

void from_json(json const& j, EnableDisableRequest& k) {
    k.connector_id = j.at("connector_id");
    k.source = j.at("source");
}

void to_json(json& j, AuthorizeResponseArgs const& k) noexcept {
    j = json{
        {"token", k.token},
        {"result", k.result},
    };
}

void from_json(json const& j, AuthorizeResponseArgs& k) {
    k.token = j.at("token");
    k.result = j.at("result");
}

void to_json(json& j, PlugAndChargeConfiguration const& k) noexcept {
    if (k.pnc_enabled) {
        j["pnc_enabled"] = k.pnc_enabled.value();
    }
    if (k.central_contract_validation_allowed) {
        j["central_contract_validation_allowed"] = k.central_contract_validation_allowed.value();
    }
    if (k.contract_certificate_installation_enabled) {
        j["contract_certificate_installation_enabled"] = k.contract_certificate_installation_enabled.value();
    }
}

void from_json(json const& j, PlugAndChargeConfiguration& k) {
    if (j.contains("pnc_enabled")) {
        k.pnc_enabled.emplace(j.at("pnc_enabled"));
    }
    if (j.contains("central_contract_validation_allowed")) {
        k.central_contract_validation_allowed.emplace(j.at("central_contract_validation_allowed"));
    }
    if (j.contains("contract_certificate_installation_enabled")) {
        k.contract_certificate_installation_enabled.emplace(j.at("contract_certificate_installation_enabled"));
    }
}

void to_json(json& j, EvseStateEnum const& k) noexcept {
    switch (k) {
    case EvseStateEnum::Unknown:
        j = "Unknown";
        return;
    case EvseStateEnum::Unplugged:
        j = "Unplugged";
        return;
    case EvseStateEnum::Disabled:
        j = "Disabled";
        return;
    case EvseStateEnum::Preparing:
        j = "Preparing";
        return;
    case EvseStateEnum::AuthRequired:
        j = "AuthRequired";
        return;
    case EvseStateEnum::WaitingForEnergy:
        j = "WaitingForEnergy";
        return;
    case EvseStateEnum::ChargingPausedEV:
        j = "ChargingPausedEV";
        return;
    case EvseStateEnum::ChargingPausedEVSE:
        j = "ChargingPausedEVSE";
        return;
    case EvseStateEnum::Charging:
        j = "Charging";
        return;
    case EvseStateEnum::AuthTimeout:
        j = "AuthTimeout";
        return;
    case EvseStateEnum::Finished:
        j = "Finished";
        return;
    case EvseStateEnum::FinishedEVSE:
        j = "FinishedEVSE";
        return;
    case EvseStateEnum::FinishedEV:
        j = "FinishedEV";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::evse_manger::EvseStateEnum";
}

void from_json(json const& j, EvseStateEnum& k) {
    std::string s = j;
    if (s == "Unknown") {
        k = EvseStateEnum::Unknown;
        return;
    }
    if (s == "Unplugged") {
        k = EvseStateEnum::Unplugged;
        return;
    }
    if (s == "Disabled") {
        k = EvseStateEnum::Disabled;
        return;
    }
    if (s == "Preparing") {
        k = EvseStateEnum::Preparing;
        return;
    }
    if (s == "AuthRequired") {
        k = EvseStateEnum::AuthRequired;
        return;
    }
    if (s == "WaitingForEnergy") {
        k = EvseStateEnum::WaitingForEnergy;
        return;
    }
    if (s == "ChargingPausedEV") {
        k = EvseStateEnum::ChargingPausedEV;
        return;
    }
    if (s == "ChargingPausedEVSE") {
        k = EvseStateEnum::ChargingPausedEVSE;
        return;
    }
    if (s == "Charging") {
        k = EvseStateEnum::Charging;
        return;
    }
    if (s == "AuthTimeout") {
        k = EvseStateEnum::AuthTimeout;
        return;
    }
    if (s == "Finished") {
        k = EvseStateEnum::Finished;
        return;
    }
    if (s == "FinishedEVSE") {
        k = EvseStateEnum::FinishedEVSE;
        return;
    }
    if (s == "FinishedEV") {
        k = EvseStateEnum::FinishedEV;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_EVSE_MANAGER_EvseStateEnum");
}

void to_json(json& j, SessionInfo const& k) noexcept {
    j = json{
        {"state", k.state},
        {"charged_energy_wh", k.charged_energy_wh},
        {"discharged_energy_wh", k.discharged_energy_wh},
        {"session_duration_s", k.session_duration_s},
        {"latest_total_w", k.latest_total_w},
        {"timestamp", k.timestamp},
    };

    if (k.selected_protocol.has_value()) {
        j["selected_protocol"] = k.selected_protocol.value();
    }
    if (k.transaction_duration_s.has_value()) {
        j["transaction_duration_s"] = k.transaction_duration_s.value();
    }
    if (k.session_start_time.has_value()) {
        j["session_start_time"] = k.session_start_time.value();
    }
    if (k.session_end_time.has_value()) {
        j["session_end_time"] = k.session_end_time.value();
    }
    if (k.transaction_start_time.has_value()) {
        j["transaction_start_time"] = k.transaction_start_time.value();
    }
    if (k.transaction_end_time.has_value()) {
        j["transaction_end_time"] = k.transaction_end_time.value();
    }
}

void from_json(json const& j, SessionInfo& k) {
    k.state = j.at("state");
    k.charged_energy_wh = j.at("charged_energy_wh");
    k.discharged_energy_wh = j.at("discharged_energy_wh");
    k.session_duration_s = j.at("session_duration_s");
    k.latest_total_w = j.at("latest_total_w");
    k.timestamp = j.at("timestamp");

    if (j.contains("selected_protocol")) {
        k.selected_protocol = j.at("selected_protocol");
    }
    if (j.contains("transaction_duration_s")) {
        k.transaction_duration_s = j.at("transaction_duration_s");
    }
    if (j.contains("session_start_time")) {
        k.session_start_time = j.at("session_start_time");
    }
    if (j.contains("session_end_time")) {
        k.session_end_time = j.at("session_end_time");
    }
    if (j.contains("transaction_start_time")) {
        k.transaction_start_time = j.at("transaction_start_time");
    }
    if (j.contains("transaction_end_time")) {
        k.transaction_end_time = j.at("transaction_end_time");
    }
}
} // namespace everest::lib::API::V1_0::types::evse_manager
