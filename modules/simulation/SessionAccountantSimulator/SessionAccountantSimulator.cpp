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

#include "include/Currencies.hpp"
#include "SingleEvseAccountant/TimerFactory.hpp"

namespace {
bool is_valid_session_event(const types::evse_manager::SessionEvent& session_event) {
    switch (session_event.event) {
    case types::evse_manager::SessionEventEnum::SessionStarted:
        return session_event.session_started.has_value();
        break;
    case types::evse_manager::SessionEventEnum::SessionFinished:
        return session_event.session_finished.has_value();
        break;
    case types::evse_manager::SessionEventEnum::TransactionStarted:
        return session_event.transaction_started.has_value();
        break;
    case types::evse_manager::SessionEventEnum::TransactionFinished:
        return session_event.transaction_finished.has_value();
        break;
    }
    return true;
}
} // namespace

namespace module {

void SessionAccountantSimulator::init() {
    invoke_init(*p_session_cost);

    // Make sure the container will never reallocate and references to element stay valid!
    // The lambdas in init_single_accountant() capture reference the container elements!
    // TODO(CB): maybe better use shared_ptr to solve permanent referencability
    m_session_monitors.reserve(r_evse_manager.size());
}

void SessionAccountantSimulator::ready() {
    invoke_ready(*p_session_cost);

    for (const auto& evse_manager : this->r_evse_manager) {
        EVLOG_info << "init session monitor ...";
        init_session_monitor(evse_manager.get());
    }
}

// FIXME(CB): Function too long!
void SessionAccountantSimulator::init_session_monitor(evse_managerIntf* evse_manager) {
    // FIXME(CB): capture evse_id in the lambdas: use for logging
    const int32_t evse_id = evse_manager->call_get_evse().id;

    SessionAccountant::Tariff tariff{.energy_price_kWh_millimau = config.kWh_price_millimau,
                                     .idle_grace_minutes = config.grace_period_minutes,
                                     .idle_hourly_price_mau = config.idle_hourly_price_mau,
                                     .reserved_amount_of_money_mau = config.pre_authorization_amount_mau,
                                     .currency = currencies::currency_for_numeric_code(config.currency)};

    SessionAccountant::TimerFactory timer_factory;

    // FIXME(CB): can the payment terminal revoke the authorization and stop EvseManager transactions?
    timer_factory.get_session_timer = [session_timeout_s = config.session_timeout_s]() {
        return std::make_shared<Timeout::SimpleTimeout>(std::chrono::seconds(session_timeout_s));
    };
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

    evse_manager->subscribe_session_event([&session_monitor, mtx](types::evse_manager::SessionEvent session_event) {
        if (!is_valid_session_event(session_event)) {
            EVLOG_error << "Received session event without the corresponding data member: " << session_event.event;
            // TODO(CB): What do we do now?! Cancel the session?
            return;
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
        this->p_session_cost->publish_session_cost(session_cost);
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

} // namespace module
