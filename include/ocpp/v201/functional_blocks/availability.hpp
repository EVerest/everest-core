// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v201/message_dispatcher.hpp>
#include <ocpp/v201/message_handler.hpp>

#include <ocpp/v201/messages/ChangeAvailability.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>

namespace ocpp::v201 {

class DeviceModel;
class EvseManagerInterface;
class ComponentStateManagerInterface;

/// \brief Combines ChangeAvailabilityRequest with persist flag for scheduled Availability changes
struct AvailabilityChange {
    ChangeAvailabilityRequest request;
    bool persist;
};

class AvailabilityInterface : public MessageHandlerInterface {
public:
    virtual ~AvailabilityInterface() {
    }

    virtual void handle_message(const ocpp::EnhancedMessage<MessageType>& message) = 0;

    // Functional Block G: Availability OCPP requests.
    ///
    /// \brief Send a StatusNotificationRequest to the CSMS.
    /// \param evse_id                      Evse id.
    /// \param connector_id                 Connector id.
    /// \param status                       Status to send.
    /// \param initiated_by_trigger_message True if sending of the request was triggered by a trigger message.
    ///
    virtual void status_notification_req(const int32_t evse_id, const int32_t connector_id,
                                         const ConnectorStatusEnum status,
                                         const bool initiated_by_trigger_message = false) = 0;

    ///
    /// \brief Send a HeartbeatRequest to the CSMS.
    /// \param initiated_by_trigger_message True if sending of the request was triggered by a trigger message.
    ///
    virtual void heartbeat_req(const bool initiated_by_trigger_message = false) = 0;

    ///
    /// \brief Handle / send the scheduled change availability requests.
    /// \param evse_id  The evse id of the change availability request.
    ///
    virtual void handle_scheduled_change_availability_requests(const int32_t evse_id) = 0;

    ///
    /// \brief Set scheduled change availability requests, that should be sent later (for example because of a
    ///        firmware update).
    /// \param evse_id              The evse id.
    /// \param availability_change  The availability change request.
    ///
    virtual void set_scheduled_change_availability_requests(const int32_t evse_id,
                                                            AvailabilityChange availability_change) = 0;

    ///
    /// \brief Set the heartbeat timer interval.
    /// \param interval The interval in seconds.
    ///
    virtual void set_heartbeat_timer_interval(const std::chrono::seconds& interval) = 0;

    ///
    /// \brief Stop the heartbeat timer.
    ///
    virtual void stop_heartbeat_timer() = 0;
};

typedef std::function<void(const ocpp::DateTime& currentTime)> TimeSyncCallback;
typedef std::function<void()> AllConnectorsUnavailableCallback;

class Availability : public AvailabilityInterface {
private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    EvseManagerInterface& evse_manager;
    ComponentStateManagerInterface& component_state_manager;

    std::optional<TimeSyncCallback> time_sync_callback;
    std::optional<AllConnectorsUnavailableCallback> all_connectors_unavailable_callback;

    std::chrono::time_point<std::chrono::steady_clock> heartbeat_request_time;

    std::map<int32_t, AvailabilityChange> scheduled_change_availability_requests;
    Everest::SteadyTimer heartbeat_timer;

public:
    Availability(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                 EvseManagerInterface& evse_manager, ComponentStateManagerInterface& component_state_manager,
                 std::optional<TimeSyncCallback> time_sync_callback,
                 std::optional<AllConnectorsUnavailableCallback> all_connectors_unavailable_callback);
    ~Availability();
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

    // Functional Block G: Availability
    void status_notification_req(const int32_t evse_id, const int32_t connector_id, const ConnectorStatusEnum status,
                                 const bool initiated_by_trigger_message = false) override;
    void heartbeat_req(const bool initiated_by_trigger_message = false) override;

    void handle_scheduled_change_availability_requests(const int32_t evse_id) override;
    void set_scheduled_change_availability_requests(const int32_t evse_id,
                                                    AvailabilityChange availability_change) override;

    void set_heartbeat_timer_interval(const std::chrono::seconds& interval) override;
    void stop_heartbeat_timer() override;

private: // Functions
    // Functional Block G: Availability

    ///
    /// \brief Called on 'ChangeAvailability' request from the CSMS.
    /// \param call The call from the CSMS.
    ///
    void handle_change_availability_req(Call<ChangeAvailabilityRequest> call);

    ///
    /// \brief Called on 'HeartbeatResponse' request from the CSMS.
    /// \param call The call from the CSMS.
    ///
    void handle_heartbeat_response(CallResult<HeartbeatResponse> call);

    /// \brief Helper function to determine if the requested change results in a state that the Connector(s) is/are
    /// already in \param request \return
    bool is_already_in_state(const ChangeAvailabilityRequest& request);

    /// \brief Immediately execute the given \param request to change the operational state of a component
    /// If \param persist is set to true, the change will be persisted across a reboot
    void execute_change_availability_request(ChangeAvailabilityRequest request, bool persist);

    /// \brief Switches the operative status of the CS
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    void set_cs_operative_status(OperationalStatusEnum new_status, bool persist);

    /// \brief Switches the operative status of an EVSE
    /// \param evse_id: The ID of the EVSE, empty if the CS is addressed
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    void set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist);

    /// \brief Switches the operative status of the CS, an EVSE, or a connector, and recomputes effective statuses
    /// \param evse_id: The ID of the EVSE, empty if the CS is addressed
    /// \param connector_id: The ID of the connector, empty if an EVSE or the CS is addressed
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    void set_connector_operative_status(int32_t evse_id, int32_t connector_id, OperationalStatusEnum new_status,
                                        bool persist);
};
} // namespace ocpp::v201
