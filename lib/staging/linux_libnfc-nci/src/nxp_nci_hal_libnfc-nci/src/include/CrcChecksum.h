/******************************************************************************
 *
 *  Copyright (C) 2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
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

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
**
** Function         crcChecksumCompute
**
** Description      Compute a checksum on a buffer of data.
**
** Returns          2-byte checksum.
**
*******************************************************************************/
uint16_t crcChecksumCompute(const unsigned char* buffer, int bufferLen);

/*******************************************************************************
**
** Function         crcChecksumVerifyIntegrity
**
** Description      Detect any corruption in a file by computing a checksum.
**                  filename: file name.
**
** Returns          True if file is good.
**
*******************************************************************************/
bool crcChecksumVerifyIntegrity(const char* filename);
