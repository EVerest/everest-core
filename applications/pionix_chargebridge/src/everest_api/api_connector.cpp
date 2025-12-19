// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "charge_bridge/utilities/string.hpp"
#include "everest/io/mqtt/mosquitto_cpp.hpp"
#include "protocol/evse_bsp_cb_to_host.h"
#include <charge_bridge/everest_api/api_connector.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <cstring>
#include <exception>
#include <stdexcept>

using namespace std::chrono_literals;
namespace mqtt = everest::lib::io::mqtt;

namespace {
const int mqtt_reconnect_to_ms = 1000;
}

namespace charge_bridge::evse_bsp {
api_connector::api_connector(everest_api_config const& config, std::string const& cb_identifier) :
    m_cb_identifier(cb_identifier),
    m_mqtt(mqtt_reconnect_to_ms),
    m_evse_bsp(config.evse, cb_identifier, m_host_status),
    m_ovm(config.ovm, cb_identifier, m_host_status),
    m_ev_bsp(config.ev, cb_identifier, m_host_status) {

    everest::lib::API::Topics api_topics;
    m_evse_bsp_enabled = config.evse.enabled;
    m_ovm_enabled = config.ovm.enabled;
    m_ev_bsp_enabled = config.ev.enabled;

    if (m_evse_bsp_enabled && m_ev_bsp_enabled) {
        throw std::runtime_error("Configuration error: Cannot enable EV and EVSE BSP at the same time");
    }
    utilities::print_error(m_cb_identifier, "BSP/CB", 0) << "ChargeBridge connected." << std::endl;

    if (m_evse_bsp_enabled) {
        api_topics.setup(config.evse.module_id, "evse_board_support", 1);
        m_evse_bsp_receive_topic = api_topics.everest_to_extern("");
        m_evse_bsp_send_topic = api_topics.extern_to_everest("");
        m_evse_bsp.set_mqtt_tx(
            [this](auto const& topic, auto const& payload) { m_mqtt.publish(m_evse_bsp_send_topic + topic, payload); });
    }
    if (m_ovm_enabled) {
        api_topics.setup(config.ovm.module_id, "over_voltage_monitor", 1);
        m_ovm_receive_topic = api_topics.everest_to_extern("");
        m_ovm_send_topic = api_topics.extern_to_everest("");
        m_ovm.set_mqtt_tx(
            [this](auto const& topic, auto const& payload) { m_mqtt.publish(m_ovm_send_topic + topic, payload); });
    }

    if (m_ev_bsp_enabled) {
        api_topics.setup(config.ev.module_id, "ev_board_support", 1);
        m_ev_bsp_receive_topic = api_topics.everest_to_extern("");
        m_ev_bsp_send_topic = api_topics.extern_to_everest("");
        m_ev_bsp.set_mqtt_tx(
            [this](auto const& topic, auto const& payload) { m_mqtt.publish(m_ev_bsp_send_topic + topic, payload); });
    }

    m_mqtt.set_error_handler([this](int code, std::string const& msg) {
        auto is_error = code == 0 ? 0 : 1;
        utilities::print_error(m_cb_identifier, "BSP/MQTT", is_error) << msg << std::endl;
    });

    m_mqtt.set_callback_connect([this, config](auto&, auto, auto, auto const&) { handle_mqtt_connect(); });

    m_mqtt.connect(config.mqtt_bind, config.mqtt_remote, config.mqtt_port, config.mqtt_ping_interval_ms);

    m_sync_timer.set_timeout(1s);

    std::memset(&m_host_status, 0, sizeof(m_host_status));
}

bool api_connector::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (m_evse_bsp_enabled) {
        result = handler.register_event_handler(&m_evse_bsp) && result;
    }
    if (m_ovm_enabled) {
        result = handler.register_event_handler(&m_ovm) && result;
    }
    if (m_ev_bsp_enabled) {
        result = handler.register_event_handler(&m_ev_bsp) && result;
    }
    result = handler.register_event_handler(&m_mqtt) && result;
    result = handler.register_event_handler(&m_sync_timer, [this](auto&) {
        if (m_evse_bsp_enabled) {
            m_evse_bsp.sync(m_cb_connected);
        }
        if (m_ovm_enabled) {
            m_ovm.sync(m_cb_connected);
        }
        if (m_ev_bsp_enabled) {
            m_ev_bsp.sync(m_cb_connected);
        }
        handle_cb_connection_state();
    }) && result;
    return result;
}

