/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "SessionMonitor.hpp"

#include <atomic>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <thread>
#include <utility>

#include "PaymentSession.hpp"

namespace {
bool is_session_start(const types::evse_manager::SessionEvent& event) {
    return event.event == types::evse_manager::SessionEventEnum::SessionStarted;
}
} // namespace

namespace SessionAccountant {

SessionMonitor::SessionMonitor(TimerFactory timer_factory) {
    m_start_new_mtx = std::make_unique<std::mutex>();
    m_finish_mtx = std::make_unique<std::mutex>();
    m_timer_factory = timer_factory;
}

void SessionMonitor::set_tariff(Tariff tariff) {
    std::scoped_lock<std::mutex> lock(*m_start_new_mtx);

    m_tariff = tariff;
    if (m_payment_session) {
        m_payment_session->handle_tariff_update(tariff);
    }
}

SessionMonitor::~SessionMonitor() {
    std::scoped_lock<std::mutex> lock(*m_start_new_mtx);
    cancel_running_session();
}

void SessionMonitor::handle_Evse_SessionEvent(const types::evse_manager::SessionEvent& event) {
    std::scoped_lock<std::mutex> lock(*m_start_new_mtx);
    if (is_session_start(event)) {
        // this will start a new session, no matter what (does not test if an old session is still running!)
        handle_session_start(event);
    } else {
        // One could make handle_event throw, if the session_id does not match
        // and reset the session here
        // in case of a new session_start this will happen anyway because then
        // the first branch of this if-statement applies
        if (m_payment_session) {
            try {
                m_payment_session->handle_Evse_SessionEvent(event);
            } catch (const std::logic_error& e) {
                // TODO(CB): And now?!
            }
        }
    }
}

void SessionMonitor::handle_session_start(const types::evse_manager::SessionEvent& event) {
    // request an eventually existing old payment_session to cancel itself
    // this will make the thread below stop (via the future)
    cancel_running_session();

    m_payment_session = std::make_shared<PaymentSession>(event, m_tariff, m_timer_factory);
    m_payment_session->set_stop_transaction_callback(m_stop_transaction_callback);
    m_payment_session->set_update_cost_callback(m_cost_update_callback);

    auto future = m_payment_session->get_future();

    std::thread([this, future = std::move(future)]() mutable { await_session_finish(std::move(future)); }).detach();

    // FIXME(CB): We use the provided_token from the Transaction_Start _or_ Session_Start:
    //            Need to decide when to start the session timeout
    m_session_timeout = m_timer_factory.get_session_timer();
    m_session_timeout->set_callback([this]() {
        std::scoped_lock<std::mutex> lock(*m_start_new_mtx);
        m_payment_session->finish_now();
    });
    m_session_timeout->start();
}

void SessionMonitor::await_session_finish(std::future<ResultType>&& future) {
    auto result = future.get();

    if (result) {
        m_cost_update_callback(result.value());
    }
    std::scoped_lock<std::mutex> lock(*m_finish_mtx);
    m_session_timeout.reset();
    m_payment_session.reset();
    m_session_finish_cv.notify_all();
}

void SessionMonitor::handle_powermeter(const PowermeterUpdate& powermeter) {
    std::scoped_lock<std::mutex> lock(*m_start_new_mtx);
    if (m_payment_session) {
        m_payment_session->handle_pmeter_update(powermeter);
    }
}

void SessionMonitor::set_cost_update_callback(const CostUpdateCallback& callback) {
    m_cost_update_callback = callback;
}

void SessionMonitor::set_stop_transaction_callback(const StopTransactionCallback& callback) {
    m_stop_transaction_callback = callback;
}

void SessionMonitor::cancel_running_session() {
    if (m_payment_session) {
        std::unique_lock<std::mutex> lock(*m_finish_mtx);
        m_payment_session->cancel();
        m_session_finish_cv.wait(lock, [this] { return m_payment_session.use_count() == 0; });
    }
}

} // namespace SessionAccountant