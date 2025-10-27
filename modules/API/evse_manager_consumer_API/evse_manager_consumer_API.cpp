// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "evse_manager_consumer_API.hpp"

#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/energy/API.hpp>
#include <everest_api_types/energy/codec.hpp>
#include <everest_api_types/energy/wrapper.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/evse_board_support/wrapper.hpp>
#include <everest_api_types/evse_manager/API.hpp>
#include <everest_api_types/evse_manager/codec.hpp>
#include <everest_api_types/evse_manager/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/iso15118_charger/API.hpp>
#include <everest_api_types/iso15118_charger/codec.hpp>
#include <everest_api_types/iso15118_charger/wrapper.hpp>
#include <everest_api_types/isolation_monitor/API.hpp>
#include <everest_api_types/isolation_monitor/codec.hpp>
#include <everest_api_types/isolation_monitor/wrapper.hpp>
#include <everest_api_types/power_supply_DC/API.hpp>
#include <everest_api_types/power_supply_DC/codec.hpp>
#include <everest_api_types/power_supply_DC/wrapper.hpp>
#include <everest_api_types/powermeter/codec.hpp>
#include <everest_api_types/powermeter/wrapper.hpp>
#include <everest_api_types/uk_random_delay/API.hpp>
#include <everest_api_types/uk_random_delay/codec.hpp>
#include <everest_api_types/uk_random_delay/wrapper.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <everest/logging.hpp>

namespace {
template <class T> T const& to_external_api(T const& val) {
    return val;
}

} // namespace

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::evse_manager;
namespace API_powermeter = API_types::powermeter;
namespace API_iso = API_types::iso15118_charger;
namespace API_energy = API_types::energy;
namespace API_evse_bsp = API_types::evse_board_support;
namespace API_imd = API_types::isolation_monitor;
namespace API_dc = API_types::power_supply_DC;
namespace API_random_delay = API_types::uk_random_delay;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void evse_manager_consumer_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "evse_manager_consumer", 1);
}

