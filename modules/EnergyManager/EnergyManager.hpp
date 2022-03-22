// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENERGY_MANAGER_HPP
#define ENERGY_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/energy_manager/Implementation.hpp>

// headers for required interface implementations
#include <generated/energy/Interface.hpp>

#include <chrono>
#include <date/date.h>
#include <date/tz.h>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <mutex>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class EnergyManager : public Everest::ModuleBase {
public:
    EnergyManager() = delete;
    EnergyManager(const ModuleInfo& info, std::unique_ptr<energy_managerImplBase> p_main,
                  std::unique_ptr<energyIntf> r_energy_trunk, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_energy_trunk(std::move(r_energy_trunk)), config(config){};

    const Conf& config;
    const std::unique_ptr<energy_managerImplBase> p_main;
    const std::unique_ptr<energyIntf> r_energy_trunk;

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
    // insert your private definitions here

    std::mutex global_energy_object_mutex;
    json global_energy_object;
    std::chrono::time_point<date::utc_clock> lastLimitUpdate;
    json si_0_rp_aca(json& child) {
        auto si_0_rp = child.at("schedule_import").at(0).at("request_parameters");
        return si_0_rp.at("ac_current_A");
    }
    static void interval_start(const std::function<void(void)>& func, unsigned int interval_ms);
    void run_enforce_limits();
    Array run_optimizer(json energy);
    void optimize_one_level(json& energy, json& optimized_values, const std::chrono::time_point<date::utc_clock> timepoint, json price_schedule);
    json get_limit_from_schedule(json s, const std::chrono::time_point<date::utc_clock> timepoint);
    void sanitize_object(json& obj_to_sanitize);

    static json get_sub_element_from_schedule_at_time(json s, const std::chrono::time_point<date::utc_clock> timepoint);
    static double get_current_limit_from_energy_object(const json& limit_object, const json& energy_object);
    static double get_currently_valid_price_per_kwh(json& energy_object,
                                                    const std::chrono::time_point<date::utc_clock> timepoint_now);
    static void check_for_children_requesting_power(json& energy_object, const double current_price_per_kwh);
    void scale_and_distribute_power(json& energy_object);
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
#define ENERGY_MANAGER_ABSOLUTE_MAX_CURRENT  double(80.0F)
#define ENERGY_MANAGER_OPTIMIZER_INTERVAL_MS int(1000)
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // ENERGY_MANAGER_HPP
