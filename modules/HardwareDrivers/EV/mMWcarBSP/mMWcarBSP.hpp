/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#ifndef M_MWCAR_BSP_HPP
#define M_MWCAR_BSP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ev_board_support/Implementation.hpp>
#include <generated/interfaces/ev_board_support_extended/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "mMWcar_mcu_comms/evSerial.h"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool invert_status_led_polarity;
    std::string serial_port;
    int baud_rate;
    std::string reset_gpio_chip;
    int reset_gpio_line;
};

class mMWcarBSP : public Everest::ModuleBase {
public:
    mMWcarBSP() = delete;
    mMWcarBSP(const ModuleInfo& info, std::unique_ptr<ev_board_support_extendedImplBase> p_board_support_extended,
              std::unique_ptr<ev_board_supportImplBase> p_simple_board_support, Conf& config) :
        ModuleBase(info),
        p_board_support_extended(std::move(p_board_support_extended)),
        p_simple_board_support(std::move(p_simple_board_support)),
        config(config){};

    const std::unique_ptr<ev_board_support_extendedImplBase> p_board_support_extended;
    const std::unique_ptr<ev_board_supportImplBase> p_simple_board_support;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    evSerial serial;
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
    types::board_support_common::BspEvent last_bsp_event;
    types::board_support_common::BspMeasurement last_bspm;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // M_MWCAR_BSP_HPP
