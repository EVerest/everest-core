// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENERGY_GRID_ENERGY_IMPL_HPP
#define ENERGY_GRID_ENERGY_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/energy/Implementation.hpp>

#include "../EnergyNode.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <everest/util/async/monitor.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace energy_grid {

struct Conf {};

class energyImpl : public energyImplBase {
public:
    energyImpl() = delete;
    energyImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EnergyNode>& mod, Conf& config) :
        energyImplBase(ev, "energy_grid"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_enforce_limits(types::energy::EnforcedLimits& value) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EnergyNode>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // Energy state protected by monitor for thread-safe access
    struct EnergyState {
        // subtree including children
        types::energy::EnergyFlowRequest energy_flow_request;

        // contains only the pricing informations last update
        types::energy_price_information::EnergyPriceSchedule energy_pricing;
    };

    everest::lib::util::monitor<EnergyState> energy_state;

    types::energy::ScheduleReqEntry get_local_schedule_req_entry();
    std::vector<types::energy::ScheduleReqEntry> get_local_schedule();

    void publish_complete_energy_object(const EnergyState& state);
    void set_external_limits(types::energy::ExternalLimits& l);
    void merge_price_into_schedule(std::vector<types::energy::ScheduleReqEntry>& schedule,
                                   const std::vector<types::energy_price_information::PricePerkWh>& price);
    std::string source_cfg;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// Standalone function for schedule processing (used by tests)
void process_schedule_with_limits(std::vector<types::energy::ScheduleReqEntry>& schedule, const std::string& source,
                                  double fuse_limit_A, int phase_count, double nominal_voltage_V,
                                  bool enhance_with_current_limits);
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace energy_grid
} // namespace module

#endif // ENERGY_GRID_ENERGY_IMPL_HPP
