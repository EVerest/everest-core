/******************************************************************************
*
* Copyright 2020 NXP.
* NXP Confidential. This software is owned or controlled by NXP and may only be
* used strictly in accordance with the applicable license terms.  By expressly
* accepting such terms or by downloading, installing, activating and/or
* otherwise using the software, you are agreeing that you have read, and that
* you agree to comply with and are bound by, such license terms.  If you do not
* agree to be bound by the applicable license terms, then you may not retain,
* install, activate or otherwise use the software.
*
******************************************************************************/
#pragma once
#include <string>
#include <iostream>

using namespace std;

#define cfgPathProvider (ConfigPathProvider::getInstance())

enum FileType {
    VENDOR_NFC_CONFIG,         // libnfc-nxp.conf
    VENDOR_ESE_CONFIG,         // libese-nxp.conf
    SYSTEM_CONFIG,             // libnfc-nci.conf
    RF_CONFIG,                 // libnfc-nxp_RF.conf
    TRANSIT_CONFIG,            // libnfc-nxpTransit.conf
    NFASTORAGE_BIN,            // nfaStorage.bin1
    FIRMWARE_LIB,              // libsn100u_fw.dll
    CONFIG_TIMESTAMP,          // libnfc-nxpConfigState.bin
    RF_CONFIG_TIMESTAMP,       // libnfc-nxpRFConfigState.bin
    TRANSIT_CONFIG_TIMESTAMP,  // libnfc-nxpTransitConfigState.bin
};

class ConfigPathProvider {

ConfigPathProvider();
/*******************************************************************************
**
** Function         getEnvVar.
**
** Description      Returns valid value stored in requested Env variable if
**                  available, else returns empty string
**
** Returns          value stored in requested Env variable
**
*******************************************************************************/
string getEnvVar(std::string const &key);

/*******************************************************************************
**
** Function         addEnvPathIfAvailable.
**
** Description      If Env variable MW_CONFIG_DIR is available and has valid
**                  path in it, then that path will be added to predefined
**                  directory structure of the file generating actual
**                  location in host system
**
** Returns          None.
**
*******************************************************************************/
void addEnvPathIfAvailable(string &path);

public:
/*******************************************************************************
**
** Function         getInstance
**
** Description      returns the static instance of ConfigPathProvider
**
** Returns          ConfigPathProvider instance
*******************************************************************************/
static ConfigPathProvider &getInstance();

/*******************************************************************************
**
** Function         getFilePath
**
** Description      Returns file path of requested type of file
**
** Returns          file path
**
*******************************************************************************/
string getFilePath(FileType type);
};
