// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP
#define MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <optional>

#include <control_service/control_service.grpc.pb.h>
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

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

    // Injectable clock — production code uses steady_clock::now(); unit tests
    // supply a fake clock to control time without real delays.
    using ClockFn = std::function<std::chrono::time_point<std::chrono::steady_clock>()>;

    LpcUseCaseHandler(
        double failsafe_control_limit, double max_nominal_power, eebus::EEBusCallbacks callbacks,
        ClockFn clock_fn = [] { return std::chrono::steady_clock::now(); });
    ~LpcUseCaseHandler() = default;
    LpcUseCaseHandler(const LpcUseCaseHandler&) = delete;
    LpcUseCaseHandler& operator=(const LpcUseCaseHandler&) = delete;
    LpcUseCaseHandler(LpcUseCaseHandler&&) = delete;
    LpcUseCaseHandler& operator=(LpcUseCaseHandler&&) = delete;

    void start();

    void set_stub(std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub);
    void handle_event(const control_service::UseCaseEvent& event);
    void run_state_machine();
    static control_service::UseCase get_use_case_info();
    void configure_use_case();

    // Process a limit received from outside (production: via gRPC; tests: directly).
    void process_received_limit(const common_types::LoadLimit& limit);

    State get_state() const {
        return this->state;
    }

private:
    using event_handler_t = void (LpcUseCaseHandler::*)();
    using TP = std::chrono::time_point<std::chrono::steady_clock>;

    void initialize_event_handlers();
    void set_state(State new_state);
    static std::string state_to_string(State state);

    void approve_pending_writes();
    void update_limit_from_event();
    void apply_limit_for_current_state();
    void start_heartbeat();

    void handle_data_update_heartbeat();
    void handle_data_update_limit();
    void handle_data_update_failsafe_duration_minimum();
    void handle_data_update_failsafe_consumption_active_power_limit();
    void handle_write_approval_required();
    void handle_use_case_support_update();

    // Per-state handlers called by run_state_machine(). Each receives only the
    // pre-computed booleans it needs; all state mutations go through set_state().
    void handle_init_state(TP now, bool limit_is_active, bool limit_is_deactivated);
    void handle_limited_state(bool heartbeat_has_timeout, bool limit_is_deactivated, bool limit_expired);
    void handle_unlimited_controlled_state(bool heartbeat_has_timeout, bool limit_is_active);
    void handle_unlimited_autonomous_state(bool limit_is_active, bool limit_is_deactivated, bool limit_expired);
    void handle_failsafe_state(TP now, bool heartbeat_has_timeout, bool limit_is_active, bool limit_is_deactivated);

    eebus::EEBusCallbacks callbacks;
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub;

    ClockFn clock_fn;

    State state;
    bool state_changed{false};

    bool limit_value_changed{false};
    std::optional<common_types::LoadLimit> current_limit;

    std::chrono::time_point<std::chrono::steady_clock> last_heartbeat_timestamp{
        std::chrono::time_point<std::chrono::steady_clock>::min()};

    std::chrono::seconds heartbeat_timeout;
    std::chrono::time_point<std::chrono::steady_clock> init_timestamp{
        std::chrono::time_point<std::chrono::steady_clock>::min()};
    // Timestamps that are set/reset atomically on every Failsafe entry.
    struct FailsafeContext {
        using TP = std::chrono::time_point<std::chrono::steady_clock>;
        TP entry{TP::min()};           // When we entered Failsafe [LPC-922 clock]
        TP first_heartbeat{TP::min()}; // First post-Failsafe heartbeat [LPC-921 clock]

        void reset(TP entry_time) {
            entry = entry_time;
            first_heartbeat = TP::min();
        }
    } failsafe_ctx;
    std::chrono::time_point<std::chrono::steady_clock> last_limit_received_timestamp{
        std::chrono::time_point<std::chrono::steady_clock>::min()};
    std::chrono::nanoseconds failsafe_duration_timeout;
    double failsafe_control_limit;
    double max_nominal_power;

    std::map<std::string, event_handler_t> event_handlers;
};

} // namespace module

#endif // MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_LPCUSECASEHANDLER_HPP
