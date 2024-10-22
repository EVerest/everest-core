/******************************************************************************
 *
 *  Copyright 2018 NXP
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
#define LOG_TAG "NfccPowerTracker"
#include "NfccPowerTracker.h"
#include "phNxpNciHal_ext.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <log/log.h>
#include <sstream>
#include <stdio.h>
#include <sys/file.h>
#include <sys/time.h>
#include "logging.h"
//#define ALOGD_IF(nfc_debug_enabled,...) { cout << (__VA_ARGS__); }
//#define ALOGE(...) { cout << (__VA_ARGS__); }
using namespace std;

extern bool nfc_debug_enabled;
extern phNxpNciHal_Control_t nxpncihal_ctrl;
static const uint64_t PWR_TRK_ERROR_MARGIN_IN_MILLISEC = 60000;
static const std::string POWER_TRACKER_LOG_FILE =
    "/data/vendor/nfc/nfc_power_state.txt";
static const uint16_t TIMER_COUNT_MASK = 0x7FFF;

NfccPowerTracker::NfccPowerTracker() {
  mIsLastUpdateScreenOn = false;
  mIsFirstPwrTrkNtfRecvd = false;
  mLastPowerTrackAborted = false;
  /*Default standby time*/
  mStandbyTimePerDiscLoopInMillisec = 1000;
}
NfccPowerTracker::~NfccPowerTracker() {}

/*******************************************************************************
**
** Function         NfccPowerTracker::getInstance
**
** Description      access class singleton
**
** Returns          pointer to the singleton object
**
*******************************************************************************/
NfccPowerTracker &NfccPowerTracker::getInstance() {
  static NfccPowerTracker sPwrInstance;
  return sPwrInstance;
}
/*******************************************************************************
**
** Function         Initialize
**
** Description      get all prerequisite information from NFCC needed for
**                  Power tracker calculations.
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::Initialize() {
  /*get total duration of discovery loop from NFCC using GET CONFIG command*/
  uint8_t cmdGetConfigDiscLoopDuration[] = {0x20, 0x03, 0x02, 0x01, 0x00};
  int status = phNxpNciHal_send_ext_cmd(sizeof(cmdGetConfigDiscLoopDuration),
                                        cmdGetConfigDiscLoopDuration);
  if (status != 0) {
    ALOGD_IF(nfc_debug_enabled, "NfccPowerTracker::Initialize: failed");
    return;
  }
  /*Check for valid get config response and update stanby time*/
  if (nxpncihal_ctrl.p_rx_data[0] == 0x40 &&
      nxpncihal_ctrl.p_rx_data[1] == 0x03 &&
      nxpncihal_ctrl.p_rx_data[2] == 0x06 &&
      nxpncihal_ctrl.p_rx_data[3] == 0x00 &&
      nxpncihal_ctrl.p_rx_data[4] == 0x01 &&
      nxpncihal_ctrl.p_rx_data[5] == 0x00 &&
      nxpncihal_ctrl.p_rx_data[6] == 0x02) {
    mStandbyTimePerDiscLoopInMillisec = (uint32_t)(
        (nxpncihal_ctrl.p_rx_data[8] << 8) | nxpncihal_ctrl.p_rx_data[7]);
    ALOGD_IF(nfc_debug_enabled, "mStandbyTimePerDiscLoopInMillisec value : %d",
             mStandbyTimePerDiscLoopInMillisec);
  }
}

/*******************************************************************************
**
** Function         TimeDiff
**
** Description      Computes time difference in milliseconds.
**
** Returns          Time difference in milliseconds
**
*******************************************************************************/
uint64_t NfccPowerTracker::TimeDiff(struct timespec start,
                                    struct timespec end) {
  uint64_t startTimeInMillisec =
      start.tv_sec * 1000 + (start.tv_nsec / 1000000);
  uint64_t endTimeInMillisec = end.tv_sec * 1000 + (end.tv_nsec / 1000000);

  assert(startTimeInMillisec > endTimeInMillisec);
  return (endTimeInMillisec - startTimeInMillisec);
}

