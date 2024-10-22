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
#ifndef _PHNXPNCIHAL_KOVIO_H_
#define _PHNXPNCIHAL_KOVIO_H_

#include <phNxpNciHal.h>
#include <phTmlNfc.h>
#include <string.h>

extern NFCSTATUS  phNxpNciHal_kovio_rsp_ext(uint8_t *p_ntf, uint16_t *p_len);
extern void phNxpNciHal_clean_Kovio_Ext();

#endif /* _PHNXPNCIHAL_KOVIO_H_ */
