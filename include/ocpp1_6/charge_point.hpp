// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <set>

#include <boost/asio.hpp>

#include <everest/timer.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/charge_point_state_machine.hpp>
#include <ocpp1_6/charging_session.hpp>
#include <ocpp1_6/message_queue.hpp>
#include <ocpp1_6/messages/Authorize.hpp>
#include <ocpp1_6/messages/BootNotification.hpp>
#include <ocpp1_6/messages/ChangeAvailability.hpp>
#include <ocpp1_6/messages/ChangeConfiguration.hpp>
#include <ocpp1_6/messages/ClearCache.hpp>
#include <ocpp1_6/messages/ClearChargingProfile.hpp>
#include <ocpp1_6/messages/DataTransfer.hpp>
#include <ocpp1_6/messages/GetCompositeSchedule.hpp>
#include <ocpp1_6/messages/GetConfiguration.hpp>
#include <ocpp1_6/messages/Heartbeat.hpp>
#include <ocpp1_6/messages/MeterValues.hpp>
#include <ocpp1_6/messages/RemoteStartTransaction.hpp>
#include <ocpp1_6/messages/RemoteStopTransaction.hpp>
#include <ocpp1_6/messages/Reset.hpp>
#include <ocpp1_6/messages/SetChargingProfile.hpp>
#include <ocpp1_6/messages/StartTransaction.hpp>
#include <ocpp1_6/messages/StatusNotification.hpp>
#include <ocpp1_6/messages/StopTransaction.hpp>
#include <ocpp1_6/messages/UnlockConnector.hpp>
#include <ocpp1_6/types.hpp>
#include <ocpp1_6/websocket.hpp>

namespace ocpp1_6 {

/// \brief Contains a ChargePoint implementation compatible with OCPP-J 1.6
class ChargePoint {
private:
    ChargePointConnectionState connection_state;
    std::unique_ptr<MessageQueue> message_queue;
    int32_t heartbeat_interval;
    bool initialized;
    std::chrono::system_clock::time_point boot_time;
    std::set<MessageType> allowed_message_types;
    std::mutex allowed_message_types_mutex;
    RegistrationStatus registration_status;
    std::unique_ptr<ChargePointStates> status;
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::unique_ptr<Everest::SteadyTimer> heartbeat_timer;
    std::unique_ptr<Everest::SteadyTimer> boot_notification_timer;
    // Everest::SteadyTimer* meter_values_sample_timer; // TODO(kai): decide if we need a continuous sampling of meter
    // values independent of transactions - maybe on connector 0, but that could be done in the chargingsession as
    // well...
    std::unique_ptr<Everest::SystemTimer> clock_aligned_meter_values_timer;
    std::chrono::time_point<std::chrono::system_clock> clock_aligned_meter_values_time_point;
    std::map<int32_t, std::vector<MeterValue>> meter_values;
    std::mutex meter_values_mutex;
    std::map<int32_t, json> power_meter;
    std::map<int32_t, double> max_current_offered;
    std::map<int32_t, int32_t> number_of_phases_available;
    std::mutex power_meter_mutex;
    std::map<int32_t, AvailabilityType> change_availability_queue; // TODO: move to Connectors
    std::mutex change_availability_mutex;                          // TODO: move to Connectors
    std::unique_ptr<ChargingSessions> charging_sessions;
    std::map<int32_t, ChargingProfile> charge_point_max_profiles;
    std::mutex charge_point_max_profiles_mutex;
    std::map<int32_t, std::map<int32_t, ChargingProfile>> tx_default_profiles;
    std::mutex tx_default_profiles_mutex;

    std::unique_ptr<Websocket> websocket;
    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;
    std::map<MessageId, std::thread> remote_start_transaction; // FIXME: this should be done differently
    std::mutex remote_start_transaction_mutex;                 // FIXME: this should be done differently
    std::map<MessageId, std::thread> remote_stop_transaction;  // FIXME: this should be done differently
    std::mutex remote_stop_transaction_mutex;                  // FIXME: this should be done differently

    std::thread reset_thread;

    // callbacks
    std::function<void(int32_t connector)> start_charging_callback;
    std::function<void(int32_t connector)> stop_charging_callback;
    std::function<bool(int32_t connector)> unlock_connector_callback;
    std::function<void(int32_t connector, double max_current)> set_max_current_callback;

