// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V2_CONVERSIONS_HPP
#define OCPP_V2_CONVERSIONS_HPP

#include <generated/types/display_message.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/iso15118.hpp>
#include <generated/types/ocpp.hpp>
#include <generated/types/reservation.hpp>
#include <generated/types/system.hpp>

#include <ocpp/v2/messages/Authorize.hpp>
#include <ocpp/v2/messages/BootNotification.hpp>
#include <ocpp/v2/messages/ClearDisplayMessage.hpp>
#include <ocpp/v2/messages/DataTransfer.hpp>
#include <ocpp/v2/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v2/messages/Get15118EVCertificate.hpp>
#include <ocpp/v2/messages/GetDisplayMessages.hpp>
#include <ocpp/v2/messages/GetLog.hpp>
#include <ocpp/v2/messages/NotifyEVChargingNeeds.hpp>
#include <ocpp/v2/messages/SetDisplayMessage.hpp>
#include <ocpp/v2/messages/TransactionEvent.hpp>
#include <ocpp/v2/messages/UpdateFirmware.hpp>

namespace module {
namespace conversions {
/// \brief Converts a given types::system::FirmwareUpdateStatusEnum \p status to a ocpp::v2::FirmwareStatusEnum.
ocpp::v2::FirmwareStatusEnum to_ocpp_firmware_status_enum(const types::system::FirmwareUpdateStatusEnum status);

/// \brief Converts a given types::ocpp::DataTransferStatus \p status to a ocpp::v2::DataTransferStatusEnum.
ocpp::v2::DataTransferStatusEnum to_ocpp_data_transfer_status_enum(types::ocpp::DataTransferStatus status);

/// \brief Converts a given types::ocpp::DataTransferRequest \p status to a ocpp::v2::DataTransferRequest.
ocpp::v2::DataTransferRequest to_ocpp_data_transfer_request(types::ocpp::DataTransferRequest request);

/// \brief Converts a given types::ocpp::DataTransferResponse \p status to a ocpp::v2::DataTransferResponse.
ocpp::v2::DataTransferResponse to_ocpp_data_transfer_response(types::ocpp::DataTransferResponse response);

/// \brief Converts the provided parameters to an ocpp::v2::SampledValue.
ocpp::v2::SampledValue to_ocpp_sampled_value(const ocpp::v2::ReadingContextEnum& reading_context,
                                             const ocpp::v2::MeasurandEnum& measurand, const std::string& unit,
                                             const std::optional<ocpp::v2::PhaseEnum> phase,
                                             ocpp::v2::LocationEnum location = ocpp::v2::LocationEnum::Outlet);

/// \brief Converts the given types::units_signed::SignedMeterValue \p signed_meter_value  to an
/// ocpp::v2::SignedMeterValue.
ocpp::v2::SignedMeterValue to_ocpp_signed_meter_value(const types::units_signed::SignedMeterValue& signed_meter_value);

/// \brief Converts the provided parameters to an ocpp::v2::MeterValue.
ocpp::v2::MeterValue to_ocpp_meter_value(const types::powermeter::Powermeter& power_meter,
                                         const ocpp::v2::ReadingContextEnum& reading_context,
                                         const std::optional<types::units_signed::SignedMeterValue> signed_meter_value);

/// \brief Converts a given types::system::UploadLogsStatus \p log_status to an ocpp::v2::LogStatusEnum.
ocpp::v2::LogStatusEnum to_ocpp_log_status_enum(types::system::UploadLogsStatus log_status);

/// \brief Converts a given types::system::UploadLogsResponse \p response to an ocpp::v2::GetLogResponse.
ocpp::v2::GetLogResponse to_ocpp_get_log_response(const types::system::UploadLogsResponse& response);

/// \brief Converts a given types::system::UpdateFirmwareResponse \p response to an
/// ocpp::v2::UpdateFirmwareStatusEnum.
ocpp::v2::UpdateFirmwareStatusEnum
to_ocpp_update_firmware_status_enum(const types::system::UpdateFirmwareResponse& response);

/// \brief Converts a given types::system::UpdateFirmwareResponse \p response to an ocpp::v2::UpdateFirmwareResponse.
ocpp::v2::UpdateFirmwareResponse
to_ocpp_update_firmware_response(const types::system::UpdateFirmwareResponse& response);

/// \brief Converts a given types::system::LogStatusEnum \p status to an ocpp::v2::UploadLogStatusEnum.
ocpp::v2::UploadLogStatusEnum to_ocpp_upload_logs_status_enum(types::system::LogStatusEnum status);

/// \brief Converts a given types::system::BootReason \p reason to an ocpp::v2::BootReasonEnum.
ocpp::v2::BootReasonEnum to_ocpp_boot_reason(types::system::BootReason reason);

/// \brief Converts a given types::evse_manager::StopTransactionReason \p reason to an ocpp::v2::ReasonEnum.
ocpp::v2::ReasonEnum to_ocpp_reason(types::evse_manager::StopTransactionReason reason);

/// \brief Converts a given types::authorization::IdToken \p id_token to an ocpp::v2::IdToken.
ocpp::v2::IdToken to_ocpp_id_token(const types::authorization::IdToken& id_token);

/// \brief Converts a given types::iso15118::CertificateActionEnum \p action to an
/// ocpp::v2::CertificateActionEnum.
ocpp::v2::CertificateActionEnum to_ocpp_certificate_action_enum(const types::iso15118::CertificateActionEnum& action);

/// \brief Converts a vector of types::iso15118::CertificateHashDataInfo to a vector of
/// ocpp::v2::OCSPRequestData.
std::vector<ocpp::v2::OCSPRequestData> to_ocpp_ocsp_request_data_vector(
    const std::vector<types::iso15118::CertificateHashDataInfo>& certificate_hash_data_info);

/// \brief Converts a given types::iso15118::HashAlgorithm \p hash_algorithm to an
/// ocpp::v2::HashAlgorithmEnum.
ocpp::v2::HashAlgorithmEnum to_ocpp_hash_algorithm_enum(const types::iso15118::HashAlgorithm hash_algorithm);

/// \brief Converts a given types::ocpp::GetVariableRequest \p get_variable_request_vector to an
/// std::vector<ocpp::v2::GetVariableData>
std::vector<ocpp::v2::GetVariableData>
to_ocpp_get_variable_data_vector(const std::vector<types::ocpp::GetVariableRequest>& get_variable_request_vector);

/// \brief Converts a given types::ocpp::SetVariableRequest \p set_variable_request_vector to an
/// std::vector<ocpp::v2::SetVariableData>
std::vector<ocpp::v2::SetVariableData>
to_ocpp_set_variable_data_vector(const std::vector<types::ocpp::SetVariableRequest>& set_variable_request_vector);

/// \brief Converts a given types::ocpp::Component \p component to a ocpp::v2::Component
ocpp::v2::Component to_ocpp_component(const types::ocpp::Component& component);

/// \brief Converts a given types::ocpp::Variable \p variable to a ocpp::v2::Variable
ocpp::v2::Variable to_ocpp_variable(const types::ocpp::Variable& variable);

/// \brief Converts a given types::ocpp::EVSE \p evse to a ocpp::v2::EVSE
ocpp::v2::EVSE to_ocpp_evse(const types::ocpp::EVSE& evse);

/// \brief Converts a given types::ocpp::AttributeEnum to ocpp::v2::AttributeEnum
ocpp::v2::AttributeEnum to_ocpp_attribute_enum(const types::ocpp::AttributeEnum attribute_enum);

/// \brief Converts a given types::types::iso15118::RequestExiStreamSchema to
/// ocpp::v2::Get15118EVCertificateRequest
ocpp::v2::Get15118EVCertificateRequest
to_ocpp_get_15118_certificate_request(const types::iso15118::RequestExiStreamSchema& request);

/// \brief Converts a given types::types::iso15118::ChargingNeeds to
/// ocpp::v2::NotifyEVChargingNeedsRequest
ocpp::v2::NotifyEVChargingNeedsRequest
to_ocpp_notify_ev_charging_needs_request(const types::iso15118::ChargingNeeds& charging_needs);

/// \brief Converts a given types::reservation::ReservationResult to ocpp::v2::ReserveNowStatusEnum
ocpp::v2::ReserveNowStatusEnum to_ocpp_reservation_status(const types::reservation::ReservationResult result);

/// \brief Converts a given types::reservation::Reservation_status to ocpp::v2::ReservationUpdateStatusEnum
/// \warning This function can throw when there is no existing ocpp::v2::ReservationUpdateStatusEnum that is equal to
///          types::reservation::Reservation_status.
ocpp::v2::ReservationUpdateStatusEnum
to_ocpp_reservation_update_status_enum(const types::reservation::Reservation_status status);

/// \brief Converts a given ocpp::v2::ReasonEnum \p stop_reason to a types::evse_manager::StopTransactionReason.
types::evse_manager::StopTransactionReason to_everest_stop_transaction_reason(const ocpp::v2::ReasonEnum& stop_reason);

/// \brief Converts a given ocpp::v2::GetLogRequest \p request to a types::system::UploadLogsRequest.
types::system::UploadLogsRequest to_everest_upload_logs_request(const ocpp::v2::GetLogRequest& request);

/// \brief Converts a given ocpp::v2::UpdateFirmwareRequest \p request to a types::system::FirmwareUpdateRequest.
types::system::FirmwareUpdateRequest to_everest_firmware_update_request(const ocpp::v2::UpdateFirmwareRequest& request);

/// \brief Converts a given ocpp::v2::Iso15118EVCertificateStatusEnum \p status to a types::iso15118::Status.
types::iso15118::Status to_everest_iso15118_status(const ocpp::v2::Iso15118EVCertificateStatusEnum& status);

/// \brief Converts a given ocpp::v2::DataTransferStatusEnum \p status to a types::ocpp::DataTransferStatus.
types::ocpp::DataTransferStatus to_everest_data_transfer_status(ocpp::v2::DataTransferStatusEnum status);

/// \brief Converts a given ocpp::v2::DataTransferRequest \p status to a types::ocpp::DataTransferRequest.
types::ocpp::DataTransferRequest to_everest_data_transfer_request(ocpp::v2::DataTransferRequest request);

/// \brief Converts a given ocpp::v2::DataTransferResponse \p status to a types::ocpp::DataTransferResponse.
types::ocpp::DataTransferResponse to_everest_data_transfer_response(ocpp::v2::DataTransferResponse response);

/// \brief Converts a given ocpp::v2::AuthorizeResponse \p response to a types::authorization::ValidationResult.
types::authorization::ValidationResult to_everest_validation_result(const ocpp::v2::AuthorizeResponse& response);

/// \brief Converts a given ocpp::v2::AuthorizationStatusEnum \p status to a
/// types::authorization::AuthorizationStatus.
types::authorization::AuthorizationStatus
to_everest_authorization_status(const ocpp::v2::AuthorizationStatusEnum status);

/// \brief Converts a given ocpp::v2::IdToken \p id_token to a types::authorization::IdToken.
types::authorization::IdToken to_everest_id_token(const ocpp::v2::IdToken& id_token);

/// \brief Converts a given ocpp::v2::AuthorizeCertificateStatusEnum \p status to a
/// types::authorization::CertificateStatus.
types::authorization::CertificateStatus
to_everest_certificate_status(const ocpp::v2::AuthorizeCertificateStatusEnum status);

/// \brief Converts a given ocpp::v2::TransactionEventRequest \p transaction_event to a
/// types::ocpp::OcppTransactionEvent.
types::ocpp::OcppTransactionEvent
to_everest_ocpp_transaction_event(const ocpp::v2::TransactionEventRequest& transaction_event);

/// \brief Converts a given ocpp::v2::MessageFormat \p message_format to a
/// types::ocpp::MessageFormat
types::text_message::MessageFormat to_everest_message_format(const ocpp::v2::MessageFormatEnum& message_format);

/// \brief Converts a given ocpp::v2::MessageContent \p message_content to a
/// types::ocpp::MessageContent
types::text_message::MessageContent to_everest_message_content(const ocpp::v2::MessageContent& message_content);

/// \brief Converts a given ocpp::v2::TransactionEventResponse \p transaction_event_response to a
/// types::ocpp::OcppTransactionEventResponse
types::ocpp::OcppTransactionEventResponse
to_everest_transaction_event_response(const ocpp::v2::TransactionEventResponse& transaction_event_response);

/// \brief Converts a given ocpp::v2::BootNotificationResponse \p boot_notification_response to a
/// types::ocpp::BootNotificationResponse
types::ocpp::BootNotificationResponse
to_everest_boot_notification_response(const ocpp::v2::BootNotificationResponse& boot_notification_response);

/// \brief Converts a given ocpp::v2::RegistrationStatusEnum \p registration_status to a
/// types::ocpp::RegistrationStatus
types::ocpp::RegistrationStatus
to_everest_registration_status(const ocpp::v2::RegistrationStatusEnum& registration_status);

/// \brief Converts a given ocpp::v2::StatusInfo \p status_info to a
/// types::ocpp::StatusInfoType
types::ocpp::StatusInfoType to_everest_status_info_type(const ocpp::v2::StatusInfo& status_info);

/// \brief Converts a given ocpp::v2::GetVariableResult \p get_variable_result_vector to a
/// std::vector<types::ocpp::GetVariableResult>
std::vector<types::ocpp::GetVariableResult>
to_everest_get_variable_result_vector(const std::vector<ocpp::v2::GetVariableResult>& get_variable_result_vector);

/// \brief Converts a given ocpp::v2::SetVariableResult \p set_variable_result_vector to a
/// std::vector<types::ocpp::SetVariableResult>
std::vector<types::ocpp::SetVariableResult>
to_everest_set_variable_result_vector(const std::vector<ocpp::v2::SetVariableResult>& set_variable_result_vector);

/// \brief Converts a given ocpp::v2::Component \p component to a types::ocpp::Component.
types::ocpp::Component to_everest_component(const ocpp::v2::Component& component);

/// \brief Converts a given ocpp::v2::Variable \p variable to a types::ocpp::Variable.
types::ocpp::Variable to_everest_variable(const ocpp::v2::Variable& variable);

/// \brief Converts a given ocpp::v2::EVSE \p evse to a types::ocpp::EVSE.
types::ocpp::EVSE to_everest_evse(const ocpp::v2::EVSE& evse);

/// \brief Converts a given ocpp::v2::AttributeEnum \p attribute_enum to a types::ocpp::AttributeEnum.
types::ocpp::AttributeEnum to_everest_attribute_enum(const ocpp::v2::AttributeEnum attribute_enum);

/// \brief Converts a given ocpp::v2::GetVariableStatusEnum \p get_variable_status to a
/// types::ocpp::GetVariableStatusEnumType
types::ocpp::GetVariableStatusEnumType
to_everest_get_variable_status_enum_type(const ocpp::v2::GetVariableStatusEnum get_variable_status);

/// \brief Converts a given ocpp::v2::SetVariableStatusEnum \p set_variable_status to a
/// types::ocpp::SetVariableStatusEnumType
types::ocpp::SetVariableStatusEnumType
to_everest_set_variable_status_enum_type(const ocpp::v2::SetVariableStatusEnum set_variable_status);

/// \brief Converts a given vector of ocpp::v2::CompositeSchedule \p composite_schedules to a
/// types::ocpp::ChargingSchedules
types::ocpp::ChargingSchedules
to_everest_charging_schedules(const std::vector<ocpp::v2::CompositeSchedule>& composite_schedules);

/// \brief Converts a given ocpp::v2::CompositeSchedule \p composite_schedule to a types::ocpp::ChargingSchedule
types::ocpp::ChargingSchedule to_everest_charging_schedule(const ocpp::v2::CompositeSchedule& composite_schedule);

/// \brief Converst a given ocpp::v2::ChargingSchedulePeriod \p period to a types::ocpp::ChargingSchedulePeriod
types::ocpp::ChargingSchedulePeriod to_everest_charging_schedule_period(const ocpp::v2::ChargingSchedulePeriod& period);

ocpp::v2::DisplayMessageStatusEnum
to_ocpp_display_message_status_enum(const types::display_message::DisplayMessageStatusEnum& from);

ocpp::v2::SetDisplayMessageResponse
to_ocpp_set_display_message_response(const types::display_message::SetDisplayMessageResponse& response);

types::display_message::MessagePriorityEnum
to_everest_display_message_priority_enum(const ocpp::v2::MessagePriorityEnum& priority);
types::display_message::MessageStateEnum
to_everest_display_message_state_enum(const ocpp::v2::MessageStateEnum& message_state);

types::display_message::GetDisplayMessageRequest
to_everest_display_message_request(const ocpp::v2::GetDisplayMessagesRequest& request);

types::display_message::ClearDisplayMessageRequest
to_everest_clear_display_message_request(const ocpp::v2::ClearDisplayMessageRequest& request);

ocpp::v2::ClearMessageStatusEnum
to_ocpp_clear_message_response_enum(const types::display_message::ClearMessageResponseEnum& response_enum);

ocpp::v2::ClearDisplayMessageResponse
to_ocpp_clear_display_message_response(const types::display_message::ClearDisplayMessageResponse& response);

} // namespace conversions
} // namespace module

#endif // OCPP_V2_CONVERSIONS_HPP
