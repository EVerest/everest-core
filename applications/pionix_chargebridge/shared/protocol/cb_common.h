// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "cb_platform.h"
#include <stdint.h>

// Structs

typedef union _SafetyErrorFlags {
    struct _flags {
        uint32_t cp_not_state_c : 1;
        uint32_t pwm_not_enabled : 1;
        uint32_t pp_invalid : 1;
        uint32_t plug_temperature_too_high : 1;
        uint32_t internal_temperature_too_high : 1;
        uint32_t emergency_input_latched : 1;
        uint32_t relay_health_latched : 1;
        uint32_t vdd_3v3_out_of_range : 1;
        uint32_t vdd_core_out_of_range : 1;
        uint32_t vdd_12V_out_of_range : 1;
        uint32_t vdd_N12V_out_of_range : 1;
        uint32_t vdd_refint_out_of_range : 1;
        uint32_t external_allow_power_on : 1;
        uint32_t config_mem_error : 1;
        uint32_t dc_hv_ov_emergency : 1;
        uint32_t dc_hv_ov_error : 1;
        uint32_t reserved : 17;
    } flags;
    uint32_t raw;
} SafetyErrorFlags;


typedef enum _CpState : uint8_t {
    CpState_A,
    CpState_B,
    CpState_C,
    CpState_D,
    CpState_E,
    CpState_F,
    CpState_DF,
    CpState_INVALID
} CpState;