/*******************************************************************************
**
** Function         NfccPowerTracker::ProcessCmd
**
** Description      Parse the commands going to NFCC,
**                  get the time at which power relevant commands are sent
**                  (ex:Screen state/OMAPI session)is sent and
**                  log/cache the timestamp to file
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::ProcessCmd(uint8_t *cmd, uint16_t len) {
  ALOGD_IF(nfc_debug_enabled,
           "NfccPowerTracker::ProcessCmd: Enter,Recieved len :%d", len);
  bool screenStateCommand;
  if (cmd[0] == 0x20 && cmd[1] == 0x09) {
    screenStateCommand = true;
  } else {
    screenStateCommand = false;
  }

  if (screenStateCommand && (cmd[3] == 0x00 || cmd[3] == 0x02)) {
    /* Command for Screen State On-Locked or Unlocked */
    clock_gettime(CLOCK_BOOTTIME, &mLastScreenOnTimeStamp);
    mIsLastUpdateScreenOn = true;
  } else if (screenStateCommand && (cmd[3] == 0x01 || cmd[3] == 0x03)) {
    /* Command for Screen State OFF-locked or Unlocked */
    clock_gettime(CLOCK_BOOTTIME, &mLastScreenOffTimeStamp);
    mIsLastUpdateScreenOn = false;
  } else if (cmd[0] == 0x20 && cmd[1] == 0x02 && cmd[2] == 0x05 &&
             cmd[3] == 0x01 && cmd[4] == 0x00 && cmd[5] == 0x02) {
    /* Command to update duration of discovery loop */
    mStandbyTimePerDiscLoopInMillisec = (cmd[7] << 8 | cmd[6]);
    ALOGD_IF(nfc_debug_enabled, "mStandbyTimePerDiscLoopInMillisec value : %d",
             mStandbyTimePerDiscLoopInMillisec);
  }
}

/*******************************************************************************
**
** Function         NfccPowerTracker::ProcessNtf
**
** Description      Parse the Notifications coming from NFCC,
**                  get the time at which power relevant notifications are
**                  received
**                  (ex:RF ON-OFF/ACTIVATE-DEACTIVATE NTF/PROP_PWR_TRACKINFO)
**                  calculate error in standby time by comparing the
**                  expectated value from NFC HAL and received value from NFCC.
**                  Cache relevant info (timestamps) to file
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::ProcessNtf(uint8_t *rsp, uint16_t rsp_len) {
  ALOGD_IF(nfc_debug_enabled, "NfccPowerTracker::ProcessNtf: Enter");

  /* Screen State Notification recieved */
  if ((rsp[0] == 0x6F && rsp[1] == 0x05)) {
    ProcessPowerTrackNtf(rsp, rsp_len);
  } else if (rsp[0] == 0x61 && rsp[1] == 0x05) {
    /*Activation notification received. Calculate the time NFCC is
    active in Reader/P2P/CE duration */
    clock_gettime(CLOCK_BOOTTIME, &mActiveTimeStart);
    if (!mIsLastUpdateScreenOn) {
      mActiveInfo.totalTransitions++;
    }
  } else if (rsp[0] == 0x61 && rsp[1] == 0x06) {
    /* Deactivation notification received Calculate the time NFCC is
    active in Reader/P2P/CE duration.Time between Activation and
    Deacivation gives the active time*/
    clock_gettime(CLOCK_BOOTTIME, &mActiveTimeEnd);
    mActiveDurationFromLastScreenUpdate +=
        TimeDiff(mActiveTimeStart, mActiveTimeEnd);
    if (!mIsLastUpdateScreenOn) {
      mStandbyInfo.totalTransitions++;
    }
    ALOGD_IF(nfc_debug_enabled, "mActiveDurationFromLastScreenUpdate: %llu",
             (unsigned long long)mActiveDurationFromLastScreenUpdate);
  }
}

