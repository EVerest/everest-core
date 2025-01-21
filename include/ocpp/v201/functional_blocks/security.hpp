// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>

#include <ocpp/v201/message_dispatcher.hpp>
#include <ocpp/v201/message_handler.hpp>
#include <ocpp/v201/ocsp_updater.hpp>

#include <ocpp/v201/messages/CertificateSigned.hpp>
#include <ocpp/v201/messages/SignCertificate.hpp>

namespace ocpp::v201 {
typedef std::function<void(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info)>
    SecurityEventCallback;

class SecurityInterface : public MessageHandlerInterface {

public:
    virtual ~SecurityInterface() override {
    }
    virtual void security_event_notification_req(const CiString<50>& event_type,
                                                 const std::optional<CiString<255>>& tech_info,
                                                 const bool triggered_internally, const bool critical,
                                                 const std::optional<DateTime>& timestamp = std::nullopt) = 0;
    virtual void sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                                      const bool initiated_by_trigger_message = false) = 0;
    virtual void stop_certificate_signed_timer() = 0;
};

class Security : public SecurityInterface {
public:
    Security(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
             MessageLogging& logging, EvseSecurity& evse_security, ConnectivityManagerInterface& connectivity_manager,
             OcspUpdaterInterface& ocsp_updater, SecurityEventCallback security_event_callback);
    virtual ~Security();
    void handle_message(const EnhancedMessage<MessageType>& message) override;
    virtual void stop_certificate_signed_timer() override;

private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    MessageLogging& logging;
    EvseSecurity& evse_security;
    ConnectivityManagerInterface& connectivity_manager;
    OcspUpdaterInterface& ocsp_updater;

    SecurityEventCallback security_event_callback;

    int csr_attempt;
    std::optional<ocpp::CertificateSigningUseEnum> awaited_certificate_signing_use_enum;
    Everest::SteadyTimer certificate_signed_timer;

private: // Functions
    /* OCPP message requests */
    virtual void security_event_notification_req(const CiString<50>& event_type,
                                                 const std::optional<CiString<255>>& tech_info,
                                                 const bool triggered_internally, const bool critical,
                                                 const std::optional<DateTime>& timestamp = std::nullopt) override;
    virtual void sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                                      const bool initiated_by_trigger_message = false) override;

    /* OCPP message handlers */
    void handle_certificate_signed_req(Call<CertificateSignedRequest> call);
    void handle_sign_certificate_response(CallResult<SignCertificateResponse> call_result);
};
} // namespace ocpp::v201
