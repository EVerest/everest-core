// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <LpcUseCaseHandler.hpp>

#include <everest/logging.hpp>
#include <helper.hpp>

namespace module {

namespace {
constexpr auto HEARTBEAT_TIMEOUT = std::chrono::seconds(120);
} // namespace

LpcUseCaseHandler::LpcUseCaseHandler(std::shared_ptr<ConfigValidator> config, eebus::EEBusCallbacks callbacks) :
    config(std::move(config)),
    callbacks(std::move(callbacks)),
    state(State::Init),
    heartbeat_timeout(HEARTBEAT_TIMEOUT) {
}

LpcUseCaseHandler::~LpcUseCaseHandler() {
    stop();
}

void LpcUseCaseHandler::start() {
    this->running = true;
    this->init_timestamp = std::chrono::steady_clock::now();
    this->worker_thread = std::thread(&LpcUseCaseHandler::run, this);
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
                if (this->state == State::UnlimitedAutonomous || this->state == State::Failsafe) {
                    set_state(State::UnlimitedControlled);
                }
            } else if (event_str == "DataUpdateLimit") {
                if (this->state != State::Failsafe) {
                    update_limit_from_event();
                    std::lock_guard<std::mutex> limit_lock(this->limit_mutex);
                    if (this->current_limit.has_value() && this->current_limit.value().is_active()) {
                        set_state(State::Limited);
                    } else {
                        set_state(State::UnlimitedControlled);
                    }
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
        if (this->state != State::Failsafe) {
            std::lock_guard<std::mutex> heartbeat_lock(this->heartbeat_mutex);
            if ((now - this->last_heartbeat_timestamp) > this->heartbeat_timeout) {
                set_state(State::Failsafe);
                continue; // Re-evaluate state in next loop
            }
        }

        // Timeout check for Init state
        if (this->state == State::Init) {
            if ((now - this->init_timestamp) > HEARTBEAT_TIMEOUT) {
                set_state(State::UnlimitedAutonomous);
            }
        }

        if (this->state_changed.exchange(false) || this->limit_value_changed.exchange(false)) {
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
        common_types::CreateLoadLimit(0, true, false, this->config->get_failsafe_control_limit(), false);
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
    }
    this->limit_value_changed = true;
}

void LpcUseCaseHandler::apply_limit_for_current_state() {
    types::energy::ExternalLimits limits;

    State state_copy = State::Init;
    {
        std::lock_guard<std::mutex> lock(this->state_mutex);
        state_copy = this->state;
    }

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
        failsafe_limit.set_value(this->config->get_failsafe_control_limit());
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
    std::lock_guard<std::mutex> lock(this->state_mutex);
    if (this->state != new_state) {
        EVLOG_info << "LPC Use Case Handler changing state from " << state_to_string(this->state) << " to "
                   << state_to_string(new_state);
        this->state = new_state;
        this->state_changed = true;
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
