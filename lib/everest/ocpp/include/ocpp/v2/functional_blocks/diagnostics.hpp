// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/v2/monitoring_updater.hpp>

namespace ocpp::v2 {
class AuthorizationInterface;
struct FunctionalBlockContext;

struct GetLogRequest;
struct GetLogResponse;
struct CustomerInformationRequest;
struct SetMonitoringBaseRequest;
struct SetMonitoringLevelRequest;
struct GetMonitoringReportRequest;
struct ClearVariableMonitoringRequest;

typedef std::function<GetLogResponse(const GetLogRequest& request)> GetLogRequestCallback;
typedef std::function<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                  const std::optional<IdToken> id_token,
                                  const std::optional<CiString<64>> customer_identifier)>
    GetCustomerInformationCallback;
typedef std::function<void(const std::optional<CertificateHashDataType> customer_certificate,
                           const std::optional<IdToken> id_token,
                           const std::optional<CiString<64>> customer_identifier)>
    ClearCustomerInformationCallback;

class DiagnosticsInterface : public MessageHandlerInterface {
public:
    virtual ~DiagnosticsInterface() = default;

    /* OCPP message requests */
    virtual void notify_event_req(const std::vector<EventData>& events) = 0;

    /* Monitoring */
    virtual void stop_monitoring() = 0;
    virtual void start_monitoring() = 0;
    virtual void process_triggered_monitors() = 0;
};

class Diagnostics : public DiagnosticsInterface {
public:
    Diagnostics(const FunctionalBlockContext& context, AuthorizationInterface& authorization,
                GetLogRequestCallback get_log_request_callback,
                std::optional<GetCustomerInformationCallback> get_customer_information_callback,
                std::optional<ClearCustomerInformationCallback> clear_customer_information_callback);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    void notify_event_req(const std::vector<EventData>& events) override;
    void stop_monitoring() override;
    void start_monitoring() override;
    void process_triggered_monitors() override;

private: // Members
    const FunctionalBlockContext& context;
    AuthorizationInterface& authorization;
    /// \brief Updater for triggered monitors
    MonitoringUpdater monitoring_updater;
    GetLogRequestCallback get_log_request_callback;
    /// \brief Callback function that can be used to get (human readable) customer information based on the given
    /// arguments
    std::optional<GetCustomerInformationCallback> get_customer_information_callback;

    /// \brief Callback function that can be called to clear customer information based on the given arguments
    std::optional<ClearCustomerInformationCallback> clear_customer_information_callback;
    const bool is_monitoring_available;

private: // Functions
    /* OCPP message requests */
    void notify_customer_information_req(const std::string& data, const int32_t request_id);
    void notify_monitoring_report_req(const int request_id, std::vector<MonitoringData>& montoring_data);

    /* OCPP message handlers */
    void handle_get_log_req(Call<GetLogRequest> call);
    void handle_customer_information_req(Call<CustomerInformationRequest> call);

    void handle_set_monitoring_base_req(Call<SetMonitoringBaseRequest> call);
    void handle_set_monitoring_level_req(Call<SetMonitoringLevelRequest> call);
    void handle_set_variable_monitoring_req(const EnhancedMessage<v2::MessageType>& message);
    void handle_get_monitoring_report_req(Call<GetMonitoringReportRequest> call);
    void handle_clear_variable_monitoring_req(Call<ClearVariableMonitoringRequest> call);

    /* Helper functions */

    /// \brief Returns customer information based on the given arguments. This function also executes the
    ///        get_customer_information_callback in case it is present
    /// \param customer_certificate Certificate of the customer this request refers to
    /// \param id_token IdToken of the customer this request refers to
    /// \param customer_identifier A (e.g. vendor specific) identifier of the customer this request refers to. This
    ///        field contains a custom identifier other than IdToken and Certificate
    /// \return customer information
    std::string get_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                         const std::optional<IdToken> id_token,
                                         const std::optional<CiString<64>> customer_identifier);

    /// \brief Clears customer information based on the given arguments. This function also executes the
    ///        clear_customer_information_callback in case it is present
    /// \param customer_certificate Certificate of the customer this request refers to
    /// \param id_token IdToken of the customer this request refers to
    /// \param customer_identifier A (e.g. vendor specific) identifier of the customer this request refers to. This
    ///        field contains a custom identifier other than IdToken and Certificate
    void clear_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                    const std::optional<IdToken> id_token,
                                    const std::optional<CiString<64>> customer_identifier);

    /// \brief Check if monitoring is available and if not, throw.
    /// \param type Message type to include in MessageTypeNotImplementedException when thrown.
    void throw_when_monitoring_not_available(const MessageType type);
};
} // namespace ocpp::v2
