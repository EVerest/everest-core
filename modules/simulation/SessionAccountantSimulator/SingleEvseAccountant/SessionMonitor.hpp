/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <optional>

#include "generated/types/evse_manager.hpp"
#include "generated/types/session_cost.hpp"

#include "SingleEvseAccountantTypes.hpp"
#include "Timer/SimpleTimeout.hpp"
#include "TimerFactory.hpp"

namespace SessionAccountant {

using CostUpdateCallback = std::function<void(types::session_cost::SessionCost)>;
using StopTransactionCallback = std::function<void(types::evse_manager::StopTransactionReason)>;

// Forward declarations
class PaymentSession;

class SessionMonitor {
    using ResultType = std::optional<types::session_cost::SessionCost>;
public:
    explicit SessionMonitor(TimerFactory);
    SessionMonitor(const SessionMonitor&) = delete;
    SessionMonitor(SessionMonitor&&) = delete;
    SessionMonitor& operator=(const SessionMonitor&) = delete;
    SessionMonitor& operator=(SessionMonitor&&) = delete;
    ~SessionMonitor();

    void handle_Evse_SessionEvent(const types::evse_manager::SessionEvent& event);
    void handle_powermeter(const PowermeterUpdate& powermeter);
    void set_cost_update_callback(const CostUpdateCallback& callback);
    void set_stop_transaction_callback(const StopTransactionCallback& callback);
    void set_tariff(SessionAccountant::Tariff tariff);

private:
    void handle_session_start(const types::evse_manager::SessionEvent& event);
    void call_stop_transaction_callback();
    void await_session_finish(std::future<ResultType>&& future);
    void cancel_running_session();

    SessionAccountant::Tariff m_tariff{};
    std::shared_ptr<PaymentSession> m_payment_session{};
    std::unique_ptr<std::mutex> m_start_new_mtx{};  // without the unique_ptr this class would not be movable
    std::unique_ptr<std::mutex> m_finish_mtx{};  // without the unique_ptr this class would not be movable
    std::condition_variable m_session_finish_cv;
    CostUpdateCallback m_cost_update_callback{};
    StopTransactionCallback m_stop_transaction_callback{};
    std::shared_ptr<Timeout::TimeoutBase> m_session_timeout{};
    TimerFactory m_timer_factory;
};

} // namespace SessionAccountant