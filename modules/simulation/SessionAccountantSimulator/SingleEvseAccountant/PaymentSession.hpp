/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <optional>

#include "generated/types/evse_manager.hpp"
#include "generated/types/session_cost.hpp"

#include "CostCalculator.hpp"
#include "SingleEvseAccountantTypes.hpp"
#include "Timer/SimpleTimeout.hpp"
#include "TimerFactory.hpp"


namespace SessionAccountant {

class PaymentSession {
    using stop_transaction_callback = std::function<void(types::evse_manager::StopTransactionReason reason)>;
    using update_cost_callback = std::function<void(types::session_cost::SessionCost)>;
    using ResultType = std::optional<types::session_cost::SessionCost>;

public:
    PaymentSession(const types::evse_manager::SessionEvent& session_started_event, SessionAccountant::Tariff tariff,
                   TimerFactory timer_factory);

    std::future<ResultType> get_future();
    void cancel();
    void handle_Evse_SessionEvent(const types::evse_manager::SessionEvent& event);

    void set_stop_transaction_callback(const stop_transaction_callback& callback);
    void set_update_cost_callback(const update_cost_callback& callback);
    void set_cost_limit(int32_t limit);
    void handle_pmeter_update(SessionAccountant::PowermeterUpdate powermeter);
    void handle_tariff_update(Tariff tariff);
    void finish_now(types::evse_manager::StopTransactionReason reason);

private:
    void handle_transaction_start(const types::evse_manager::SessionEvent& event);
    void handle_transaction_finish(const types::evse_manager::SessionEvent& event);
    void finish_payment_session();

    void update_session_cost();
    void check_cost_limit();

    void call_stop_transaction_callback(types::evse_manager::StopTransactionReason reason);
    void call_update_cost_callback();

    types::session_cost::SessionCost m_session_cost{};
    std::promise<ResultType> m_done_promise{};
    int32_t m_cost_limit{0};
    uint32_t m_total_cost{0};
    bool m_enabled{true};
    bool m_transaction_running{false};
    bool m_end_after_transaction_finish{false};
    std::atomic<bool> m_is_first_call_to_stop_callback{true};

    SessionAccountant::CostCalculator m_cost_calculator;
    stop_transaction_callback m_stop_transaction_callback{};
    update_cost_callback m_update_cost_callback{};
    TimerFactory m_timer_factory;
    std::shared_ptr<Timeout::TimeoutBase> m_powermeter_timeout{};
};

} // namespace SessionAccountant
