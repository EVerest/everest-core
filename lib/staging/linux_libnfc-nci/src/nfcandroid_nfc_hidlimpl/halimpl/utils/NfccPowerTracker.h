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
#pragma once

#include <string>
#include <time.h>
#include <vector>

/*Time spent in Active mode per count provided by NFCC*/
static const uint32_t ACTIVE_TIME_PER_TIMER_COUNT_IN_MILLISEC = 20;
/*Types of  Power states supported by NFCC */
typedef struct NfccPowerStateInfo {
  /* state name: Active/Standby */
  std::string name;
  /* Time spent in msec at this  power state since boot */
  uint64_t residencyInMsecSinceBoot;
  /* Total number of times Nfcc entered this state */
  uint64_t totalTransitions;
} NfccPowerStateInfo_t;

/*Class to track the time spent in Standby mode by NFCC*/
class NfccPowerTracker {
public:
  static NfccPowerTracker &getInstance();

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
  void Initialize();

  /*******************************************************************************
  **
  ** Function         ProcessCmd
  **
  ** Description      Parse the commands going to NFCC,
  **                  get the time at which power relevant commands are sent
  **                  (ex:Screen state/OMAPI session)is sent and
  **                  log/cache the timestamp to file.
  **
  ** Returns          void
  **
  *******************************************************************************/
  void ProcessCmd(uint8_t *, uint16_t len);

  /*******************************************************************************
  **
  ** Function         ProcessNtf
  **
  ** Description      Parse the Notifications coming from NFCC,
  **                  get the time at which power relevant notifications are
  **                  received (ex:RF ON-OFF/ACTIVATE-DEACTIVATE NTF/
  **                  PROP_PWR_TRACKINFO). Calculate error in standby time by
  **                  comparing the expectated value from NFC HAL and received
  **                  value from NFCC. Update power state duration info
  **                  to file.
  **
  ** Returns          void
  **
  *******************************************************************************/
  void ProcessNtf(uint8_t *cmd, uint16_t len);

  /*******************************************************************************
  **
  ** Function         Pause
  **
  ** Description      Pause Power state Information Tracking,Tracking will
  **                  resume once next power tracker notification is recieved as
  **                  part of ProcessNtf.
  **
  ** Returns          void
  **
  *******************************************************************************/
  void Pause();

  /*******************************************************************************
  **
  ** Function         Reset
  **
  ** Description      Stop power tracker information processing and delete
  **                  power track log file.
  **
  ** Returns          void
  **
  *******************************************************************************/
  void Reset();

private:
  NfccPowerTracker();
  ~NfccPowerTracker();

  /*******************************************************************************
  **
  ** Function         UpdatePowerStateLog
  **
  ** Description      update the powerstate related information in log file
  **
  ** Returns          void
  **
  *******************************************************************************/
  void UpdatePowerStateLog(NfccPowerStateInfo_t standbyTime,
                           NfccPowerStateInfo_t activeTime);

  /*******************************************************************************
   **
   ** Function         ReadPowerStateLog
   **
   ** Description      Retrieve powerstate related information from log file.
   **
   ** Returns          true if read successful, false otherwise.
   **
   *******************************************************************************/
  bool ReadPowerStateLog();

  /*******************************************************************************
   **
   ** Function         ProcessPowerTrackNtf
   **
   ** Description      Process Power Tracker notification.
   **
   ** Returns          void
   **
   *******************************************************************************/
  void ProcessPowerTrackNtf(uint8_t *rsp, uint16_t rsp_len);

  /*******************************************************************************
  **
  ** Function         TimeDiff
  **
  ** Description      Computes time difference in milliseconds.
  **
  ** Returns          Time difference in milliseconds
  **
  *******************************************************************************/
  uint64_t TimeDiff(timespec start, timespec end);
  /*******************************************************************************
  **
  ** Function         TryLockFile
  **
  ** Description      Lock PowerTracker log file. Any application trying to read
  **                  from PowerTracker log file shall acquire lock before
  **                  reading to avoid inconsistent data.
  **
  ** Returns          true if locking was successful
  **                  false if there was a failure to lock file.
  *******************************************************************************/
  bool TryLockFile(FILE *fp);
  /*******************************************************************************
  **
  ** Function         UnlockFile
  **
  ** Description      Unlock previously locked PowerTracker log file.
  **
  ** Returns          void
  *******************************************************************************/
  void UnlockFile(FILE *fp);
  struct timespec mLastScreenOffTimeStamp = {0, 0},
                  mLastScreenOnTimeStamp = {0, 0};
  /*Used to calculate time NFCC is active during Card emulation/P2P/Reader
   * modes*/
  struct timespec mActiveTimeStart = {0, 0}, mActiveTimeEnd = {0, 0};

  bool mIsLastUpdateScreenOn;
  bool mIsFirstPwrTrkNtfRecvd;

  uint64_t mActiveDurationFromLastScreenUpdate = 0;
  NfccPowerStateInfo_t mActiveInfo, mStandbyInfo, mErrorInStandbyInfo;

  /*Last powertracker processing aborted due to NFC HAL Service abort*/
  bool mLastPowerTrackAborted = false;
  /* Time spent in standby mode in one discovery loop containing poll */
  uint32_t mStandbyTimePerDiscLoopInMillisec;
  const std::string STR_ACTIVE = "Active: ", STR_STANDBY = "StandBy: ";
};
