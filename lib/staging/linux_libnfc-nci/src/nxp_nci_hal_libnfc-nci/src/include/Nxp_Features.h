/******************************************************************************
 *
 *  Copyright 2018,2020 NXP
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

#ifndef NXP_FEATURES_H
#define NXP_FEATURES_H
#include <unistd.h>
#include <string>

#define FW_MOBILE_MAJOR_NUMBER_PN553 0x01
#define FW_MOBILE_MAJOR_NUMBER_PN81A 0x02
#define FW_MOBILE_MAJOR_NUMBER_PN551 0x05
#define FW_MOBILE_MAJOR_NUMBER_PN557 0x01
#define FW_MOBILE_MAJOR_NUMBER_PN548AD 0x01
/*Including T4T NFCEE by incrementing 1*/
#define NFA_EE_MAX_EE_SUPPORTED 5
 using namespace std;
typedef enum {
  unknown,
  pn547C2,
  pn65T,
  pn548C2,
  pn66T,
  pn551,
  pn67T,
  pn553,
  pn80T,
  pn557,
  pn81T
} tNFC_chipType;

typedef struct {
  tNFC_chipType chipType;
  std::string _FW_LIB_PATH;
  std::string _FW_BIN_PATH;
  uint16_t _PHDNLDNFC_USERDATA_EEPROM_OFFSET;
  uint16_t _PHDNLDNFC_USERDATA_EEPROM_LEN;
  uint8_t _FW_MOBILE_MAJOR_NUMBER;
} tNfc_featureList;

extern tNfc_featureList nfcFL;

#define CONFIGURE_FEATURELIST(chipType)                                      \
  {                                                                          \
    nfcFL.chipType = chipType;                                               \
    if (chipType == pn81T) {                                                 \
      nfcFL.chipType = pn557;                                                \
    } else if (chipType == pn80T) {                                          \
      nfcFL.chipType = pn553;                                                \
    } else if (chipType == pn67T) {                                          \
      nfcFL.chipType = pn551;                                                \
    } else if (chipType == pn66T) {                                          \
      nfcFL.chipType = pn548C2;                                              \
     }                                                                       \
      CONFIGURE_FEATURELIST_NFCC(chipType)                                   \
  }

#define CONFIGURE_FEATURELIST_NFCC(chipType)                                \
  {                                                                         \
    nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x023CU;                      \
    nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C80U;                         \
                                                                            \
    if (chipType == pn557 || chipType == pn81T) {                           \
                                                                            \
      STRCPY_FW_LIB("libpn7160_fw")                                          \
      STRCPY_FW_BIN("pn7160")                                                \
                                                                            \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN557;         \
    } else if (chipType == pn553 || chipType == pn80T) {                    \
                                                                            \
      STRCPY_FW_LIB("libpn553_fw")                                          \
      STRCPY_FW_BIN("pn553")                                                \
                                                                            \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN553;         \
                                                                            \
    } else if (chipType == pn551 || chipType == pn67T) {                    \
                                                                            \
      STRCPY_FW_LIB("libpn551_fw")                                          \
      STRCPY_FW_BIN("pn551")                                                \
                                                                            \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x02BCU;                    \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C00U;                       \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN551;         \
                                                                            \
    } else if (chipType == pn548C2 || chipType == pn66T) {                  \
                                                                            \
      STRCPY_FW_LIB("libpn548ad_fw")                                        \
      STRCPY_FW_BIN("pn548")                                                \
                                                                            \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x02BCU;                    \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C00U;                       \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN548AD;       \
     }                                                                      \
  }
#define STRCPY_FW_LIB(str) {                                                \
  nfcFL._FW_LIB_PATH.clear();                                               \
  nfcFL._FW_LIB_PATH.append(FW_LIB_ROOT_DIR);                               \
  nfcFL._FW_LIB_PATH.append(str);                                           \
  nfcFL._FW_LIB_PATH.append(FW_LIB_EXTENSION);                              \
}
#define STRCPY_FW_BIN(str) {                                                \
  nfcFL._FW_BIN_PATH.clear();                                               \
  nfcFL._FW_BIN_PATH.append(FW_BIN_ROOT_DIR);                               \
  nfcFL._FW_BIN_PATH.append(str);                                           \
  nfcFL._FW_BIN_PATH.append(FW_BIN_EXTENSION);                              \
}
#endif
