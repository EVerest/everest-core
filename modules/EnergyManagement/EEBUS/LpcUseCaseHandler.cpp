// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <LpcUseCaseHandler.hpp>

#include <everest/logging.hpp>
#include <helper.hpp>

namespace module {

namespace {
// Per LPC-911 / LPC-912 the CS enters Failsafe after 120 s without a heartbeat.
constexpr auto HEARTBEAT_TIMEOUT = std::chrono::seconds(120);
// LPC_INIT_TIMEOUT: time limit for reaching a controlled state from Init [LPC-906].
constexpr auto LPC_INIT_TIMEOUT = std::chrono::seconds(120);
// LPC_RECOVERY_TIMEOUT: window after the first post-Failsafe heartbeat; if no limit arrives
// within this window, the CS MAY exit to Unlimited/autonomous [LPC-921].
constexpr auto LPC_RECOVERY_TIMEOUT = std::chrono::seconds(120);
// Default failsafe duration; matches the value pushed to the gRPC service in configure_use_case().
constexpr auto LPC_DEFAULT_FAILSAFE_DURATION = std::chrono::hours(2);

using TP = std::chrono::time_point<std::chrono::steady_clock>;
} // namespace

LpcUseCaseHandler::LpcUseCaseHandler(double failsafe_control_limit, double max_nominal_power,
                                     eebus::EEBusCallbacks callbacks, ClockFn clock_fn) :
    callbacks(std::move(callbacks)),
    clock_fn(std::move(clock_fn)),
    state(State::Init),
    heartbeat_timeout(HEARTBEAT_TIMEOUT),
    failsafe_duration_timeout(LPC_DEFAULT_FAILSAFE_DURATION),
    failsafe_control_limit(failsafe_control_limit),
    max_nominal_power(max_nominal_power) {
    this->initialize_event_handlers();
}

void LpcUseCaseHandler::initialize_event_handlers() {
    this->event_handlers = {
        {"DataUpdateHeartbeat", &LpcUseCaseHandler::handle_data_update_heartbeat},
        {"DataUpdateLimit", &LpcUseCaseHandler::handle_data_update_limit},
        {"DataUpdateFailsafeDurationMinimum", &LpcUseCaseHandler::handle_data_update_failsafe_duration_minimum},
        {"DataUpdateFailsafeConsumptionActivePowerLimit",
         &LpcUseCaseHandler::handle_data_update_failsafe_consumption_active_power_limit},
        {"WriteApprovalRequired", &LpcUseCaseHandler::handle_write_approval_required},
        {"UseCaseSupportUpdate", &LpcUseCaseHandler::handle_use_case_support_update},
    };
}

void LpcUseCaseHandler::start() {
    this->init_timestamp = this->clock_fn();
    this->last_heartbeat_timestamp = this->clock_fn(); // Seed to prevent spurious timeout on startup
    this->start_heartbeat();
    // Per [LPC-901/1] the CS SHALL start in Init already limited by the Failsafe Consumption
    // Active Power Limit, so apply it immediately rather than waiting for the first state change.
    this->apply_limit_for_current_state();
}

void LpcUseCaseHandler::set_stub(std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub) {
    this->stub = std::move(stub);
}

void LpcUseCaseHandler::handle_event(const control_service::UseCaseEvent& event) {
    const auto& event_str = event.event();

    auto it = this->event_handlers.find(event_str);
    if (it != this->event_handlers.end()) {
        (this->*(it->second))();
    } else if (!event_str.empty()) {
        EVLOG_error << "Unknown event received: " << event_str;
    }

    this->run_state_machine();
}

void LpcUseCaseHandler::handle_data_update_heartbeat() {
    this->last_heartbeat_timestamp = this->clock_fn();
    EVLOG_debug << "Heartbeat received";
}

void LpcUseCaseHandler::handle_data_update_limit() {
    update_limit_from_event();
}

void LpcUseCaseHandler::handle_data_update_failsafe_duration_minimum() {
    cs_lpc::FailsafeDurationMinimumRequest read_duration_req;
    cs_lpc::FailsafeDurationMinimumResponse read_duration_res;
    auto read_status = cs_lpc::CallFailsafeDurationMinimum(this->stub, read_duration_req, &read_duration_res);
    if (read_status.ok()) {
        this->failsafe_duration_timeout = std::chrono::nanoseconds(read_duration_res.duration_nanoseconds());
        EVLOG_info << "FailsafeDurationMinimum updated to " << read_duration_res.duration_nanoseconds() << "ns";
    } else {
        EVLOG_warning << "Could not re-read FailsafeDurationMinimum after update event: "
                      << read_status.error_message();
    }
}

void LpcUseCaseHandler::handle_data_update_failsafe_consumption_active_power_limit() {
    cs_lpc::FailsafeConsumptionActivePowerLimitRequest read_limit_req;
    cs_lpc::FailsafeConsumptionActivePowerLimitResponse read_limit_res;
    auto read_status = cs_lpc::CallFailsafeConsumptionActivePowerLimit(this->stub, read_limit_req, &read_limit_res);
    if (read_status.ok()) {
        this->failsafe_control_limit = read_limit_res.limit();
        EVLOG_info << "FailsafeConsumptionActivePowerLimit updated to " << this->failsafe_control_limit;
    } else {
        EVLOG_warning << "Could not re-read FailsafeConsumptionActivePowerLimit after update event: "
                      << read_status.error_message();
    }
}

void LpcUseCaseHandler::handle_write_approval_required() {
    approve_pending_writes();
}

void LpcUseCaseHandler::handle_use_case_support_update() {
    // The LPC use case does not require action on support changes from the remote HEMS;
    // use-case support negotiation is handled at the connection level by EebusConnectionHandler.
    EVLOG_debug << "Received UseCaseSupportUpdate event (ignored at LPC handler level)";
}

void LpcUseCaseHandler::run_state_machine() {
    if (this->init_timestamp == TP::min()) {
        return;
    }

    auto now = this->clock_fn();

    bool heartbeat_has_timeout = (now - this->last_heartbeat_timestamp) > this->heartbeat_timeout;
    // Capture a local snapshot of current_limit for stable evaluation; see process_received_limit().
    std::optional<common_types::LoadLimit> limit = this->current_limit;
    const bool limit_is_active = limit.has_value() && limit->is_active();
    const bool limit_is_deactivated = limit.has_value() && !limit->is_active();
    const bool limit_expired =
        limit.has_value() && !limit->delete_duration() && limit->duration_nanoseconds() != 0 &&
        now >= (this->last_limit_received_timestamp + std::chrono::nanoseconds(limit->duration_nanoseconds()));

    switch (this->state) {
    case State::Init:
        handle_init_state(now, limit_is_active, limit_is_deactivated);
        break;
    case State::Limited:
        handle_limited_state(heartbeat_has_timeout, limit_is_deactivated, limit_expired);
        break;
    case State::UnlimitedControlled:
        handle_unlimited_controlled_state(heartbeat_has_timeout, limit_is_active);
        break;
    case State::UnlimitedAutonomous:
        handle_unlimited_autonomous_state(limit_is_active, limit_is_deactivated, limit_expired);
        break;
    case State::Failsafe:
        handle_failsafe_state(now, heartbeat_has_timeout, limit_is_active, limit_is_deactivated);
        break;
    }

    if (this->state_changed || this->limit_value_changed) {
        this->state_changed = false;
        this->limit_value_changed = false;
        this->apply_limit_for_current_state();
    }
}

void LpcUseCaseHandler::handle_init_state(TP now, bool limit_is_active, bool limit_is_deactivated) {
    // Three outgoing edges per spec Figure 2: [LPC-902/904/905/906]
    if ((now - this->init_timestamp) > LPC_INIT_TIMEOUT) {
        set_state(State::UnlimitedAutonomous);
    } else if (limit_is_active) {
        set_state(State::Limited);
    } else if (limit_is_deactivated) {
        set_state(State::UnlimitedControlled);
    }
}

void LpcUseCaseHandler::handle_limited_state(bool heartbeat_has_timeout, bool limit_is_deactivated,
                                             bool limit_expired) {
    if (heartbeat_has_timeout) {
        set_state(State::Failsafe);
    } else if (limit_is_deactivated || limit_expired) {
        // Either the HEMS explicitly deactivated the limit, or its duration elapsed.
        set_state(State::UnlimitedControlled);
    }
}

void LpcUseCaseHandler::handle_unlimited_controlled_state(bool heartbeat_has_timeout, bool limit_is_active) {
    if (heartbeat_has_timeout) {
        set_state(State::Failsafe);
    } else if (limit_is_active) {
        set_state(State::Limited);
    }
}

void LpcUseCaseHandler::handle_unlimited_autonomous_state(bool limit_is_active, bool limit_is_deactivated,
                                                          bool limit_expired) {
    // Two outgoing edges per spec: [LPC-918/919/920]. No heartbeat-timeout edge.
    if (limit_is_deactivated || limit_expired) {
        set_state(State::UnlimitedControlled);
    } else if (limit_is_active) {
        set_state(State::Limited);
    }
}

void LpcUseCaseHandler::handle_failsafe_state(TP now, bool heartbeat_has_timeout, bool limit_is_active,
                                              bool limit_is_deactivated) {
    // [LPC-922]: duration minimum expired — exit regardless of heartbeat.
    if (now >= (this->failsafe_ctx.entry + this->failsafe_duration_timeout)) {
        set_state(State::UnlimitedAutonomous);
        return;
    }

    // [LPC-921]: 120 s after first post-Failsafe heartbeat with no new limit — exit.
    if (this->failsafe_ctx.first_heartbeat != TP::min() &&
        now >= (this->failsafe_ctx.first_heartbeat + LPC_RECOVERY_TIMEOUT)) {
        set_state(State::UnlimitedAutonomous);
        return;
    }

    // No heartbeat from EG — stay in Failsafe.
    if (heartbeat_has_timeout) {
        return;
    }

    // Heartbeat received — check whether a new limit arrived after Failsafe entry [LPC-916].
    if (this->last_limit_received_timestamp >= this->failsafe_ctx.entry) {
        set_state(limit_is_active ? State::Limited : State::UnlimitedControlled);
        return;
    }

    // Heartbeat received but no new limit yet — start the LPC-921 window on first occurrence.
    if (this->failsafe_ctx.first_heartbeat == TP::min()) {
        this->failsafe_ctx.first_heartbeat = this->last_heartbeat_timestamp;
    }
}

control_service::UseCase LpcUseCaseHandler::get_use_case_info() {
    return control_service::CreateUseCase(
        control_service::UseCase_ActorType_Enum::UseCase_ActorType_Enum_ControllableSystem,
        control_service::UseCase_NameType_Enum::UseCase_NameType_Enum_limitationOfPowerConsumption);
}

void LpcUseCaseHandler::configure_use_case() {
    if (!this->stub) {
        return;
    }

    // Helper: execute a gRPC call and log on failure. Returns true if the call succeeded.
    auto call = [](std::string_view name, const grpc::Status& status) {
        if (!status.ok()) {
            EVLOG_error << name << " failed: " << status.error_message();
        }
        return status.ok();
    };

    cs_lpc::SetConsumptionNominalMaxRequest req1 =
        cs_lpc::CreateSetConsumptionNominalMaxRequest(this->max_nominal_power);
    cs_lpc::SetConsumptionNominalMaxResponse resp1;
    call("SetConsumptionNominalMax", cs_lpc::CallSetConsumptionNominalMax(this->stub, req1, &resp1));

    common_types::LoadLimit load_limit = common_types::CreateLoadLimit(0, true, false, this->max_nominal_power, false);
    cs_lpc::SetConsumptionLimitRequest req2 = cs_lpc::CreateSetConsumptionLimitRequest(&load_limit);
    cs_lpc::SetConsumptionLimitResponse resp2;
    call("SetConsumptionLimit", cs_lpc::CallSetConsumptionLimit(this->stub, req2, &resp2));
    // Release the stack-allocated load_limit from Protobuf ownership before req2 is destroyed,
    // to prevent Protobuf from attempting to delete memory it doesn't own.
    std::ignore = req2.release_load_limit();

    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest req3 =
        cs_lpc::CreateSetFailsafeConsumptionActivePowerLimitRequest(this->failsafe_control_limit, true);
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse resp3;
    call("SetFailsafeConsumptionActivePowerLimit",
         cs_lpc::CallSetFailsafeConsumptionActivePowerLimit(this->stub, req3, &resp3));

    // Convert the default failsafe duration to nanoseconds for the gRPC request.
    const auto failsafe_duration_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(LPC_DEFAULT_FAILSAFE_DURATION).count();
    cs_lpc::SetFailsafeDurationMinimumRequest req4 =
        cs_lpc::CreateSetFailsafeDurationMinimumRequest(static_cast<double>(failsafe_duration_ns), true);
    cs_lpc::SetFailsafeDurationMinimumResponse resp4;
    call("SetFailsafeDurationMinimum", cs_lpc::CallSetFailsafeDurationMinimum(this->stub, req4, &resp4));
}

void LpcUseCaseHandler::approve_pending_writes() {
    if (!this->stub) {
        return;
    }

    cs_lpc::PendingConsumptionLimitRequest request = cs_lpc::CreatePendingConsumptionLimitRequest();
    cs_lpc::PendingConsumptionLimitResponse response;
    auto status = cs_lpc::CallPendingConsumptionLimit(this->stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "PendingConsumptionLimit failed: " << status.error_message();
        return;
    }

    for (const auto& entry : response.load_limits()) {
        uint64_t msg_counter = entry.first;
        cs_lpc::ApproveOrDenyConsumptionLimitRequest approve_request =
            cs_lpc::CreateApproveOrDenyConsumptionLimitRequest(msg_counter, true, "");
        cs_lpc::ApproveOrDenyConsumptionLimitResponse approve_response;
        auto approve_status = cs_lpc::CallApproveOrDenyConsumptionLimit(this->stub, approve_request, &approve_response);
        if (!approve_status.ok()) {
            EVLOG_error << "ApproveOrDenyConsumptionLimit failed for msg_counter " << msg_counter << ": "
                        << approve_status.error_message();
        }
    }
}

void LpcUseCaseHandler::process_received_limit(const common_types::LoadLimit& limit) {
    this->current_limit = limit;
    this->last_limit_received_timestamp = this->clock_fn();
    this->limit_value_changed = true;
    this->run_state_machine();
}

void LpcUseCaseHandler::update_limit_from_event() {
    if (!this->stub) {
        return;
    }

    cs_lpc::ConsumptionLimitRequest request = cs_lpc::CreateConsumptionLimitRequest();
    cs_lpc::ConsumptionLimitResponse response;
    auto status = cs_lpc::CallConsumptionLimit(this->stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "ConsumptionLimit failed: " << status.error_message()
                    << ". Continuing with stale limit data; state machine may be holding a power "
                    << "constraint the remote energy manager has already revoked.";
        return;
    }
    this->process_received_limit(response.load_limit());
}

void LpcUseCaseHandler::start_heartbeat() {
    if (!this->stub) {
        return;
    }

    cs_lpc::StartHeartbeatRequest request;
    cs_lpc::StartHeartbeatResponse response;
    auto status = cs_lpc::CallStartHeartbeat(this->stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "StartHeartbeat subscription failed: " << status.error_message()
                    << ". System will enter Failsafe state in approximately 120 seconds unless the stub "
                    << "connection is restored.";
    }
}

void LpcUseCaseHandler::apply_limit_for_current_state() {
    if (!this->callbacks.all_callbacks_valid()) {
        EVLOG_error << "LPC Use Case Handler callbacks not initialized; skipping limit update";
        return;
    }

    types::energy::ExternalLimits limits;

    switch (this->state) {
    case State::Init: {
        // Per [LPC-901/1] the CS SHALL already be limited by the Failsafe Consumption Active
        // Power Limit while in Init. The Active Power Consumption Limit data point is
        // deactivated [LPC-009/2], but the physical power cap still applies.
        common_types::LoadLimit failsafe_limit;
        failsafe_limit.set_is_active(true);
        failsafe_limit.set_value(this->failsafe_control_limit);
        limits = translate_to_external_limits(failsafe_limit);
        break;
    }
    case State::Limited: {
        if (this->current_limit.has_value()) {
            limits = translate_to_external_limits(this->current_limit.value());
        } else {
            // No valid limit data in Limited state — fall back to failsafe limit to avoid
            // unexpected unlimited power.
            common_types::LoadLimit failsafe_limit;
            failsafe_limit.set_is_active(true);
            failsafe_limit.set_value(this->failsafe_control_limit);
            limits = translate_to_external_limits(failsafe_limit);
            EVLOG_warning << "Limited state without valid limit; applying failsafe limit";
        }
        break;
    }
    case State::Failsafe: {
        common_types::LoadLimit failsafe_limit;
        failsafe_limit.set_is_active(true);
        failsafe_limit.set_value(this->failsafe_control_limit);
        limits = translate_to_external_limits(failsafe_limit);
        break;
    }
    case State::UnlimitedControlled:
    case State::UnlimitedAutonomous: {
        common_types::LoadLimit unlimited_limit;
        unlimited_limit.set_is_active(false);
        limits = translate_to_external_limits(unlimited_limit);
        break;
    }
    }

    this->callbacks.update_limits_callback(limits);
}

void LpcUseCaseHandler::set_state(State new_state) {
    State old_state = this->state;
    if (old_state != new_state) {
        EVLOG_info << "LPC Use Case Handler changing state from " << state_to_string(old_state) << " to "
                   << state_to_string(new_state);
        this->state = new_state;
        this->state_changed = true;

        if (new_state == State::Failsafe) {
            // reset() sets entry timestamp and clears the LPC-921 heartbeat window.
            this->failsafe_ctx.reset(this->clock_fn());
            // Discard any limit the EG had set before we lost contact. After Failsafe exit
            // the EG must reconnect and explicitly resend a limit; the stale value must not
            // drive transitions in post-Failsafe states (UnlimitedAutonomous, UnlimitedControlled).
            this->current_limit = std::nullopt;
        }
    }
}

std::string LpcUseCaseHandler::state_to_string(State state) {
    switch (state) {
    case State::Init:
        return "Init";
    case State::UnlimitedControlled:
        return "Unlimited/controlled";
    case State::Limited:
        return "Limited";
    case State::Failsafe:
        return "Failsafe state";
    case State::UnlimitedAutonomous:
        return "Unlimited/autonomous";
    }
    return "Unknown";
}

} // namespace module
