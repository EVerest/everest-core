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

#ifndef LINUX_MACRO_H
#define LINUX_MACRO_H

#include <fcntl.h>
#include <sys/stat.h> //S_ISREG
#include <vector>
#define UNUSED(x) (void)(x)

/* FALLTHROUGH_INTENDED:
 *       Above macro is used in AOSP code for falling through the switch case statements.
 *       The definitation is not present on the Linux platform hence defining void here.
 *       This is to avoid compilation errors with Zero modifications.                      */
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED \
    do {                     \
    } while (0)
#endif

//#define nanosleep(X,Y) 0;Sleep(((X)->tv_sec*1000000)+((X)->tv_nsec/1000))
//As nanosleep returns int and Sleep returns void
//#define usleep(X) Sleep((X)/1000) //us->ms
//#define sleep(X) Sleep((X)*1000)  //s->ms
//Replace with Linux specific functions
#define phOsalNfc_Thread_GetTaskId() GetCurrentThreadId()

//Replace by NULL statement as this function can't be implemented in Linux.
#define pthread_condattr_setclock(ATTR,CLCK_ID) ;

/* DUMMY
*  Please write code for below mentioned Function as per the requirements.
*  Currently macro is defined to avoid Build errors.
* NOTE: Modify config.h from libnfc_dll_Visual_Studio_Premake5\libnfc-nci\src\include
*/
//#define GetNumValue(A,B,C) 1 //Replicate always return False from this Func
//#define GetStrValue(A,B,C) 1//

//#define initializeGlobalAppLogLevel() ;

//#define nfcsnoop_capture(A, B) ;
//phTmlNfc.h and phTmlNfc_i2c.h
//#define phTmlNfc_get_ese_access(X,Y) 0//returned status is treated as 0

//#define phTmlNfc_set_power_scheme(X,Y) 0
#define acquire_wake_lock(A,B) ; //
#define release_wake_lock(A) ;   // Need to check for as used in GKI
//#define DispLLCP(A,B) ;
//#define DispHcp(A,B) ;
//#define phOsalNfc_Timer_Cleanup(void) ;

//#define debug_nfcsnoop_dump(x) 
//#define debug_nfcsnoop_init() 
/****************************************************************************
** Defined in libmain.c                                                    **
** This is done beacuse currenltly Download code is Disabled               **
** Need to be removed once Download code is enabled                        **
*****************************************************************************/
//extern uint16_t wFwVer; /* Firmware version no */
//extern uint8_t gRecFWDwnld; /* flag set to true to indicate dummy FW download */


/*************Used in the phDal4Nfc_messageQueueLib.c*****************/
typedef int key_t;

//Declarations to adapt uinstd.h on Linux
#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif


#define S_ISREG( m ) (((m) & S_IFMT) == S_IFREG)

//#define S_IRUSR _S_IREAD
//#define S_IWUSR _S_IWRITE
#define TEMP_FAILURE_RETRY

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif
//size_t strlcpy(char *destination, const char *source, size_t size);
//size_t strlcat(char *destination, const char *source, size_t size);
#endif //WINDOWS_DLL_MACRO_H
