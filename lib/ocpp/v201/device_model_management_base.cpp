// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

// AUTO GENERATED - DO NOT EDIT!

#include <ocpp/v201/device_model_management_base.hpp>

namespace ocpp {
namespace v201 {

// from: StandardizedComponent
namespace conversions {
std::string standardized_component_to_string(StandardizedComponent e) {
    switch (e) {
    case StandardizedComponent::AuthCtrlr:
        return "AuthCtrlr";
    case StandardizedComponent::InternalCtrlr:
        return "InternalCtrlr";
    case StandardizedComponent::SampledDataCtrlr:
        return "SampledDataCtrlr";
    case StandardizedComponent::OCPPCommCtrlr:
        return "OCPPCommCtrlr";
    case StandardizedComponent::FiscalMetering:
        return "FiscalMetering";
    case StandardizedComponent::ChargingStation:
        return "ChargingStation";
    case StandardizedComponent::DistributionPanel:
        return "DistributionPanel";
    case StandardizedComponent::Controller:
        return "Controller";
    case StandardizedComponent::MonitoringCtrlr:
        return "MonitoringCtrlr";
    case StandardizedComponent::ClockCtrlr:
        return "ClockCtrlr";
    case StandardizedComponent::SmartChargingCtrlr:
        return "SmartChargingCtrlr";
    case StandardizedComponent::LocalEnergyStorage:
        return "LocalEnergyStorage";
    case StandardizedComponent::AlignedDataCtrlr:
        return "AlignedDataCtrlr";
    case StandardizedComponent::EVSE:
        return "EVSE";
    case StandardizedComponent::TariffCostCtrlr:
        return "TariffCostCtrlr";
    case StandardizedComponent::DisplayMessageCtrlr:
        return "DisplayMessageCtrlr";
    case StandardizedComponent::SecurityCtrlr:
        return "SecurityCtrlr";
    case StandardizedComponent::ConnectedEV:
        return "ConnectedEV";
    case StandardizedComponent::CustomizationCtrlr:
        return "CustomizationCtrlr";
    case StandardizedComponent::Connector:
        return "Connector";
    case StandardizedComponent::DeviceDataCtrlr:
        return "DeviceDataCtrlr";
    case StandardizedComponent::LocalAuthListCtrlr:
        return "LocalAuthListCtrlr";
    case StandardizedComponent::TokenReader:
        return "TokenReader";
    case StandardizedComponent::CPPWMController:
        return "CPPWMController";
    case StandardizedComponent::ISO15118Ctrlr:
        return "ISO15118Ctrlr";
    case StandardizedComponent::TxCtrlr:
        return "TxCtrlr";
    case StandardizedComponent::ReservationCtrlr:
        return "ReservationCtrlr";
    case StandardizedComponent::AuthCacheCtrlr:
        return "AuthCacheCtrlr";
    case StandardizedComponent::CHAdeMOCtrlr:
        return "CHAdeMOCtrlr";
    }

    throw std::out_of_range("No known string conversion for provided enum of type StandardizedComponent");
}

StandardizedComponent string_to_standardized_component(const std::string& s) {
    if (s == "AuthCtrlr") {
        return StandardizedComponent::AuthCtrlr;
    }
    if (s == "InternalCtrlr") {
        return StandardizedComponent::InternalCtrlr;
    }
    if (s == "SampledDataCtrlr") {
        return StandardizedComponent::SampledDataCtrlr;
    }
    if (s == "OCPPCommCtrlr") {
        return StandardizedComponent::OCPPCommCtrlr;
    }
    if (s == "FiscalMetering") {
        return StandardizedComponent::FiscalMetering;
    }
    if (s == "ChargingStation") {
        return StandardizedComponent::ChargingStation;
    }
    if (s == "DistributionPanel") {
        return StandardizedComponent::DistributionPanel;
    }
    if (s == "Controller") {
        return StandardizedComponent::Controller;
    }
    if (s == "MonitoringCtrlr") {
        return StandardizedComponent::MonitoringCtrlr;
    }
    if (s == "ClockCtrlr") {
        return StandardizedComponent::ClockCtrlr;
    }
    if (s == "SmartChargingCtrlr") {
        return StandardizedComponent::SmartChargingCtrlr;
    }
    if (s == "LocalEnergyStorage") {
        return StandardizedComponent::LocalEnergyStorage;
    }
    if (s == "AlignedDataCtrlr") {
        return StandardizedComponent::AlignedDataCtrlr;
    }
    if (s == "EVSE") {
        return StandardizedComponent::EVSE;
    }
    if (s == "TariffCostCtrlr") {
        return StandardizedComponent::TariffCostCtrlr;
    }
    if (s == "DisplayMessageCtrlr") {
        return StandardizedComponent::DisplayMessageCtrlr;
    }
    if (s == "SecurityCtrlr") {
        return StandardizedComponent::SecurityCtrlr;
    }
    if (s == "ConnectedEV") {
        return StandardizedComponent::ConnectedEV;
    }
    if (s == "CustomizationCtrlr") {
        return StandardizedComponent::CustomizationCtrlr;
    }
    if (s == "Connector") {
        return StandardizedComponent::Connector;
    }
    if (s == "DeviceDataCtrlr") {
        return StandardizedComponent::DeviceDataCtrlr;
    }
    if (s == "LocalAuthListCtrlr") {
        return StandardizedComponent::LocalAuthListCtrlr;
    }
    if (s == "TokenReader") {
        return StandardizedComponent::TokenReader;
    }
    if (s == "CPPWMController") {
        return StandardizedComponent::CPPWMController;
    }
    if (s == "ISO15118Ctrlr") {
        return StandardizedComponent::ISO15118Ctrlr;
    }
    if (s == "TxCtrlr") {
        return StandardizedComponent::TxCtrlr;
    }
    if (s == "ReservationCtrlr") {
        return StandardizedComponent::ReservationCtrlr;
    }
    if (s == "AuthCacheCtrlr") {
        return StandardizedComponent::AuthCacheCtrlr;
    }
    if (s == "CHAdeMOCtrlr") {
        return StandardizedComponent::CHAdeMOCtrlr;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type StandardizedComponent");
}
}

std::ostream& operator<<(std::ostream& os, const StandardizedComponent& standardized_component) {
    os << conversions::standardized_component_to_string(standardized_component);
    return os;
}

bool DeviceModelManagerBase::exists(const StandardizedComponent &standardized_component, const std::string &variable_name, const AttributeEnum &attribute_enum) {
    return this->components.count(standardized_component) and 
           this->components.at(standardized_component).variables.count(variable_name) and 
           this->components.at(standardized_component).variables.at(variable_name).attributes.count(attribute_enum) and 
           this->components.at(standardized_component)
                .variables.at(variable_name)
                .attributes.at(attribute_enum)
                .value.has_value();
}

boost::optional<bool> DeviceModelManagerBase::get_internal_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "InternalCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("InternalCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_internal_ctrlr_enabled(const bool &internal_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "InternalCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("InternalCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(internal_ctrlr_enabled));
        }
}

std::string DeviceModelManagerBase::get_charge_point_id(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("ChargePointId")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_central_system_uri(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("CentralSystemURI")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_charge_box_serial_number(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("ChargeBoxSerialNumber")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_charge_point_model(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("ChargePointModel")
        .attributes.at(attribute_enum)
        .value.value();
}


boost::optional<std::string> DeviceModelManagerBase::get_charge_point_serial_number(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "ChargePointSerialNumber", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("ChargePointSerialNumber")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_charge_point_vendor(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("ChargePointVendor")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_firmware_version(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("FirmwareVersion")
        .attributes.at(attribute_enum)
        .value.value();
}


boost::optional<std::string> DeviceModelManagerBase::get_iccid(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "ICCID", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("ICCID")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_imsi(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "IMSI", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("IMSI")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_meter_serial_number(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "MeterSerialNumber", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("MeterSerialNumber")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_meter_type(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "MeterType", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("MeterType")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_supported_ciphers12(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("SupportedCiphers12")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_supported_ciphers13(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("SupportedCiphers13")
        .attributes.at(attribute_enum)
        .value.value();
}


int DeviceModelManagerBase::get_websocket_reconnect_interval(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("WebsocketReconnectInterval")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_authorize_connector_zero_on_connector_one(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "AuthorizeConnectorZeroOnConnectorOne", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("AuthorizeConnectorZeroOnConnectorOne")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_log_messages(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "LogMessages", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("LogMessages")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_log_messages_format(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "LogMessagesFormat", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("LogMessagesFormat")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_supported_charging_profile_purpose_types(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "SupportedChargingProfilePurposeTypes", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("SupportedChargingProfilePurposeTypes")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_max_composite_schedule_duration(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "MaxCompositeScheduleDuration", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("MaxCompositeScheduleDuration")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_number_of_connectors(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("NumberOfConnectors")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_use_ssl_default_verify_paths(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "UseSslDefaultVerifyPaths", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::InternalCtrlr)
        .variables.at("UseSslDefaultVerifyPaths")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_ocsp_request_interval(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "OcspRequestInterval", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("OcspRequestInterval")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_ocsp_request_interval(const int &ocsp_request_interval, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "OcspRequestInterval", attribute_enum)) {
            this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("OcspRequestInterval")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(ocsp_request_interval));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_websocket_ping_payload(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::InternalCtrlr, "WebsocketPingPayload", attribute_enum)) {
            return this->components.at(StandardizedComponent::InternalCtrlr)
            .variables.at("WebsocketPingPayload")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_aligned_data_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_aligned_data_ctrlr_enabled(const bool &aligned_data_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::AlignedDataCtrlr)
            .variables.at("AlignedDataCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(aligned_data_ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_aligned_data_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_aligned_data_interval(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataInterval")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_aligned_data_interval(const int &aligned_data_interval, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataInterval")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(aligned_data_interval));
}

std::string DeviceModelManagerBase::get_aligned_data_measurands(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataMeasurands")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_aligned_data_measurands(const std::string &aligned_data_measurands, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataMeasurands")
        .attributes.at(attribute_enum)
        .value.emplace(aligned_data_measurands);
}

boost::optional<bool> DeviceModelManagerBase::get_aligned_data_send_during_idle(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataSendDuringIdle", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataSendDuringIdle")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_aligned_data_send_during_idle(const bool &aligned_data_send_during_idle, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataSendDuringIdle", attribute_enum)) {
            this->components.at(StandardizedComponent::AlignedDataCtrlr)
            .variables.at("AlignedDataSendDuringIdle")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(aligned_data_send_during_idle));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_aligned_data_sign_readings(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataSignReadings", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataSignReadings")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_aligned_data_sign_readings(const bool &aligned_data_sign_readings, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AlignedDataCtrlr, "AlignedDataSignReadings", attribute_enum)) {
            this->components.at(StandardizedComponent::AlignedDataCtrlr)
            .variables.at("AlignedDataSignReadings")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(aligned_data_sign_readings));
        }
}

int DeviceModelManagerBase::get_aligned_data_tx_ended_interval(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataTxEndedInterval")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_aligned_data_tx_ended_interval(const int &aligned_data_tx_ended_interval, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataTxEndedInterval")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(aligned_data_tx_ended_interval));
}

std::string DeviceModelManagerBase::get_aligned_data_tx_ended_measurands(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataTxEndedMeasurands")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_aligned_data_tx_ended_measurands(const std::string &aligned_data_tx_ended_measurands, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AlignedDataCtrlr)
        .variables.at("AlignedDataTxEndedMeasurands")
        .attributes.at(attribute_enum)
        .value.emplace(aligned_data_tx_ended_measurands);
}

boost::optional<bool> DeviceModelManagerBase::get_auth_cache_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCacheCtrlr)
        .variables.at("AuthCacheCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_auth_cache_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCacheCtrlr)
        .variables.at("AuthCacheCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_auth_cache_ctrlr_enabled(const bool &auth_cache_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCacheCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(auth_cache_ctrlr_enabled));
        }
}

boost::optional<int> DeviceModelManagerBase::get_auth_cache_life_time(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheLifeTime", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCacheLifeTime")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_auth_cache_life_time(const int &auth_cache_life_time, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheLifeTime", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCacheLifeTime")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(auth_cache_life_time));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_auth_cache_policy(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCachePolicy", attribute_enum)) {
            return this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCachePolicy")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_auth_cache_policy(const std::string &auth_cache_policy, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCachePolicy", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCachePolicy")
            .attributes.at(attribute_enum)
            .value.emplace(auth_cache_policy);
        }
}

boost::optional<int> DeviceModelManagerBase::get_auth_cache_storage(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheStorage", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCacheStorage")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_auth_cache_disable_post_authorize(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheDisablePostAuthorize", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCacheCtrlr)
        .variables.at("AuthCacheDisablePostAuthorize")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_auth_cache_disable_post_authorize(const bool &auth_cache_disable_post_authorize, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCacheCtrlr, "AuthCacheDisablePostAuthorize", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCacheCtrlr)
            .variables.at("AuthCacheDisablePostAuthorize")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(auth_cache_disable_post_authorize));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_auth_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "AuthCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("AuthCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_auth_ctrlr_enabled(const bool &auth_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "AuthCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("AuthCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(auth_ctrlr_enabled));
        }
}

boost::optional<int> DeviceModelManagerBase::get_additional_info_items_per_message(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "AdditionalInfoItemsPerMessage", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("AdditionalInfoItemsPerMessage")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


bool DeviceModelManagerBase::get_authorize_remote_start(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("AuthorizeRemoteStart")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_authorize_remote_start(const bool &authorize_remote_start, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("AuthorizeRemoteStart")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(authorize_remote_start));
}

bool DeviceModelManagerBase::get_local_authorize_offline(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("LocalAuthorizeOffline")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_local_authorize_offline(const bool &local_authorize_offline, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("LocalAuthorizeOffline")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(local_authorize_offline));
}

bool DeviceModelManagerBase::get_local_pre_authorize(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("LocalPreAuthorize")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_local_pre_authorize(const bool &local_pre_authorize, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("LocalPreAuthorize")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(local_pre_authorize));
}

boost::optional<std::string> DeviceModelManagerBase::get_master_pass_group_id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "MasterPassGroupId", attribute_enum)) {
            return this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("MasterPassGroupId")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_master_pass_group_id(const std::string &master_pass_group_id, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "MasterPassGroupId", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("MasterPassGroupId")
            .attributes.at(attribute_enum)
            .value.emplace(master_pass_group_id);
        }
}

boost::optional<bool> DeviceModelManagerBase::get_offline_tx_for_unknown_id_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "OfflineTxForUnknownIdEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("OfflineTxForUnknownIdEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_offline_tx_for_unknown_id_enabled(const bool &offline_tx_for_unknown_id_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "OfflineTxForUnknownIdEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("OfflineTxForUnknownIdEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(offline_tx_for_unknown_id_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_disable_remote_authorization(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "DisableRemoteAuthorization", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::AuthCtrlr)
        .variables.at("DisableRemoteAuthorization")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_disable_remote_authorization(const bool &disable_remote_authorization, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::AuthCtrlr, "DisableRemoteAuthorization", attribute_enum)) {
            this->components.at(StandardizedComponent::AuthCtrlr)
            .variables.at("DisableRemoteAuthorization")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(disable_remote_authorization));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_chade_moctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "CHAdeMOCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("CHAdeMOCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_chade_moctrlr_enabled(const bool &chade_moctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "CHAdeMOCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::CHAdeMOCtrlr)
            .variables.at("CHAdeMOCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(chade_moctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_selftest_active(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "SelftestActive", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("SelftestActive")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_chade_moprotocol_number(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "CHAdeMOProtocolNumber", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
            .variables.at("CHAdeMOProtocolNumber")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_vehicle_status(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "VehicleStatus", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("VehicleStatus")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_dynamic_control(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "DynamicControl", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("DynamicControl")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_high_current_control(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "HighCurrentControl", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("HighCurrentControl")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_high_voltage_control(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "HighVoltageControl", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
        .variables.at("HighVoltageControl")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_auto_manufacturer_code(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CHAdeMOCtrlr, "AutoManufacturerCode", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::CHAdeMOCtrlr)
            .variables.at("AutoManufacturerCode")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_allow_new_sessions_pending_firmware_update(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ChargingStation, "AllowNewSessionsPendingFirmwareUpdate", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ChargingStation)
        .variables.at("AllowNewSessionsPendingFirmwareUpdate")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_allow_new_sessions_pending_firmware_update(const bool &allow_new_sessions_pending_firmware_update, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ChargingStation, "AllowNewSessionsPendingFirmwareUpdate", attribute_enum)) {
            this->components.at(StandardizedComponent::ChargingStation)
            .variables.at("AllowNewSessionsPendingFirmwareUpdate")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(allow_new_sessions_pending_firmware_update));
        }
}

std::string DeviceModelManagerBase::get_charging_station_availability_state(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::ChargingStation)
        .variables.at("ChargingStationAvailabilityState")
        .attributes.at(attribute_enum)
        .value.value();
}


bool DeviceModelManagerBase::get_charging_station_available(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ChargingStation)
        .variables.at("ChargingStationAvailable")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<std::string> DeviceModelManagerBase::get_model(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ChargingStation, "Model", attribute_enum)) {
            return this->components.at(StandardizedComponent::ChargingStation)
            .variables.at("Model")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_charging_station_supply_phases(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::ChargingStation)
        .variables.at("ChargingStationSupplyPhases")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<std::string> DeviceModelManagerBase::get_vendor_name(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ChargingStation, "VendorName", attribute_enum)) {
            return this->components.at(StandardizedComponent::ChargingStation)
            .variables.at("VendorName")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_clock_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "ClockCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ClockCtrlr)
        .variables.at("ClockCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_clock_ctrlr_enabled(const bool &clock_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "ClockCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("ClockCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(clock_ctrlr_enabled));
        }
}

ocpp::DateTime DeviceModelManagerBase::get_date_time(const AttributeEnum &attribute_enum) {
    return ocpp::DateTime(this->components.at(StandardizedComponent::ClockCtrlr)
        .variables.at("DateTime")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<ocpp::DateTime> DeviceModelManagerBase::get_next_time_offset_transition_date_time(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NextTimeOffsetTransitionDateTime", attribute_enum)) {
            return ocpp::DateTime(this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NextTimeOffsetTransitionDateTime")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_next_time_offset_transition_date_time(const ocpp::DateTime &next_time_offset_transition_date_time, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NextTimeOffsetTransitionDateTime", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NextTimeOffsetTransitionDateTime")
            .attributes.at(attribute_enum)
            .value.emplace(next_time_offset_transition_date_time.to_rfc3339());
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_ntp_server_uri(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NtpServerUri", attribute_enum)) {
            return this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NtpServerUri")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_ntp_server_uri(const std::string &ntp_server_uri, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NtpServerUri", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NtpServerUri")
            .attributes.at(attribute_enum)
            .value.emplace(ntp_server_uri);
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_ntp_source(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NtpSource", attribute_enum)) {
            return this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NtpSource")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_ntp_source(const std::string &ntp_source, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "NtpSource", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("NtpSource")
            .attributes.at(attribute_enum)
            .value.emplace(ntp_source);
        }
}

boost::optional<int> DeviceModelManagerBase::get_time_adjustment_reporting_threshold(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeAdjustmentReportingThreshold", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeAdjustmentReportingThreshold")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_time_adjustment_reporting_threshold(const int &time_adjustment_reporting_threshold, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeAdjustmentReportingThreshold", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeAdjustmentReportingThreshold")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(time_adjustment_reporting_threshold));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_time_offset(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeOffset", attribute_enum)) {
            return this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeOffset")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_time_offset(const std::string &time_offset, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeOffset", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeOffset")
            .attributes.at(attribute_enum)
            .value.emplace(time_offset);
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_time_offset_next_transition(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeOffsetNextTransition", attribute_enum)) {
            return this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeOffsetNextTransition")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_time_offset_next_transition(const std::string &time_offset_next_transition, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeOffsetNextTransition", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeOffsetNextTransition")
            .attributes.at(attribute_enum)
            .value.emplace(time_offset_next_transition);
        }
}

std::string DeviceModelManagerBase::get_time_source(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::ClockCtrlr)
        .variables.at("TimeSource")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_time_source(const std::string &time_source, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::ClockCtrlr)
        .variables.at("TimeSource")
        .attributes.at(attribute_enum)
        .value.emplace(time_source);
}

boost::optional<std::string> DeviceModelManagerBase::get_time_zone(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeZone", attribute_enum)) {
            return this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeZone")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_time_zone(const std::string &time_zone, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ClockCtrlr, "TimeZone", attribute_enum)) {
            this->components.at(StandardizedComponent::ClockCtrlr)
            .variables.at("TimeZone")
            .attributes.at(attribute_enum)
            .value.emplace(time_zone);
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_protocol_agreed(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ConnectedEV, "ProtocolAgreed", attribute_enum)) {
            return this->components.at(StandardizedComponent::ConnectedEV)
            .variables.at("ProtocolAgreed")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_protocol_supported_by_ev_priority_(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ConnectedEV, "ProtocolSupportedByEV<Priority>", attribute_enum)) {
            return this->components.at(StandardizedComponent::ConnectedEV)
            .variables.at("ProtocolSupportedByEV<Priority>")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_vehicle_id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ConnectedEV, "VehicleID", attribute_enum)) {
            return this->components.at(StandardizedComponent::ConnectedEV)
            .variables.at("VehicleID")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_connector_availability_state(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::Connector, "ConnectorAvailabilityState", attribute_enum)) {
            return this->components.at(StandardizedComponent::Connector)
            .variables.at("ConnectorAvailabilityState")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


bool DeviceModelManagerBase::get_connector_available(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::Connector)
        .variables.at("ConnectorAvailable")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<std::string> DeviceModelManagerBase::get_charge_protocol(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::Connector, "ChargeProtocol", attribute_enum)) {
            return this->components.at(StandardizedComponent::Connector)
            .variables.at("ChargeProtocol")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_connector_type(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::Connector)
        .variables.at("ConnectorType")
        .attributes.at(attribute_enum)
        .value.value();
}


int DeviceModelManagerBase::get_connector_supply_phases(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::Connector)
        .variables.at("ConnectorSupplyPhases")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_max_msg_elements(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::Controller, "MaxMsgElements", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::Controller)
            .variables.at("MaxMsgElements")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_cppwmcontroller_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CPPWMController, "CPPWMControllerEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CPPWMController)
        .variables.at("CPPWMControllerEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_cppwmcontroller_enabled(const bool &cppwmcontroller_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CPPWMController, "CPPWMControllerEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::CPPWMController)
            .variables.at("CPPWMControllerEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(cppwmcontroller_enabled));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_state(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CPPWMController, "State", attribute_enum)) {
            return this->components.at(StandardizedComponent::CPPWMController)
            .variables.at("State")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_customization_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CustomizationCtrlr, "CustomizationCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CustomizationCtrlr)
        .variables.at("CustomizationCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_customization_ctrlr_enabled(const bool &customization_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CustomizationCtrlr, "CustomizationCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::CustomizationCtrlr)
            .variables.at("CustomizationCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(customization_ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_custom_implementation_enabled_vendor_id_(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CustomizationCtrlr, "CustomImplementationEnabled<vendorId>", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::CustomizationCtrlr)
        .variables.at("CustomImplementationEnabled<vendorId>")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_custom_implementation_enabled_vendor_id_(const bool &custom_implementation_enabled_vendor_id_, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::CustomizationCtrlr, "CustomImplementationEnabled<vendorId>", attribute_enum)) {
            this->components.at(StandardizedComponent::CustomizationCtrlr)
            .variables.at("CustomImplementationEnabled<vendorId>")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(custom_implementation_enabled_vendor_id_));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_device_data_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DeviceDataCtrlr, "DeviceDataCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("DeviceDataCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_device_data_ctrlr_enabled(const bool &device_data_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DeviceDataCtrlr, "DeviceDataCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::DeviceDataCtrlr)
            .variables.at("DeviceDataCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(device_data_ctrlr_enabled));
        }
}

int DeviceModelManagerBase::get_bytes_per_message_get_report(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("BytesPerMessageGetReport")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_bytes_per_message_get_variables(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("BytesPerMessageGetVariables")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_bytes_per_message_set_variables(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("BytesPerMessageSetVariables")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_configuration_value_size(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DeviceDataCtrlr, "ConfigurationValueSize", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
            .variables.at("ConfigurationValueSize")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_items_per_message_get_report(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("ItemsPerMessageGetReport")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_items_per_message_get_variables(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("ItemsPerMessageGetVariables")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_items_per_message_set_variables(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
        .variables.at("ItemsPerMessageSetVariables")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_reporting_value_size(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DeviceDataCtrlr, "ReportingValueSize", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
            .variables.at("ReportingValueSize")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_value_size(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DeviceDataCtrlr, "ValueSize", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::DeviceDataCtrlr)
            .variables.at("ValueSize")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_display_message_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DisplayMessageCtrlr, "DisplayMessageCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::DisplayMessageCtrlr)
        .variables.at("DisplayMessageCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_number_of_display_messages(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::DisplayMessageCtrlr)
        .variables.at("NumberOfDisplayMessages")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_display_message_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DisplayMessageCtrlr, "DisplayMessageCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::DisplayMessageCtrlr)
        .variables.at("DisplayMessageCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_display_message_ctrlr_enabled(const bool &display_message_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DisplayMessageCtrlr, "DisplayMessageCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::DisplayMessageCtrlr)
            .variables.at("DisplayMessageCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(display_message_ctrlr_enabled));
        }
}

boost::optional<int> DeviceModelManagerBase::get_personal_message_size(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DisplayMessageCtrlr, "PersonalMessageSize", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::DisplayMessageCtrlr)
            .variables.at("PersonalMessageSize")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_display_message_supported_formats(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::DisplayMessageCtrlr)
        .variables.at("DisplayMessageSupportedFormats")
        .attributes.at(attribute_enum)
        .value.value();
}


std::string DeviceModelManagerBase::get_display_message_supported_priorities(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::DisplayMessageCtrlr)
        .variables.at("DisplayMessageSupportedPriorities")
        .attributes.at(attribute_enum)
        .value.value();
}


boost::optional<std::string> DeviceModelManagerBase::get_charging_station(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DistributionPanel, "ChargingStation", attribute_enum)) {
            return this->components.at(StandardizedComponent::DistributionPanel)
            .variables.at("ChargingStation")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_distribution_panel(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DistributionPanel, "DistributionPanel", attribute_enum)) {
            return this->components.at(StandardizedComponent::DistributionPanel)
            .variables.at("DistributionPanel")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_fuse_n_(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::DistributionPanel, "Fuse<n>", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::DistributionPanel)
            .variables.at("Fuse<n>")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_allow_reset(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::EVSE, "AllowReset", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::EVSE)
        .variables.at("AllowReset")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_evse_availability_state(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::EVSE)
        .variables.at("EVSEAvailabilityState")
        .attributes.at(attribute_enum)
        .value.value();
}


bool DeviceModelManagerBase::get_evse_available(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::EVSE)
        .variables.at("EVSEAvailable")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<std::string> DeviceModelManagerBase::get_evse__id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::EVSE, "EvseId", attribute_enum)) {
            return this->components.at(StandardizedComponent::EVSE)
            .variables.at("EvseId")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


double DeviceModelManagerBase::get_power(const AttributeEnum &attribute_enum) {
    return std::stod(this->components.at(StandardizedComponent::EVSE)
        .variables.at("Power")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_evse_supply_phases(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::EVSE)
        .variables.at("EVSESupplyPhases")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<std::string> DeviceModelManagerBase::get_iso15118_evse__id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::EVSE, "ISO15118EvseId", attribute_enum)) {
            return this->components.at(StandardizedComponent::EVSE)
            .variables.at("ISO15118EvseId")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_iso15118_evse__id(const std::string &iso15118_evse__id, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::EVSE, "ISO15118EvseId", attribute_enum)) {
            this->components.at(StandardizedComponent::EVSE)
            .variables.at("ISO15118EvseId")
            .attributes.at(attribute_enum)
            .value.emplace(iso15118_evse__id);
        }
}

boost::optional<double> DeviceModelManagerBase::get_energy_export(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::FiscalMetering, "EnergyExport", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::FiscalMetering)
            .variables.at("EnergyExport")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<double> DeviceModelManagerBase::get_energy_export_register(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::FiscalMetering, "EnergyExportRegister", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::FiscalMetering)
            .variables.at("EnergyExportRegister")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<double> DeviceModelManagerBase::get_energy_import(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::FiscalMetering, "EnergyImport", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::FiscalMetering)
            .variables.at("EnergyImport")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<double> DeviceModelManagerBase::get_energy_import_register(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::FiscalMetering, "EnergyImportRegister", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::FiscalMetering)
            .variables.at("EnergyImportRegister")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_iso15118ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ISO15118CtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("ISO15118CtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_iso15118ctrlr_enabled(const bool &iso15118ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ISO15118CtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("ISO15118CtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(iso15118ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_central_contract_validation_allowed(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "CentralContractValidationAllowed", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("CentralContractValidationAllowed")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_central_contract_validation_allowed(const bool &central_contract_validation_allowed, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "CentralContractValidationAllowed", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("CentralContractValidationAllowed")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(central_contract_validation_allowed));
        }
}

bool DeviceModelManagerBase::get_contract_validation_offline(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("ContractValidationOffline")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_contract_validation_offline(const bool &contract_validation_offline, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("ContractValidationOffline")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(contract_validation_offline));
}

boost::optional<std::string> DeviceModelManagerBase::get_secc_id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "SeccId", attribute_enum)) {
            return this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("SeccId")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_secc_id(const std::string &secc_id, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "SeccId", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("SeccId")
            .attributes.at(attribute_enum)
            .value.emplace(secc_id);
        }
}

boost::optional<int> DeviceModelManagerBase::get_max_schedule_entries(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "MaxScheduleEntries", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("MaxScheduleEntries")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_requested_energy_transfer_mode(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "RequestedEnergyTransferMode", attribute_enum)) {
            return this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("RequestedEnergyTransferMode")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_request_metering_receipt(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "RequestMeteringReceipt", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("RequestMeteringReceipt")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_request_metering_receipt(const bool &request_metering_receipt, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "RequestMeteringReceipt", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("RequestMeteringReceipt")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(request_metering_receipt));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_country_name(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "CountryName", attribute_enum)) {
            return this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("CountryName")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_country_name(const std::string &country_name, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "CountryName", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("CountryName")
            .attributes.at(attribute_enum)
            .value.emplace(country_name);
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_iso15118ctrlr_organization_name(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ISO15118CtrlrOrganizationName", attribute_enum)) {
            return this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("ISO15118CtrlrOrganizationName")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_iso15118ctrlr_organization_name(const std::string &iso15118ctrlr_organization_name, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ISO15118CtrlrOrganizationName", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("ISO15118CtrlrOrganizationName")
            .attributes.at(attribute_enum)
            .value.emplace(iso15118ctrlr_organization_name);
        }
}

boost::optional<bool> DeviceModelManagerBase::get_pn_cenabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "PnCEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("PnCEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_pn_cenabled(const bool &pn_cenabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "PnCEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("PnCEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(pn_cenabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_v2gcertificate_installation_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "V2GCertificateInstallationEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("V2GCertificateInstallationEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_v2gcertificate_installation_enabled(const bool &v2gcertificate_installation_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "V2GCertificateInstallationEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("V2GCertificateInstallationEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(v2gcertificate_installation_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_contract_certificate_installation_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ContractCertificateInstallationEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ISO15118Ctrlr)
        .variables.at("ContractCertificateInstallationEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_contract_certificate_installation_enabled(const bool &contract_certificate_installation_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ISO15118Ctrlr, "ContractCertificateInstallationEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ISO15118Ctrlr)
            .variables.at("ContractCertificateInstallationEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(contract_certificate_installation_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_local_auth_list_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "LocalAuthListCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("LocalAuthListCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_bytes_per_message_send_local_list(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("BytesPerMessageSendLocalList")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_local_auth_list_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "LocalAuthListCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("LocalAuthListCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_local_auth_list_ctrlr_enabled(const bool &local_auth_list_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "LocalAuthListCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::LocalAuthListCtrlr)
            .variables.at("LocalAuthListCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(local_auth_list_ctrlr_enabled));
        }
}

int DeviceModelManagerBase::get_entries(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("Entries")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_items_per_message_send_local_list(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("ItemsPerMessageSendLocalList")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_storage(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "Storage", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
            .variables.at("Storage")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_disable_post_authorize(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "DisablePostAuthorize", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::LocalAuthListCtrlr)
        .variables.at("DisablePostAuthorize")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_disable_post_authorize(const bool &disable_post_authorize, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalAuthListCtrlr, "DisablePostAuthorize", attribute_enum)) {
            this->components.at(StandardizedComponent::LocalAuthListCtrlr)
            .variables.at("DisablePostAuthorize")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(disable_post_authorize));
        }
}

boost::optional<double> DeviceModelManagerBase::get_capacity(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::LocalEnergyStorage, "Capacity", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::LocalEnergyStorage)
            .variables.at("Capacity")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_monitoring_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "MonitoringCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::MonitoringCtrlr)
        .variables.at("MonitoringCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_bytes_per_message_clear_variable_monitoring(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "BytesPerMessageClearVariableMonitoring", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("BytesPerMessageClearVariableMonitoring")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_bytes_per_message_set_variable_monitoring(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
        .variables.at("BytesPerMessageSetVariableMonitoring")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_monitoring_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "MonitoringCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::MonitoringCtrlr)
        .variables.at("MonitoringCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_monitoring_ctrlr_enabled(const bool &monitoring_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "MonitoringCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("MonitoringCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(monitoring_ctrlr_enabled));
        }
}

boost::optional<int> DeviceModelManagerBase::get_items_per_message_clear_variable_monitoring(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "ItemsPerMessageClearVariableMonitoring", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("ItemsPerMessageClearVariableMonitoring")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_items_per_message_set_variable_monitoring(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
        .variables.at("ItemsPerMessageSetVariableMonitoring")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_offline_queuing_severity(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "OfflineQueuingSeverity", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("OfflineQueuingSeverity")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_offline_queuing_severity(const int &offline_queuing_severity, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "OfflineQueuingSeverity", attribute_enum)) {
            this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("OfflineQueuingSeverity")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(offline_queuing_severity));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_monitoring_base(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "MonitoringBase", attribute_enum)) {
            return this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("MonitoringBase")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_monitoring_level(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "MonitoringLevel", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("MonitoringLevel")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_active_monitoring_base(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "ActiveMonitoringBase", attribute_enum)) {
            return this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("ActiveMonitoringBase")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_active_monitoring_level(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::MonitoringCtrlr, "ActiveMonitoringLevel", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::MonitoringCtrlr)
            .variables.at("ActiveMonitoringLevel")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_ocppcomm_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "OCPPCommCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("OCPPCommCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_ocppcomm_ctrlr_enabled(const bool &ocppcomm_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "OCPPCommCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("OCPPCommCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(ocppcomm_ctrlr_enabled));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_active_network_profile(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "ActiveNetworkProfile", attribute_enum)) {
            return this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("ActiveNetworkProfile")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_file_transfer_protocols(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("FileTransferProtocols")
        .attributes.at(attribute_enum)
        .value.value();
}


boost::optional<int> DeviceModelManagerBase::get_heartbeat_interval(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "HeartbeatInterval", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("HeartbeatInterval")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_heartbeat_interval(const int &heartbeat_interval, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "HeartbeatInterval", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("HeartbeatInterval")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(heartbeat_interval));
        }
}

int DeviceModelManagerBase::get_message_timeout_default(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("MessageTimeoutDefault")
        .attributes.at(attribute_enum)
        .value.value());
}


int DeviceModelManagerBase::get_message_attempt_interval_transaction_event(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("MessageAttemptIntervalTransactionEvent")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_message_attempt_interval_transaction_event(const int &message_attempt_interval_transaction_event, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("MessageAttemptIntervalTransactionEvent")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(message_attempt_interval_transaction_event));
}

int DeviceModelManagerBase::get_message_attempts_transaction_event(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("MessageAttemptsTransactionEvent")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_message_attempts_transaction_event(const int &message_attempts_transaction_event, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("MessageAttemptsTransactionEvent")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(message_attempts_transaction_event));
}

std::string DeviceModelManagerBase::get_network_configuration_priority(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("NetworkConfigurationPriority")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_network_configuration_priority(const std::string &network_configuration_priority, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("NetworkConfigurationPriority")
        .attributes.at(attribute_enum)
        .value.emplace(network_configuration_priority);
}

int DeviceModelManagerBase::get_network_profile_connection_attempts(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("NetworkProfileConnectionAttempts")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_network_profile_connection_attempts(const int &network_profile_connection_attempts, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("NetworkProfileConnectionAttempts")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(network_profile_connection_attempts));
}

int DeviceModelManagerBase::get_offline_threshold(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("OfflineThreshold")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_offline_threshold(const int &offline_threshold, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("OfflineThreshold")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(offline_threshold));
}

boost::optional<std::string> DeviceModelManagerBase::get_public_key_with_signed_meter_value(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "PublicKeyWithSignedMeterValue", attribute_enum)) {
            return this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("PublicKeyWithSignedMeterValue")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_public_key_with_signed_meter_value(const std::string &public_key_with_signed_meter_value, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "PublicKeyWithSignedMeterValue", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("PublicKeyWithSignedMeterValue")
            .attributes.at(attribute_enum)
            .value.emplace(public_key_with_signed_meter_value);
        }
}

boost::optional<bool> DeviceModelManagerBase::get_queue_all_messages(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "QueueAllMessages", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("QueueAllMessages")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_queue_all_messages(const bool &queue_all_messages, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "QueueAllMessages", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("QueueAllMessages")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(queue_all_messages));
        }
}

int DeviceModelManagerBase::get_reset_retries(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("ResetRetries")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_reset_retries(const int &reset_retries, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("ResetRetries")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(reset_retries));
}

boost::optional<int> DeviceModelManagerBase::get_retry_back_off_random_range(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffRandomRange", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffRandomRange")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_retry_back_off_random_range(const int &retry_back_off_random_range, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffRandomRange", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffRandomRange")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(retry_back_off_random_range));
        }
}

boost::optional<int> DeviceModelManagerBase::get_retry_back_off_repeat_times(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffRepeatTimes", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffRepeatTimes")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_retry_back_off_repeat_times(const int &retry_back_off_repeat_times, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffRepeatTimes", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffRepeatTimes")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(retry_back_off_repeat_times));
        }
}

boost::optional<int> DeviceModelManagerBase::get_retry_back_off_wait_minimum(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffWaitMinimum", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffWaitMinimum")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_retry_back_off_wait_minimum(const int &retry_back_off_wait_minimum, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "RetryBackOffWaitMinimum", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("RetryBackOffWaitMinimum")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(retry_back_off_wait_minimum));
        }
}

bool DeviceModelManagerBase::get_unlock_on_evside_disconnect(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("UnlockOnEVSideDisconnect")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_unlock_on_evside_disconnect(const bool &unlock_on_evside_disconnect, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::OCPPCommCtrlr)
        .variables.at("UnlockOnEVSideDisconnect")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(unlock_on_evside_disconnect));
}

boost::optional<int> DeviceModelManagerBase::get_web_socket_ping_interval(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "WebSocketPingInterval", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("WebSocketPingInterval")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_web_socket_ping_interval(const int &web_socket_ping_interval, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "WebSocketPingInterval", attribute_enum)) {
            this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("WebSocketPingInterval")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(web_socket_ping_interval));
        }
}

boost::optional<int> DeviceModelManagerBase::get_field_length(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::OCPPCommCtrlr, "FieldLength", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::OCPPCommCtrlr)
            .variables.at("FieldLength")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_reservation_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ReservationCtrlr, "ReservationCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ReservationCtrlr)
        .variables.at("ReservationCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_reservation_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ReservationCtrlr, "ReservationCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ReservationCtrlr)
        .variables.at("ReservationCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_reservation_ctrlr_enabled(const bool &reservation_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ReservationCtrlr, "ReservationCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::ReservationCtrlr)
            .variables.at("ReservationCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(reservation_ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_non__evse__specific(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::ReservationCtrlr, "NonEvseSpecific", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::ReservationCtrlr)
        .variables.at("NonEvseSpecific")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_sampled_data_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "SampledDataCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_sampled_data_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "SampledDataCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_sampled_data_ctrlr_enabled(const bool &sampled_data_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "SampledDataCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::SampledDataCtrlr)
            .variables.at("SampledDataCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(sampled_data_ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_sampled_data_sign_readings(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "SampledDataSignReadings", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataSignReadings")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_sampled_data_sign_readings(const bool &sampled_data_sign_readings, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "SampledDataSignReadings", attribute_enum)) {
            this->components.at(StandardizedComponent::SampledDataCtrlr)
            .variables.at("SampledDataSignReadings")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(sampled_data_sign_readings));
        }
}

int DeviceModelManagerBase::get_sampled_data_tx_ended_interval(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxEndedInterval")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_sampled_data_tx_ended_interval(const int &sampled_data_tx_ended_interval, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxEndedInterval")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(sampled_data_tx_ended_interval));
}

std::string DeviceModelManagerBase::get_sampled_data_tx_ended_measurands(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxEndedMeasurands")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_sampled_data_tx_ended_measurands(const std::string &sampled_data_tx_ended_measurands, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxEndedMeasurands")
        .attributes.at(attribute_enum)
        .value.emplace(sampled_data_tx_ended_measurands);
}

std::string DeviceModelManagerBase::get_sampled_data_tx_started_measurands(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxStartedMeasurands")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_sampled_data_tx_started_measurands(const std::string &sampled_data_tx_started_measurands, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxStartedMeasurands")
        .attributes.at(attribute_enum)
        .value.emplace(sampled_data_tx_started_measurands);
}

int DeviceModelManagerBase::get_sampled_data_tx_updated_interval(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxUpdatedInterval")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_sampled_data_tx_updated_interval(const int &sampled_data_tx_updated_interval, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxUpdatedInterval")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(sampled_data_tx_updated_interval));
}

std::string DeviceModelManagerBase::get_sampled_data_tx_updated_measurands(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxUpdatedMeasurands")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_sampled_data_tx_updated_measurands(const std::string &sampled_data_tx_updated_measurands, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("SampledDataTxUpdatedMeasurands")
        .attributes.at(attribute_enum)
        .value.emplace(sampled_data_tx_updated_measurands);
}

boost::optional<bool> DeviceModelManagerBase::get_register_values_without_phases(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "RegisterValuesWithoutPhases", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SampledDataCtrlr)
        .variables.at("RegisterValuesWithoutPhases")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_register_values_without_phases(const bool &register_values_without_phases, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SampledDataCtrlr, "RegisterValuesWithoutPhases", attribute_enum)) {
            this->components.at(StandardizedComponent::SampledDataCtrlr)
            .variables.at("RegisterValuesWithoutPhases")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(register_values_without_phases));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_security_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "SecurityCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("SecurityCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_security_ctrlr_enabled(const bool &security_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "SecurityCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("SecurityCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(security_ctrlr_enabled));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_additional_root_certificate_check(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "AdditionalRootCertificateCheck", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("AdditionalRootCertificateCheck")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_basic_auth_password(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "BasicAuthPassword", attribute_enum)) {
            return this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("BasicAuthPassword")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_certificate_entries(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("CertificateEntries")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<int> DeviceModelManagerBase::get_cert_signing_repeat_times(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "CertSigningRepeatTimes", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("CertSigningRepeatTimes")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_cert_signing_repeat_times(const int &cert_signing_repeat_times, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "CertSigningRepeatTimes", attribute_enum)) {
            this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("CertSigningRepeatTimes")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(cert_signing_repeat_times));
        }
}

boost::optional<int> DeviceModelManagerBase::get_cert_signing_wait_minimum(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "CertSigningWaitMinimum", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("CertSigningWaitMinimum")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_cert_signing_wait_minimum(const int &cert_signing_wait_minimum, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "CertSigningWaitMinimum", attribute_enum)) {
            this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("CertSigningWaitMinimum")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(cert_signing_wait_minimum));
        }
}

