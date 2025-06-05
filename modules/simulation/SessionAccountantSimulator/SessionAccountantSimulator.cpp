/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#include "SessionAccountantSimulator.hpp"

#include <map>
#include <utility>

#include "generated/types/evse_manager.hpp"
#include "generated/types/session_cost.hpp"

#include "SingleEvseAccountant/TimerFactory.hpp"
#include "include/Currencies.hpp"

namespace {
bool is_valid_session_event(const types::evse_manager::SessionEvent& session_event) {
    switch (session_event.event) {
    case types::evse_manager::SessionEventEnum::SessionStarted:
        return session_event.session_started.has_value();
    case types::evse_manager::SessionEventEnum::SessionFinished:
        return session_event.session_finished.has_value();
    case types::evse_manager::SessionEventEnum::TransactionStarted:
        return session_event.transaction_started.has_value();
    case types::evse_manager::SessionEventEnum::TransactionFinished:
        return session_event.transaction_finished.has_value();
    default:
        return true;
    }
}

uint32_t calc_total_cost(const std::vector<types::session_cost::SessionCostChunk>& cost_chunks) {
    auto sum_optionals = [](uint32_t lhs, std::optional<types::session_cost::SessionCostChunk> rhs) {
        return lhs + (rhs ? (rhs->cost ? rhs->cost->value : 0) : 0);
    };

    uint32_t total_cost{0};
    total_cost = std::accumulate(cost_chunks.cbegin(), cost_chunks.cend(), 0, sum_optionals);
    return total_cost;
}
} // namespace

