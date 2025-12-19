// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSE_BSP_HOST_TO_CB_H
#define EVSE_BSP_HOST_TO_CB_H

#include <stdint.h>
#include "cb_platform.h"

struct CB_COMPILER_ATTR_PACK evse_bsp_host_to_cb {
    uint8_t connector_lock;     /* 0: unlock, otherwise: lock */
    uint32_t pwm_duty_cycle;    /* in 0.01 %, 0 = State F, 10000 = X1 */
    uint8_t allow_power_on;     /* 0 false, true otherwise */
    uint8_t reset;              /* 0 false, true otherwise */
    uint8_t ovm_enable;         /* 0 disabled, 1: enabled */
    uint8_t ovm_reset_errors;   /* 0 leave errors untouched, 1: clear error bits for OVM */
    uint32_t ovm_limit_emergency_mV;  /* 9ms limit in mV */
    uint32_t ovm_limit_error_mV;  /* 400ms limit in mV */
    CpState ev_set_cp_state;  /* Set CP state (EV side only) */
    uint8_t ev_set_diodefault; /* Set/Clear DF state (EV side only) */
};

#include "test/evse_bsp_host_to_cb_test.h"

#endif // EVSE_BSP_H