boost::optional<std::string> DeviceModelManagerBase::get_identity(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "Identity", attribute_enum)) {
            return this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("Identity")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<int> DeviceModelManagerBase::get_max_certificate_chain_size(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SecurityCtrlr, "MaxCertificateChainSize", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::SecurityCtrlr)
            .variables.at("MaxCertificateChainSize")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_security_ctrlr_organization_name(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("SecurityCtrlrOrganizationName")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_security_ctrlr_organization_name(const std::string &security_ctrlr_organization_name, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("SecurityCtrlrOrganizationName")
        .attributes.at(attribute_enum)
        .value.emplace(security_ctrlr_organization_name);
}

int DeviceModelManagerBase::get_security_profile(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SecurityCtrlr)
        .variables.at("SecurityProfile")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_acphase_switching_supported(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "ACPhaseSwitchingSupported", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("ACPhaseSwitchingSupported")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_smart_charging_ctrlr_available(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "SmartChargingCtrlrAvailable", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("SmartChargingCtrlrAvailable")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_smart_charging_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "SmartChargingCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("SmartChargingCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_smart_charging_ctrlr_enabled(const bool &smart_charging_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "SmartChargingCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::SmartChargingCtrlr)
            .variables.at("SmartChargingCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(smart_charging_ctrlr_enabled));
        }
}

int DeviceModelManagerBase::get_entries_charging_profiles(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("EntriesChargingProfiles")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_external_control_signals_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "ExternalControlSignalsEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("ExternalControlSignalsEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_external_control_signals_enabled(const bool &external_control_signals_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "ExternalControlSignalsEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::SmartChargingCtrlr)
            .variables.at("ExternalControlSignalsEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(external_control_signals_enabled));
        }
}

double DeviceModelManagerBase::get_limit_change_significance(const AttributeEnum &attribute_enum) {
    return std::stod(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("LimitChangeSignificance")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_limit_change_significance(const double &limit_change_significance, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("LimitChangeSignificance")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::double_to_string(limit_change_significance));
}

boost::optional<bool> DeviceModelManagerBase::get_notify_charging_limit_with_schedules(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "NotifyChargingLimitWithSchedules", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("NotifyChargingLimitWithSchedules")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_notify_charging_limit_with_schedules(const bool &notify_charging_limit_with_schedules, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "NotifyChargingLimitWithSchedules", attribute_enum)) {
            this->components.at(StandardizedComponent::SmartChargingCtrlr)
            .variables.at("NotifyChargingLimitWithSchedules")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(notify_charging_limit_with_schedules));
        }
}

int DeviceModelManagerBase::get_periods_per_schedule(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("PeriodsPerSchedule")
        .attributes.at(attribute_enum)
        .value.value());
}


boost::optional<bool> DeviceModelManagerBase::get_phases3to1(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::SmartChargingCtrlr, "Phases3to1", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("Phases3to1")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_charging_profile_max_stack_level(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("ChargingProfileMaxStackLevel")
        .attributes.at(attribute_enum)
        .value.value());
}


std::string DeviceModelManagerBase::get_charging_schedule_charging_rate_unit(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::SmartChargingCtrlr)
        .variables.at("ChargingScheduleChargingRateUnit")
        .attributes.at(attribute_enum)
        .value.value();
}


boost::optional<bool> DeviceModelManagerBase::get_available_tariff(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "AvailableTariff", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("AvailableTariff")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_available_cost(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "AvailableCost", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("AvailableCost")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}


std::string DeviceModelManagerBase::get_currency(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("Currency")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_currency(const std::string &currency, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("Currency")
        .attributes.at(attribute_enum)
        .value.emplace(currency);
}

boost::optional<bool> DeviceModelManagerBase::get_enabled_tariff(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "EnabledTariff", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("EnabledTariff")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_enabled_tariff(const bool &enabled_tariff, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "EnabledTariff", attribute_enum)) {
            this->components.at(StandardizedComponent::TariffCostCtrlr)
            .variables.at("EnabledTariff")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(enabled_tariff));
        }
}

boost::optional<bool> DeviceModelManagerBase::get_enabled_cost(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "EnabledCost", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("EnabledCost")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_enabled_cost(const bool &enabled_cost, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TariffCostCtrlr, "EnabledCost", attribute_enum)) {
            this->components.at(StandardizedComponent::TariffCostCtrlr)
            .variables.at("EnabledCost")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(enabled_cost));
        }
}