namespace module {

void SessionAccountantSimulator::init() {
    invoke_init(*p_total_amount);

    // Make sure the container will never reallocate and references to element stay valid!
    // The lambdas in init_single_accountant() capture reference the container elements!
    // TODO(CB): maybe better use shared_ptr to solve permanent referenceability
    m_session_monitors.reserve(r_evse_manager.size());
}

void SessionAccountantSimulator::ready() {
    invoke_ready(*p_total_amount);

    for (const auto& evse_manager : this->r_evse_manager) {
        EVLOG_info << "init session monitor ...";
        init_session_monitor(evse_manager.get());
    }
}

bool SessionAccountantSimulator::add_preauthorization(const types::payment::BankingPreauthorization& preauthorization) {
    std::scoped_lock lock(m_preauthorizations_mtx);

    if (m_unmatched_preauthorizations.count(preauthorization.id_token.value) == 0) {
        m_unmatched_preauthorizations[preauthorization.id_token.value] = {preauthorization, nullptr};
        EVLOG_info << "Got announcement for new pre-authorization: id '" << preauthorization.id_token.value << "'";
        return true;
    }
    return false;
}

bool SessionAccountantSimulator::withdraw_preauthorization(const types::authorization::IdToken& id_token) {
    std::scoped_lock lock(m_preauthorizations_mtx);

    if (m_unmatched_preauthorizations.erase(id_token.value) > 0) {
        return true;
    }
    if (auto element = m_matched_preauthorizations.extract(id_token.value); not element.empty()) {
        SessionAccountant::SessionMonitor* session_monitor = element.mapped().second;

        session_monitor->stop_session(types::evse_manager::StopTransactionReason::TimeLimitReached);

        return true;
    }
    return false;
}

// FIXME(CB): Function too long!
void SessionAccountantSimulator::init_session_monitor(evse_managerIntf* evse_manager) {
    // FIXME(CB): capture evse_id in the lambdas: use for logging
    const int32_t evse_id = evse_manager->call_get_evse().id;

    SessionAccountant::Tariff tariff{.energy_price_kWh_millimau = config.kWh_price_millimau,
                                     .idle_grace_minutes = config.grace_period_minutes,
                                     .idle_hourly_price_mau = config.idle_hourly_price_mau,
                                     .currency = currencies::currency_for_numeric_code(config.currency)};

    SessionAccountant::TimerFactory timer_factory;

    // FIXME(CB): can the payment terminal revoke the authorization and stop EvseManager transactions?
    timer_factory.get_powermeter_timeout_timer = [powermeter_timeout_s = config.powermeter_timeout_s]() {
        return std::make_shared<Timeout::SimpleTimeout>(std::chrono::seconds(powermeter_timeout_s));
    };
    timer_factory.get_generic_timer = []() {
        return std::make_shared<Timeout::SimpleTimeout>(std::chrono::seconds(365 * 86400));
    };

    auto& session_monitor =
        m_session_monitors.emplace_back(std::make_unique<SessionAccountant::SessionMonitor>(timer_factory));

    session_monitor->set_tariff(tariff);

    std::shared_ptr<std::mutex> mtx = std::make_shared<std::mutex>();

    evse_manager->subscribe_session_event(
        [this, &session_monitor, mtx](types::evse_manager::SessionEvent session_event) {
            if (!is_valid_session_event(session_event)) {
                EVLOG_error << "Received session event without the corresponding data member: " << session_event.event;
                // TODO(CB): What do we do now?! Cancel the session?
                return;
            }
            if (session_event.event == types::evse_manager::SessionEventEnum::TransactionStarted) {
                auto preauthorized_amount = get_preauthorization_amount_for_IdToken(
                    session_event.transaction_started->id_tag.id_token, session_monitor.get());
                if (preauthorized_amount.has_value()) {
                    session_monitor->set_cost_limit(preauthorized_amount->value);
                }
                // if there is no amount or amount is 0 there would be no need to start a transaction
                // but we cannot stop the EvseManager from continuing ...
            }
            EVLOG_info << "Received session event: " << session_event.event;
            std::scoped_lock<std::mutex> lock(*mtx);
            session_monitor->handle_Evse_SessionEvent(session_event);
        });

    evse_manager->subscribe_powermeter([&session_monitor, mtx](types::powermeter::Powermeter powermeter) {
        std::scoped_lock<std::mutex> lock(*mtx);
        session_monitor->handle_powermeter(SessionAccountant::extract_Powermeter_data(powermeter));
    });

    session_monitor->set_cost_update_callback([this](types::session_cost::SessionCost session_cost) {
        // TODO(CB): implement rate limiting here?
        int32_t total_cost = session_cost.cost_chunks ? calc_total_cost(session_cost.cost_chunks.value()) : 0;
        if (total_cost < 0) {
            EVLOG_info << "total_cost = " << total_cost << " ;  " << session_cost;
        }
        if (not session_cost.id_tag) {
            // Dont't throw an exception: if the car is connected and disconnected later without any
            // authorization happening in between, the id_tag cannot be there
            return;
        }
        types::payment::TotalCostAmount total_cost_amount{
            .cost = types::money::MoneyAmount{total_cost},
            .id_token = session_cost.id_tag->id_token,
            .final = (session_cost.status == types::session_cost::SessionStatus::Finished)};
        this->p_total_amount->publish_total_cost_amount(total_cost_amount);
    });

    session_monitor->set_stop_transaction_callback([evse_manager](types::evse_manager::StopTransactionReason reason) {
        bool success =
            evse_manager->call_stop_transaction(types::evse_manager::StopTransactionRequest{.reason = reason});
        if (success) {
            EVLOG_info << "Called EvseManager->call_stop_transaction(" << reason << ") returned true";
        } else {
            EVLOG_info << "Called EvseManager->call_stop_transaction(" << reason << ") returned false";
        }
    });
}

std::optional<types::money::MoneyAmount> SessionAccountantSimulator::get_preauthorization_amount_for_IdToken(
    types::authorization::IdToken id_token, SessionAccountant::SessionMonitor* session_monitor) {
    std::scoped_lock lock(m_preauthorizations_mtx);

    auto matched_element = m_unmatched_preauthorizations.extract(id_token.value);
    if (matched_element.empty()) {
        EVLOG_info << "Preauthorized amount requested for ID: '" << id_token.value << "' - no record found!";
        return std::nullopt;
    }

    auto amount = matched_element.mapped().first.amount;
    matched_element.mapped().second = session_monitor;

    m_matched_preauthorizations.insert(std::move(matched_element));

    EVLOG_info << "Preauthorized amount requested for ID: '" << id_token.value << "' => " << amount.value << " minmal accountable units reserved";
    return amount;
}

} // namespace module
