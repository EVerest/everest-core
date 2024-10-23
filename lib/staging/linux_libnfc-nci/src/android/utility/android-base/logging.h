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
#ifndef PH_ABASE_LOGGING_H
#define PH_ABASE_LOGGING_H
#include <iostream>
#include "macro.h"
#include "phNxpLog.h"
#include <sys/stat.h> //S_ISREG
#include <base/logging.h>

#define __attribute__(A)  /*Do nothing*/

#define CHECK(x)  if(!(x)){std::cout<<__FILE__<<__LINE__ << "FATAL::Check failed: "<<#x<<std::endl;}std::cout
#define CHECK_EQ(x, y) CHECK((x) == (y))
#define CHECK_NE(x, y) CHECK((x) != (y))
#define CHECK_LE(x, y) CHECK((x) <= (y))
#define CHECK_LT(x, y) CHECK((x) < (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))
#define CHECK_GT(x, y) CHECK((x) > (y))


#define S_ISREG( m ) (((m) & S_IFMT) == S_IFREG)

#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#define TEMP_FAILURE_RETRY

#endif//PH_ABASE_LOGGING_H