/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#ifndef PAYMENT_TERMINAL_SIMULATOR_HPP
#define PAYMENT_TERMINAL_SIMULATOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/auth_token_provider/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/session_cost/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "PaymentTerminalDriver/PTDriver.hpp"
#include "include/PTSimulationIfc.hpp"
#include "VirtualPaymentTerminal/VirtualPT.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int currency;
    int pre_authorization_amount;
    int token_debounce_interval_ms;
    bool simulated_PT_has_open_preauthorizations;
    int max_open_transactions;
};

class PaymentTerminalSimulator : public Everest::ModuleBase {
public:
    PaymentTerminalSimulator() = delete;
    PaymentTerminalSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                             std::unique_ptr<auth_token_providerImplBase> p_token_provider,
                             std::unique_ptr<session_costIntf> r_cost_provider, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_token_provider(std::move(p_token_provider)),
        r_cost_provider(std::move(r_cost_provider)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<auth_token_providerImplBase> p_token_provider;
    const std::unique_ptr<session_costIntf> r_cost_provider;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
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
    void receive_session_cost(types::session_cost::SessionCost session_cost);
    void read_cards_forever();
    void publish_costs_to_nodered(const types::session_cost::SessionCost& session_cost, int32_t total_cost);
    void print_costs_to_log(const types::session_cost::SessionCost& session_cost, int32_t total_cost);
    bool finalize_payment(const types::session_cost::SessionCost& session_cost, int32_t total_cost);
    void print_and_publish_to_nodered_finalization_result(const types::session_cost::SessionCost& session_cost, int32_t total_cost, bool success);

    const std::string bankcard_mqtttopic{"everest_external/nodered/payment_terminal/cmd/present_banking_card"};
    const std::string rfidcard_mqtttopic{"everest_external/nodered/payment_terminal/cmd/present_rfid_card"};
    const std::string preauthorizations_mqtttopic{"everest_external/nodered/payment_terminal/stat/open_preauthorizations"};
    const std::string partial_reversal_mqtttopic{"everest_external/nodered/payment_terminal/stat/partial_reversal"};
    const std::string running_costs_mqtttopic{"everest_external/nodered/payment_terminal/stat/running_costs"};
    std::unique_ptr<pterminal::PTDriver> m_PT_drv{};
    std::shared_ptr<pterminal::VirtualPT> m_VPT{};
    std::shared_ptr<pterminal::PTSimulationIfc> m_simIfc{};
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // PAYMENT_TERMINAL_SIMULATOR_HPP
