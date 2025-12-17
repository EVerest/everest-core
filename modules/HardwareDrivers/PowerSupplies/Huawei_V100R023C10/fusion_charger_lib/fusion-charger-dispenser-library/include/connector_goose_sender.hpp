// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <fusion_charger/goose/power_request.hpp>
#include <fusion_charger/goose/stop_charge_request.hpp>
#include <goose/sender.hpp>
#include <logs/logs.hpp>

struct PowerRequirement {
    fusion_charger::goose::RequirementType type;
    fusion_charger::goose::Mode mode;
    float current;
    float voltage;
};

class ConnectorGooseSender {
    goose::sender::Sender goose_sender;
    std::optional<std::vector<std::uint8_t>> hmac_key;
    logs::LogIntf logs;
    bool secure;

    void send_goose_frame(goose::frame::GoosePDU pdu, std::uint16_t appid);
    std::uint8_t destination_mac_address[6];

public:
    ConnectorGooseSender(std::shared_ptr<goose_ethernet::EthernetInterfaceIntf> intf, bool secure = false,
                         logs::LogIntf log = logs::log_printf);

    /**
     * @brief Start the sender thread (analogue to
     * \c goose::sender::Sender::start)
     *
     */
    void start();
    /**
     * @brief Stop the sender thread (analogue to
     * \c goose::sender::Sender::stop)
     *
     */
    void stop();

    /**
     * @brief Call this if the dispenser receives a new hmac key
     *
     * @param hmac_key the new hmac key
     */
    void on_new_hmac_key(std::vector<std::uint8_t> hmac_key);
    /**
     * @brief Call this if the dispenser receives a new destination mac address
     *
     * @param mac_address the new destination mac address
     */
    void on_new_mac_address(std::vector<std::uint8_t> mac_address);

    /**
     * @brief Send a stop request Goose frame; either secure or insecure,
     * depending on whether the secure flag was sent in the constructor and if the
     * hmac key was received
     *
     * @param connector_no The global connector number
     */
    void send_stop_request(std::uint16_t connector_no);

    /**
     * @brief Send a PowerRequirement goose frame; either secure or insecure (see
     * \c send_stop_request)
     *
     *
     * @param connector_no The global connector number
     * @param requirement The power requirement which is converted to a goose
     * frame
     */
    void send_power_requirement(std::uint16_t connector_no, PowerRequirement requirement);

    /**
     * @brief Send a PowerRequirement frame with type ModulePlaceholderRequest via
     * \c send_power_requirement
     *
     * @param connector_no The global connector number
     */
    void send_module_placeholder_request(std::uint16_t connector_no);

    /**
     * @brief Send an PowerRequirement frame with type
     * InsulationDetectionVoltageOutput via \c send_power_requirement
     *
     * @param connector_no The global connector number
     * @param voltage The requested voltage
     * @param current The requested current
     */
    void send_insulation_detection_voltage_output(std::uint16_t connector_no, float voltage, float current);

    /**
     * @brief Send an PowerRequirement frame with type
     * InsulationDetectionVoltageOutputStoppage via \c send_power_requirement
     *
     * @param connector_no The global connector number
     */
    void send_insulation_detection_voltage_output_stoppage(std::uint16_t connector_no);

    /**
     * @brief Send an PowerRequirement frame with type PrechargeVoltageOutput via
     * \c send_power_requirement
     *
     * @param connector_no The global connector number
     * @param voltage The requested voltage
     * @param current The requested current
     */
    void send_precharge_voltage_output(std::uint16_t connector_no, float voltage, float current);

    /**
     * @brief Send an PowerRequirement frame with type RequirementDuringCharging
     * via \c send_power_requirement
     *
     * @param connector_no The global connector number
     * @param voltage The requested voltage
     * @param current The requested current
     */
    void send_charging_voltage_output(std::uint16_t connector_no, float voltage, float current);
};
