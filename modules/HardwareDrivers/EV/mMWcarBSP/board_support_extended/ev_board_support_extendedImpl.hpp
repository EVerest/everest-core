// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef BOARD_SUPPORT_EV_BOARD_SUPPORT_EXTENDED_IMPL_HPP
#define BOARD_SUPPORT_EV_BOARD_SUPPORT_EXTENDED_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ev_board_support_extended/Implementation.hpp>

#include "../mMWcarBSP.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "board_support_common.hpp"
#include "mMWcar.pb.h"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace board_support_extended {

struct Conf {};

class ev_board_support_extendedImpl : public ev_board_support_extendedImplBase {
public:
    ev_board_support_extendedImpl() = delete;
    ev_board_support_extendedImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<mMWcarBSP>& mod,
                                  Conf& config) :
        ev_board_support_extendedImplBase(ev, "board_support_extended"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual bool handle_set_status_led(bool& state) override;
    virtual void handle_reset(bool& soft_reset) override;
    virtual void handle_get_ac_data_instant() override;
    virtual void handle_get_ac_statistics() override;
    virtual void handle_get_dc_data_instant() override;
    virtual void handle_set_cp_voltage(double& voltage) override;
    virtual void handle_set_cp_load_en(bool& state) override;
    virtual void handle_set_cp_short_to_gnd_en(bool& state) override;
    virtual void handle_set_cp_diode_fault_en(bool& state) override;
    virtual void handle_trigger_edge_timing_measurement(int& num_periods, bool& force_start) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<mMWcarBSP>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace board_support_extended
} // namespace module

#endif // BOARD_SUPPORT_EV_BOARD_SUPPORT_EXTENDED_IMPL_HPP
