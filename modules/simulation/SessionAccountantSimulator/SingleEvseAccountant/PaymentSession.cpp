/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "PaymentSession.hpp"

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

namespace {

// TODO(CB): this function is more or less a duplicate from PaymentTerminalSimulator
uint32_t calc_total_cost(const std::vector<types::session_cost::SessionCostChunk>& cost_chunks) {
    auto sum_optionals = [](uint32_t lhs, std::optional<types::session_cost::SessionCostChunk> rhs) {
        return lhs + (rhs ? (rhs->cost ? rhs->cost->value : 0) : 0);
    };

    uint32_t total_cost{0};
    total_cost = std::accumulate(cost_chunks.cbegin(), cost_chunks.cend(), 0, sum_optionals);
    return total_cost;
}

} // namespace

namespace SessionAccountant {

PaymentSession::PaymentSession(const types::evse_manager::SessionEvent& session_started_event,
                               SessionAccountant::Tariff tariff, SessionAccountant::TimerFactory timer_factory) :
    m_cost_calculator(tariff),
    m_reserved_amount_of_money(tariff.reserved_amount_of_money_mau),
    m_timer_factory(timer_factory) {
    m_session_cost.session_id = session_started_event.uuid;
    m_session_cost.status = types::session_cost::SessionStatus::Idle;
    m_session_cost.currency = tariff.currency;

    if (session_started_event.session_started && session_started_event.session_started->id_tag) {
        m_session_cost.id_tag = session_started_event.session_started->id_tag;
    }
}

std::future<PaymentSession::ResultType> PaymentSession::get_future() {
    return m_done_promise.get_future();
}

void PaymentSession::cancel() {
    if (not m_enabled) {
        return;
    }
    m_enabled = false;
    if (m_powermeter_timeout) {
        m_powermeter_timeout.reset();
    }
    m_done_promise.set_value(std::nullopt);
}

void PaymentSession::handle_Evse_SessionEvent(const types::evse_manager::SessionEvent& event) {
    if (not m_enabled) {
        return;
    }
    if (event.uuid != m_session_cost.session_id) {
        // This should not happen and is the responsibility of the calling code
        throw std::logic_error("session_id of SessionEvent does not fit expected id session_id of PaymentSession!");
    }
    switch (event.event) {
    case types::evse_manager::SessionEventEnum::TransactionStarted:
        handle_transaction_start(event);
        break;
    case types::evse_manager::SessionEventEnum::TransactionFinished:
        // FIXME(CB): Throw if there wasn't a transaction running?!
        handle_transaction_finish(event);
        break;
    case types::evse_manager::SessionEventEnum::SessionFinished:
        // FIXME(CB): Throw if there is still a transaction running?!
        finish_payment_session();
        break;
    default:
        break;
    }
}

void PaymentSession::handle_transaction_start(const types::evse_manager::SessionEvent& event) {
    m_transaction_running = true;
    if (m_session_cost.id_tag) {
        // TODO(CB): Not a complete comparison!
        if (m_session_cost.id_tag.value().id_token.value == event.transaction_started->id_tag.id_token.value) {
            // TODO(CB): That's fine
        } else {
            // TODO(CB): Any valid scenario where one expects different authentication IDs?
        }
    } else {
        m_session_cost.id_tag = event.transaction_started->id_tag;
    }

    m_cost_calculator.start_charging(
        SessionAccountant::extract_Powermeter_data(event.transaction_started->meter_value));

    m_powermeter_timeout = m_timer_factory.get_powermeter_timeout_timer();
    m_powermeter_timeout->set_callback(
        [this]() { call_stop_transaction_callback(types::evse_manager::StopTransactionReason::Other); });
    m_powermeter_timeout->start();

    m_session_cost.status = types::session_cost::SessionStatus::Running;
}

void PaymentSession::handle_transaction_finish(const types::evse_manager::SessionEvent& event) {
    m_transaction_running = false;
    if (m_powermeter_timeout) {
        m_powermeter_timeout.reset();
    }

    m_cost_calculator.stop_charging(extract_Powermeter_data(event.transaction_finished->meter_value));
    m_session_cost.status = types::session_cost::SessionStatus::Idle;

    if (m_end_after_transaction_finish) {
        finish_payment_session();
    } else {
        update_session_cost();
    }
}

void PaymentSession::finish_payment_session() {
    if (not m_enabled) {
        return;
    }
    m_enabled = false;

    m_cost_calculator.handle_time_update(date::utc_clock::now());

    update_session_cost();

    m_session_cost.status = types::session_cost::SessionStatus::Finished;

    // TODO(CB): Sanitize cost_record (do not exceed available amount)
    m_done_promise.set_value(std::move(m_session_cost));
}

void PaymentSession::set_stop_transaction_callback(const stop_transaction_callback& callback) {
    m_stop_transaction_callback = callback;
}

void PaymentSession::set_update_cost_callback(const update_cost_callback& callback) {
    m_update_cost_callback = callback;
}

void PaymentSession::handle_pmeter_update(SessionAccountant::PowermeterUpdate powermeter) {
    if (not m_transaction_running or not m_enabled) {
        return;
    }
    if (m_powermeter_timeout) {
        m_powermeter_timeout->reset();
    }

    // CostCalculator will still ignore updates if status is not charging
    m_cost_calculator.handle_powermeter_update(powermeter);

    update_session_cost();
    check_cost_limit();
}

void PaymentSession::handle_tariff_update(Tariff tariff) {
    if (not m_enabled) {
        return;
    }
    m_cost_calculator.handle_tariff_update(tariff);
}

void PaymentSession::finish_now() {
    if (m_transaction_running) {
        m_end_after_transaction_finish = true;
        call_stop_transaction_callback(types::evse_manager::StopTransactionReason::TimeLimitReached);
    } else {
        finish_payment_session();
    }
}

void PaymentSession::update_session_cost() {
    // TODO(CB): Very bad: It's only called here in 'handle_pmeter_update' - what if there are no powermeter updates
    // for example during Idle?!
    m_session_cost.cost_chunks = *(m_cost_calculator.get_cost_chunks());

    m_total_cost = calc_total_cost(m_session_cost.cost_chunks.value());

    // TODO(CB): extrapolate when the pre_authorized_amount of money will be exhausted (Timer)

    call_update_cost_callback();
}

void PaymentSession::check_cost_limit() {
    if (m_reserved_amount_of_money <= m_total_cost) {
        call_stop_transaction_callback(types::evse_manager::StopTransactionReason::LocalOutOfCredit);
        m_end_after_transaction_finish = true;   // TODO(CB): Commented for debugging
    }
}

void PaymentSession::call_stop_transaction_callback(types::evse_manager::StopTransactionReason reason) {
    if (m_stop_transaction_callback && m_is_first_call_to_stop_callback.load()) {
        m_stop_transaction_callback(reason);
        m_is_first_call_to_stop_callback.store(false);
    }
}

void PaymentSession::call_update_cost_callback() {
    if (m_update_cost_callback) {
        m_update_cost_callback(m_session_cost);
    }
}

} // namespace SessionAccountant