std::string DeviceModelManagerBase::get_tariff_fallback_message(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("TariffFallbackMessage")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_tariff_fallback_message(const std::string &tariff_fallback_message, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("TariffFallbackMessage")
        .attributes.at(attribute_enum)
        .value.emplace(tariff_fallback_message);
}

std::string DeviceModelManagerBase::get_total_cost_fallback_message(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("TotalCostFallbackMessage")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_total_cost_fallback_message(const std::string &total_cost_fallback_message, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TariffCostCtrlr)
        .variables.at("TotalCostFallbackMessage")
        .attributes.at(attribute_enum)
        .value.emplace(total_cost_fallback_message);
}

boost::optional<std::string> DeviceModelManagerBase::get_token(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TokenReader, "Token", attribute_enum)) {
            return this->components.at(StandardizedComponent::TokenReader)
            .variables.at("Token")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<std::string> DeviceModelManagerBase::get_token_type(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TokenReader, "TokenType", attribute_enum)) {
            return this->components.at(StandardizedComponent::TokenReader)
            .variables.at("TokenType")
            .attributes.at(attribute_enum)
            .value.value().get();
        } else {
        return boost::none;
    }
}


boost::optional<bool> DeviceModelManagerBase::get_tx_ctrlr_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "TxCtrlrEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxCtrlrEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_tx_ctrlr_enabled(const bool &tx_ctrlr_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "TxCtrlrEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::TxCtrlr)
            .variables.at("TxCtrlrEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(tx_ctrlr_enabled));
        }
}

