#include "charge_bridge/utilities/string.hpp"
#include "everest/io/mqtt/dataset.hpp"
#include "protocol/evse_bsp_cb_to_host.h"
#include <charge_bridge/everest_api/api_connector.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <cstring>

using namespace std::chrono_literals;

namespace charge_bridge::evse_bsp {
api_connector::api_connector(everest_api_config const& config, std::string const& cb_identifier) :
    m_cb_identifier(cb_identifier),
    m_mqtt(config.mqtt_remote, config.mqtt_port, config.mqtt_ping_interval_ms),
    m_bsp(config.bsp, cb_identifier, m_host_status),
    m_ovm(config.ovm, cb_identifier, m_host_status) {

    everest::lib::API::Topics api_topics;
    api_topics.setTargetApiModuleID(config.bsp.module_id, "evse_board_support");
    m_bsp_receive_topic = api_topics.everest_to_extern("");
    m_bsp_send_topic = api_topics.extern_to_everest("");
    m_bsp.set_mqtt_tx([this](auto& val) {
        val.topic = m_bsp_send_topic + val.topic;
        m_mqtt.tx(val);
    });
    m_mqtt.subscribe(m_bsp_receive_topic + "#");

    m_ovm_enabled = config.ovm.enabled;
    if (m_ovm_enabled) {
        api_topics.setTargetApiModuleID(config.ovm.module_id, "over_voltage_monitor");
        m_ovm_receive_topic = api_topics.everest_to_extern("");
        m_ovm_send_topic = api_topics.extern_to_everest("");
        m_ovm.set_mqtt_tx([this](auto& val) {
            val.topic = m_ovm_send_topic + val.topic;
            m_mqtt.tx(val);
        });
        m_mqtt.subscribe(m_ovm_receive_topic + "#");
    }

    m_mqtt.set_message_handler([this](auto& data, auto&) { dispatch(data); });
    m_mqtt.set_error_handler([this, config](int id, std::string const& msg) {
        m_mqtt_on_error = id not_eq 0;
        utilities::print_error(m_cb_identifier, "MQTT/EVSE", id) << msg << std::endl;
    });

    m_mqtt_timer.set_timeout(5s);
    m_sync_timer.set_timeout(1s);

    std::memset(&m_host_status, 0, sizeof(m_host_status));
}

void api_connector::dispatch(everest::lib::io::mqtt::Dataset const& data) {
    auto& topic = data.topic;
    auto& payload = data.message;

    auto operation = utilities::string_after_pattern(topic, m_bsp_receive_topic);
    if (not operation.empty()) {
        m_bsp.dispatch(operation, payload);
        return;
    }
    if (m_ovm_enabled) {
        operation = utilities::string_after_pattern(topic, m_ovm_receive_topic);
        if (not operation.empty()) {
            m_ovm.dispatch(operation, payload);
            return;
        }
    }
}

bool api_connector::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = handler.register_event_handler(&m_bsp);
    if (m_ovm_enabled) {
        result = handler.register_event_handler(&m_ovm) && result;
    }
    result = handler.register_event_handler(&m_mqtt) && result;
    result = handler.register_event_handler(&m_mqtt_timer, [this](auto&) {
        if (m_mqtt_on_error) {
            m_mqtt.reset();
        }
    }) && result;
    result = handler.register_event_handler(&m_sync_timer, [this](auto&) {
        m_bsp.sync(m_cb_connected);
        if (m_ovm_enabled) {
            m_ovm.sync(m_cb_connected);
        }
        handle_cb_connection_state();
    }) && result;
    return result;
}

bool api_connector::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = handler.unregister_event_handler(&m_mqtt);
    result = handler.unregister_event_handler(&m_mqtt_timer) && result;
    result = handler.unregister_event_handler(&m_bsp) && result;
    result = handler.unregister_event_handler(&m_sync_timer) && result;
    return result;
}

void api_connector::set_cb_tx(tx_ftor const& handler) {
    m_tx = handler;
    m_bsp.set_cb_tx(handler);
}

void api_connector::set_cb_message(evse_bsp_cb_to_host const& msg) {
    m_last_cb_heartbeat = std::chrono::steady_clock::now();
    m_bsp.set_cb_message(msg);
    m_ovm.set_cb_message(msg);
}

bool api_connector::check_cb_heartbeat() {
    if (m_last_cb_heartbeat == std::chrono::steady_clock::time_point::max()) {
        return false;
    }
    return std::chrono::steady_clock::now() - m_last_cb_heartbeat < 2s;
}

void api_connector::handle_cb_connection_state() {
    m_tx(m_host_status);
    auto current = check_cb_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "EVSE/CB", 0) << "ChargeBridge connected." << std::endl;
            m_bsp.clear_comm_fault();
            if (m_ovm_enabled) {
                m_ovm.clear_comm_fault();
            }

        } else {
            m_bsp.raise_comm_fault();
            if (m_ovm_enabled) {
                m_ovm.raise_comm_fault();
            }

            utilities::print_error(m_cb_identifier, "EVSE/CB", 1) << "Waiting for ChargeBridge...." << std::endl;
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
