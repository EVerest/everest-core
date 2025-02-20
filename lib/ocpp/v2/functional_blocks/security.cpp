// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/functional_blocks/security.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/messages/SecurityEventNotification.hpp>
#include <ocpp/v201/utils.hpp>

#include <ocpp/v201/messages/CertificateSigned.hpp>
#include <ocpp/v201/messages/DeleteCertificate.hpp>
#include <ocpp/v201/messages/Get15118EVCertificate.hpp>
#include <ocpp/v201/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v201/messages/InstallCertificate.hpp>
#include <ocpp/v201/messages/SignCertificate.hpp>

constexpr int32_t minimum_cert_signing_wait_time_seconds = 250;

namespace ocpp::v201 {

Security::Security(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                   MessageLogging& logging, EvseSecurity& evse_security,
                   ConnectivityManagerInterface& connectivity_manager, OcspUpdaterInterface& ocsp_updater,
                   SecurityEventCallback security_event_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    logging(logging),
    evse_security(evse_security),
    connectivity_manager(connectivity_manager),
    ocsp_updater(ocsp_updater),
    security_event_callback(security_event_callback),
    csr_attempt(1),
    client_certificate_expiration_check_timer([this]() { this->scheduled_check_client_certificate_expiration(); }),
    v2g_certificate_expiration_check_timer([this]() { this->scheduled_check_v2g_certificate_expiration(); }) {
}

Security::~Security() {
    stop_certificate_signed_timer();
    stop_certificate_expiration_check_timers();
}

void Security::handle_message(const EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::CertificateSigned:
        this->handle_certificate_signed_req(json_message);
        break;
    case MessageType::SignCertificateResponse:
        this->handle_sign_certificate_response(json_message);
        break;
    case MessageType::GetInstalledCertificateIds:
        this->handle_get_installed_certificate_ids_req(json_message);
        break;
    case MessageType::InstallCertificate:
        this->handle_install_certificate_req(json_message);
        break;
    case MessageType::DeleteCertificate:
        this->handle_delete_certificate_req(json_message);
        break;
    default:
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void Security::stop_certificate_signed_timer() {
    this->certificate_signed_timer.stop();
}

Get15118EVCertificateResponse
Security::on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) {
    Get15118EVCertificateResponse response;

    if (!this->device_model
             .get_optional_value<bool>(ControllerComponentVariables::ContractCertificateInstallationEnabled)
             .value_or(false)) {
        EVLOG_warning << "Can not fulfill Get15118EVCertificateRequest because ContractCertificateInstallationEnabled "
                         "is configured as false!";
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    EVLOG_debug << "Received Get15118EVCertificateRequest " << request;
    auto future_res = this->message_dispatcher.dispatch_call_async(ocpp::Call<Get15118EVCertificateRequest>(request));

    if (future_res.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for Get15118EVCertificateRequest.conf future timed out!";
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    const auto response_message = future_res.get();
    EVLOG_debug << "Received Get15118EVCertificateResponse " << response_message.message;
    if (response_message.messageType != MessageType::Get15118EVCertificateResponse) {
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    try {
        ocpp::CallResult<Get15118EVCertificateResponse> call_result = response_message.message;
        return call_result.msg;
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(response_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->message_dispatcher.dispatch_call_error(call_error);
        return response;
    }
}

void Security::init_certificate_expiration_check_timers() {
    // Timers started with initial delays; callback functions are supposed to re-schedule on their own!

    // Client Certificate only needs to be checked for SecurityProfile 3; if SecurityProfile changes, timers get
    // re-initialized at reconnect
    if (this->device_model.get_value<int>(ControllerComponentVariables::SecurityProfile) == 3) {
        this->client_certificate_expiration_check_timer.timeout(std::chrono::seconds(
            this->device_model
                .get_optional_value<int>(ControllerComponentVariables::ClientCertificateExpireCheckInitialDelaySeconds)
                .value_or(60)));
    }

    // V2G Certificate timer is started in any case; condition (V2GCertificateInstallationEnabled) is validated in
    // callback (ChargePoint::scheduled_check_v2g_certificate_expiration)
    this->v2g_certificate_expiration_check_timer.timeout(std::chrono::seconds(
        this->device_model
            .get_optional_value<int>(ControllerComponentVariables::V2GCertificateExpireCheckInitialDelaySeconds)
            .value_or(60)));
}

void Security::stop_certificate_expiration_check_timers() {
    this->client_certificate_expiration_check_timer.stop();
    this->v2g_certificate_expiration_check_timer.stop();
}

void Security::security_event_notification_req(const CiString<50>& event_type,
                                               const std::optional<CiString<255>>& tech_info,
                                               const bool triggered_internally, const bool critical,
                                               const std::optional<DateTime>& timestamp) {
    EVLOG_debug << "Sending SecurityEventNotification";
    SecurityEventNotificationRequest req;

    req.type = event_type;
    if (timestamp.has_value()) {
        req.timestamp = timestamp.value();
    } else {
        req.timestamp = DateTime();
    }
    req.techInfo = tech_info;
    this->logging.security(json(req).dump());
    if (critical) {
        ocpp::Call<SecurityEventNotificationRequest> call(req);
        this->message_dispatcher.dispatch_call(call);
    }
    if (triggered_internally and this->security_event_callback != nullptr) {
        this->security_event_callback(event_type, tech_info);
    }
}

void Security::sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                                    const bool initiated_by_trigger_message) {
    if (this->awaited_certificate_signing_use_enum.has_value()) {
        EVLOG_warning
            << "Not sending new SignCertificate.req because still waiting for CertificateSigned.req from CSMS";
        return;
    }

    SignCertificateRequest req;

    std::optional<std::string> common;
    std::optional<std::string> country;
    std::optional<std::string> organization;
    bool should_use_tpm = false;

    if (certificate_signing_use == ocpp::CertificateSigningUseEnum::ChargingStationCertificate) {
        req.certificateType = ocpp::v201::CertificateSigningUseEnum::ChargingStationCertificate;
        common =
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::ChargeBoxSerialNumber);
        organization =
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::OrganizationName);
        country =
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrCountryName);
        should_use_tpm =
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::UseTPM).value_or(false);
    } else {
        req.certificateType = ocpp::v201::CertificateSigningUseEnum::V2GCertificate;
        common = this->device_model.get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrSeccId);
        organization = this->device_model.get_optional_value<std::string>(
            ControllerComponentVariables::ISO15118CtrlrOrganizationName);
        country =
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrCountryName);
        should_use_tpm =
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::UseTPMSeccLeafCertificate)
                .value_or(false);
    }

    if (!common.has_value()) {
        EVLOG_warning << "Missing configuration of commonName to generate CSR";
        return;
    }

    if (!country.has_value()) {
        EVLOG_warning << "Missing configuration country to generate CSR";
        return;
    }

    if (!organization.has_value()) {
        EVLOG_warning << "Missing configuration of organizationName to generate CSR";
        return;
    }

    const auto result = this->evse_security.generate_certificate_signing_request(
        certificate_signing_use, country.value(), organization.value(), common.value(), should_use_tpm);

    if (result.status != GetCertificateSignRequestStatus::Accepted or !result.csr.has_value()) {
        EVLOG_error << "CSR generation was unsuccessful for sign request: "
                    << ocpp::conversions::certificate_signing_use_enum_to_string(certificate_signing_use);

        std::string gen_error = "Sign certificate req failed due to:" +
                                ocpp::conversions::generate_certificate_signing_request_status_to_string(result.status);
        this->security_event_notification_req(ocpp::security_events::CSRGENERATIONFAILED,
                                              std::optional<CiString<255>>(gen_error), true, true);
        return;
    }

    req.csr = result.csr.value();

    this->awaited_certificate_signing_use_enum = certificate_signing_use;

    ocpp::Call<SignCertificateRequest> call(req);
    this->message_dispatcher.dispatch_call(call, initiated_by_trigger_message);
}

