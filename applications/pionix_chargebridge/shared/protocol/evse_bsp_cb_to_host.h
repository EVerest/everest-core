// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSE_BSP_CB_TO_HOST_H
#define EVSE_BSP_CB_TO_HOST_H

#include <stdint.h>
#include "cb_platform.h"
#include "cb_common.h"

struct CB_COMPILER_ATTR_PACK evse_bsp_cb_to_host {
    // add version number as first uint16 ???? 
    // potentially unused, ignore for  now
    uint8_t reset_reason;
    CpState cp_state;
    uint8_t relay_state;
    SafetyErrorFlags error_flags;
    uint8_t pp_state_type1;
    uint8_t pp_state_type2;
    uint8_t lock_state;
    uint32_t hv_mV;
    // still define handling set for
    uint8_t stop_charging;
    uint16_t cp_duty_cycle;
};

/* Enum definitions */
typedef enum _RelayState {
    RelayState_Open = 0,
    RelayState_Closed = 1
} RelaiseState;

typedef enum _ResetReason {
    ResetReason_USER = 0,
    ResetReason_WATCHDOG = 1
} ResetReason;

typedef enum _PpState_Type2 {
    PpState_Type2_STATE_NC = 0,
    PpState_Type2_STATE_13A = 1,
    PpState_Type2_STATE_20A = 2,
    PpState_Type2_STATE_32A = 3,
    PpState_Type2_STATE_70A = 4,
    PpState_Type2_STATE_FAULT = 5
} PpState_Type2;

typedef enum _PpState_Type1 {
	PpState_Type1_STATE_NotConnected,
	PpState_Type1_STATE_Connected_Button_Pressed,
	PpState_Type1_STATE_Connected,
	PpState_Type1_STATE_Invalid
} PpState_Type1;

typedef enum _LockState {
    LockState_UNDEFINED = 0,
    LockState_UNLOCKED = 1,
    LockState_LOCKED = 2
} LockState;

#include "test/evse_bsp_cb_to_host_test.h"

#endif // EVSE_BSP_CB_TO_HOST_H
