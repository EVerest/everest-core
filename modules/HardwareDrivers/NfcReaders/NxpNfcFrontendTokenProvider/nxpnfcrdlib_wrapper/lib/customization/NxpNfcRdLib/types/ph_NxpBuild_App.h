// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/* Definitions for the build of the NXP NFC Reader Library */
/* Additional protocols, etc, need to be added here, if required*/

#ifndef PH_NXPBUILD_APP_H_INC
#define PH_NXPBUILD_APP_H_INC

/** Protocol Abstraction Layer Components */

#define NXPBUILD__PHPAL_I14443P3A_SW                        /** ISO 14443-3A SW Component */

//#define NXPBUILD__PHPAL_MIFARE_SW                           /**< MIFARE SW Component */

#define NXPBUILD__PHPAL_SLI15693_SW                         /**< SLI 15693 Component */

/** Discovery Loop Properties */

#define NXPBUILD__PHAC_DISCLOOP_SW                          /**< Discovery Loop Activity */
#define NXPBUILD__PHAC_DISCLOOP_LPCD                        /**< Low Power Card Detection */
#ifdef  NXPBUILD__PHPAL_I14443P3A_SW
    #define NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS         /**< SRC/DATA to Detect/Collision Resolute/Activate cards such as MFC, MFUL, MFP SL1 */
#endif /* NXPBUILD__PHPAL_I14443P3A_SW */
#ifdef NXPBUILD__PHPAL_SLI15693_SW
    #define NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS              /**< SRC/DATA to Detect Type V Cards */
#endif /* NXPBUILD__PHPAL_SLI15693_SW */

/** Simplified API */

#define NXPBUILD__PHNFCLIB                                  /**< Simplified API Interface */

#define NXPBUILD__PH_KEYSTORE_SW                            /**< SW KeyStore Component */

#endif /* PH_NXPBUILD_APP_H_INC */