void Security::handle_certificate_signed_req(Call<CertificateSignedRequest> call) {
    // reset these parameters
    this->csr_attempt = 1;
    this->awaited_certificate_signing_use_enum = std::nullopt;
    this->certificate_signed_timer.stop();

    CertificateSignedResponse response;
    response.status = CertificateSignedStatusEnum::Rejected;

    const auto certificate_chain = call.msg.certificateChain.get();
    ocpp::CertificateSigningUseEnum cert_signing_use;

    if (!call.msg.certificateType.has_value() or
        call.msg.certificateType.value() == CertificateSigningUseEnum::ChargingStationCertificate) {
        cert_signing_use = ocpp::CertificateSigningUseEnum::ChargingStationCertificate;
    } else {
        cert_signing_use = ocpp::CertificateSigningUseEnum::V2GCertificate;
    }

    const auto result = this->evse_security.update_leaf_certificate(certificate_chain, cert_signing_use);

    if (result == ocpp::InstallCertificateResult::Accepted) {
        response.status = CertificateSignedStatusEnum::Accepted;
        // For V2G certificates, also trigger an OCSP cache update
        if (cert_signing_use == ocpp::CertificateSigningUseEnum::V2GCertificate) {
            this->ocsp_updater.trigger_ocsp_cache_update();
        }
    }

    // Trigger a symlink update for V2G certificates
    if ((cert_signing_use == ocpp::CertificateSigningUseEnum::V2GCertificate) and
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::UpdateCertificateSymlinks)
            .value_or(false)) {
        this->evse_security.update_certificate_links(cert_signing_use);
    }

    ocpp::CallResult<CertificateSignedResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if (result != ocpp::InstallCertificateResult::Accepted) {
        this->security_event_notification_req("InvalidChargingStationCertificate",
                                              ocpp::conversions::install_certificate_result_to_string(result), true,
                                              true);
    }

    // reconnect with new certificate if valid and security profile is 3
    if (response.status == CertificateSignedStatusEnum::Accepted and
        cert_signing_use == ocpp::CertificateSigningUseEnum::ChargingStationCertificate and
        this->device_model.get_value<int>(ControllerComponentVariables::SecurityProfile) == 3) {
        this->connectivity_manager.on_charging_station_certificate_changed();

        const auto& security_event = ocpp::security_events::RECONFIGURATIONOFSECURITYPARAMETERS;
        std::string tech_info = "Changed charging station certificate";
        this->security_event_notification_req(CiString<50>(security_event), CiString<255>(tech_info), true,
                                              utils::is_critical(security_event));
    }
}

