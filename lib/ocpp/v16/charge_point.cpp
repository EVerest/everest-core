// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <everest/logging.hpp>
#include <ocpp/v16/charge_point.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v16/charge_point_impl.hpp>

namespace ocpp {
namespace v16 {

ChargePoint::ChargePoint(const std::string& config, const fs::path& share_path, const fs::path& user_config_path,
                         const fs::path& database_path, const fs::path& sql_init_path, const fs::path& message_log_path,
                         const std::shared_ptr<EvseSecurity> evse_security,
                         const std::optional<SecurityConfiguration> security_configuration) {
    this->charge_point =
        std::make_unique<ChargePointImpl>(config, share_path, user_config_path, database_path, sql_init_path,
                                          message_log_path, evse_security, security_configuration);
}

ChargePoint::~ChargePoint() = default;

bool ChargePoint::start(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason) {
    return this->charge_point->start(connector_status_map, bootreason);
}

bool ChargePoint::restart(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason) {
    return this->charge_point->restart(connector_status_map, bootreason);
}

bool ChargePoint::stop() {
    return this->charge_point->stop();
}

void ChargePoint::connect_websocket() {
    this->charge_point->connect_websocket();
}

void ChargePoint::disconnect_websocket() {
    this->charge_point->disconnect_websocket();
}

void ChargePoint::call_set_connection_timeout() {
    this->charge_point->call_set_connection_timeout();
}

IdTagInfo ChargePoint::authorize_id_token(CiString<20> id_token) {
    return this->charge_point->authorize_id_token(id_token);
}

ocpp::v201::AuthorizeResponse ChargePoint::data_transfer_pnc_authorize(

    const std::string& emaid, const std::optional<std::string>& certificate,
    const std::optional<std::vector<ocpp::v201::OCSPRequestData>>& iso15118_certificate_hash_data) {
    return this->charge_point->data_transfer_pnc_authorize(emaid, certificate, iso15118_certificate_hash_data);
}

void ChargePoint::data_transfer_pnc_get_15118_ev_certificate(
    const int32_t connector_id, const std::string& exi_request, const std::string& iso15118_schema_version,
    const ocpp::v201::CertificateActionEnum& certificate_action) {

    this->charge_point->data_transfer_pnc_get_15118_ev_certificate(connector_id, exi_request, iso15118_schema_version,
                                                                   certificate_action);
}

DataTransferResponse ChargePoint::data_transfer(const CiString<255>& vendorId,
                                                const std::optional<CiString<50>>& messageId,
                                                const std::optional<std::string>& data) {
    return this->charge_point->data_transfer(vendorId, messageId, data);
}

std::map<int32_t, ChargingSchedule> ChargePoint::get_all_composite_charging_schedules(const int32_t duration_s) {
    return this->charge_point->get_all_composite_charging_schedules(duration_s);
}

std::map<int32_t, EnhancedChargingSchedule>
ChargePoint::get_all_enhanced_composite_charging_schedules(const int32_t duration_s) {
    return this->charge_point->get_all_enhanced_composite_charging_schedules(duration_s);
}

void ChargePoint::on_meter_values(int32_t connector, const Measurement& measurement) {
    this->charge_point->on_meter_values(connector, measurement);
}

void ChargePoint::on_max_current_offered(int32_t connector, int32_t max_current) {
    this->charge_point->on_max_current_offered(connector, max_current);
}

void ChargePoint::on_max_power_offered(int32_t connector, int32_t max_power) {
    this->charge_point->on_max_power_offered(connector, max_power);
}

void ChargePoint::on_session_started(int32_t connector, const std::string& session_id,
                                     const ocpp::SessionStartedReason reason,
                                     const std::optional<std::string>& session_logging_path) {

    this->charge_point->on_session_started(connector, session_id, reason, session_logging_path);
}

void ChargePoint::on_session_stopped(const int32_t connector, const std::string& session_id) {
    this->charge_point->on_session_stopped(connector, session_id);
}

void ChargePoint::on_transaction_started(const int32_t& connector, const std::string& session_id,
                                         const std::string& id_token, const int32_t& meter_start,
                                         std::optional<int32_t> reservation_id, const ocpp::DateTime& timestamp,
                                         std::optional<std::string> signed_meter_value) {
    this->charge_point->on_transaction_started(connector, session_id, id_token, meter_start, reservation_id, timestamp,
                                               signed_meter_value);
}

void ChargePoint::on_transaction_stopped(const int32_t connector, const std::string& session_id, const Reason& reason,
                                         ocpp::DateTime timestamp, float energy_wh_import,
                                         std::optional<CiString<20>> id_tag_end,
                                         std::optional<std::string> signed_meter_value) {
    this->charge_point->on_transaction_stopped(connector, session_id, reason, timestamp, energy_wh_import, id_tag_end,
                                               signed_meter_value);
}

void ChargePoint::on_suspend_charging_ev(int32_t connector) {
    this->charge_point->on_suspend_charging_ev(connector);
}

void ChargePoint::on_suspend_charging_evse(int32_t connector) {
    this->charge_point->on_suspend_charging_evse(connector);
}

void ChargePoint::on_resume_charging(int32_t connector) {
    this->charge_point->on_resume_charging(connector);
}

void ChargePoint::on_error(int32_t connector, const ChargePointErrorCode& error_code,
                           const std::optional<CiString<50>>& info, const std::optional<CiString<255>>& vendor_id,
                           const std::optional<CiString<50>>& vendor_error_code) {
    this->charge_point->on_error(connector, error_code, info, vendor_id, vendor_error_code);
}

void ChargePoint::on_fault(int32_t connector, const ChargePointErrorCode& error_code,
                           const std::optional<CiString<50>>& info, const std::optional<CiString<255>>& vendor_id,
                           const std::optional<CiString<50>>& vendor_error_code) {
    this->charge_point->on_fault(connector, error_code, info, vendor_id, vendor_error_code);
}

void ChargePoint::on_log_status_notification(int32_t request_id, std::string log_status) {
    this->charge_point->on_log_status_notification(request_id, log_status);
}

void ChargePoint::on_firmware_update_status_notification(int32_t request_id,
                                                         const FirmwareStatusNotification firmware_update_status) {
    this->charge_point->on_firmware_update_status_notification(request_id, firmware_update_status);
}

void ChargePoint::on_reservation_start(int32_t connector) {
    this->charge_point->on_reservation_start(connector);
}

void ChargePoint::on_reservation_end(int32_t connector) {
    this->charge_point->on_reservation_end(connector);
}

void ChargePoint::on_enabled(int32_t connector) {
    this->charge_point->on_enabled(connector);
}

void ChargePoint::on_disabled(int32_t connector) {
    this->charge_point->on_disabled(connector);
}

void ChargePoint::on_plugin_timeout(int32_t connector) {
    this->charge_point->on_plugin_timeout(connector);
}

void ChargePoint::on_security_event(const std::string& type, const std::string& tech_info) {
    this->charge_point->on_security_event(type, tech_info);
}

ChangeAvailabilityResponse ChargePoint::on_change_availability(const ChangeAvailabilityRequest& request) {
    return this->charge_point->on_change_availability(request);
}

void ChargePoint::register_data_transfer_callback(
    const CiString<255>& vendorId, const CiString<50>& messageId,
    const std::function<DataTransferResponse(const std::optional<std::string>& msg)>& callback) {
    this->charge_point->register_data_transfer_callback(vendorId, messageId, callback);
}

void ChargePoint::register_data_transfer_callback(
    const std::function<DataTransferResponse(const DataTransferRequest& request)>& callback) {
    this->charge_point->register_data_transfer_callback(callback);
}

void ChargePoint::register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_enable_evse_callback(callback);
}

void ChargePoint::register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_disable_evse_callback(callback);
}

