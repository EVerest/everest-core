// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/message_handler.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

#include <everest/timer.hpp>
#include <everest/util/async/monitor.hpp>

#include <ocpp/v21/messages/ClearDERControl.hpp>
#include <ocpp/v21/messages/GetDERControl.hpp>
#include <ocpp/v21/messages/NotifyDERStartStop.hpp>
#include <ocpp/v21/messages/ReportDERControl.hpp>
#include <ocpp/v21/messages/SetDERControl.hpp>

namespace ocpp::v21 {

class DERControlInterface : public v2::MessageHandlerInterface {
public:
    ~DERControlInterface() override = default;
};

/// Functional block implementing OCPP 2.1 use case R04 "Configure DER control settings at Charging Station".
/// All DER controls are persisted in the database, no in-memory cache.
class DERControl : public DERControlInterface {
public:
    explicit DERControl(const v2::FunctionalBlockContext& context);
    ~DERControl() override;

    void handle_message(const ocpp::EnhancedMessage<v2::MessageType>& message) override;

    /// Check if a controlType is supported by any EVSE (DC or AC DERCtrlr)
    bool is_control_type_supported(v2::DERControlEnum control_type) const;

    /// Periodic check for expired scheduled controls. Call this to trigger a manual check.
    void check_scheduled_controls();

private:
    const v2::FunctionalBlockContext& context;
    // Synchronization for the periodic timer callback. We need to BLOCK in
    // the destructor until any in-flight callback finishes (a non-blocking
    // atomic flag is not enough), so the bool lives inside a monitor and
    // the lambda holds a handle for the duration of the callback. The
    // destructor sets the flag under the lock, stops the timer (cancels
    // future fires), and re-acquires a handle to drain any in-flight
    // callback before member destruction. This is required because
    // FunctionalBlockContext holds a raw reference to the DatabaseHandler
    // owned by ChargePoint, and ChargePoint frees the DatabaseHandler
    // before DERControl on shutdown, so an in-flight sweep callback could
    // UAF the handler during the io-thread join inside ~SteadyTimer.
    everest::lib::util::monitor<bool> sweep_stopping{false};
    Everest::SteadyTimer scheduled_control_timer;

    void handle_set_der_control(ocpp::Call<SetDERControlRequest> call);
    void handle_get_der_control(ocpp::Call<GetDERControlRequest> call);
    void handle_clear_der_control(ocpp::Call<ClearDERControlRequest> call);

    /// Send NotifyDERStartStopRequest to CSMS
    void send_notify_start_stop(const CiString<36>& control_id, bool started, const ocpp::DateTime& timestamp,
                                const std::optional<std::vector<CiString<36>>>& superseded_ids = std::nullopt);

    /// Build and send ReportDERControlRequest(s) to CSMS
    void send_report(int32_t request_id, const std::vector<std::string>& control_jsons);

    /// Validate that the correct control field is populated for the given controlType (R04.FR.16-17)
    bool validate_control_fields(const SetDERControlRequest& req) const;
};

} // namespace ocpp::v21