void evse_manager_consumer_API::ready() {
    invoke_ready(*p_main);

    generate_api_cmd_get_evse();
    generate_api_cmd_enable_disable();
    generate_api_cmd_authorize_response();
    generate_api_cmd_withdraw_authorization();
    generate_api_cmd_reserve();
    generate_api_cmd_cancel_reservation();
    generate_api_cmd_pause_charging();
    generate_api_cmd_resume_charging();
    generate_api_cmd_stop_transaction();
    generate_api_cmd_force_unlock();
    generate_api_cmd_external_ready_to_start_charging();
    generate_api_cmd_random_delay_enable();
    generate_api_cmd_random_delay_disable();
    generate_api_cmd_random_delay_cancel();
    generate_api_cmd_random_delay_set_duration_s();

    generate_api_var_session_event();
    generate_api_var_limits();
    generate_api_var_ev_info();
    generate_api_var_car_manufacturer();
    generate_api_var_powermeter();
    generate_api_var_evse_id();
    generate_api_var_hw_capabilities();
    generate_api_var_enforced_limits();
    generate_api_var_waiting_for_external_ready();
    generate_api_var_ready();
    generate_api_var_selected_protocol();
    generate_api_var_powermeter_public_key_ocmf();
    generate_api_var_supported_energy_transfer_modes();

    generate_api_var_ac_nr_of_phases_available();
    generate_api_var_ac_pp_ampacity();
    generate_api_var_dlink_ready();
    generate_api_var_isolation_measurement();
    generate_api_var_dc_voltage_current();
    generate_api_var_dc_mode();
    generate_api_var_dc_capabilities();
    generate_api_var_random_delay_countdown();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

auto evse_manager_consumer_API::forward_api_var(std::string const& var) {
    using namespace API_types_ext;
    using namespace API_powermeter;
    using namespace API_generic;
    using namespace API_energy;
    using namespace API_evse_bsp;
    using namespace API_iso;
    using namespace API_imd;
    using namespace API_dc;
    using namespace API_random_delay;
    auto topic = topics.everest_to_extern(var);
    return [this, topic](auto const& val) {
        try {
            auto&& external = to_external_api(val);
            auto&& payload = serialize(external);
            mqtt.publish(topic, payload);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Cannot convert internal to external or serialize it.\n" << topic;
        }
    };
}

void evse_manager_consumer_API::generate_api_cmd_get_evse() {
    subscribe_api_topic("get_evse", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto reply = API_types_ext::to_external_api(r_evse_manager->call_get_evse());
            mqtt.publish(msg.replyTo, serialize(reply));
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_enable_disable() {
    subscribe_api_topic("enable_disable", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::EnableDisableRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto reply = r_evse_manager->call_enable_disable(payload.connector_id, to_internal_api(payload.source));
                mqtt.publish(msg.replyTo, API_generic::serialize(reply));
                return true;
            }
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_authorize_response() {
    subscribe_api_topic("authorize_response", [this](std::string const& data) {
        API_types_ext::AuthorizeResponseArgs payload;
        if (deserialize(data, payload)) {
            r_evse_manager->call_authorize_response(to_internal_api(payload.token), to_internal_api(payload.result));
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_withdraw_authorization() {
    subscribe_api_topic("withdraw_authorization", [this](std::string const&) {
        r_evse_manager->call_withdraw_authorization();
        return true;
    });
}

void evse_manager_consumer_API::generate_api_cmd_reserve() {
    subscribe_api_topic("reserve", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            int payload;
            if (deserialize(msg.payload, payload)) {
                auto reply = r_evse_manager->call_reserve(payload);
                mqtt.publish(msg.replyTo, API_generic::serialize(reply));
                return true;
            }
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_cancel_reservation() {
    subscribe_api_topic("cancel_reservation", [this](std::string const&) {
        r_evse_manager->call_cancel_reservation();
        return true;
    });
}

void evse_manager_consumer_API::generate_api_cmd_pause_charging() {
    subscribe_api_topic("pause_charging", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto result = r_evse_manager->call_pause_charging();
            mqtt.publish(msg.replyTo, result);
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_resume_charging() {
    subscribe_api_topic("resume_charging", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto result = r_evse_manager->call_resume_charging();
            mqtt.publish(msg.replyTo, result);
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_stop_transaction() {
    subscribe_api_topic("stop_transaction", [this](std::string const& data) {
        auto result = false;
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::StopTransactionRequest_External payload;
            if (deserialize(msg.payload, payload)) {
                result = r_evse_manager->call_stop_transaction(API_types_ext::to_internal_api(payload));
                mqtt.publish(msg.replyTo, result);
                return true;
            }
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_force_unlock() {
    subscribe_api_topic("force_unlock", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            int payload;
            if (deserialize(msg.payload, payload)) {
                auto result = r_evse_manager->call_force_unlock(payload);
                mqtt.publish(msg.replyTo, result);
            }
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_external_ready_to_start_charging() {
    subscribe_api_topic("external_ready_to_start_charging", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto result = r_evse_manager->call_external_ready_to_start_charging();
            mqtt.publish(msg.replyTo, result);
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::generate_api_cmd_random_delay_enable() {
    if (not r_random_delay.empty()) {
        subscribe_api_topic("random_delay_enable", [this](std::string const&) {
            r_random_delay[0]->call_enable();
            return true;
        });
    }
}

void evse_manager_consumer_API::generate_api_cmd_random_delay_disable() {
    if (not r_random_delay.empty()) {
        subscribe_api_topic("random_delay_disable", [=](std::string const&) {
            r_random_delay[0]->call_disable();
            return true;
        });
    }
}

void evse_manager_consumer_API::generate_api_cmd_random_delay_cancel() {
    if (not r_random_delay.empty()) {
        subscribe_api_topic("random_delay_cancel", [=](std::string const&) {
            r_random_delay[0]->call_cancel();
            return true;
        });
    }
}

void evse_manager_consumer_API::generate_api_cmd_random_delay_set_duration_s() {
    if (not r_random_delay.empty())
        subscribe_api_topic("random_delay_set_duration_s", [=](std::string const& data) {
            int32_t duration;
            if (deserialize(data, duration)) {
                r_random_delay[0]->call_set_duration_s(duration);
                return true;
            }
            return false;
        });
}

void evse_manager_consumer_API::generate_api_var_session_event() {
    r_evse_manager->subscribe_session_event(forward_api_var("session_event"));
}

void evse_manager_consumer_API::generate_api_var_limits() {
    r_evse_manager->subscribe_limits(forward_api_var("limits"));
}

void evse_manager_consumer_API::generate_api_var_ev_info() {
    r_evse_manager->subscribe_ev_info(forward_api_var("ev_info"));
}

void evse_manager_consumer_API::generate_api_var_car_manufacturer() {
    r_evse_manager->subscribe_car_manufacturer(forward_api_var("car_manufacturer"));
}

void evse_manager_consumer_API::generate_api_var_powermeter() {
    r_evse_manager->subscribe_powermeter(forward_api_var("powermeter"));
}

void evse_manager_consumer_API::generate_api_var_evse_id() {
    r_evse_manager->subscribe_evse_id(forward_api_var("evse_id"));
}

void evse_manager_consumer_API::generate_api_var_hw_capabilities() {
    r_evse_manager->subscribe_hw_capabilities(forward_api_var("hw_capabilities"));
}

void evse_manager_consumer_API::generate_api_var_enforced_limits() {
    r_evse_manager->subscribe_enforced_limits(forward_api_var("enforced_limits"));
}

void evse_manager_consumer_API::generate_api_var_waiting_for_external_ready() {
    r_evse_manager->subscribe_waiting_for_external_ready(forward_api_var("waiting_for_external_ready"));
}

void evse_manager_consumer_API::generate_api_var_ready() {
    r_evse_manager->subscribe_ready(forward_api_var("ready"));
}

void evse_manager_consumer_API::generate_api_var_selected_protocol() {
    r_evse_manager->subscribe_selected_protocol(forward_api_var("selected_protocol"));
}

void evse_manager_consumer_API::generate_api_var_supported_energy_transfer_modes() {
    r_evse_manager->subscribe_supported_energy_transfer_modes(forward_api_var("supported_energy_transfer_modes"));
}

void evse_manager_consumer_API::generate_api_var_powermeter_public_key_ocmf() {
    r_evse_manager->subscribe_powermeter_public_key_ocmf(forward_api_var("powermeter_public_key_ocmf"));
}

void evse_manager_consumer_API::generate_api_var_ac_nr_of_phases_available() {
    if (not r_evse_bsp.empty()) {
        r_evse_bsp[0]->subscribe_ac_nr_of_phases_available(forward_api_var("ac_nr_of_phases_available"));
    }
}

void evse_manager_consumer_API::generate_api_var_ac_pp_ampacity() {
    if (not r_evse_bsp.empty()) {
        r_evse_bsp[0]->subscribe_ac_pp_ampacity(forward_api_var("ac_pp_ampacity"));
    }
}

void evse_manager_consumer_API::generate_api_var_dlink_ready() {
    if (not r_slac.empty()) {
        r_slac[0]->subscribe_dlink_ready(forward_api_var("dlink_ready"));
    }
}

void evse_manager_consumer_API::generate_api_var_isolation_measurement() {
    if (not r_imd.empty()) {
        r_imd[0]->subscribe_isolation_measurement(forward_api_var("isolation_measurement"));
    }
}

void evse_manager_consumer_API::generate_api_var_dc_voltage_current() {
    if (not r_ps_dc.empty()) {
        r_ps_dc[0]->subscribe_voltage_current(forward_api_var("dc_voltage_current"));
    }
}

void evse_manager_consumer_API::generate_api_var_dc_mode() {
    if (not r_ps_dc.empty()) {
        r_ps_dc[0]->subscribe_mode(forward_api_var("dc_mode"));
    }
}

void evse_manager_consumer_API::generate_api_var_dc_capabilities() {
    if (not r_ps_dc.empty()) {
        r_ps_dc[0]->subscribe_capabilities(forward_api_var("dc_capabilities"));
    }
}

void evse_manager_consumer_API::generate_api_var_random_delay_countdown() {
    if (not r_random_delay.empty()) {
        r_random_delay[0]->subscribe_countdown(forward_api_var("random_delay_countdown"));
    }
}

void evse_manager_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void evse_manager_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void evse_manager_consumer_API::subscribe_api_topic(std::string const& var,
                                                    ParseAndPublishFtor const& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Topic: '" << topic << "' failed with -> " << e.what() << "\n => " << data;
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}
} // namespace module
