// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <future>

#include <ocpp/common/charging_station_base.hpp>

#include <ocpp/v201/device_model_management.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>

#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/StatusNotification.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>

namespace ocpp {
namespace v201 {

/// \brief Class implements OCPP2.0.1 Charging Station
class ChargePoint : ocpp::ChargingStationBase {

private:
    std::unique_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::shared_ptr<DeviceModelManager> device_model_manager;

    std::map<int32_t, std::unique_ptr<Evse>> evses;

    // timers
    Everest::SteadyTimer heartbeat_timer;
    Everest::SteadyTimer boot_notification_timer;

    // states
    RegistrationStatusEnum registration_status;
    WebsocketConnectionStatusEnum websocket_connection_status;

    // general message handling
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);

    void init_websocket();

    void handle_message(const json& json_message, const MessageType& message_type);
    void message_callback(const std::string& message);

    // message requests

    // Provisioning
    void boot_notification_req(const BootReasonEnum& reason);
    void notify_report_req(const int request_id, const int seq_no, const std::vector<ReportData>& report_data);

    // Availability
    void status_notification_req(const int32_t evse_id, const int32_t connector_id, const ConnectorStatusEnum status);
    void heartbeat_req();

    // message handlers

    // Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(Call<GetVariablesRequest> call);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(Call<GetReportRequest> call);

public:
    /// \brief Construct a new ChargePoint object
    /// \param config OCPP json config
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param message_log_path Path to where logfiles are written to
    ChargePoint(const json& config, const std::string& ocpp_main_path, const std::string& message_log_path);

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    void start();

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    void stop();

    /// \brief Event handler that should be called when a session has started
    /// \param evse_id
    /// \param connector_id
    void on_session_started(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when a session has finished
    /// \param evse_id
    /// \param connector_id
    void on_session_finished(const int32_t evse_id, const int32_t connector_id);
};

} // namespace v201
} // namespace ocpp