void ChargePoint::register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_pause_charging_callback(callback);
}

void ChargePoint::register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_resume_charging_callback(callback);
}

void ChargePoint::register_provide_token_callback(
    const std::function<void(const std::string& id_token, std::vector<int32_t> referenced_connectors,
                             bool prevalidated)>& callback) {
    this->charge_point->register_provide_token_callback(callback);
}

void ChargePoint::register_stop_transaction_callback(
    const std::function<bool(int32_t connector, Reason reason)>& callback) {
    this->charge_point->register_stop_transaction_callback(callback);
}

void ChargePoint::register_reserve_now_callback(
    const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp::DateTime expiryDate,
                                          CiString<20> idTag, std::optional<CiString<20>> parent_id)>& callback) {
    this->charge_point->register_reserve_now_callback(callback);
}

void ChargePoint::register_cancel_reservation_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_cancel_reservation_callback(callback);
}

void ChargePoint::register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback) {
    this->charge_point->register_unlock_connector_callback(callback);
}

void ChargePoint::register_upload_diagnostics_callback(
    const std::function<GetLogResponse(const GetDiagnosticsRequest& request)>& callback) {
    this->charge_point->register_upload_diagnostics_callback(callback);
}

void ChargePoint::register_update_firmware_callback(
    const std::function<void(const UpdateFirmwareRequest msg)>& callback) {
    this->charge_point->register_update_firmware_callback(callback);
}

