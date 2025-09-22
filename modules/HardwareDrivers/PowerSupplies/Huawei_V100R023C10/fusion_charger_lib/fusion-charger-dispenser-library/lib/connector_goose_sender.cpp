// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "connector_goose_sender.hpp"

ConnectorGooseSender::ConnectorGooseSender(std::shared_ptr<goose_ethernet::EthernetInterfaceIntf> intf, bool secure,
                                           logs::LogIntf log) :
    secure(secure),
    logs(log),
    goose_sender(std::chrono::milliseconds(1000),
                 std::vector<std::chrono::milliseconds>{std::chrono::milliseconds(2), std::chrono::milliseconds(2),
                                                        std::chrono::milliseconds(4), std::chrono::milliseconds(8)},
                 intf, log) {
    for (int i = 0; i < 6; i++) {
        destination_mac_address[i] = 0;
    }
}

void ConnectorGooseSender::start() {
    goose_sender.start();
}

void ConnectorGooseSender::stop() {
    goose_sender.stop();
}

void ConnectorGooseSender::on_new_hmac_key(std::vector<uint8_t> hmac_key) {
    this->hmac_key = hmac_key;
}

void ConnectorGooseSender::on_new_mac_address(std::vector<uint8_t> mac_address) {
    for (int i = 0; i < 6; i++) {
        this->destination_mac_address[i] = mac_address[i];
    }
}

void ConnectorGooseSender::send_goose_frame(goose::frame::GoosePDU pdu, uint16_t appid) {
    if (secure && !hmac_key) {
        logs.info << "Not sending goose frame, because no hmac key was received";
        return;
    }

    std::unique_ptr<goose::sender::SendPacketIntf> packet;

    if (secure) {
        goose::frame::SecureGooseFrame frame;
        memcpy(frame.destination_mac_address, destination_mac_address, 6);
        memcpy(frame.source_mac_address, goose_sender.get_mac_address(), 6);
        frame.appid[0] = (appid >> 8) & 0xff;
        frame.appid[1] = (appid >> 0) & 0xff;
        frame.pdu = pdu;
        frame.vlan_id = 0;
        frame.priority = 5;
        packet = std::make_unique<goose::sender::SendPacketSecure>(frame, hmac_key.value());
    } else {
        goose::frame::GooseFrame frame;
        memcpy(frame.destination_mac_address, destination_mac_address, 6);
        memcpy(frame.source_mac_address, goose_sender.get_mac_address(), 6);
        frame.appid[0] = (appid >> 8) & 0xff;
        frame.appid[1] = (appid >> 0) & 0xff;
        frame.pdu = pdu;
        frame.vlan_id = 0;
        frame.priority = 5;
        packet = std::make_unique<goose::sender::SendPacketNormal>(frame);
    }

    goose_sender.send(std::move(packet));
}

void ConnectorGooseSender::send_stop_request(uint16_t connector_no) {
    logs.verbose << "Sending stop request for connector ";

    auto request = fusion_charger::goose::StopChargeRequest{
        .charging_connector_no = connector_no,
        .charging_sn = 0xffff,
        .reason = fusion_charger::goose::StopChargeRequest::Reason::NORMAL,
    };

    send_goose_frame(request.to_pdu(), 0x3002);
}

void ConnectorGooseSender::send_power_requirement(uint16_t connector_no, PowerRequirement requirement) {
    logs.verbose << "Sending power requirement for connector ";
    auto request = fusion_charger::goose::PowerRequirementRequest{
        .charging_connector_no = connector_no,
        .charging_sn = 0xffff,
        .requirement_type = requirement.type,
        .mode = requirement.mode,
        .voltage = requirement.voltage,
        .current = requirement.current,
    };

    send_goose_frame(request.to_pdu(), 0x0001);
}

void ConnectorGooseSender::send_module_placeholder_request(uint16_t connector_no) {
    send_power_requirement(connector_no, {
                                             .type = fusion_charger::goose::RequirementType::ModulePlaceholderRequest,
                                             .mode = fusion_charger::goose::Mode::ConstantCurrent, // todo: None?
                                             .current = 0,
                                             .voltage = 0,
                                         });
}

void ConnectorGooseSender::send_insulation_detection_voltage_output(uint16_t connector_no, float voltage,
                                                                    float current) {
    send_power_requirement(connector_no,
                           {
                               .type = fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutput,
                               .mode = fusion_charger::goose::Mode::ConstantCurrent,
                               .current = current,
                               .voltage = voltage,
                           });
}

void ConnectorGooseSender::send_insulation_detection_voltage_output_stoppage(uint16_t connector_no) {
    send_power_requirement(connector_no,
                           {
                               .type = fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutputStoppage,
                               .mode = fusion_charger::goose::Mode::ConstantCurrent,
                               .current = 0,
                               .voltage = 0,
                           });
}

void ConnectorGooseSender::send_precharge_voltage_output(uint16_t connector_no, float voltage, float current) {
    send_power_requirement(connector_no, {
                                             .type = fusion_charger::goose::RequirementType::PrechargeVoltageOutput,
                                             .mode = fusion_charger::goose::Mode::ConstantCurrent,
                                             .current = current,
                                             .voltage = voltage,
                                         });
}

void ConnectorGooseSender::send_charging_voltage_output(uint16_t connector_no, float voltage, float current) {
    send_power_requirement(connector_no, {
                                             .type = fusion_charger::goose::RequirementType::Charging,
                                             .mode = fusion_charger::goose::Mode::ConstantCurrent,
                                             .current = current,
                                             .voltage = voltage,
                                         });
}
