/*
 * Copyright (C) 2012-2019 NXP Semiconductors
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

#ifndef _PHNXPNCIHAL_ADAPTATION_H_
#define _PHNXPNCIHAL_ADAPTATION_H_

//#include <hardware/nfc.h>
//#include <android/hardware/nfc/1.2/INfc.h>
//#include <android/hardware/nfc/1.2/types.h>

//using ::android::hardware::nfc::V1_2::NfcConfig;

typedef struct {
  //struct nfc_nci_device nci_device;

  /* Local definitions */
} pn547_dev_t;

/* NXP HAL functions */
int phNxpNciHal_open(nfc_stack_callback_t* p_cback,
                     nfc_stack_data_callback_t* p_data_cback);
int phNxpNciHal_MinOpen();
int phNxpNciHal_write(uint16_t data_len, const uint8_t* p_data);
int phNxpNciHal_write_internal(uint16_t data_len, const uint8_t* p_data);
int phNxpNciHal_core_initialized(uint8_t* p_core_init_rsp_params);
int phNxpNciHal_pre_discover(void);
int phNxpNciHal_close(bool);
int phNxpNciHal_configDiscShutdown(void);
int phNxpNciHal_control_granted(void);
int phNxpNciHal_power_cycle(void);
int phNxpNciHal_ioctl(long arg, void* p_data);
void phNxpNciHal_do_factory_reset(void);
//void phNxpNciHal_getVendorConfig(android::hardware::nfc::V1_1::NfcConfig& config);
//void phNxpNciHal_getVendorConfig_1_2(NfcConfig& config);
#endif /* _PHNXPNCIHAL_ADAPTATION_H_ */
