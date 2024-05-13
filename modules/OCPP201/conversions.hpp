// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_CONVERSIONS_HPP
#define OCPP_V201_CONVERSIONS_HPP

#include <generated/types/evse_manager.hpp>
#include <generated/types/iso15118_charger.hpp>
#include <generated/types/ocpp.hpp>
#include <generated/types/system.hpp>

#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v201/messages/Get15118EVCertificate.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

namespace module {
namespace conversions {
/// \brief Converts a given types::system::FirmwareUpdateStatusEnum \p status to a ocpp::v201::FirmwareStatusEnum.
ocpp::v201::FirmwareStatusEnum to_ocpp_firmware_status_enum(const types::system::FirmwareUpdateStatusEnum status);

/// \brief Converts a given types::ocpp::DataTransferStatus \p status to a ocpp::v201::DataTransferStatusEnum.
ocpp::v201::DataTransferStatusEnum to_ocpp_data_transfer_status_enum(types::ocpp::DataTransferStatus status);

/// \brief Converts a given types::ocpp::DataTransferRequest \p status to a ocpp::v201::DataTransferRequest.
ocpp::v201::DataTransferRequest to_ocpp_data_transfer_request(types::ocpp::DataTransferRequest request);

/// \brief Converts a given types::ocpp::DataTransferResponse \p status to a ocpp::v201::DataTransferResponse.
ocpp::v201::DataTransferResponse to_ocpp_data_transfer_response(types::ocpp::DataTransferResponse response);

/// \brief Converts the provided parameters to an ocpp::v201::SampledValue.
ocpp::v201::SampledValue to_ocpp_sampled_value(const ocpp::v201::ReadingContextEnum& reading_context,
                                               const ocpp::v201::MeasurandEnum& measurand, const std::string& unit,
                                               const std::optional<ocpp::v201::PhaseEnum> phase);

/// \brief Converts the given types::units_signed::SignedMeterValue \p signed_meter_value  to an
/// ocpp::v201::SignedMeterValue.
ocpp::v201::SignedMeterValue
to_ocpp_signed_meter_value(const types::units_signed::SignedMeterValue& signed_meter_value);

/// \brief Converts the provided parameters to an ocpp::v201::MeterValue.
ocpp::v201::MeterValue
to_ocpp_meter_value(const types::powermeter::Powermeter& power_meter,
                    const ocpp::v201::ReadingContextEnum& reading_context,
                    const std::optional<types::units_signed::SignedMeterValue> signed_meter_value);

/// \brief Converts a given types::system::UploadLogsStatus \p log_status to an ocpp::v201::LogStatusEnum.
ocpp::v201::LogStatusEnum to_ocpp_log_status_enum(types::system::UploadLogsStatus log_status);

/// \brief Converts a given types::system::UploadLogsResponse \p response to an ocpp::v201::GetLogResponse.
ocpp::v201::GetLogResponse to_ocpp_get_log_response(const types::system::UploadLogsResponse& response);

/// \brief Converts a given types::system::UpdateFirmwareResponse \p response to an
/// ocpp::v201::UpdateFirmwareStatusEnum.
ocpp::v201::UpdateFirmwareStatusEnum
to_ocpp_update_firmware_status_enum(const types::system::UpdateFirmwareResponse& response);

/// \brief Converts a given types::system::UpdateFirmwareResponse \p response to an ocpp::v201::UpdateFirmwareResponse.
ocpp::v201::UpdateFirmwareResponse
to_ocpp_update_firmware_response(const types::system::UpdateFirmwareResponse& response);

/// \brief Converts a given types::system::LogStatusEnum \p status to an ocpp::v201::UploadLogStatusEnum.
ocpp::v201::UploadLogStatusEnum to_ocpp_upload_logs_status_enum(types::system::LogStatusEnum status);

/// \brief Converts a given types::system::BootReason \p reason to an ocpp::v201::BootReasonEnum.
ocpp::v201::BootReasonEnum to_ocpp_boot_reason(types::system::BootReason reason);

/// \brief Converts a given types::evse_manager::StopTransactionReason \p reason to an ocpp::v201::ReasonEnum.
ocpp::v201::ReasonEnum to_ocpp_reason(types::evse_manager::StopTransactionReason reason);

/// \brief Converts a given types::authorization::IdTokenType \p id_token_type to an ocpp::v201::IdTokenEnum.
ocpp::v201::IdTokenEnum to_ocpp_id_token_enum(types::authorization::IdTokenType id_token_type);

/// \brief Converts a given types::authorization::IdToken \p id_token to an ocpp::v201::IdToken.
ocpp::v201::IdToken to_ocpp_id_token(const types::authorization::IdToken& id_token);

/// \brief Converts a given types::iso15118_charger::CertificateActionEnum \p action to an
/// ocpp::v201::CertificateActionEnum.
ocpp::v201::CertificateActionEnum
to_ocpp_certificate_action_enum(const types::iso15118_charger::CertificateActionEnum& action);

/// \brief Converts a vector of types::iso15118_charger::CertificateHashDataInfo to a vector of
/// ocpp::v201::OCSPRequestData.
std::vector<ocpp::v201::OCSPRequestData> to_ocpp_ocsp_request_data_vector(
    const std::vector<types::iso15118_charger::CertificateHashDataInfo>& certificate_hash_data_info);

/// \brief Converts a given types::iso15118_charger::HashAlgorithm \p hash_algorithm to an
/// ocpp::v201::HashAlgorithmEnum.
ocpp::v201::HashAlgorithmEnum to_ocpp_hash_algorithm_enum(const types::iso15118_charger::HashAlgorithm hash_algorithm);

/// \brief Converts a given types::ocpp::GetVariableRequest \p get_variable_request_vector to an
/// std::vector<ocpp::v201::GetVariableData>
std::vector<ocpp::v201::GetVariableData>
to_ocpp_get_variable_data_vector(const std::vector<types::ocpp::GetVariableRequest>& get_variable_request_vector);

/// \brief Converts a given types::ocpp::SetVariableRequest \p set_variable_request_vector to an
/// std::vector<ocpp::v201::SetVariableData>
std::vector<ocpp::v201::SetVariableData>
to_ocpp_set_variable_data_vector(const std::vector<types::ocpp::SetVariableRequest>& set_variable_request_vector);

/// \brief Converts a given types::ocpp::Component \p component to a ocpp::v201::Component
ocpp::v201::Component to_ocpp_component(const types::ocpp::Component& component);

/// \brief Converts a given types::ocpp::Variable \p variable to a ocpp::v201::Variable
ocpp::v201::Variable to_ocpp_variable(const types::ocpp::Variable& variable);

/// \brief Converts a given types::ocpp::EVSE \p evse to a ocpp::v201::EVSE
ocpp::v201::EVSE to_ocpp_evse(const types::ocpp::EVSE& evse);

/// \brief Converts a given types::ocpp::AttributeEnum to ocpp::v201::AttributeEnum
ocpp::v201::AttributeEnum to_ocpp_attribute_enum(const types::ocpp::AttributeEnum attribute_enum);

/// \brief Converts a given types::types::iso15118_charger::Request_Exi_Stream_Schema to
/// ocpp::v201::Get15118EVCertificateRequest
ocpp::v201::Get15118EVCertificateRequest
to_ocpp_get_15118_certificate_request(const types::iso15118_charger::Request_Exi_Stream_Schema& request);

/// \brief Converts a given ocpp::v201::ReasonEnum \p stop_reason to a types::evse_manager::StopTransactionReason.
types::evse_manager::StopTransactionReason
to_everest_stop_transaction_reason(const ocpp::v201::ReasonEnum& stop_reason);

/// \brief Converts a given ocpp::v201::GetLogRequest \p request to a types::system::UploadLogsRequest.
types::system::UploadLogsRequest to_everest_upload_logs_request(const ocpp::v201::GetLogRequest& request);

/// \brief Converts a given ocpp::v201::UpdateFirmwareRequest \p request to a types::system::FirmwareUpdateRequest.
types::system::FirmwareUpdateRequest
to_everest_firmware_update_request(const ocpp::v201::UpdateFirmwareRequest& request);

/// \brief Converts a given ocpp::v201::Iso15118EVCertificateStatusEnum \p status to a types::iso15118_charger::Status.
types::iso15118_charger::Status
to_everest_iso15118_charger_status(const ocpp::v201::Iso15118EVCertificateStatusEnum& status);

/// \brief Converts a given ocpp::v201::DataTransferStatusEnum \p status to a types::ocpp::DataTransferStatus.
types::ocpp::DataTransferStatus to_everest_data_transfer_status(ocpp::v201::DataTransferStatusEnum status);

/// \brief Converts a given ocpp::v201::DataTransferRequest \p status to a types::ocpp::DataTransferRequest.
types::ocpp::DataTransferRequest to_everest_data_transfer_request(ocpp::v201::DataTransferRequest request);

/// \brief Converts a given ocpp::v201::DataTransferResponse \p status to a types::ocpp::DataTransferResponse.
types::ocpp::DataTransferResponse to_everest_data_transfer_response(ocpp::v201::DataTransferResponse response);

/// \brief Converts a given ocpp::v201::AuthorizeResponse \p response to a types::authorization::ValidationResult.
types::authorization::ValidationResult to_everest_validation_result(const ocpp::v201::AuthorizeResponse& response);

/// \brief Converts a given ocpp::v201::AuthorizationStatusEnum \p status to a
/// types::authorization::AuthorizationStatus.
types::authorization::AuthorizationStatus
to_everest_authorization_status(const ocpp::v201::AuthorizationStatusEnum status);

/// \brief Converts a given ocpp::v201::IdTokenEnum \p type to a types::authorization::IdTokenType.
types::authorization::IdTokenType to_everest_id_token_type(const ocpp::v201::IdTokenEnum& type);

/// \brief Converts a given ocpp::v201::IdToken \p id_token to a types::authorization::IdToken.
types::authorization::IdToken to_everest_id_token(const ocpp::v201::IdToken& id_token);

/// \brief Converts a given ocpp::v201::AuthorizeCertificateStatusEnum \p status to a
/// types::authorization::CertificateStatus.
types::authorization::CertificateStatus
to_everest_certificate_status(const ocpp::v201::AuthorizeCertificateStatusEnum status);

/// \brief Converts a given ocpp::v201::TransactionEventRequest \p transaction_event to a
/// types::ocpp::OcppTransactionEvent.
types::ocpp::OcppTransactionEvent
to_everest_ocpp_transaction_event(const ocpp::v201::TransactionEventRequest& transaction_event);

/// \brief Converts a given ocpp::v201::MessageFormat \p message_format to a
/// types::ocpp::MessageFormat
types::ocpp::MessageFormat to_everest_message_format(const ocpp::v201::MessageFormatEnum& message_format);

/// \brief Converts a given ocpp::v201::MessageContent \p message_content to a
/// types::ocpp::MessageContent
types::ocpp::MessageContent to_everest_message_content(const ocpp::v201::MessageContent& message_content);

/// \brief Converts a given ocpp::v201::TransactionEventResponse \p transaction_event_response to a
/// types::ocpp::OcppTransactionEventResponse
types::ocpp::OcppTransactionEventResponse
to_everest_transaction_event_response(const ocpp::v201::TransactionEventResponse& transaction_event_response);

/// \brief Converts a given ocpp::v201::BootNotificationResponse \p boot_notification_response to a
/// types::ocpp::BootNotificationResponse
types::ocpp::BootNotificationResponse
to_everest_boot_notification_response(const ocpp::v201::BootNotificationResponse& boot_notification_response);

/// \brief Converts a given ocpp::v201::RegistrationStatusEnum \p registration_status to a
/// types::ocpp::RegistrationStatus
types::ocpp::RegistrationStatus
to_everest_registration_status(const ocpp::v201::RegistrationStatusEnum& registration_status);

/// \brief Converts a given ocpp::v201::StatusInfo \p status_info to a
/// types::ocpp::StatusInfoType
types::ocpp::StatusInfoType to_everest_status_info_type(const ocpp::v201::StatusInfo& status_info);

/// \brief Converts a given ocpp::v201::GetVariableResult \p get_variable_result_vector to a
/// std::vector<types::ocpp::GetVariableResult>
std::vector<types::ocpp::GetVariableResult>
to_everest_get_variable_result_vector(const std::vector<ocpp::v201::GetVariableResult>& get_variable_result_vector);

/// \brief Converts a given ocpp::v201::SetVariableResult \p set_variable_result_vector to a
/// std::vector<types::ocpp::SetVariableResult>
std::vector<types::ocpp::SetVariableResult>
to_everest_set_variable_result_vector(const std::vector<ocpp::v201::SetVariableResult>& set_variable_result_vector);

/// \brief Converts a given ocpp::v201::Component \p component to a types::ocpp::Component.
types::ocpp::Component to_everest_component(const ocpp::v201::Component& component);

/// \brief Converts a given ocpp::v201::Variable \p variable to a types::ocpp::Variable.
types::ocpp::Variable to_everest_variable(const ocpp::v201::Variable& variable);

/// \brief Converts a given ocpp::v201::EVSE \p evse to a types::ocpp::EVSE.
types::ocpp::EVSE to_everest_evse(const ocpp::v201::EVSE& evse);

/// \brief Converts a given ocpp::v201::AttributeEnum \p attribute_enum to a types::ocpp::AttributeEnum.
types::ocpp::AttributeEnum to_everest_attribute_enum(const ocpp::v201::AttributeEnum attribute_enum);

/// \brief Converts a given ocpp::v201::GetVariableStatusEnum \p get_variable_status to a
/// types::ocpp::GetVariableStatusEnumType
types::ocpp::GetVariableStatusEnumType
to_everest_get_variable_status_enum_type(const ocpp::v201::GetVariableStatusEnum get_variable_status);

/// \brief Converts a given ocpp::v201::SetVariableStatusEnum \p set_variable_status to a
/// types::ocpp::SetVariableStatusEnumType
types::ocpp::SetVariableStatusEnumType
to_everest_set_variable_status_enum_type(const ocpp::v201::SetVariableStatusEnum set_variable_status);

} // namespace conversions
} // namespace module

#endif // OCPP_V201_CONVERSIONS_HPP