void ChargePoint::register_signed_update_firmware_callback(
    const std::function<UpdateFirmwareStatusEnumType(const SignedUpdateFirmwareRequest msg)>& callback) {
    this->charge_point->register_signed_update_firmware_callback(callback);
}

void ChargePoint::register_all_connectors_unavailable_callback(const std::function<void()>& callback) {
    this->charge_point->register_all_connectors_unavailable_callback(callback);
}

void ChargePoint::register_upload_logs_callback(const std::function<GetLogResponse(GetLogRequest req)>& callback) {
    this->charge_point->register_upload_logs_callback(callback);
}

void ChargePoint::register_set_connection_timeout_callback(
    const std::function<void(int32_t connection_timeout)>& callback) {
    this->charge_point->register_set_connection_timeout_callback(callback);
}

void ChargePoint::register_is_reset_allowed_callback(const std::function<bool(const ResetType& reset_type)>& callback) {
    this->charge_point->register_is_reset_allowed_callback(callback);
}

void ChargePoint::register_reset_callback(const std::function<void(const ResetType& reset_type)>& callback) {
    this->charge_point->register_reset_callback(callback);
}

void ChargePoint::register_set_system_time_callback(
    const std::function<void(const std::string& system_time)>& callback) {
    this->charge_point->register_set_system_time_callback(callback);
}

void ChargePoint::register_signal_set_charging_profiles_callback(const std::function<void()>& callback) {
    this->charge_point->register_signal_set_charging_profiles_callback(callback);
}

void ChargePoint::register_connection_state_changed_callback(const std::function<void(bool is_connected)>& callback) {
    this->charge_point->register_connection_state_changed_callback(callback);
}

void ChargePoint::register_get_15118_ev_certificate_response_callback(
    const std::function<void(const int32_t connector,
                             const ocpp::v201::Get15118EVCertificateResponse& certificate_response,
                             const ocpp::v201::CertificateActionEnum& certificate_action)>& callback) {
    this->charge_point->register_get_15118_ev_certificate_response_callback(callback);
}

void ChargePoint::register_transaction_started_callback(
    const std::function<void(const int32_t connector, const std::string& session_id)>& callback) {
    this->charge_point->register_transaction_started_callback(callback);
}

void ChargePoint::register_transaction_stopped_callback(
    const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id)>&
        callback) {
    this->charge_point->register_transaction_stopped_callback(callback);
}

void ChargePoint::register_transaction_updated_callback(
    const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id,
                             const IdTagInfo& id_tag_info)>& callback) {
    this->charge_point->register_transaction_updated_callback(callback);
}

void ChargePoint::register_configuration_key_changed_callback(
    const CiString<50>& key, const std::function<void(const KeyValue& key_value)>& callback) {
    this->charge_point->register_configuration_key_changed_callback(key, callback);
}

void ChargePoint::register_generic_configuration_key_changed_callback(
    const std::function<void(const KeyValue& key_value)>& callback) {
    this->charge_point->register_generic_configuration_key_changed_callback(callback);
}

void ChargePoint::register_security_event_callback(
    const std::function<void(const std::string& type, const std::string& tech_info)>& callback) {
    this->charge_point->register_security_event_callback(callback);
}

GetConfigurationResponse ChargePoint::get_configuration_key(const GetConfigurationRequest& request) {
    return this->charge_point->get_configuration_key(request);
}

ConfigurationStatus ChargePoint::set_custom_configuration_key(CiString<50> key, CiString<500> value) {
    return this->charge_point->set_custom_configuration_key(key, value);
}

void ChargePoint::register_is_token_reserved_for_connector_callback(
    const std::function<bool(const int32_t connector, const std::string& id_token)>& callback) {
    this->charge_point->register_is_token_reserved_for_connector_callback(callback);
}

void ChargePoint::set_message_queue_resume_delay(std::chrono::seconds delay) {
    this->charge_point->set_message_queue_resume_delay(delay);
}

} // namespace v16
} // namespace ocpp
