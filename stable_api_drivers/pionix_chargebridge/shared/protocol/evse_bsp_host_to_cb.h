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
    uint32_t ovm_limit_9ms_mV;  /* 9ms limit in mV */
    uint32_t ovm_limit_400ms_mV;  /* 400ms limit in mV */
};

#include "test/evse_bsp_host_to_cb_test.h"

#endif // EVSE_BSP_H