boost::optional<double> DeviceModelManagerBase::get_charging_time(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "ChargingTime", attribute_enum)) {
            return std::stod(this->components.at(StandardizedComponent::TxCtrlr)
            .variables.at("ChargingTime")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}


int DeviceModelManagerBase::get_evconnection_time_out(const AttributeEnum &attribute_enum) {
    return std::stoi(this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("EVConnectionTimeOut")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_evconnection_time_out(const int &evconnection_time_out, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("EVConnectionTimeOut")
        .attributes.at(attribute_enum)
        .value.emplace(std::to_string(evconnection_time_out));
}

boost::optional<int> DeviceModelManagerBase::get_max_energy_on_invalid_id(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "MaxEnergyOnInvalidId", attribute_enum)) {
            return std::stoi(this->components.at(StandardizedComponent::TxCtrlr)
            .variables.at("MaxEnergyOnInvalidId")
            .attributes.at(attribute_enum)
            .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_max_energy_on_invalid_id(const int &max_energy_on_invalid_id, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "MaxEnergyOnInvalidId", attribute_enum)) {
            this->components.at(StandardizedComponent::TxCtrlr)
            .variables.at("MaxEnergyOnInvalidId")
            .attributes.at(attribute_enum)
            .value.emplace(std::to_string(max_energy_on_invalid_id));
        }
}

bool DeviceModelManagerBase::get_stop_tx_on_evside_disconnect(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("StopTxOnEVSideDisconnect")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_stop_tx_on_evside_disconnect(const bool &stop_tx_on_evside_disconnect, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("StopTxOnEVSideDisconnect")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(stop_tx_on_evside_disconnect));
}

bool DeviceModelManagerBase::get_stop_tx_on_invalid_id(const AttributeEnum &attribute_enum) {
    return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("StopTxOnInvalidId")
        .attributes.at(attribute_enum)
        .value.value());
}

void DeviceModelManagerBase::set_stop_tx_on_invalid_id(const bool &stop_tx_on_invalid_id, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("StopTxOnInvalidId")
        .attributes.at(attribute_enum)
        .value.emplace(ocpp::conversions::bool_to_string(stop_tx_on_invalid_id));
}

boost::optional<bool> DeviceModelManagerBase::get_tx_before_accepted_enabled(const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "TxBeforeAcceptedEnabled", attribute_enum)) {
        return ocpp::conversions::string_to_bool(this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxBeforeAcceptedEnabled")
        .attributes.at(attribute_enum)
        .value.value());
        } else {
        return boost::none;
    }
}

