// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
class OcspUpdaterInterface;

class AvailabilityInterface;
class SecurityInterface;
class MeterValuesInterface;
class DiagnosticsInterface;
class TransactionInterface;

struct BootNotificationResponse;
struct SetVariablesRequest;
struct GetBaseReportRequest;
struct ResetRequest;
struct SetNetworkProfileRequest;

typedef std::function<void(const ocpp::DateTime& currentTime)> TimeSyncCallback;
typedef std::function<void(const BootNotificationResponse& boot_notification_response)> BootNotificationCallback;
typedef std::function<SetNetworkProfileStatusEnum(const int32_t configuration_slot,
                                                  const NetworkConnectionProfile& network_connection_profile)>
    ValidateNetworkProfileCallback;
typedef std::function<bool(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
    IsResetAllowedCallback;
typedef std::function<void(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)> ResetCallback;
typedef std::function<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
    StopTransactionCallback;
typedef std::function<void(const SetVariableData& set_variable_data)> VariableChangedCallback;

class ProvisioningInterface : public MessageHandlerInterface {
public:
    /* OCPP message requests */

    // Functional Block B: Provisioning
    virtual void boot_notification_req(const BootReasonEnum& reason,
                                       const bool initiated_by_trigger_message = false) = 0;
    virtual void stop_bootnotification_timer() = 0;
    /// \brief Event handler that will update the variable internally when it has been changed on the fly.
    /// \param set_variable_data contains data of the variable to set
    ///
    virtual void on_variable_changed(const SetVariableData& set_variable_data) = 0;

    /// \brief Gets variables specified within \p get_variable_data_vector from the device model and returns the result.
    /// This function is used internally in order to handle GetVariables.req messages and it can be used to get
    /// variables externally.
    /// \param get_variable_data_vector contains data of the variables to get
    /// \return Vector containing a result for each requested variable
    virtual std::vector<GetVariableResult>
    get_variables(const std::vector<GetVariableData>& get_variable_data_vector) = 0;

    /// \brief Sets variables specified within \p set_variable_data_vector in the device model and returns the result.
    /// \param set_variable_data_vector contains data of the variables to set
    /// \return Map containing the SetVariableData as a key and the  SetVariableResult as a value for each requested
    /// change
    virtual std::map<SetVariableData, SetVariableResult>
    set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) = 0;
};

class Provisioning : public ProvisioningInterface {
public:
    Provisioning(const FunctionalBlockContext& functional_block_context, MessageQueue<v2::MessageType>& message_queue,
                 OcspUpdaterInterface& ocsp_updater, AvailabilityInterface& availability,
                 MeterValuesInterface& meter_values, SecurityInterface& security, DiagnosticsInterface& diagnostics,
                 TransactionInterface& transaction, std::optional<TimeSyncCallback> time_sync_callback,
                 std::optional<BootNotificationCallback> boot_notification_callback,
                 std::optional<ValidateNetworkProfileCallback> validate_network_profile_callback,
                 IsResetAllowedCallback is_reset_allowed_callback, ResetCallback reset_callback,
                 StopTransactionCallback stop_transaction_callback,
                 std::optional<VariableChangedCallback> variable_changed_callback,

                 std::atomic<RegistrationStatusEnum>& registration_status);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    void boot_notification_req(const BootReasonEnum& reason, const bool initiated_by_trigger_message = false) override;
    void stop_bootnotification_timer() override;
    void on_variable_changed(const SetVariableData& set_variable_data) override;
    std::vector<GetVariableResult> get_variables(const std::vector<GetVariableData>& get_variable_data_vector) override;
    std::map<SetVariableData, SetVariableResult>
    set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) override;

private: // Members
    const FunctionalBlockContext& context;
    MessageQueue<v2::MessageType>& message_queue;
    OcspUpdaterInterface& ocsp_updater;

    AvailabilityInterface& availability;
    MeterValuesInterface& meter_values;
    SecurityInterface& security;
    DiagnosticsInterface& diagnostics;
    TransactionInterface& transaction;

    std::optional<TimeSyncCallback> time_sync_callback;
    std::optional<BootNotificationCallback> boot_notification_callback;
    std::optional<ValidateNetworkProfileCallback> validate_network_profile_callback;
    IsResetAllowedCallback is_reset_allowed_callback;
    ResetCallback reset_callback;
    StopTransactionCallback stop_transaction_callback;
    std::optional<VariableChangedCallback> variable_changed_callback;

    std::atomic<RegistrationStatusEnum>& registration_status;

    Everest::SteadyTimer boot_notification_timer;

private: // Functions
    /* OCPP message requests */

    void notify_report_req(const int request_id, const std::vector<ReportData>& report_data);

    /* OCPP message handlers */

    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(const EnhancedMessage<v2::MessageType>& message);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(const EnhancedMessage<v2::MessageType>& message);
    void handle_set_network_profile_req(Call<SetNetworkProfileRequest> call);
    void handle_reset_req(Call<ResetRequest> call);

    /* Helper functions. */

    /// \brief Helper function to determine if the requested change results in a state that the Connector(s) is/are
    /// already in \param request \return
    void handle_variable_changed(const SetVariableData& set_variable_data);
    void handle_variables_changed(const std::map<SetVariableData, SetVariableResult>& set_variable_results);
    bool validate_set_variable(const SetVariableData& set_variable_data);

    /// \brief Sets variables specified within \p set_variable_data_vector in the device model and returns the result.
    /// \param set_variable_data_vector contains data of the variables to set
    /// \param source   value source (who sets the value, for example 'csms' or 'libocpp')
    /// \param allow_read_only if true, setting VariableAttribute values with mutability ReadOnly is allowed
    /// \return Map containing the SetVariableData as a key and the  SetVariableResult as a value for each requested
    /// change
    std::map<SetVariableData, SetVariableResult>
    set_variables_internal(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source,
                           const bool allow_read_only);
};
} // namespace ocpp::v2