    /// \brief This function is called after a successful connection to the Websocket
    void connected_callback();
    void message_callback(const std::string& message);
    bool allowed_to_send_message(json::array_t message_type);
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);
    void heartbeat();
    void boot_notification();
    void clock_aligned_meter_values_sample();
    void connection_timeout(int32_t connector);
    void update_heartbeat_interval();
    void update_meter_values_sample_interval();
    void update_clock_aligned_meter_values_interval();
    MeterValue get_latest_meter_value(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                      ReadingContext context);
    void send_meter_value(int32_t connector, MeterValue meter_value);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, CiString50Type info,
                             ChargePointStatus status, DateTime timestamp);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status);
    void stop_all_transactions();
    void stop_all_transactions(Reason reason);

    // new transaction handling:
    bool start_transaction(int32_t connector);
    int32_t get_meter_wh(int32_t connector);
    bool stop_transaction(int32_t connector, Reason reason);

    // core profile
    void handleBootNotificationResponse(
        CallResult<BootNotificationResponse> call_result); // TODO(kai):: async/promise based version?
    void handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call);
    void handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call);
    void handleClearCacheRequest(Call<ClearCacheRequest> call);
    void handleDataTransferRequest(Call<DataTransferRequest> call);
    void handleGetConfigurationRequest(Call<GetConfigurationRequest> call);
    void handleRemoteStartTransactionRequest(Call<RemoteStartTransactionRequest> call);
    void handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call);
    void handleResetRequest(Call<ResetRequest> call);
    void handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call);

    // smart charging profile
    void handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call);
    void handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call);
    void handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call);

public:
    /// \brief Creates a ChargePoint object with the provided \p configuration
    explicit ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration);

    /// \brief Starts the ChargePoint, connecting to the Websocket
    void start();

    /// \brief Stops the ChargePoint, stopping timers, transactions and disconnecting from the Websocket
    void stop();

    // public API for Core profile

    /// \brief Authorizes the provided \p idTag with the central system
    /// \returns the AuthorizationStatus
    AuthorizationStatus authorize_id_tag(CiString20Type idTag);

    /// \brief Allows the exchange of arbitrary \p data identified by a \p vendorId and \p messageId with a central
    /// system \returns the DataTransferResponse
    DataTransferResponse data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                       const std::string data);

    /// \brief Stores the given \p powermeter values for the given \p connector
    void receive_power_meter(int32_t connector, json powermeter);

    /// \brief Stores the given \p max_current for the given \p connector
    void receive_max_current_offered(int32_t connector, double max_current);

    /// \brief Stores the given \p number_of_phases for the given \p connector
    void receive_number_of_phases_available(int32_t connector, double number_of_phases);

    /// \brief Initiates a new charging session on the given \p connector with the given \p timestamp and \p
    /// energy_Wh_import as start energy
    /// \returns true if the session could be stated successfully
    bool start_session(int32_t connector, DateTime timestamp, double energy_Wh_import);

    /// \brief Stops a charging session on the given \p connector with the given \p timestamp and \p energy_Wh_import as
    /// stop energy
    /// \returns true if the session could be stopped successfully
    bool stop_session(int32_t connector, DateTime timestamp, double energy_Wh_import);

    /// \brief EV indicates that it starts charging on the given \p connector
    /// \returns true if this state change was possible
    bool start_charging(int32_t connector);

    /// \brief EV indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    bool suspend_charging_ev(int32_t connector);

    /// \brief EVSE indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    bool suspend_charging_evse(int32_t connector);

    /// \brief EV/EVSE indicates that it resumed charging on the given \p connector
    /// \returns true if this state change was possible
    bool resume_charging(int32_t connector);

    // /// EV/EVSE indicates that charging has finished
    // bool stop_charging(int32_t connector);
    /// EV/EVSE indicates that an error with the given \p error_code occured
    /// \returns true if this state change was possible
    bool error(int32_t connector, ChargePointErrorCode error_code);

    /// EV/EVSE indicates that a vendor specific error with the given \p vendor_error_code occured on the given \p
    /// connector \returns true if this state change was possible
    bool vendor_error(int32_t connector, CiString50Type vendor_error_code);

    /// EVSE indicates a permanent fault on the given \p connector
    /// \returns true if this state change was possible
    bool permanent_fault(int32_t connector);

    /// \brief registers a \p callback function that can be used to pause charging
    void register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to resume charging
    void register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to cancel charging
    void register_cancel_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to reserve a connector for a idTag until a timeout is
    /// reached
    void register_reserve_now_callback(
        const std::function<bool(int32_t connector, CiString20Type idTag, std::chrono::seconds timeout)>& callback);

    /// \brief registers a \p callback function that can be used to cancel a reservation on a connector
    void register_cancel_reservation_callback(const std::function<bool(int32_t connector)>& callback);

    /// registers a \p callback function that can be used to unlock the connector
    void register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback);

    /// registers a \p callback function that can be used to set a max_current on a given connector
    void register_set_max_current_callback(const std::function<void(int32_t connector, double max_current)>& callback);

    // FIXME: rework the following API functions, do we want to expose them?
    // insert plug
    // bool plug_connected(int32_t connector);
    // remove plug
    // bool plug_disconnected(int32_t connector);
};

} // namespace ocpp1_6
