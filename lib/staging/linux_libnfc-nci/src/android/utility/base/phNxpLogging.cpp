/******************************************************************************
 *  Copyright 2018-2021 NXP
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
 ******************************************************************************/
#include <iostream>
#include <string>
#include "logging.h"

phLogger::phLogger() {
    // Default log level
    debug_log_level = false;
}
phLogger& phLogger::getInstance() {
    static phLogger theInstance;
    return theInstance;
}

static string sever;
phLogger& phLogger::logs(std::string severity, bool level) {
    if (!instance.debug_log_level) sever = severity;
    return instance;
}

// will be invoked when RHS is instance and LHS is <<"str"
phLogger& operator<<(phLogger& p, std::string str) {
    if (!p.debug_log_level) {  // BOOST_LOG_TRIVIAL(info) << str;
        string infoStr = INFO;
        string errorStr = ERROR;
        string warningStr = WARNING;
        string debugStr = DEBUG;
        if (sever == errorStr) {
            NXPLOG_API_E(str.c_str())
        } else if (sever == warningStr) {
            NXPLOG_API_W(str.c_str())
        } else if (sever == infoStr) {
            NXPLOG_API_D(str.c_str())
        } else if (sever == debugStr) {
            NXPLOG_API_D(str.c_str())
        } else { /*do nothing*/
        }
    }
    return instance;
}
phLogger& operator<<(phLogger& p, int val) {
    if (!p.debug_log_level) std::cout << val << std::endl;
    return instance;
}
/****************************************************
** Global Variables used for Logging on Windows    **
****************************************************/
phLogger logger;
// A global reference to a glabal phLogger class varibale.
// It will be used in << operator overloading.
phLogger& instance = logger;

/****************************************************
**  __android_log_error_write is Android API       **
**  This definitation is Windows specific and      **
****************************************************/
int __android_log_error_write(int tag, const char* subTag, int32_t uid, const char* data,
                              uint32_t dataLen) {
    std::cout << "tag:" << tag << " subTag:" << subTag;
    if (-1 != uid) std::cout << "uid:" << uid;
    if (NULL != data && 0 != dataLen) {
        std::cout << data;
    }
    return dataLen;
}
