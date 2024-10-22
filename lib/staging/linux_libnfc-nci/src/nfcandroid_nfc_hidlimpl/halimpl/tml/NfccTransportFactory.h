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

#pragma once
#include <NfccTransport.h>
#include <memory>

#define transportFactory (NfccTransportFactory::getInstance())
typedef std::shared_ptr<NfccTransport> spTransport;
#ifdef LINUX
enum transportIntf { I2C, SPI, ALT_I2C, ALT_SPI, UNKNOWN };
#else
enum transportIntf { I2C, SPI, UNKNOWN };
#endif
extern spTransport gpTransportObj;
class NfccTransportFactory {
  /*****************************************************************************
   **
   ** Function         NfccTransportFactory
   **
   ** Description      Constructor for transportFactory. This will be private to
   **                  support singleton
   **
   ** Parameters       none
   **
   ** Returns          none
   ****************************************************************************/
  NfccTransportFactory();

public:
  /*****************************************************************************
   **
   ** Function         getInstance
   **
   ** Description      returns the static instance of TransportFactory
   **
   ** Parameters       none
   **
   ** Returns          TransportFactory instance
   ****************************************************************************/
  static NfccTransportFactory &getInstance();

  /*****************************************************************************
  **
  ** Function         getTransport
  **
  ** Description      selects and returns transport channel based on the input
  **                  parameter
  **
  ** Parameters       Required transport Type
  **
  ** Returns          Selected transport channel
  ****************************************************************************/
  spTransport getTransport(transportIntf transportType);
};
