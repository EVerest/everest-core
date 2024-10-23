/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 *
 *  This file contains compile-time configurable constants for vendor specific
 *proprietary protocols
 *
 ******************************************************************************/
#ifndef __NFC_VENDOR_CFG_H__
#define __NFC_VENDOR_CFG_H__

#include <stdint.h>

/* compile-time configuration structure for proprietary protocol and discovery
 * value */
typedef struct {
  uint8_t pro_protocol_18092_active;
  uint8_t pro_protocol_b_prime;
  uint8_t pro_protocol_dual;
  uint8_t pro_protocol_15693;
  uint8_t pro_protocol_kovio;
  uint8_t pro_protocol_mfc;

  uint8_t pro_discovery_kovio_poll;
  uint8_t pro_discovery_b_prime_poll;
  uint8_t pro_discovery_b_prime_listen;
} tNFA_PROPRIETARY_CFG;

extern tNFA_PROPRIETARY_CFG* p_nfa_proprietary_cfg;

/**********************************************
* Proprietary Protocols
**********************************************/


#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)

#ifndef NCI_PROTOCOL_ISO7816
#define NCI_PROTOCOL_ISO7816             0xA0
#endif

#ifndef NCI_PROTOCOL_MIFARE
#define NCI_PROTOCOL_MIFARE             0x80
#endif

#ifndef NCI_PROTOCOL_18092_ACTIVE
#define NCI_PROTOCOL_18092_ACTIVE       0x05
#endif

#else

#ifndef NCI_PROTOCOL_MIFARE
#define NCI_PROTOCOL_MIFARE             0xFF
#endif

#ifndef NCI_PROTOCOL_18092_ACTIVE
#define NCI_PROTOCOL_18092_ACTIVE       0x80
#endif

#endif /* (NFC_NXP_NOT_OPEN_INCLUDED == TRUE) */

#ifndef NCI_PROTOCOL_B_PRIME
#define NCI_PROTOCOL_B_PRIME            0x81
#endif

#ifndef NCI_PROTOCOL_DUAL
#define NCI_PROTOCOL_DUAL               0x82
#endif

#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#ifndef NCI_PROTOCOL_15693
#define NCI_PROTOCOL_15693              0x06
#endif
#else
#ifndef NCI_PROTOCOL_15693
#define NCI_PROTOCOL_15693              0x83
#endif
#endif

/*
 * Below configuration as meant for future use
 * The above definitions can be removed if the
 * below method of configuration to be used
 * */
#ifndef NCI_PROTOCOL_18092_ACTIVE
#define NCI_PROTOCOL_18092_ACTIVE \
  (p_nfa_proprietary_cfg->pro_protocol_18092_active)
#endif
#ifndef NCI_PROTOCOL_B_PRIME
#define NCI_PROTOCOL_B_PRIME (p_nfa_proprietary_cfg->pro_protocol_b_prime)
#endif
#ifndef NCI_PROTOCOL_DUAL
#define NCI_PROTOCOL_DUAL (p_nfa_proprietary_cfg->pro_protocol_dual)
#endif
#ifndef NCI_PROTOCOL_15693
#define NCI_PROTOCOL_15693 (p_nfa_proprietary_cfg->pro_protocol_15693)
#endif
#ifndef NCI_PROTOCOL_KOVIO
#define NCI_PROTOCOL_KOVIO (p_nfa_proprietary_cfg->pro_protocol_kovio)
#endif
#ifndef NCI_PROTOCOL_MIFARE
#define NCI_PROTOCOL_MIFARE (p_nfa_proprietary_cfg->pro_protocol_mfc)
#endif

/**********************************************
* Proprietary Discovery technology and mode
**********************************************/
#ifndef NCI_DISCOVERY_TYPE_POLL_KOVIO
#define NCI_DISCOVERY_TYPE_POLL_KOVIO \
  (p_nfa_proprietary_cfg->pro_discovery_kovio_poll)
#endif

//#ifndef NCI_DISCOVERY_TYPE_POLL_B_PRIME
//#define NCI_DISCOVERY_TYPE_POLL_B_PRIME \
//  (p_nfa_proprietary_cfg->pro_discovery_b_prime_poll)
//#endif

#ifndef NCI_DISCOVERY_TYPE_LISTEN_B_PRIME
#define NCI_DISCOVERY_TYPE_LISTEN_B_PRIME \
  (p_nfa_proprietary_cfg->pro_discovery_b_prime_listen)
#endif

/**********************************************
 * Proprietary Protocols
 **********************************************/
#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#ifndef NCI_PROTOCOL_T3BT
#define NCI_PROTOCOL_T3BT               0x8b
#endif
#endif /* (NFC_NXP_NOT_OPEN_INCLUDED == TRUE) */

#endif /* __NFC_VENDOR_CFG_H__ */