void DeviceModelManagerBase::set_tx_before_accepted_enabled(const bool &tx_before_accepted_enabled, const AttributeEnum &attribute_enum) {
    if (this->exists(StandardizedComponent::TxCtrlr, "TxBeforeAcceptedEnabled", attribute_enum)) {
            this->components.at(StandardizedComponent::TxCtrlr)
            .variables.at("TxBeforeAcceptedEnabled")
            .attributes.at(attribute_enum)
            .value.emplace(ocpp::conversions::bool_to_string(tx_before_accepted_enabled));
        }
}

std::string DeviceModelManagerBase::get_tx_start_point(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxStartPoint")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_tx_start_point(const std::string &tx_start_point, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxStartPoint")
        .attributes.at(attribute_enum)
        .value.emplace(tx_start_point);
}

std::string DeviceModelManagerBase::get_tx_stop_point(const AttributeEnum &attribute_enum) {
    return this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxStopPoint")
        .attributes.at(attribute_enum)
        .value.value();
}

void DeviceModelManagerBase::set_tx_stop_point(const std::string &tx_stop_point, const AttributeEnum &attribute_enum) {
    this->components.at(StandardizedComponent::TxCtrlr)
        .variables.at("TxStopPoint")
        .attributes.at(attribute_enum)
        .value.emplace(tx_stop_point);
}


} //namespace v201
} // namespace ocpp
