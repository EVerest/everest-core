/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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
 *  This is the interface file for non valtile memory call-in functions.
 *
 ******************************************************************************/
#ifndef NFA_NV_CI_H
#define NFA_NV_CI_H

#include "nfa_nv_co.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         nfa_nv_ci_write
**
** Description      This function sends an event to NFAA indicating the phone
**                  has written the number of bytes specified in the call-out
**                  function, nfa_nv_co_write (), and is ready for more data.
**                  This function is used to control the TX data flow.
**                  Note: The data buffer is released by the stack aioer
**                        calling this function.
**
** Parameters       status - NFA_NV_CO_OK, NFA_NV_CO_NOSPACE, or NFA_NV_CO_FAIL
**                  evt - Used Internally by NFA -> MUST be same value passed
**                       in call-out function.
**
** Returns          void
**
*******************************************************************************/
extern void nfa_nv_ci_write(tNFA_NV_CO_STATUS status);

/*******************************************************************************
**
** Function         nfa_nv_ci_read
**
** Description      This function sends an event to NFA indicating the phone has
**                  read in the requested amount of data specified in the
**                  nfa_nv_co_read () call-out function.  It should only be
**                  called when the requested number of bytes has been read.
**
** Parameters       num_bytes_read - number of bytes read into the buffer
**                      specified in the read callout-function.
**                  status - NFA_NV_CO_OK if full buffer of data,
**                           NFA_NV_CO_EOF if the end of file has been reached,
**                           NFA_NV_CO_FAIL if an error has occurred.
**                  evt - Used Internally by NFA -> MUST be same value passed
**                       in call-out function.
**
** Returns          void
**
*******************************************************************************/
extern void nfa_nv_ci_read(uint16_t num_bytes_read, tNFA_NV_CO_STATUS status,
                           uint8_t block);

#endif /* BTA_FS_CI_H */
