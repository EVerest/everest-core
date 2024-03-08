// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef BUM_MWCAR_HPP
#define BUM_MWCAR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ev_board_support/Interface.hpp>
#include <generated/interfaces/ev_board_support_extended/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class BUmMWcar : public Everest::ModuleBase {
public:
    BUmMWcar() = delete;
    BUmMWcar(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
             std::unique_ptr<ev_board_support_extendedIntf> r_board_support_extended,
             std::unique_ptr<ev_board_supportIntf> r_board_support, Conf& config) :
        ModuleBase(info),
        p_main(std::move(p_main)),
        r_board_support_extended(std::move(r_board_support_extended)),
        r_board_support(std::move(r_board_support)),
        config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<ev_board_support_extendedIntf> r_board_support_extended;
    const std::unique_ptr<ev_board_supportIntf> r_board_support;
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
    // insert your private definitions here
    std::mutex data_mutex;

    types::ev_board_support_extended::KeepAliveData last_keep_alive_data;
    types::ev_board_support_extended::ACVoltageData last_ac_instant_data;
    types::ev_board_support_extended::ACPhaseStatistics last_ac_stats_data;
    double last_dc_instant_data;
    types::ev_board_support_extended::EdgeTimingData last_edge_data;
    types::board_support_common::BspEvent last_bsp_event;
    types::board_support_common::BspMeasurement last_bsp_meas;

    bool ac_instant_poll = false;
    std::chrono::_V2::system_clock::time_point last_ac_instant_poll;
    std::string set_cp_voltage_input_str{"0.0"};
    std::string set_edge_timing_num_periods_str{"100"};
    bool edge_meas_force_restart = true;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // BUM_MWCAR_HPP
