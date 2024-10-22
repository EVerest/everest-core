/******************************************************************************
 *
 *  Copyright 2019 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#pragma once
/*******************************************************************************
**
** Function         NFA_T4tNfcEeOpenConnection
**
** Description      Creates logical connection with T4T Nfcee
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeOpenConnection();

/*******************************************************************************
**
** Function         NFA_T4tNfcEeClear
**
** Description      Clear Ndef data to T4T NFC EE.
**                  For file ID NDEF, perform the NDEF detection procedure
**                  and set the NDEF tag data to zero.
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeClear(uint8_t* p_fileId);

/*******************************************************************************
**
** Function         NFA_T4tNfcEeWrite
**
** Description      Write data to the T4T NFC EE of given file id.
**                  If file ID is of NDEF, perform the NDEF detection procedure
**                  and write the NDEF tag data using the appropriate method for
**                  NDEF EE.
**                  If File ID is Not NDEF then reads proprietary way
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeWrite(uint8_t* p_fileId, uint8_t* p_data, uint32_t len);

/*******************************************************************************
**
** Function         NFA_T4tNfcEeRead
**
** Description      Read T4T message from NFCC area.of given file id
**                  If file ID is of NDEF, perform the NDEF detection
*procedure
**                  and read the NDEF tag data using the appropriate method
**                  for NDEF EE.
**                  If File ID is Not NDEF then reads proprietary way
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeRead(uint8_t* p_fileId);

/*******************************************************************************
**
** Function         NFA_T4tNfcEeCloseConnection
**
** Description      Closes logical connection with T4T Nfcee
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeCloseConnection();
