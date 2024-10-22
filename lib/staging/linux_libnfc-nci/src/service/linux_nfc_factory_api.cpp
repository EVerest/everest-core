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

#ifdef NXP_HW_SELF_TEST

#include "linux_nfc_factory_api.h"
#include "phNxpNciHal_SelfTest.h"
#include "nfa_api.h"

/*******************************************************************************
 **
 ** Function         nfcFactory_testMode_open
 **
 ** Description      It opens the physical connection with NFCC and
 **                  creates required client thread for operation.
 **
 ** Returns          0 if successful,otherwise failed.
 **
 *******************************************************************************/

int nfcFactory_testMode_open (void)
{
    return phNxpNciHal_TestMode_open();
}

/*******************************************************************************
 **
 ** Function         nfcFactory_testMode_close
 **
 ** Description      This function close the NFCC interface and free all
 **                  resources.
 **
 ** Returns          None.
 **
 *******************************************************************************/

void nfcFactory_testMode_close (void)
{
    return phNxpNciHal_TestMode_close();
}


/*******************************************************************************
 **
 ** Function         nfcFactory_PrbsTestStart
 **
 ** Description      Test function start RF generation for RF technology and bit
 **                  rate. RF technology and bit rate are sent as parameter to
 **                  the API.
 **
 ** Returns          0 if RF generation successful,
 **                  otherwise failed.
 **
 *******************************************************************************/

int nfcFactory_PrbsTestStart (nfcFactory_PRBSTech_t tech, nfcFactory_PRBSBitrate_t bitrate)
{
    int tech1 = (int) tech;
    int bitrate1 = (int) bitrate;
    return phNxpNciHal_PrbsTestStart(NFC_HW_PRBS, NFC_HW_PRBS15, (phNxpNfc_Tech_t) tech1, (phNxpNfc_Bitrate_t)bitrate1);
}


/*******************************************************************************
 **
 ** Function         nfcFactory_PrbsTestStop
 **
 ** Description      Test function stop RF generation for RF technology started
 **                  by phNxpNciHal_PrbsTestStart.
 **
 ** Returns          0 if operation successful,
 **                  otherwise failed.
 **
 *******************************************************************************/

int nfcFactory_PrbsTestStop ()
{
    return phNxpNciHal_PrbsTestStop();
}

/*******************************************************************************
**
** Function         nfcFactory_AntennaSelfTest
**
** Description      Test function to validate the Antenna's discrete
**                  components connection.
**
** Returns          0 if successful,otherwise failed.
**
*******************************************************************************/

int nfcFactory_AntennaSelfTest(nfcFactory_Antenna_St_Resp_t * phAntenna_St_Resp )
{
    return phNxpNciHal_AntennaSelfTest(
            (phAntenna_St_Resp_t *) phAntenna_St_Resp);
}


/*******************************************************************************
**
** Function         nfcFactory_GetMwVersion
**
** Description      Test function to return Mw version.
**
** Returns          Mw version of stack.
**
*******************************************************************************/

int nfcFactory_GetMwVersion ()
{
    tNFA_MW_VERSION mwVer = {0};
    //mwVer = NFA_GetMwVersion();
    return ((mwVer.android_version & 0xFF ) << 16) | ((mwVer.major_version & 0xFF ) << 8) | (mwVer.minor_version & 0xFF);
}

#endif
