/******************************************************************************
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
*  Copyright 2018-2019 NXP
*
******************************************************************************/
#ifndef PH_LOGGING_H
#define PH_LOGGING_H
#include <iostream>
#include "phNxpLog.h"
#include "macro.h"
#include "phOsalNfc_Timer.h"
#include <sys/times.h> //times()
#ifdef ANDROID
#include <boost/log/trivial.hpp>
#endif
#include <android-base/stringprintf.h>

using android::base::StringPrintf;

/**************************************************************************************
** Class to prints logs in libnfc.dll
** NFC_DEBUG_ENABLED param from libnfc-nci.conf will determine the log level
**    if ( 0==NFC_DEBUG_ENABLED) will print all the DLL logs
**    if ( 1==NFC_DEBUG_ENABLED) will print Only Sent and received data and ERROR logs
***************************************************************************************/
class phLogger{
public:
    //var should be initialized in  init loge level function
    bool debug_log_level;
    bool debug_ese_log_level;
    friend phLogger& operator << (phLogger& p, std::string str);
    friend phLogger& operator << (phLogger& p, int val);
    //friend  phLogger& operator <<(phLogger& p, char* str);
    //friend  phLogger& operator <<(phLogger& p, const char* str);
    phLogger();
    phLogger& logs(std::string severity, bool level = 1);
    phLogger& getInstance();
};
extern phLogger logger;
extern phLogger& instance;

/******************************************************************************
** ANDROID P Logging Macros (replacement for base/logging.h file on Linux) **
** NXP                                                                       **
******************************************************************************/
#define INFO "info"
#define DEBUG "debug"
#define WARNING "warning"
#define ERROR "error"

#define LOG(severity) logger.logs(severity)
#define LOG_IF(severity, condition) logger.logs(severity, condition)
#define DLOG_IF(severity, condition) logger.logs(severity, condition)
// Override Android's ALOGD macro for Linux.
#define ALOGD(...) {                                              \
        if(!logger.debug_log_level) {                             \
	  NXPLOG_API_D(__VA_ARGS__)                              \
        }                                                         \
}								  
#define ALOGE(...) {                                              \
	NXPLOG_API_E(__VA_ARGS__)                              \
}								
#define NXPLOG_EXTNS_D(...) {                                     \
        if(!logger.debug_log_level) {                             \
	  NXPLOG_API_D(__VA_ARGS__)                              \
        }							  
#define ALOGD_IF(debug_enabled, ...) {                            \
        if (logger.debug_ese_log_level) {                         \
	  NXPLOG_API_D(__VA_ARGS__)                              \
        }                                                         \
}
#define __attribute__(A)  /*Do nothing*/

#define CHECK(x)  if(!(x)){std::cout<<__FILE__<<__LINE__ << "ERROR::Check failed: "<<#x<<std::endl;}std::cout
#define CHECK_EQ(x, y) CHECK((x) == (y))
#define CHECK_NE(x, y) CHECK((x) != (y))
#define CHECK_LE(x, y) CHECK((x) <= (y))
#define CHECK_LT(x, y) CHECK((x) < (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))
#define CHECK_GT(x, y) CHECK((x) > (y))


#define S_ISREG( m ) (((m) & S_IFMT) == S_IFREG)

//#define S_IRUSR _S_IREAD
//#define S_IWUSR _S_IWRITE
#define TEMP_FAILURE_RETRY

#endif//PH_LOGGING_H
