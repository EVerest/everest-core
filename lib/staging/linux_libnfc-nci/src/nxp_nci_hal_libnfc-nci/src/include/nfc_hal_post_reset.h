/******************************************************************************
 *
 *  Copyright (C) 2009-2014 Broadcom Corporation
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
 *  Post NCI reset routines
 *
 ******************************************************************************/
#ifndef NFC_HAL_POST_RESET_H
#define NFC_HAL_POST_RESET_H

/*****************************************************************************
** Application control block definitions
******************************************************************************/
#define NFA_APP_PATCHFILE_MAX_PATH 255

typedef struct {
  uint8_t prm_file[NFA_APP_PATCHFILE_MAX_PATH + 1]; /* Filename of patchram */
  uint8_t* p_prm_buf; /* Pointer to buffer for holding patchram data */

  /* Patchfile for I2C fix */
  uint8_t prm_i2c_patchfile[NFA_APP_PATCHFILE_MAX_PATH + 1];
  uint8_t* p_prm_i2c_buf;

  uint8_t userial_baud;

  tNFC_HAL_DEV_INIT_CFG dev_init_config;

  /* snooze mode setting */
  uint8_t snooze_mode;
  uint8_t idle_threshold_dh;
  uint8_t idle_threshold_nfcc;
  uint8_t nfc_wake_active_mode;
  uint8_t dh_wake_active_mode;

  /* NVM detection retry (some platforms require re-attempts to detect NVM) */
  uint8_t spd_nvm_detection_max_count; /* max retry to get NVM type */
  uint8_t spd_nvm_detection_cur_count; /* current retry count       */

  /* handling for failure to download patch */
  bool spd_debug_mode; /* debug mode for downloading patchram, report failure
                          immediately and obviously */
  bool spd_skip_on_power_cycle; /* skip downloading patchram after power cycle
                                   because of patch download failure */
} tNFC_POST_RESET_CB;
extern tNFC_POST_RESET_CB nfc_post_reset_cb;

#endif /* NFC_HAL_POST_RESET_H */