/*******************************************************************************
**
** Function         ProcessPowerTrackNtf
**
** Description      Process Power Tracker notification and update timingInfo to
**                  Log File.
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::ProcessPowerTrackNtf(uint8_t *rsp, uint16_t rsp_len) {
  /* Enable Power Tracking computations after 1st Power tracker notification
   * is received. */
  if (!mIsFirstPwrTrkNtfRecvd) {
    mIsFirstPwrTrkNtfRecvd = true;
    ifstream ifile(POWER_TRACKER_LOG_FILE.c_str());
    if ((bool)ifile == true) {
      mLastPowerTrackAborted = true;
    }
    return;
  }

  /*Duration between screen state change is taken as reference for calculating
  active and standby time*/
  uint64_t totalDuration = 0;
  totalDuration =
      mIsLastUpdateScreenOn
          ? TimeDiff(mLastScreenOffTimeStamp, mLastScreenOnTimeStamp)
          : TimeDiff(mLastScreenOnTimeStamp, mLastScreenOffTimeStamp);
  if (totalDuration == 0)
    return;

  /*Calculate Active and Standby time based on the pollCount provided in the
  Power tracker Notification from NFCC*/
  uint16_t sPollCount = (TIMER_COUNT_MASK & ((rsp[5] << 8) | rsp[4]));
  ALOGD_IF(nfc_debug_enabled,
           "Poll/Timer count recived from FW is %d and rsp_len :%d", sPollCount,
           rsp_len);
  uint64_t standbyTime = 0, activeTime = 0;
  if (mIsLastUpdateScreenOn) {
    activeTime = sPollCount * ACTIVE_TIME_PER_TIMER_COUNT_IN_MILLISEC;
    /*Check for errors in count provided by NFCC*/
    uint64_t error = (activeTime > mActiveDurationFromLastScreenUpdate)
                         ? (activeTime - mActiveDurationFromLastScreenUpdate)
                         : (mActiveDurationFromLastScreenUpdate - activeTime);
    if (error > PWR_TRK_ERROR_MARGIN_IN_MILLISEC) {
      ALOGD_IF(nfc_debug_enabled,
               "Active Time Error observed with value is %llu",
               (unsigned long long)error);
      mErrorInStandbyInfo.residencyInMsecSinceBoot += error;
    }
    standbyTime = (totalDuration > activeTime) ? (totalDuration - activeTime)
                                               : (activeTime - totalDuration);
    if (rsp[3]) {
      /*If notification trigger is counter overflow, update the screen on
      timestamp as there is no screen state change*/
      clock_gettime(CLOCK_BOOTTIME, &mLastScreenOnTimeStamp);
    }
    mActiveInfo.totalTransitions++;
  } else {
    standbyTime = (sPollCount * ((uint64_t)mStandbyTimePerDiscLoopInMillisec));
    activeTime = totalDuration > standbyTime ? (totalDuration - standbyTime)
                                             : (standbyTime - totalDuration);
    if (rsp[3]) {
      /*If notification trigger is counter overflow, update the screen off
      timestamp as there is no screen state change*/
      clock_gettime(CLOCK_BOOTTIME, &mLastScreenOffTimeStamp);
    }
    /*Total transitions in screen on -> Screen Off window is same as poll count
    provided by NFCC, as, there is transition in each discovery loop*/
    mActiveInfo.totalTransitions += sPollCount;
    /*1 additional transition for screen state update*/
    mStandbyInfo.totalTransitions += (sPollCount + 1);
  }

  ALOGD_IF(nfc_debug_enabled,
           "activeTime: %llu, standbyTime: %llu, totalDuration :%llu",
           (unsigned long long)activeTime, (unsigned long long)standbyTime,
           (unsigned long long)totalDuration);
  if (mLastPowerTrackAborted) {
    ALOGD_IF(nfc_debug_enabled,
             "Last Hal service aborted,so retrive the power info data and "
             "continue\n");
    /*Read the file content and store in mActiveInfo.residencyInMsecSinceBoot
    and mStandbyInfo.residencyInMsecSinceBoot*/
    if (ReadPowerStateLog()) {
      mLastPowerTrackAborted = false;
    }
  }
  mStandbyInfo.residencyInMsecSinceBoot += standbyTime;
  mActiveInfo.residencyInMsecSinceBoot += activeTime;
  UpdatePowerStateLog(mStandbyInfo, mActiveInfo);
  mActiveDurationFromLastScreenUpdate = 0;
}
/*******************************************************************************
**
** Function         NfccPowerTracker::UpdatePowerStateLog
**
** Description      update the powerstate related information in log file
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::UpdatePowerStateLog(NfccPowerStateInfo_t mStandbyInfo,
                                           NfccPowerStateInfo_t mActiveInfo) {
  FILE *fp;
  const string PWR_TRK_LOG_FILE_VERSION = "1.0";
  /*Write the Active and standby timestamp into the file*/
  fp = fopen(POWER_TRACKER_LOG_FILE.c_str(), "w");
  if (fp == NULL) {
    ALOGD_IF(nfc_debug_enabled, "Failed to Open Pwr Tracker Info File\n");
    return;
  }
  ostringstream PwrTrackerInfo;
  PwrTrackerInfo << "Version: " << PWR_TRK_LOG_FILE_VERSION.c_str() << endl;
  PwrTrackerInfo << "NFC {" << endl;
  PwrTrackerInfo << " { " << STR_ACTIVE
                 << std::to_string(mActiveInfo.residencyInMsecSinceBoot) << " }"
                 << endl;
  PwrTrackerInfo << " { " << STR_STANDBY
                 << std::to_string(mStandbyInfo.residencyInMsecSinceBoot)
                 << " }" << endl;
  PwrTrackerInfo << "}";
  ALOGD_IF(nfc_debug_enabled,
           "mActiveInfo.residencyInMsecSinceBoot: %llu, "
           "mActiveInfo.totalTransitions: %llu,"
           "mStandbyInfo.residencyInMsecSinceBoot "
           ":%llu,mStandbyInfo.totalTransitions: %llu"
           "mErrorInStandbyInfo.residencyInMsecSinceBoot: %llu",
           (unsigned long long)mActiveInfo.residencyInMsecSinceBoot,
           (unsigned long long)mActiveInfo.totalTransitions,
           (unsigned long long)mStandbyInfo.residencyInMsecSinceBoot,
           (unsigned long long)mStandbyInfo.totalTransitions,
           (unsigned long long)mErrorInStandbyInfo.residencyInMsecSinceBoot);
  string PwrInfo = PwrTrackerInfo.str();
  if (!TryLockFile(fp)) {
    ALOGD_IF(nfc_debug_enabled,
             "Failed to Lock PwrTracker File.Skipping update\n");
    fclose(fp);
    return;
  }
  fwrite(PwrInfo.c_str(), sizeof(char), PwrInfo.length(), fp);
  fflush(fp);
  UnlockFile(fp);
  fclose(fp);
}
/*******************************************************************************
 **
 ** Function         ReadPowerStateLog
 **
 ** Description      Retrieve powerstate related information from log file.
 **
 ** Returns          true if read successful, false otherwise.
 **
 *******************************************************************************/
