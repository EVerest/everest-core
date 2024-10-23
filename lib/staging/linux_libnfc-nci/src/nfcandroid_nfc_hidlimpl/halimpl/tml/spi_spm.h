/*
 * Copyright (C) 2012-2014 NXP Semiconductors
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

/**
 * \addtogroup SPI_Power_Management
 *
 * @{ */
#define PH_PALESE_RESETDEVICE (0x00008001)
typedef enum {
  PN67T_POWER_SCHEME = 0x01,
  PN80T_LEGACY_SCHEME,
  PN80T_EXT_PMU_SCHEME,
} phNxpEse_PowerScheme;

typedef enum {
  phPalEse_e_Invalid = 0,                         /*!< Invalid control code */
  phPalEse_e_ResetDevice = PH_PALESE_RESETDEVICE, /*!< Reset the device */
  phPalEse_e_EnableLog,      /*!< Enable the spi driver logs */
  phPalEse_e_EnablePollMode, /*!< Enable the polling for SPI */
  phPalEse_e_GetEseAccess,   /*!< get the bus access in specified timeout */
  phPalEse_e_ChipRst,        /*!< eSE Chip reset using ISO RST pin*/
  phPalEse_e_EnableThroughputMeasurement, /*!< Enable throughput measurement */
  phPalEse_e_SetPowerScheme,              /*!< Set power scheme */
  phPalEse_e_GetSPMStatus,                /*!< Get SPM(power mgt) status */
  phPalEse_e_DisablePwrCntrl,
  phPalEse_e_SetJcopDwnldState, /*!< Set Jcop Download state */
} phPalEse_ControlCode_t;

/*******************************************************************************
**
** Function         phPalEse_spi_ioctl
**
** Description      Exposed ioctl by p61 spi driver
**
** Parameters       pDevHandle     - valid device handle
**                  level          - reset level
**
** Returns           0   - ioctl operation success
**                  -1   - ioctl operation failure
**
*******************************************************************************/
int phPalEse_spi_ioctl(phPalEse_ControlCode_t eControlCode,void *pDevHandle, long level);