void Security::handle_sign_certificate_response(CallResult<SignCertificateResponse> call_result) {
    if (!this->awaited_certificate_signing_use_enum.has_value()) {
        EVLOG_warning
            << "Received SignCertificate.conf while not awaiting a CertificateSigned.req . This should not happen.";
        return;
    }

    if (call_result.msg.status == GenericStatusEnum::Accepted) {
        // set timer waiting for certificate signed
        const auto cert_signing_wait_minimum =
            this->device_model.get_optional_value<int>(ControllerComponentVariables::CertSigningWaitMinimum);
        const auto cert_signing_repeat_times =
            this->device_model.get_optional_value<int>(ControllerComponentVariables::CertSigningRepeatTimes);

        if (!cert_signing_wait_minimum.has_value()) {
            EVLOG_warning << "No CertSigningWaitMinimum is configured, will not attempt to retry SignCertificate.req "
                             "in case CSMS doesn't send CertificateSigned.req";
            return;
        }
        if (!cert_signing_repeat_times.has_value()) {
            EVLOG_warning << "No CertSigningRepeatTimes is configured, will not attempt to retry SignCertificate.req "
                             "in case CSMS doesn't send CertificateSigned.req";
            return;
        }

        if (this->csr_attempt > cert_signing_repeat_times.value()) {
            this->csr_attempt = 1;
            this->certificate_signed_timer.stop();
            this->awaited_certificate_signing_use_enum = std::nullopt;
            return;
        }
        int retry_backoff_seconds =
            std::max(minimum_cert_signing_wait_time_seconds, cert_signing_wait_minimum.value()) *
            std::pow(2, this->csr_attempt); // prevent immediate repetition in case of value 0
        this->certificate_signed_timer.timeout(
            [this]() {
                EVLOG_info << "Did not receive CertificateSigned.req in time. Will retry with SignCertificate.req";
                this->csr_attempt++;
                const auto current_awaited_certificate_signing_use_enum =
                    this->awaited_certificate_signing_use_enum.value();
                this->awaited_certificate_signing_use_enum.reset();
                this->sign_certificate_req(current_awaited_certificate_signing_use_enum);
            },
            std::chrono::seconds(retry_backoff_seconds));
    } else {
        this->awaited_certificate_signing_use_enum = std::nullopt;
        this->csr_attempt = 1;
        EVLOG_warning << "SignCertificate.req has not been accepted by CSMS";
    }
}

