// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "evse_board_support_API.hpp"

#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/evse_board_support/wrapper.hpp>
#include <everest_api_types/evse_manager/API.hpp>
#include <everest_api_types/evse_manager/codec.hpp>
#include <everest_api_types/evse_manager/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include "utils/error.hpp"

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_evse_manager = API_types::evse_manager;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void evse_board_support_API::init() {
    invoke_init(*p_main);
    invoke_init(*p_rcd);
    invoke_init(*p_connector_lock);

    topics.setTargetApiModuleID(info.id, "evse_board_support");

    generate_api_var_event();
    generate_api_var_phase_count();
    generate_api_var_capabilities();
    generate_api_var_ac_pp_ampacity();
    generate_api_var_request_stop_transaction();
    generate_api_var_rcd_current();

    generate_api_var_communication_check();

    generate_api_var_raise_error();
    generate_api_var_clear_error();
}

void evse_board_support_API::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_rcd);
    invoke_ready(*p_connector_lock);

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void evse_board_support_API::generate_api_var_event() {
    subscribe_api_var("event", [=](std::string const& data) {
        API_types_ext::BspEvent ext;
        if (deserialize(data, ext)) {
            p_main->publish_event(to_internal_api(ext));
            return true;
        }
        return false;
    });
}

void evse_board_support_API::generate_api_var_phase_count() {
    subscribe_api_var("ac_nr_of_phases", [=](std::string const& data) {
        auto phase_count = API_generic::deserialize<int>(data);
        p_main->publish_ac_nr_of_phases_available(phase_count);
        return true;
    });
}

void evse_board_support_API::generate_api_var_capabilities() {
    subscribe_api_var("capabilities", [=](std::string const& data) {
        API_types_ext::HardwareCapabilities ext;
        if (deserialize(data, ext)) {
            p_main->publish_capabilities(to_internal_api(ext));
            return true;
        }
        return false;
    });
}

void evse_board_support_API::generate_api_var_ac_pp_ampacity() {
    subscribe_api_var("ac_pp_ampacity", [=](std::string const& data) {
        API_types_ext::ProximityPilot ext;
        if (deserialize(data, ext)) {
            p_main->publish_ac_pp_ampacity(to_internal_api(ext));
            return true;
        }
        return false;
    });
}

void evse_board_support_API::generate_api_var_request_stop_transaction() {
    subscribe_api_var("request_stop_transaction", [=](std::string const& data) {
        API_evse_manager::StopTransactionRequest ext;
        if (deserialize(data, ext)) {
            p_main->publish_request_stop_transaction(to_internal_api(ext));
            return true;
        }
        return false;
    });
}

void evse_board_support_API::generate_api_var_rcd_current() {
    subscribe_api_var("rcd_current", [=](std::string const& data) {
        auto rcd_current = API_generic::deserialize<double>(data);
        p_rcd->publish_rcd_current_mA(rcd_current);
        return true;
    });
}

void evse_board_support_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](std::string const& data) {
        auto val = API_generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
    });
}

void evse_board_support_API::generate_api_var_raise_error() {
    subscribe_api_var("raise_error", [=](std::string const& data) {
        API_types_ext::Error error;
        if (deserialize(data, error)) {
            auto handler = make_error_handler(error);
            handler.raiser();
            return true;
        }
        return false;
    });
}

void evse_board_support_API::generate_api_var_clear_error() {
    subscribe_api_var("clear_error", [=](std::string const& data) {
        API_types_ext::Error error;
        if (deserialize(data, error)) {
            auto handler = make_error_handler(error);
            handler.clearer();
            return true;
        }
        return false;
    });
}

void evse_board_support_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void evse_board_support_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ev_API::Topics& evse_board_support_API::get_topics() const {
    return topics;
}

evse_board_support_API::ErrorHandler evse_board_support_API::make_error_handler(API_types_ext::Error const& error) {
    using namespace API_types_ext;
    auto error_str = API_generic::trimmed(serialize(error.type));
    ErrorHandler result;
    auto sub_type_str = error.sub_type ? error.sub_type.value() : "";
    auto message_str = error.message ? error.message.value() : "";
    std::string error_id;

    switch (error.type) {
    case ErrorEnum::DiodeFault:
    case ErrorEnum::VentilationNotAvailable:
    case ErrorEnum::BrownOut:
    case ErrorEnum::EnergyManagement:
    case ErrorEnum::PermanentFault:
    case ErrorEnum::MREC2GroundFailure:
    case ErrorEnum::MREC3HighTemperature:
    case ErrorEnum::MREC4OverCurrentFailure:
    case ErrorEnum::MREC5OverVoltage:
    case ErrorEnum::MREC6UnderVoltage:
    case ErrorEnum::MREC8EmergencyStop:
    case ErrorEnum::MREC10InvalidVehicleMode:
    case ErrorEnum::MREC14PilotFault:
    case ErrorEnum::MREC15PowerLoss:
    case ErrorEnum::MREC17EVSEContactorFault:
    case ErrorEnum::MREC18CableOverTempDerate:
    case ErrorEnum::MREC19CableOverTempStop:
    case ErrorEnum::MREC20PartialInsertion:
    case ErrorEnum::MREC23ProximityFault:
    case ErrorEnum::MREC24ConnectorVoltageHigh:
    case ErrorEnum::MREC25BrokenLatch:
    case ErrorEnum::MREC26CutCable:
    case ErrorEnum::VendorError:
    case ErrorEnum::VendorWarning:
    case ErrorEnum::CommunicationFault:
        error_id = "evse_board_support/" + error_str;
        result.raiser = [this, sub_type_str, message_str, error_id]() {
            auto ev_error = p_main->error_factory->create_error(error_id, sub_type_str, message_str,
                                                                         Everest::error::Severity::High);
            p_main->raise_error(ev_error);
        };
        result.clearer = [this, error_id, sub_type_str] { p_main->clear_error(error_id, sub_type_str); };
        break;
    case ErrorEnum::ConnectorLockCapNotCharged:
    case ErrorEnum::ConnectorLockUnexpectedOpen:
    case ErrorEnum::ConnectorLockUnexpectedClose:
    case ErrorEnum::ConnectorLockFailedLock:
    case ErrorEnum::ConnectorLockFailedUnlock:
    case ErrorEnum::MREC1ConnectorLockFailure:
        error_id = "connector_lock/" + error_str;
        result.raiser = [this, sub_type_str, message_str, error_id]() {
            auto ev_error = p_connector_lock->error_factory->create_error(error_id, sub_type_str, message_str,
                                                                          Everest::error::Severity::High);
            p_connector_lock->raise_error(ev_error);
        };
        result.clearer = [this, error_id, sub_type_str] { p_connector_lock->clear_error(error_id, sub_type_str); };

        break;
    case ErrorEnum::Selftest:
    case ErrorEnum::DC:
    case ErrorEnum::AC:
        error_id = "acd_rcd/" + error_str;
        result.raiser = [this, sub_type_str, message_str, error_id]() {
            auto ev_error =
                p_rcd->error_factory->create_error(error_id, sub_type_str, message_str, Everest::error::Severity::High);
            p_rcd->raise_error(ev_error);
        };
        result.clearer = [this, error_id, sub_type_str] { p_rcd->clear_error(error_id, sub_type_str); };
        break;
    }
    return result;
}

} // namespace module
