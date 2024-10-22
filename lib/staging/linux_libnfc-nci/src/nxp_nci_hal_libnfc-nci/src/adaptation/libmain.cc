/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <fcntl.h>
#include <vector>

#include "CrcChecksum.h"
#include "nfa_nv_ci.h"
#include "nfc_hal_nv_co.h"
#include <unistd.h>
//using android::base::StringPrintf;

extern std::string nfc_storage_path;
extern bool nfc_debug_enabled;

namespace {
std::string getFilenameForBlock(const unsigned block) {
  char str[200]={0};
  std::string bin = "nfaStorage.bin";
  snprintf(str,200,"%s/%s%u", nfc_storage_path.c_str(), bin.c_str(), block);
  string s1 = str;
  return s1;//StringPrintf("%s/%s%u", nfc_storage_path.c_str(), bin.c_str(), block);
}
}  // namespace

/*******************************************************************************
**
** Function         nfa_mem_co_alloc
**
** Description      allocate a buffer from platform's memory pool
**
** Returns:
**                  pointer to buffer if successful
**                  NULL otherwise
**
*******************************************************************************/
extern void* nfa_mem_co_alloc(uint32_t num_bytes) { return malloc(num_bytes); }

/*******************************************************************************
**
** Function         nfa_mem_co_free
**
** Description      free buffer previously allocated using nfa_mem_co_alloc
**
** Returns:
**                  Nothing
**
*******************************************************************************/
extern void nfa_mem_co_free(void* pBuffer) { free(pBuffer); }

/*******************************************************************************
**
** Function         nfa_nv_co_read
**
** Description      This function is called by NFA to read in data from the
**                  previously opened file.
**
** Parameters       pBuffer   - buffer to read the data into.
**                  nbytes  - number of bytes to read into the buffer.
**
** Returns          void
**
**                  Note: Upon completion of the request, nfa_nv_ci_read() is
**                        called with the buffer of data, along with the number
**                        of bytes read into the buffer, and a status.  The
**                        call-in function should only be called when ALL
**                        requested bytes have been read, the end of file has
**                        been detected, or an error has occurred.
**
*******************************************************************************/
extern void nfa_nv_co_read(uint8_t* pBuffer, uint16_t nbytes, uint8_t block) {
  std::string filename = getFilenameForBlock(block);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: buffer len=%u; file=%s", __func__, nbytes, filename.c_str());
  int fileStream = open(filename.c_str(), O_RDONLY);
  if (fileStream >= 0) {
    uint16_t checksum = 0;
    size_t checkSumRdData = read(fileStream, &checksum, sizeof(checksum));
    if (checkSumRdData <= 0) {
      LOG(ERROR) << StringPrintf("%s: failed to read checksum, errno = 0x%02x",
                                 __func__, errno);
    }
    size_t actualReadData = read(fileStream, pBuffer, nbytes);
    close(fileStream);
    if (actualReadData > 0) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: data size=%zu", __func__, actualReadData);
      nfa_nv_ci_read(actualReadData, NFA_NV_CO_OK, block);
    } else {
      LOG(ERROR) << StringPrintf("%s: fail to read", __func__);
      nfa_nv_ci_read(0, NFA_NV_CO_FAIL, block);
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: fail to open", __func__);
    nfa_nv_ci_read(0, NFA_NV_CO_FAIL, block);
  }
}

/*******************************************************************************
**
** Function         nfa_nv_co_write
**
** Description      This function is called by io to send file data to the
**                  phone.
**
** Parameters       pBuffer   - buffer to read the data from.
**                  nbytes  - number of bytes to write out to the file.
**
** Returns          void
**
**                  Note: Upon completion of the request, nfa_nv_ci_write() is
**                        called with the file descriptor and the status.  The
**                        call-in function should only be called when ALL
**                        requested bytes have been written, or an error has
**                        been detected,
**
*******************************************************************************/
extern void nfa_nv_co_write(const uint8_t* pBuffer, uint16_t nbytes,
                            uint8_t block) {
  std::string filename = getFilenameForBlock(block);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: bytes=%u; file=%s", __func__, nbytes, filename.c_str());

  int fileStream =
      open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fileStream >= 0) {
    uint16_t checksum = crcChecksumCompute(pBuffer, nbytes);
    size_t actualWrittenCrc = write(fileStream, &checksum, sizeof(checksum));
    size_t actualWrittenData = write(fileStream, pBuffer, nbytes);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: %zu bytes written", __func__, actualWrittenData);
    if ((actualWrittenData == nbytes) &&
        (actualWrittenCrc == sizeof(checksum))) {
      nfa_nv_ci_write(NFA_NV_CO_OK);
    } else {
      LOG(ERROR) << StringPrintf("%s: fail to write", __func__);
      nfa_nv_ci_write(NFA_NV_CO_FAIL);
    }
    close(fileStream);
  } else {
    LOG(ERROR) << StringPrintf("%s: fail to open, error = %d", __func__, errno);
    nfa_nv_ci_write(NFA_NV_CO_FAIL);
  }
}

/*******************************************************************************
**
** Function         delete_stack_non_volatile_store
**
** Description      Delete all the content of the stack's storage location.
**
** Parameters       forceDelete: unconditionally delete the storage.
**
** Returns          none
**
*******************************************************************************/
void delete_stack_non_volatile_store(bool forceDelete) {
  static bool firstTime = true;

  if ((firstTime == false) && (forceDelete == false)) return;
  firstTime = false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);

  if (remove(getFilenameForBlock(DH_NV_BLOCK).c_str())) {
    LOG(ERROR) << StringPrintf(
        "%s: fail to delete DH_NV_BLOCK file, errno = 0x%02X", __func__, errno);
  }
  if (remove(getFilenameForBlock(HC_F2_NV_BLOCK).c_str())) {
    LOG(ERROR) << StringPrintf(
        "%s: fail to delete HC_F2_NV_BLOCK file, errno = 0x%02X", __func__,
        errno);
  }
  if (remove(getFilenameForBlock(HC_F3_NV_BLOCK).c_str())) {
    LOG(ERROR) << StringPrintf(
        "%s: fail to delete HC_F3_NV_BLOCK file, errno = 0x%02X", __func__,
        errno);
  }
  if (remove(getFilenameForBlock(HC_F4_NV_BLOCK).c_str())) {
    LOG(ERROR) << StringPrintf(
        "%s: fail to delete HC_F4_NV_BLOCK file, errno = 0x%02X", __func__,
        errno);
  }
  if (remove(getFilenameForBlock(HC_F5_NV_BLOCK).c_str())) {
    LOG(ERROR) << StringPrintf(
        "%s: fail to delete HC_F5_NV_BLOCK file, errno = 0x%02X", __func__,
        errno);
  }
}

/*******************************************************************************
**
** Function         verify_stack_non_volatile_store
**
** Description      Verify the content of all non-volatile store.
**
** Parameters       none
**
** Returns          none
**
*******************************************************************************/
void verify_stack_non_volatile_store() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);

  const std::vector<unsigned> verify_blocks = {DH_NV_BLOCK, HC_F2_NV_BLOCK,
                                               HC_F3_NV_BLOCK, HC_F4_NV_BLOCK,
                                               HC_F5_NV_BLOCK};

  size_t verified = 0;
  for (auto block : verify_blocks) {
    if (!crcChecksumVerifyIntegrity(getFilenameForBlock(block).c_str())) break;
    ++verified;
  }

  if (verified != verify_blocks.size()) delete_stack_non_volatile_store(true);
}
