/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#ifndef SESSION_ACCOUNTANT_SIMULATOR_HPP
#define SESSION_ACCOUNTANT_SIMULATOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/payment_total_amount_provider/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "SingleEvseAccountant/CostCalculator.hpp"
#include "SingleEvseAccountant/SessionMonitor.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int start_session_price_mau;
    int kWh_price_millimau;
    int idle_hourly_price_mau;
    int currency;
    int session_timeout_s;
    int powermeter_timeout_s;
    int grace_period_minutes;
};

class SessionAccountantSimulator : public Everest::ModuleBase {
public:
    SessionAccountantSimulator() = delete;
    SessionAccountantSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                               std::unique_ptr<payment_total_amount_providerImplBase> p_total_amount,
                               std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_total_amount(std::move(p_total_amount)),
        r_evse_manager(std::move(r_evse_manager)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<payment_total_amount_providerImplBase> p_total_amount;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    bool add_preauthorization(const types::payment::BankingPreauthorization& preauthorization);
    bool withdraw_preauthorization(const types::authorization::IdToken& id_token);
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    void init_session_monitor(evse_managerIntf* evse_manager);
    std::optional<types::money::MoneyAmount>
    get_preauthorization_amount_for_IdToken(types::authorization::IdToken, SessionAccountant::SessionMonitor*);

    std::vector<std::unique_ptr<SessionAccountant::SessionMonitor>> m_session_monitors{};

    std::map<std::string, std::pair<types::payment::BankingPreauthorization, SessionAccountant::SessionMonitor*>>
        m_unmatched_preauthorizations{};
    std::map<std::string, std::pair<types::payment::BankingPreauthorization, SessionAccountant::SessionMonitor*>>
        m_matched_preauthorizations{};

    std::mutex m_preauthorizations_mtx;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // SESSION_ACCOUNTANT_SIMULATOR_HPP
