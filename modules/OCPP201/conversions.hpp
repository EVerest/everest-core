// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_CONVERSIONS_HPP
#define OCPP_V201_CONVERSIONS_HPP

#include <generated/types/evse_manager.hpp>
#include <generated/types/iso15118_charger.hpp>
#include <generated/types/ocpp.hpp>
#include <generated/types/system.hpp>

#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

namespace module {
namespace conversions {
/// \brief Converts a given types::system::FirmwareUpdateStatusEnum \p status to a ocpp::v201::FirmwareStatusEnum.
ocpp::v201::FirmwareStatusEnum to_ocpp_firmware_status_enum(const types::system::FirmwareUpdateStatusEnum status);

/// \brief Converts a given types::ocpp::DataTransferStatus \p status to a ocpp::v201::DataTransferStatusEnum.
ocpp::v201::DataTransferStatusEnum to_ocpp_data_transfer_status_enum(types::ocpp::DataTransferStatus status);

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

} // namespace conversions
} // namespace module

#endif // OCPP_V201_CONVERSIONS_HPP
