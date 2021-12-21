// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <set>

#include <boost/asio.hpp>

#include <everest/timer.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
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

class ChargePointStateMachine {
private:
    std::map<ChargePointStatus, std::map<ChargePointStatusTransition, ChargePointStatus>> status_transitions;

    ChargePointStatus current_state;
    ChargePointStatus previous_state;
    std::mutex state_mutex;

    /// sets the previous_state to current_state, updates the current_state with the value from new_state and returns
    /// this value
    ChargePointStatus modify_state(ChargePointStatus new_state);

public:
    ChargePointStateMachine(ChargePointStatus initial_state);
    /// changes from the current state to a new state via he given transition and returns the new state
    ChargePointStatus change_state(ChargePointStatusTransition transition);
    /// if in Faulted state, returns to the previous state and returns it. If not in faulted state does not change the
    /// current state
    ChargePointStatus get_current_state();

    ChargePointStatus fault();
    ChargePointStatus fault_resolved();

    ChargePointStatus finishing();

    ChargePointStatus suspended_ev();
    ChargePointStatus suspended_evse();

    ChargePointStatus resume_ev();

    ChargePointStatus timeout();
};

struct ActiveTransaction {
    CiString20Type idTag; // FIXME remove
    IdTagInfo idTagInfo;  // FIXME remove
    int32_t transactionId;
    Everest::SteadyTimer* meter_values_sample_timer;
    std::vector<MeterValue> sampled_meter_values;
    std::vector<MeterValue> clock_aligned_meter_values;
    boost::optional<ChargingProfile> tx_charging_profile;
};

/// \brief A charging session starts with the first interaction with the EV or user
struct ChargingSession {
    boost::optional<CiString20Type> idTag;  // the presented authentication
    boost::optional<IdTagInfo> idTagInfo;   // contains the response after Authorization
    bool plug_connected;                    // if the EV is plugged in or not
    Everest::SteadyTimer* connection_timer; // checks if the EV is plugged in until a certain timeout
    boost::optional<ActiveTransaction>
        transaction; // once all conditions for charging are met this contains the transaction
};

class ChargePoint {
private:
    ChargePointConnection connection;
    MessageQueue* message_queue;
    int32_t heartbeat_interval;
    bool initialized;
    std::chrono::system_clock::time_point boot_time;
    std::set<std::string> allowed_message_types;
    RegistrationStatus registration_status;
    ChargePointConfiguration* configuration;
    Everest::SteadyTimer* heartbeat_timer;
    Everest::SteadyTimer* boot_notification_timer;
    Everest::SteadyTimer* meter_values_sample_timer;
    Everest::SystemTimer* clock_aligned_meter_values_timer;
    std::chrono::time_point<std::chrono::system_clock> clock_aligned_meter_values_time_point;
    std::map<int32_t, std::vector<MeterValue>> meter_values;
    std::mutex meter_values_mutex;
    std::map<int32_t, json> power_meter;
    std::map<int32_t, double> max_current_offered;
    std::mutex power_meter_mutex;
    std::map<int32_t, ActiveTransaction> active_transactions;
    std::map<int32_t, int32_t> transaction_at_connector;
    std::mutex active_transactions_mutex;
    std::map<int32_t, AvailabilityType> change_availability_queue;
    std::mutex change_availability_mutex;
    std::map<int32_t, ChargePointStateMachine*> status;
    std::mutex status_mutex;
    std::map<int32_t, ChargingSession> charging_session;
    std::mutex charging_session_mutex;
    std::map<int32_t, ChargingProfile> charge_point_max_profiles;
    std::mutex charge_point_max_profiles_mutex;
    std::map<int32_t, std::map<int32_t, ChargingProfile>> tx_default_profiles;
    std::mutex tx_default_profiles_mutex;

    Websocket* websocket;
    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;
    std::function<void(bool)> authorize_handler;
    std::map<MessageId, std::thread> remote_start_transaction;
    std::mutex remote_start_transaction_mutex;
    std::map<MessageId, std::thread> remote_stop_transaction;
    std::mutex remote_stop_transaction_mutex;
    std::map<MessageId, std::function<void(StartTransactionResponse)>> start_transaction_handler;
    std::mutex start_transaction_handler_mutex;
    std::map<MessageId, std::function<void(StopTransactionResponse)>> stop_transaction_handler;
    std::mutex stop_transaction_handler_mutex;

    std::thread reset_thread;