void Security::handle_get_installed_certificate_ids_req(Call<GetInstalledCertificateIdsRequest> call) {
    EVLOG_debug << "Received GetInstalledCertificateIdsRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetInstalledCertificateIdsResponse response;

    const auto msg = call.msg;

    // prepare argument for getRootCertificate
    std::vector<ocpp::CertificateType> certificate_types;
    if (msg.certificateType.has_value()) {
        certificate_types = ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateType.value());
    } else {
        certificate_types.push_back(CertificateType::CSMSRootCertificate);
        certificate_types.push_back(CertificateType::MFRootCertificate);
        certificate_types.push_back(CertificateType::MORootCertificate);
        certificate_types.push_back(CertificateType::V2GCertificateChain);
        certificate_types.push_back(CertificateType::V2GRootCertificate);
    }

    // retrieve installed certificates
    const auto certificate_hash_data_chains = this->evse_security.get_installed_certificates(certificate_types);

    // convert the common type back to the v201 type(s) for the response
    std::vector<CertificateHashDataChain> certificate_hash_data_chain_v201;
    for (const auto& certificate_hash_data_chain_entry : certificate_hash_data_chains) {
        certificate_hash_data_chain_v201.push_back(
            ocpp::evse_security_conversions::to_ocpp_v201(certificate_hash_data_chain_entry));
    }

    if (certificate_hash_data_chain_v201.empty()) {
        response.status = GetInstalledCertificateStatusEnum::NotFound;
    } else {
        response.certificateHashDataChain = certificate_hash_data_chain_v201;
        response.status = GetInstalledCertificateStatusEnum::Accepted;
    }

    ocpp::CallResult<GetInstalledCertificateIdsResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void Security::handle_install_certificate_req(Call<InstallCertificateRequest> call) {
    EVLOG_debug << "Received InstallCertificateRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto msg = call.msg;
    InstallCertificateResponse response;

    if (!should_allow_certificate_install(msg.certificateType)) {
        response.status = InstallCertificateStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "UnsecureConnection";
        response.statusInfo->additionalInfo = "CertificateInstallationNotAllowedWithUnsecureConnection";
    } else {
        const auto result = this->evse_security.install_ca_certificate(
            msg.certificate.get(), ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateType));
        response.status = ocpp::evse_security_conversions::to_ocpp_v201(result);
        if (response.status == InstallCertificateStatusEnum::Accepted) {
            const auto& security_event = ocpp::security_events::RECONFIGURATIONOFSECURITYPARAMETERS;
            std::string tech_info =
                "Installed certificate: " + conversions::install_certificate_use_enum_to_string(msg.certificateType);
            this->security_event_notification_req(CiString<50>(security_event), CiString<255>(tech_info), true,
                                                  utils::is_critical(security_event));
        }
    }
    ocpp::CallResult<InstallCertificateResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void Security::handle_delete_certificate_req(Call<DeleteCertificateRequest> call) {
    EVLOG_debug << "Received DeleteCertificateRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto msg = call.msg;
    DeleteCertificateResponse response;

    const auto certificate_hash_data = ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateHashData);

    const auto status = this->evse_security.delete_certificate(certificate_hash_data);

    response.status = ocpp::evse_security_conversions::to_ocpp_v201(status);

    if (response.status == DeleteCertificateStatusEnum::Accepted) {
        const auto& security_event = ocpp::security_events::RECONFIGURATIONOFSECURITYPARAMETERS;
        std::string tech_info = "Deleted certificate wit serial number: " + msg.certificateHashData.serialNumber.get();
        this->security_event_notification_req(CiString<50>(security_event), CiString<255>(tech_info), true,
                                              utils::is_critical(security_event));
    }

    ocpp::CallResult<DeleteCertificateResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

