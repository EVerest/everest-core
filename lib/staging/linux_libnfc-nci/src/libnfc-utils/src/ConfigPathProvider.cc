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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include "configPathProvider.h"
#include <vector>
#include<sys/stat.h>
extern bool nfc_debug_enabled;
extern string nfc_storage_path;
using android::base::StringPrintf;
ConfigPathProvider::ConfigPathProvider() {}

/*******************************************************************************
**
** Function         getInstance
**
** Description      returns the static instance of ConfigPathProvider
**
** Returns          ConfigPathProvider instance
*******************************************************************************/
ConfigPathProvider &ConfigPathProvider::getInstance() {
static ConfigPathProvider mConfigPathProviderInstance;
return mConfigPathProviderInstance;
}

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
string ConfigPathProvider::getEnvVar(std::string const &key) {
    char *val = getenv(key.c_str());
    return val == NULL ? std::string("") : std::string(val);
}

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
void ConfigPathProvider::addEnvPathIfAvailable(string &path) {
    string mwConfigDir(getEnvVar("MW_CONFIG_DIR"));
    struct stat file_stat;
    if (!mwConfigDir.empty() && (stat(mwConfigDir.c_str(), &file_stat) == 0)) {
        path.erase(0, 1);
        path.insert(0, mwConfigDir);
    }
}

/*******************************************************************************
**
** Function         getFilePath
**
** Description      Returns file path of requested type of file
**
** Returns          file path
**
*******************************************************************************/
string ConfigPathProvider::getFilePath(FileType type) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter FileType:0x%02x", __func__, type);
    switch (type) {
    case VENDOR_NFC_CONFIG: {
        string path = "//usr//local//etc//libnfc-nxp.conf";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case VENDOR_ESE_CONFIG: {
        string path = "//usr//local//etc//libese-nxp.conf";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case SYSTEM_CONFIG: {
        const vector<string> searchPath = { "//usr/local//etc//" };
        for (string path : searchPath) {
            addEnvPathIfAvailable(path);
            path.append("libnfc-nci.conf");
            LOG(INFO) << "path: " << path;
            struct stat file_stat;
            if (stat(path.c_str(), &file_stat) != 0) continue;
            if (S_ISREG(file_stat.st_mode)) return path;
        }
        return "";
    } break;
    case RF_CONFIG: {
        string path = "//usr//local//etc//libnfc-nxp_RF.conf";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case TRANSIT_CONFIG: {
        string path = "//usr/local//etc//libnfc-nxpTransit.conf";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case NFASTORAGE_BIN: {
        string path = ".\\data\\nfc";
        addEnvPathIfAvailable(path);
        nfc_storage_path.assign(path);
        return nfc_storage_path;
    } break;
    case FIRMWARE_LIB: {
        string path = "//usr/local//etc//libsn100u_fw.dll";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case CONFIG_TIMESTAMP: {
        string path = "//usr//local//etc//libnfc-nxpConfigState.bin";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case RF_CONFIG_TIMESTAMP: {
        string path = "//usr//local//etc//libnfc-nxpRFConfigState.bin";
        addEnvPathIfAvailable(path);
        return path;
    } break;
    case TRANSIT_CONFIG_TIMESTAMP: {
        string path = "//usr//local//etc//libnfc-nxpTransitConfigState.bin";
        addEnvPathIfAvailable(path);
        return path;
    } break;

    default:
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Unnknown FileType", __func__);
        break;
    }
}
