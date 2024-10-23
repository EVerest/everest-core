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
#ifndef _PHNXPNCIHAL_API_H_
#define _PHNXPNCIHAL_API_H_

#include <phNfcStatus.h>
#include <phNxpNciHal.h>
#include <phTmlNfc.h>

/*******************************************************************************
 **
 ** Function         phNxpNciHal_get_version
 **
 ** Description      Function to get the HW, FW and SW versions.
 **
 ** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED.
 **
 ******************************************************************************/

NFCSTATUS phNxpNciHal_get_version(uint32_t* hw_ver, uint32_t* fw_ver,
                                  uint32_t* sw_ver);

#endif /* _PHNXPNCIHAL_API_H_ */
