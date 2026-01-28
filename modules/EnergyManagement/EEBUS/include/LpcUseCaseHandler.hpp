// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP
#define MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <optional>
#include <queue>
#include <thread>

#include <control_service/control_service.grpc.pb.h>
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

#include <ConfigValidator.hpp>
#include <EebusCallbacks.hpp>

namespace module {

class LpcUseCaseHandler {
public:
    enum class State : uint8_t {
        Init,
        UnlimitedControlled,
        Limited,
        Failsafe,
        UnlimitedAutonomous
    };

    LpcUseCaseHandler(std::shared_ptr<ConfigValidator> config, eebus::EEBusCallbacks callbacks);
    ~LpcUseCaseHandler();
    LpcUseCaseHandler(const LpcUseCaseHandler&) = delete;
    LpcUseCaseHandler& operator=(const LpcUseCaseHandler&) = delete;
    LpcUseCaseHandler(LpcUseCaseHandler&&) = delete;
    LpcUseCaseHandler& operator=(LpcUseCaseHandler&&) = delete;

    void start();
    void stop();

    void set_stub(std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub);
    void handle_event(const control_service::UseCaseEvent& event);
    static control_service::UseCase get_use_case_info();
    void configure_use_case();

private:
    void run();
    void set_state(State new_state);
    static std::string state_to_string(State state);

    void approve_pending_writes();
    void update_limit_from_event();
    void apply_limit_for_current_state();
    void start_heartbeat();

    std::shared_ptr<ConfigValidator> config;
    eebus::EEBusCallbacks callbacks;
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub;

    std::thread worker_thread;
    std::atomic<bool> running{false};
    std::atomic<State> state;
    std::atomic<bool> state_changed{false};
    std::condition_variable cv;

    std::queue<control_service::UseCaseEvent> event_queue;
    std::mutex queue_mutex;

    std::atomic<bool> limit_value_changed{false};
    std::optional<common_types::LoadLimit> current_limit;
    std::mutex limit_mutex;

    std::chrono::time_point<std::chrono::steady_clock> last_heartbeat_timestamp;
    std::mutex heartbeat_mutex;

    std::chrono::seconds heartbeat_timeout;
    std::chrono::time_point<std::chrono::steady_clock> init_timestamp;
    std::chrono::time_point<std::chrono::steady_clock> failsafe_entry_timestamp;
    std::chrono::time_point<std::chrono::steady_clock> last_limit_received_timestamp;
    std::chrono::nanoseconds failsafe_duration_timeout;
    double failsafe_control_limit;
};

} // namespace module

#endif // MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP
