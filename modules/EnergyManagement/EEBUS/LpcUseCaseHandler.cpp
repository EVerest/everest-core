// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <LpcUseCaseHandler.hpp>

#include <everest/logging.hpp>
#include <helper.hpp>

namespace module {

namespace {
constexpr auto HEARTBEAT_TIMEOUT = std::chrono::seconds(60);
constexpr auto LPC_TIMEOUT = std::chrono::seconds(120); // Common timeout in spec
} // namespace

LpcUseCaseHandler::LpcUseCaseHandler(std::shared_ptr<ConfigValidator> config, eebus::EEBusCallbacks callbacks) :
    config(std::move(config)),
    callbacks(std::move(callbacks)),
    state(State::Init),
    heartbeat_timeout(HEARTBEAT_TIMEOUT),
    failsafe_duration_timeout(std::chrono::hours(2)), // Default to 2 hours, as per spec
    failsafe_control_limit(this->config->get_failsafe_control_limit()) {
}

LpcUseCaseHandler::~LpcUseCaseHandler() {
    stop();
}

void LpcUseCaseHandler::start() {
    this->running = true;
    this->init_timestamp = std::chrono::steady_clock::now();
    this->worker_thread = std::thread(&LpcUseCaseHandler::run, this);
    this->start_heartbeat();
}

void LpcUseCaseHandler::stop() {
    if (running.exchange(false)) {
        cv.notify_all();
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }
}

void LpcUseCaseHandler::set_stub(std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub) {
    this->stub = std::move(stub);
}

void LpcUseCaseHandler::handle_event(const control_service::UseCaseEvent& event) {
    std::lock_guard<std::mutex> lock(this->queue_mutex);
    this->event_queue.push(event);
    this->cv.notify_one();
}

void LpcUseCaseHandler::run() {
    this->set_state(State::Init);

    {
        std::lock_guard<std::mutex> lock(this->heartbeat_mutex);
        this->last_heartbeat_timestamp = std::chrono::steady_clock::now(); // Simulate initial heartbeat
    }

    while (this->running) {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        if (cv.wait_for(lock, std::chrono::seconds(1), [this] { return !this->event_queue.empty(); })) {
            // Event received
            control_service::UseCaseEvent event = this->event_queue.front();
            this->event_queue.pop();
            lock.unlock();

            const auto& event_str = event.event();

            if (event_str == "DataUpdateHeartbeat") {
                std::lock_guard<std::mutex> heartbeat_lock(this->heartbeat_mutex);
                this->last_heartbeat_timestamp = std::chrono::steady_clock::now();
                EVLOG_debug << "Heartbeat received";
            } else if (event_str == "DataUpdateLimit") {
                update_limit_from_event();
            } else if (event_str == "DataUpdateFailsafeDurationMinimum") {
                cs_lpc::FailsafeDurationMinimumRequest read_duration_req;
                cs_lpc::FailsafeDurationMinimumResponse read_duration_res;
                auto read_status =
                    cs_lpc::CallFailsafeDurationMinimum(this->stub, read_duration_req, &read_duration_res);
                if (read_status.ok()) {
                    this->failsafe_duration_timeout =
                        std::chrono::nanoseconds(read_duration_res.duration_nanoseconds());
                    EVLOG_info << "FailsafeDurationMinimum updated to " << read_duration_res.duration_nanoseconds()
                               << "ns";
                } else {
                    EVLOG_warning << "Could not re-read FailsafeDurationMinimum after update event: "
                                  << read_status.error_message();
                }
            } else if (event_str == "DataUpdateFailsafeConsumptionActivePowerLimit") {
                cs_lpc::FailsafeConsumptionActivePowerLimitRequest read_limit_req;
                cs_lpc::FailsafeConsumptionActivePowerLimitResponse read_limit_res;
                auto read_status =
                    cs_lpc::CallFailsafeConsumptionActivePowerLimit(this->stub, read_limit_req, &read_limit_res);
                if (read_status.ok()) {
                    this->failsafe_control_limit = read_limit_res.limit();
                    EVLOG_info << "FailsafeConsumptionActivePowerLimit updated to " << this->failsafe_control_limit;
                } else {
                    EVLOG_warning << "Could not re-read FailsafeConsumptionActivePowerLimit after update event: "
                                  << read_status.error_message();
                }
            } else if (event_str == "WriteApprovalRequired") {
                approve_pending_writes();
            } else if (event_str == "UseCaseSupportUpdate") {
                // ignore
            } else {
                EVLOG_error << "Unknown event received: " << event_str;
            }
        } else {
            // Timeout
            lock.unlock();
        }

        auto now = std::chrono::steady_clock::now();

        // Heartbeat check
        bool heartbeat_has_timeout = false;
        {
            std::lock_guard<std::mutex> heartbeat_lock(this->heartbeat_mutex);
            heartbeat_has_timeout = (now - this->last_heartbeat_timestamp) > this->heartbeat_timeout;
        }
        std::optional<common_types::LoadLimit> limit;
        {
            std::lock_guard<std::mutex> limit_lock(this->limit_mutex);
            limit = this->current_limit;
        }
        const bool limit_is_active = limit.has_value() && limit.value().is_active();
        const bool limit_is_deactivated = limit.has_value() && !limit.value().is_active();
        const bool limit_expired =
            limit.has_value() && !limit->delete_duration() && limit->duration_nanoseconds() != 0 &&
            now >= (std::chrono::nanoseconds(limit->duration_nanoseconds()) + this->last_limit_received_timestamp);

        switch (this->state.load()) {
        case State::Init:
            if (heartbeat_has_timeout) {
                set_state(State::Failsafe);
                break;
            }
            if ((now - this->init_timestamp) > LPC_TIMEOUT) {
                set_state(State::UnlimitedAutonomous);
                break;
            }
            if (limit_is_active) {
                set_state(State::Limited);
                break;
            }
            if (limit_is_deactivated) {
                set_state(State::UnlimitedControlled);
            }
            break;
        case State::Limited:
            if (heartbeat_has_timeout) {
                set_state(State::Failsafe);
                break;
            }
            if (limit_is_deactivated or limit_expired) {
                set_state(State::UnlimitedControlled);
            }
            // duration of limit expired, deactivated power limit received -> UnlimitedControlled
            break;
        case State::UnlimitedControlled:
            if (heartbeat_has_timeout) {
                set_state(State::Failsafe);
                break;
            }
            if (limit_is_active) {
                set_state(State::Limited);
            }
            break;
        case State::UnlimitedAutonomous:
            if (heartbeat_has_timeout) {
                set_state(State::Failsafe);
                break;
            }
            if (limit_is_deactivated or limit_expired) {
                set_state(State::UnlimitedControlled);
                break;
            }
            if (limit_is_active) {
                set_state(State::Limited);
            }
            break;
        case State::Failsafe:
            // No heartbeat stay in failsafe
            if (heartbeat_has_timeout) {
                break;
            }
            // Received heartbeat with a new limit
            if (this->last_limit_received_timestamp >= this->failsafe_entry_timestamp) {
                if (limit_is_active) {
                    set_state(State::Limited);
                } else if (limit_is_deactivated) {
                    set_state(State::UnlimitedControlled);
                }
                break;
            }
            // Received heartbeat, but no new limit within failsafe duration timeout + 120 seconds spec timeout
            if (now > (this->failsafe_entry_timestamp + this->failsafe_duration_timeout +
                       std::chrono::seconds(LPC_TIMEOUT))) {
                set_state(State::UnlimitedAutonomous);
                break;
            }
            break;
        }

        const bool state_has_changed = this->state_changed.exchange(false);
        const bool limit_has_changed = this->limit_value_changed.exchange(false);
        if (state_has_changed || limit_has_changed) {
            this->apply_limit_for_current_state();
        }
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

    cs_lpc::SetConsumptionNominalMaxRequest request1 =
        cs_lpc::CreateSetConsumptionNominalMaxRequest(this->config->get_max_nominal_power());
    cs_lpc::SetConsumptionNominalMaxResponse response1;
    auto status1 = cs_lpc::CallSetConsumptionNominalMax(this->stub, request1, &response1);
    if (!status1.ok()) {
        EVLOG_error << "SetConsumptionNominalMax failed: " << status1.error_message();
    }

    common_types::LoadLimit load_limit =
        common_types::CreateLoadLimit(0, true, false, this->config->get_max_nominal_power(), false);
    cs_lpc::SetConsumptionLimitRequest request2 = cs_lpc::CreateSetConsumptionLimitRequest(&load_limit);
    cs_lpc::SetConsumptionLimitResponse response2;
    auto status2 = cs_lpc::CallSetConsumptionLimit(this->stub, request2, &response2);
    std::ignore = request2.release_load_limit();
    if (!status2.ok()) {
        EVLOG_error << "SetConsumptionLimit failed: " << status2.error_message();
    }

    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest request3 =
        cs_lpc::CreateSetFailsafeConsumptionActivePowerLimitRequest(this->config->get_failsafe_control_limit(), true);
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse response3;
    auto status3 = cs_lpc::CallSetFailsafeConsumptionActivePowerLimit(this->stub, request3, &response3);
    if (!status3.ok()) {
        EVLOG_error << "SetFailsafeConsumptionActivePowerLimit failed: " << status3.error_message();
    }

    cs_lpc::SetFailsafeDurationMinimumRequest request4 =
        cs_lpc::CreateSetFailsafeDurationMinimumRequest(7.2e12, true); // 2 hours
    cs_lpc::SetFailsafeDurationMinimumResponse response4;
    auto status4 = cs_lpc::CallSetFailsafeDurationMinimum(this->stub, request4, &response4);
    if (!status4.ok()) {
        EVLOG_error << "SetFailsafeDurationMinimum failed: " << status4.error_message();
    }
}

void LpcUseCaseHandler::approve_pending_writes() {
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

void LpcUseCaseHandler::update_limit_from_event() {
    cs_lpc::ConsumptionLimitRequest request = cs_lpc::CreateConsumptionLimitRequest();
    cs_lpc::ConsumptionLimitResponse response;
    auto status = cs_lpc::CallConsumptionLimit(this->stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "ConsumptionLimit failed: " << status.error_message();
        return;
    }
    {
        std::lock_guard<std::mutex> lock(this->limit_mutex);
        this->current_limit = response.load_limit();
        this->last_limit_received_timestamp = std::chrono::steady_clock::now();
    }
    this->limit_value_changed = true;
}

void LpcUseCaseHandler::start_heartbeat() {
    if (!this->stub) {
        return;
    }

    cs_lpc::StartHeartbeatRequest request;
    cs_lpc::StartHeartbeatResponse response;
    auto status = cs_lpc::CallStartHeartbeat(this->stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "StartHeartbeat failed: " << status.error_message();
    }
}

void LpcUseCaseHandler::apply_limit_for_current_state() {
    types::energy::ExternalLimits limits;

    State state_copy = this->state.load();

    switch (state_copy) {
    case State::Limited: {
        std::lock_guard<std::mutex> lock(this->limit_mutex);
        if (this->current_limit.has_value()) {
            limits = translate_to_external_limits(this->current_limit.value());
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
    case State::Init:
        // TODO(mlitre): Check what to apply
        break;
    }

    this->callbacks.update_limits_callback(limits);
}

void LpcUseCaseHandler::set_state(State new_state) {
    State old_state = this->state.load();
    if (old_state != new_state) {
        EVLOG_info << "LPC Use Case Handler changing state from " << state_to_string(old_state) << " to "
                   << state_to_string(new_state);
        this->state.store(new_state);
        this->state_changed = true;

        if (new_state == State::Failsafe) {
            this->failsafe_entry_timestamp = std::chrono::steady_clock::now();
        }

        this->cv.notify_all();
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