    // callbacks
    std::function<void(int32_t connector)> start_charging_callback;
    std::function<void(int32_t connector)> stop_charging_callback;
    std::function<bool(int32_t connector)> unlock_connector_callback;
    std::function<std::vector<MeterValue>(int32_t connector)> get_meter_values_callback;
    std::function<std::vector<MeterValue>(int32_t connector)> get_meter_values_signed_callback;
    std::function<int32_t(int32_t connector)> get_meter_wh_callback;
    std::function<void(int32_t connector, double max_current)> set_max_current_callback;

    void connected_callback();
    void message_callback(const std::string& message);
    bool send(json message);
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    void heartbeat();
    void boot_notification();
    void meter_values_sample();
    void meter_values_transaction_sample(int32_t connector);
    void clock_aligned_meter_values_sample();
    void connection_timeout(int32_t connector);
    void update_heartbeat_interval();
    void update_meter_values_sample_interval();
    void update_clock_aligned_meter_values_interval();
    int32_t get_meter_wh(int32_t connector);
    boost::optional<int32_t> get_transaction_id(int32_t connector);
    std::vector<MeterValue> get_meter_values(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest);
    MeterValue get_latest_meter_value(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                      ReadingContext context);
    void send_meter_values(std::vector<MeasurandWithPhase> values_of_interest, ReadingContext context);
    void send_meter_values(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                           ReadingContext context);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, CiString50Type info,
                             ChargePointStatus status, DateTime timestamp);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status);
    ActiveTransaction start_transaction(int32_t connector, CiString20Type idTag);
    StopTransactionResponse stop_transaction(CiString20Type idTag, int32_t meterStop, int32_t transactionId,
                                             Reason reason);
    void stop_all_transactions();
    void stop_all_transactions(Reason reason);

    // core profile
    void handleAuthorizeResponse(CallResult<AuthorizeResponse> call_result, Call<AuthorizeRequest> call_request);
    void handleBootNotificationResponse(CallResult<BootNotificationResponse> call_result);
    void handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call);
    void handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call);
    void handleClearCacheRequest(Call<ClearCacheRequest> call);
    void handleDataTransferRequest(Call<DataTransferRequest> call);
    void handleGetConfigurationRequest(Call<GetConfigurationRequest> call);
    void handleRemoteStartTransactionRequest(Call<RemoteStartTransactionRequest> call);
    void handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call);
    void handleResetRequest(Call<ResetRequest> call);
    void handleStartTransactionResponse(CallResult<StartTransactionResponse> call_result);
    void handleStopTransactionResponse(CallResult<StopTransactionResponse> call_result);
    void handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call);

    // smart charging profile
    void handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call);
    void handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call);
    void handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call);

public:
    ChargePoint(ChargePointConfiguration* configuration);

    void start();
    void stop();

    void receive_power_meter(int32_t connector, json powermeter);
    void receive_max_current_offered(int32_t connector, double max_current);

    // public API for Core profile

    /// authorize the provided idTag
    AuthorizationStatus authorize_id_tag(CiString20Type idTag);

    // exchange arbitrary data with central system
    DataTransferResponse data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                       const std::string data);

    /// insert plug without presenting an id tag, this either needs a present_id_tag before or after
    bool plug_connected(int32_t connector);
    // remove plug
    bool plug_disconnected(int32_t connector);
    /// insert plug with presenting an id tag, maybe for plug and charge?
    bool plug_connected_with_authorization(int32_t connector, CiString20Type idTag);
    bool present_id_tag(int32_t connector, CiString20Type idTag);
    /// EV indicates that it starts charging
    bool start_charging(int32_t connector);
    /// EV indicates that it suspends charging
    bool suspend_charging_ev(int32_t connector);
    /// EVSE indicates that it suspends charging
    bool suspend_charging_evse(int32_t connector);
    /// EV indicates that it stops charging
    bool stop_charging(int32_t connector); // TODO(kai): this probably needs a _with_authorization version too

    // register callbacks for functionality that can be triggered by the ocpp broker
    // TODO(kai): decide if we want to require them in the constructor already
    /// start charging from occp side, for example with a RemoteStartTransaction message
    void register_start_charging_callback(const std::function<void(int32_t connector)>& callback);
    /// stop charging from occp side, for example with a RemoteStopTransaction message
    void register_stop_charging_callback(const std::function<void(int32_t connector)>& callback);
    void register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback);
    void register_get_meter_values_callback(const std::function<std::vector<MeterValue>(int32_t connector)>& callback);
    void register_get_meter_values_signed_callback(
        const std::function<std::vector<MeterValue>(int32_t connector)>& callback);
    void register_get_meter_wh_callback(const std::function<int32_t(int32_t connector)>& callback);
    void register_set_max_current_callback(const std::function<void(int32_t connector, double max_current)>& callback);
};

} // namespace ocpp1_6