bool NfccPowerTracker::ReadPowerStateLog() {
  ifstream pwrStateFileStream;
  string itemName;
  ALOGD_IF(nfc_debug_enabled, "NfccPowerTracker::ReadPowerStateLog: Enter \n");
  pwrStateFileStream.open(POWER_TRACKER_LOG_FILE.c_str());
  if (pwrStateFileStream.fail()) {
    ALOGE("Error: %s", strerror(errno));
    return false;
  }

  /*Check for required string(time in millisec) in the log file and convert it
    to integer*/
  while (pwrStateFileStream >> itemName) {
    if (STR_ACTIVE.compare(itemName) == 0) {
      pwrStateFileStream >> itemName;
      mActiveInfo.residencyInMsecSinceBoot = stoull(itemName.c_str(), nullptr);
    } else if (STR_STANDBY.compare(itemName) == 0) {
      pwrStateFileStream >> itemName;
      mStandbyInfo.residencyInMsecSinceBoot = stoull(itemName.c_str(), nullptr);
    }
  }

  ALOGD_IF(nfc_debug_enabled,
           "Value retrieved from Powertracker file is"
           "activeTime: %llu and standbyTime: %llu\n",
           (unsigned long long)mActiveInfo.residencyInMsecSinceBoot,
           (unsigned long long)mStandbyInfo.residencyInMsecSinceBoot);
  pwrStateFileStream.close();
  return true;
}
/*******************************************************************************
**
** Function         Pause
**
** Description      Pause Power state Information Tracking,Tracking will resume
**                  once next power tracker notification is recieved as part of
**                  ProcessNtf.
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::Pause() { mIsFirstPwrTrkNtfRecvd = false; }

/*******************************************************************************
**
** Function         Reset
**
** Description      Stop power track information processing and delete
**                  power tracker log file.
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::Reset() {
  ALOGD_IF(nfc_debug_enabled, "NfccPowerTracker::Reset enter");
  if (remove(POWER_TRACKER_LOG_FILE.c_str()) != 0) {
    ALOGD_IF(nfc_debug_enabled, "Error deleting Power tracker file");
  }
}
/*******************************************************************************
**
** Function         TryLockFile
**
** Description      Lock PowerTracker log file. Any application trying to read
**                  from PowerTracker log file shall acquire lock before reading
**                  to avoid inconsistent data.
**
** Returns          true if locking was successful
**                  false if there was a failure to lock PowerTracker log file.
*******************************************************************************/
bool NfccPowerTracker::TryLockFile(FILE *fp) {
  uint8_t retryCount = 5;
  do {
    if (!flock(fileno(fp), LOCK_EX | LOCK_NB))
      return true;
    usleep(10000); /*10 millisec*/
  } while (retryCount--);

  return false;
}
/*******************************************************************************
**
** Function         UnlockFile
**
** Description      Unlock previously locked PowerTracker log file.
**
** Returns          void
**
*******************************************************************************/
void NfccPowerTracker::UnlockFile(FILE *fp) { flock(fileno(fp), LOCK_UN); }
