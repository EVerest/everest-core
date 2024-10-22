/*
 * Copyright (C) 2012 The Android Open Source Project
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

/*
 *  Import and export general routing data using a XML file.
 */
#pragma once
#include "NfcJniUtil.h"
#include "nfa_api.h"
#ifndef LINUX
#include <libxml/parser.h>
#endif
#include <string>
#include <vector>

/*****************************************************************************
**
**  Name:           RouteData
**
**  Description:    Base class for every kind of route data.
**
*****************************************************************************/
class RouteData {
 public:
  enum RouteType { ProtocolRoute, TechnologyRoute };
  RouteType mRouteType;

 protected:
  RouteData(RouteType routeType) : mRouteType(routeType) {}
};

/*****************************************************************************
**
**  Name:           RouteDataForProtocol
**
**  Description:    Data for protocol routes.
**
*****************************************************************************/
class RouteDataForProtocol : public RouteData {
 public:
  int mNfaEeHandle;  // for example 0x4f3, 0x4f4
  bool mSwitchOn;
  bool mSwitchOff;
  bool mBatteryOff;
  tNFA_PROTOCOL_MASK mProtocol;

  RouteDataForProtocol()
      : RouteData(ProtocolRoute),
        mNfaEeHandle(NFA_HANDLE_INVALID),
        mSwitchOn(false),
        mSwitchOff(false),
        mBatteryOff(false),
        mProtocol(0) {}
};

/*****************************************************************************
**
**  Name:           RouteDataForTechnology
**
**  Description:    Data for technology routes.
**
*****************************************************************************/
class RouteDataForTechnology : public RouteData {
 public:
  int mNfaEeHandle;  // for example 0x4f3, 0x4f4
  bool mSwitchOn;
  bool mSwitchOff;
  bool mBatteryOff;
  tNFA_TECHNOLOGY_MASK mTechnology;

  RouteDataForTechnology()
      : RouteData(TechnologyRoute),
        mNfaEeHandle(NFA_HANDLE_INVALID),
        mSwitchOn(false),
        mSwitchOff(false),
        mBatteryOff(false),
        mTechnology(0) {}
};

/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
**
**  Name:           AidBuffer
**
**  Description:    Buffer to store AID after converting a string of hex
**                  values to bytes.
**
*****************************************************************************/
class AidBuffer {
 public:
  /*******************************************************************************
  **
  ** Function:        AidBuffer
  **
  ** Description:     Parse a string of hex numbers.  Store result in an array
  *of
  **                  bytes.
  **                  aid: string of hex numbers.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  AidBuffer(std::string& aid);

  /*******************************************************************************
  **
  ** Function:        ~AidBuffer
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~AidBuffer();

  uint8_t* buffer() { return mBuffer; };
  int length() { return mBufferLen; };

 private:
  uint8_t* mBuffer;
  uint32_t mBufferLen;
};

/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
**
**  Name:           RouteDataSet
**
**  Description:    Import and export general routing data using a XML file.
**                  See /data/bcm/param/route.xml
**
*****************************************************************************/
class RouteDataSet {
 public:
  typedef std::vector<RouteData*> Database;
  enum DatabaseSelection { DefaultRouteDatabase, SecElemRouteDatabase };

  /*******************************************************************************
  **
  ** Function:        ~RouteDataSet
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~RouteDataSet();

  /*******************************************************************************
  **
  ** Function:        initialize
  **
  ** Description:     Initialize resources.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool initialize();

  /*******************************************************************************
  **
  ** Function:        import
  **
  ** Description:     Import data from an XML file.  Fill the database.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool import();

  /*******************************************************************************
  **
  ** Function:        getDatabase
  **
  ** Description:     Obtain a database of routing data.
  **                  selection: which database.
  **
  ** Returns:         Pointer to database.
  **
  *******************************************************************************/
  Database* getDatabase(DatabaseSelection selection);

  /*******************************************************************************
  **
  ** Function:        saveToFile
  **
  ** Description:     Save XML data from a string into a file.
  **                  routesXml: XML that represents routes.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  static bool saveToFile(const char* routesXml);

  /*******************************************************************************
  **
  ** Function:        loadFromFile
  **
  ** Description:     Load XML data from file into a string.
  **                  routesXml: string to receive XML data.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  static bool loadFromFile(std::string& routesXml);

  /*******************************************************************************
  **
  ** Function:        deleteFile
  **
  ** Description:     Delete route data XML file.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  static bool deleteFile();

  /*******************************************************************************
  **
  ** Function:        printDiagnostic
  **
  ** Description:     Print some diagnostic output.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void printDiagnostic();

 private:
  Database mSecElemRouteDatabase;  // routes when NFC service selects sec elem
  Database mDefaultRouteDatabase;  // routes when NFC service deselects sec elem
  static const char* sConfigFile;
  static const bool sDebug = false;

  /*******************************************************************************
  **
  ** Function:        deleteDatabase
  **
  ** Description:     Delete all routes stored in all databases.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void deleteDatabase();
#ifndef LINUX
  /*******************************************************************************
  **
  ** Function:        importProtocolRoute
  **
  ** Description:     Parse data for protocol routes.
  **                  element: XML node for one protocol route.
  **                  database: store data in this database.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void importProtocolRoute(xmlNodePtr& element, Database& database);

  /*******************************************************************************
  **
  ** Function:        importTechnologyRoute
  **
  ** Description:     Parse data for technology routes.
  **                  element: XML node for one technology route.
  **                  database: store data in this database.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void importTechnologyRoute(xmlNodePtr& element, Database& database);
#endif
};