bool api_connector::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (m_evse_bsp_enabled) {
        result = handler.unregister_event_handler(&m_evse_bsp) && result;
    }
    if (m_ovm_enabled) {
        result = handler.unregister_event_handler(&m_ovm) && result;
    }
    if (m_ev_bsp_enabled) {
        result = handler.unregister_event_handler(&m_ev_bsp) && result;
    }
    result = handler.unregister_event_handler(&m_mqtt) && result;
    result = handler.unregister_event_handler(&m_sync_timer) && result;
    return result;
}

void api_connector::set_cb_tx(tx_ftor const& handler) {
    m_tx = handler;
    m_evse_bsp.set_cb_tx(handler);
    m_ev_bsp.set_cb_tx(handler);
}

void api_connector::set_cb_message(evse_bsp_cb_to_host const& msg) {
    m_last_cb_heartbeat = std::chrono::steady_clock::now();
    if (m_evse_bsp_enabled) {
        m_evse_bsp.set_cb_message(msg);
    }
    if (m_ev_bsp_enabled) {
        m_ev_bsp.set_cb_message(msg);
    }
    if (m_ovm_enabled) {
        m_ovm.set_cb_message(msg);
    }
}

bool api_connector::check_cb_heartbeat() {
    if (m_last_cb_heartbeat == std::chrono::steady_clock::time_point::max()) {
        return false;
    }
    return std::chrono::steady_clock::now() - m_last_cb_heartbeat < 2s;
}

void api_connector::handle_mqtt_connect() {
    if (m_evse_bsp_enabled) {
        m_mqtt.subscribe(m_evse_bsp_receive_topic + "#",
                         [this](auto&, auto const& topic, auto const& payload, auto, auto const&) {
                             auto operation = utilities::string_after_pattern(topic, m_evse_bsp_receive_topic);
                             if (not operation.empty()) {
                                 m_evse_bsp.dispatch(operation, static_cast<std::string>(payload));
                             }
                         });
    }
    if (m_ovm_enabled) {
        m_mqtt.subscribe(m_ovm_receive_topic + "#",
                         [this](auto&, auto const& topic, auto const& payload, auto, auto const&) {
                             auto operation = utilities::string_after_pattern(topic, m_ovm_receive_topic);
                             if (not operation.empty()) {
                                 m_ovm.dispatch(operation, static_cast<std::string>(payload));
                             }
                         });
    }
    if (m_ev_bsp_enabled) {
        m_mqtt.subscribe(m_ev_bsp_receive_topic + "#",
                         [this](auto&, auto const& topic, auto const& payload, auto, auto const&) {
                             auto operation = utilities::string_after_pattern(topic, m_ev_bsp_receive_topic);
                             if (not operation.empty()) {
                                 m_ev_bsp.dispatch(operation, static_cast<std::string>(payload));
                             }
                         });
    }
}

void api_connector::handle_cb_connection_state() {
    m_tx(m_host_status);
    auto current = check_cb_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "BSP/CB", 0) << "ChargeBridge connected." << std::endl;
            if (m_evse_bsp_enabled) {
                m_evse_bsp.clear_comm_fault();
            }
            if (m_ovm_enabled) {
                m_ovm.clear_comm_fault();
            }
            if (m_ev_bsp_enabled) {
                m_ev_bsp.clear_comm_fault();
            }

        } else {
            if (m_evse_bsp_enabled) {
                m_evse_bsp.raise_comm_fault();
            }
            if (m_ovm_enabled) {
                m_ovm.raise_comm_fault();
            }
            if (m_ev_bsp_enabled) {
                m_ev_bsp.raise_comm_fault();
            }

            utilities::print_error(m_cb_identifier, "BSP/CB", 1) << "Waiting for ChargeBridge...." << std::endl;
        }
    };
    if (m_cb_initial_comm_check) {
        handle_status(current);
        m_cb_initial_comm_check = false;
    }
    if (m_cb_connected != current) {
        handle_status(not m_cb_connected);
    }
    m_cb_connected = current;
}

} // namespace charge_bridge::evse_bsp
