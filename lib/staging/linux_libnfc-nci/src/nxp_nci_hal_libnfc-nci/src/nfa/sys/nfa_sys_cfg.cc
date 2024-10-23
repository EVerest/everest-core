/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains compile-time configurable constants for the NFA
 *  system manager.
 *
 ******************************************************************************/
#include "gki.h"
#include "nfa_sys.h"
#include "nfc_target.h"

const tNFA_SYS_CFG nfa_sys_cfg = {
    NFA_MBOX_EVT_MASK,       /* GKI mailbox event */
    NFA_MBOX_ID,             /* GKI mailbox id */
    NFA_TIMER_ID,            /* GKI timer id */
};

tNFA_SYS_CFG* p_nfa_sys_cfg = (tNFA_SYS_CFG*)&nfa_sys_cfg;