bool Security::should_allow_certificate_install(InstallCertificateUseEnum cert_type) const {
    const int security_profile = this->device_model.get_value<int>(ControllerComponentVariables::SecurityProfile);

    if (security_profile > 1) {
        return true;
    }
    switch (cert_type) {
    case InstallCertificateUseEnum::CSMSRootCertificate:
        return this->device_model
            .get_optional_value<bool>(ControllerComponentVariables::AllowCSMSRootCertInstallWithUnsecureConnection)
            .value_or(true);

    case InstallCertificateUseEnum::ManufacturerRootCertificate:
        return this->device_model
            .get_optional_value<bool>(ControllerComponentVariables::AllowMFRootCertInstallWithUnsecureConnection)
            .value_or(true);
    default:
        return true;
    }
}

void Security::scheduled_check_client_certificate_expiration() {
    EVLOG_info << "Checking if CSMS client certificate has expired";
    int expiry_days_count =
        this->evse_security.get_leaf_expiry_days_count(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    if (expiry_days_count < 30) {
        EVLOG_info << "CSMS client certificate is invalid in " << expiry_days_count
                   << " days. Requesting new certificate with certificate signing request";
        this->sign_certificate_req(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    } else {
        EVLOG_info << "CSMS client certificate is still valid.";
    }

    this->client_certificate_expiration_check_timer.interval(std::chrono::seconds(
        this->device_model
            .get_optional_value<int>(ControllerComponentVariables::ClientCertificateExpireCheckIntervalSeconds)
            .value_or(12 * 60 * 60)));
}

void Security::scheduled_check_v2g_certificate_expiration() {
    if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::V2GCertificateInstallationEnabled)
            .value_or(false)) {
        EVLOG_info << "Checking if V2GCertificate has expired";
        int expiry_days_count =
            this->evse_security.get_leaf_expiry_days_count(ocpp::CertificateSigningUseEnum::V2GCertificate);
        if (expiry_days_count < 30) {
            EVLOG_info << "V2GCertificate is invalid in " << expiry_days_count
                       << " days. Requesting new certificate with certificate signing request";
            this->sign_certificate_req(ocpp::CertificateSigningUseEnum::V2GCertificate);
        } else {
            EVLOG_info << "V2GCertificate is still valid.";
        }
    } else {
        if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::PnCEnabled).value_or(false)) {
            EVLOG_warning << "PnC is enabled but V2G certificate installation is not, so no certificate expiration "
                             "check is performed.";
        }
    }

    this->v2g_certificate_expiration_check_timer.interval(std::chrono::seconds(
        this->device_model
            .get_optional_value<int>(ControllerComponentVariables::V2GCertificateExpireCheckIntervalSeconds)
            .value_or(12 * 60 * 60)));
}

} // namespace ocpp::v201
