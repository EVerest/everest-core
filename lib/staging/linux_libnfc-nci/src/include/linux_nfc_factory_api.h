/******************************************************************************
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
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

#ifndef _LINUX_NFC_FACTORY_H_
#define _LINUX_NFC_FACTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NXP_HW_SELF_TEST

/* RF Technology */
typedef enum
{
    NFC_FACTORY_RF_TECHNOLOGY_A = 0,
    NFC_FACTORY_RF_TECHNOLOGY_B,
    NFC_FACTORY_RF_TECHNOLOGY_F,
} nfcFactory_PRBSTech_t;

/* Bit rates */
typedef enum
{
    NFC_FACTORY_BIT_RATE_106 = 0,
    NFC_FACTORY_BIT_RATE_212,
    NFC_FACTORY_BIT_RATE_424,
    NFC_FACTORY_BIT_RATE_848,
} nfcFactory_PRBSBitrate_t;
/**
 * \brief  Instance of Transaction structure
 */
typedef struct
{
     /** Txdo Raw Value*/
    unsigned short            wTxdoRawValue;
    /**Txdo Measured Range Max */
    unsigned short            wTxdoMeasuredRangeMin;
    /**Txdo Measured Range Min */
    unsigned short            wTxdoMeasuredRangeMax;
    /**Txdo Measured Range Tolerance */
    unsigned short            wTxdoMeasuredTolerance;
     /* Agc Values */
    /**Agc Min Value*/
    unsigned short            wAgcValue;
    /**Txdo Measured Range*/
    unsigned short            wAgcValueTolerance;
     /* Agc value with NFCLD */
    /**Agc Value with Fixed NFCLD Max */
    unsigned short            wAgcValuewithfixedNFCLD;
    /**Agc Value with Fixed NFCLD Tolerance */
    unsigned short            wAgcValuewithfixedNFCLDTolerance;
     /* Agc Differential Values With Open/Short RM */
    /**Agc Differential With Open 1*/
    unsigned short            wAgcDifferentialWithOpen1;
    /**Agc Differential With Open Tolerance 1*/
    unsigned short            wAgcDifferentialWithOpenTolerance1;
    /**Agc Differential With Open 2*/
    unsigned short            wAgcDifferentialWithOpen2;
    /**Agc Differential With Open Tolerance 2*/
    unsigned short            wAgcDifferentialWithOpenTolerance2;
}nfcFactory_Antenna_St_Resp_t;


/**
* \brief It opens the physical connection with NFCC and
*                  creates required client thread for operation.
* \return 0 if successful,otherwise failed.
*/

int nfcFactory_testMode_open (void);

/**
* \brief This function close the NFCC interface and free all
*                  resources.
* \return None.
*/

void nfcFactory_testMode_close (void);


/**
* \brief Test function start RF generation for RF technology and bit
*                  rate. RF technology and bit rate are sent as parameter to
*                  the API.
* \return 0 if RF generation successful,
*                  otherwise failed.
*/

int nfcFactory_PrbsTestStart (nfcFactory_PRBSTech_t tech, nfcFactory_PRBSBitrate_t bitrate);

/**
* \brief Test function stop RF generation for RF technology started
*                  by phNxpNciHal_PrbsTestStart.
* \return 0 if operation successful,
*                  otherwise failed.
*/

int nfcFactory_PrbsTestStop ();

/**
* \brief Test function to validate the Antenna's discrete
*                  components connection.
* \return 0 if successful,otherwise failed.
*/

int nfcFactory_AntennaSelfTest(nfcFactory_Antenna_St_Resp_t * phAntenna_St_Resp );

/**
* \brief Test function to return Mw version.
* \return Mw version on chip.
*/

int nfcFactory_GetMwVersion ();

#endif
#ifdef __cplusplus
}
#endif

#endif /* _LINUX_NFC_FACTORY_H_ */

