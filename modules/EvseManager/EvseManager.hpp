// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_MANAGER_HPP
#define EVSE_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/energy/Implementation.hpp>
#include <generated/evse_manager/Implementation.hpp>

// headers for required interface implementations
#include <generated/ISO15118_ac_charger/Interface.hpp>
#include <generated/auth/Interface.hpp>
#include <generated/board_support_AC/Interface.hpp>
#include <generated/powermeter/Interface.hpp>
#include <generated/slac/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "Charger.hpp"
#include <chrono>
#include <ctime>
#include <date/date.h>
#include <date/tz.h>
#include <iostream>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string evse_id;
    bool three_phases;
    bool has_ventilation;
    std::string country_code;
    bool rcd_enabled;
    double max_current;
    std::string charge_mode;
    bool ac_hlc_enabled;
    bool ac_hlc_use_5percent;
    bool ac_enforce_hlc;
};

class EvseManager : public Everest::ModuleBase {
public:
    EvseManager() = delete;
    EvseManager(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                std::unique_ptr<evse_managerImplBase> p_evse, std::unique_ptr<energyImplBase> p_energy_grid,
                std::unique_ptr<board_support_ACIntf> r_bsp, std::unique_ptr<powermeterIntf> r_powermeter,
                std::unique_ptr<authIntf> r_auth, std::vector<std::unique_ptr<slacIntf>> r_slac,
                std::vector<std::unique_ptr<ISO15118_ac_chargerIntf>> r_hlc, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_evse(std::move(p_evse)),
        p_energy_grid(std::move(p_energy_grid)),
        r_bsp(std::move(r_bsp)),
        r_powermeter(std::move(r_powermeter)),
        r_auth(std::move(r_auth)),
        r_slac(std::move(r_slac)),
        r_hlc(std::move(r_hlc)),
        config(config){};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<evse_managerImplBase> p_evse;
    const std::unique_ptr<energyImplBase> p_energy_grid;
    const std::unique_ptr<board_support_ACIntf> r_bsp;
    const std::unique_ptr<powermeterIntf> r_powermeter;
    const std::unique_ptr<authIntf> r_auth;
    const std::vector<std::unique_ptr<slacIntf>> r_slac;
    const std::vector<std::unique_ptr<ISO15118_ac_chargerIntf>> r_hlc;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<Charger> charger;
    sigslot::signal<int> signalNrOfPhasesAvailable;
    json get_latest_powermeter_data();
    json get_hw_capabilities();
    bool updateLocalMaxCurrentLimit(float max_current);
    float getLocalMaxCurrentLimit();
    std::string reserve_now(const int _reservation_id, const std::string& token,
                            const std::chrono::time_point<date::utc_clock>& valid_until, const std::string& parent_id);
    bool cancel_reservation();
    bool reservation_valid();
    int32_t get_reservation_id();
    sigslot::signal<json> signalReservationEvent;
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
    json latest_powermeter_data;
    bool authorization_available;
    Everest::Thread energyThreadHandle;
    json hw_capabilities;
    bool local_three_phases;
    float local_max_current_limit;
    const float EVSE_ABSOLUTE_MAX_CURRENT = 80.0;
    bool slac_enabled;
    bool hlc_enabled;

    // Reservations
    std::string reserved_auth_token;
    std::string reserved_auth_token_parent_id;
    std::chrono::time_point<date::utc_clock> reservation_valid_until;
    bool reserved; // internal, use reservation_valid() if you want to find out if it is reserved
    int reservation_id;
    void use_reservation_to_start_charging();
    bool reserved_for_different_token(const std::string& token);
    Everest::Thread reservationThreadHandle;
    std::mutex reservation_mutex;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVSE_MANAGER_HPP
