/******************************************************************************
 *
 *  Copyright 2020-2021 NXP
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

#include <NfccI2cTransport.h>
#include <NfccAltI2cTransport.h>
#include <NfccAltSpiTransport.h>

#include <NfccTransportFactory.h>
#include <phNxpLog.h>
/*******************************************************************************
 **
 ** Function         NfccTransportFactory
 **
 ** Description      Constructor for transportFactory. This will be private to
 **                  support singleton
 **
 ** Parameters       none
 **
 ** Returns          none
 ******************************************************************************/
NfccTransportFactory::NfccTransportFactory() {}

/*******************************************************************************
**
** Function         getTransport
**
** Description      selects and returns transport channel based on the input
**                  parameter
**
** Parameters       Required transport Type
**
** Returns          Selected transport channel
******************************************************************************/
NfccTransportFactory &NfccTransportFactory::getInstance() {
  static NfccTransportFactory mTransprtFactoryInstance;
  return mTransprtFactoryInstance;
}

/*******************************************************************************
**
** Function         getTransport
**
** Description      selects and returns transport channel based on the input
**                  parameter
**
** Parameters       Required transport Type
**
** Returns          Selected transport channel
******************************************************************************/
spTransport NfccTransportFactory::getTransport(transportIntf transportType) {
  NXPLOG_TML_D("%s Requested transportType: %d\n", __func__, transportType);
  spTransport mspTransportInterface;
  switch (transportType) {
    case I2C:
      //NfccI2cTransport is common code for both nxpnfc I2C driver and nxpnfc spi driver
      mspTransportInterface = std::make_shared<NfccI2cTransport>();
      break;
    case ALT_I2C:
      mspTransportInterface = std::make_shared<NfccAltI2cTransport>();
      break;
    case ALT_SPI:
      mspTransportInterface = std::make_shared<NfccAltSpiTransport>();
      break;
    default:
      mspTransportInterface = std::make_shared<NfccI2cTransport>();
      break;
  }
  return mspTransportInterface;
}
