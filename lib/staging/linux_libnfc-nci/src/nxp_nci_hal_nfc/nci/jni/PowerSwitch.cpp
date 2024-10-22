/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  Adjust the controller's power states.
 */
#include "PowerSwitch.h"
#include "NfcJniUtil.h"
#include "nfc_config.h"

#include <android-base/stringprintf.h>
#include <base/logging.h>

using android::base::StringPrintf;

namespace android {
void doStartupConfig();
}

extern bool gActivated;
extern bool nfc_debug_enabled;
extern SyncEvent gDeactivatedEvent;

PowerSwitch PowerSwitch::sPowerSwitch;
const PowerSwitch::PowerActivity PowerSwitch::DISCOVERY = 0x01;
const PowerSwitch::PowerActivity PowerSwitch::SE_ROUTING = 0x02;
const PowerSwitch::PowerActivity PowerSwitch::SE_CONNECTED = 0x04;
const PowerSwitch::PowerActivity PowerSwitch::HOST_ROUTING = 0x08;

/*******************************************************************************
**
** Function:        PowerSwitch
**
** Description:     Initialize member variables.
**
** Returns:         None
**
*******************************************************************************/
PowerSwitch::PowerSwitch()
    : mCurrLevel(UNKNOWN_LEVEL),
      mCurrDeviceMgtPowerState(NFA_DM_PWR_STATE_UNKNOWN),
      mExpectedDeviceMgtPowerState(NFA_DM_PWR_STATE_UNKNOWN),
      mDesiredScreenOffPowerState(0),
      mCurrActivity(0) {}

/*******************************************************************************
**
** Function:        ~PowerSwitch
**
** Description:     Release all resources.
**
** Returns:         None
**
*******************************************************************************/
PowerSwitch::~PowerSwitch() {}

/*******************************************************************************
**
** Function:        getInstance
**
** Description:     Get the singleton of this object.
**
** Returns:         Reference to this object.
**
*******************************************************************************/
PowerSwitch& PowerSwitch::getInstance() { return sPowerSwitch; }

/*******************************************************************************
**
** Function:        initialize
**
** Description:     Initialize member variables.
**
** Returns:         None
**
*******************************************************************************/
void PowerSwitch::initialize(PowerLevel level) {
  static const char fn[] = "PowerSwitch::initialize";

  mMutex.lock();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: level=%s (%u)", fn, powerLevelToString(level), level);
  if (NfcConfig::hasKey(NAME_SCREEN_OFF_POWER_STATE))
    mDesiredScreenOffPowerState =
        (int)NfcConfig::getUnsigned(NAME_SCREEN_OFF_POWER_STATE);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: desired screen-off state=%d", fn, mDesiredScreenOffPowerState);

  switch (level) {
    case FULL_POWER:
      mCurrDeviceMgtPowerState = NFA_DM_PWR_MODE_FULL;
      mCurrLevel = level;
      break;

    case UNKNOWN_LEVEL:
      mCurrDeviceMgtPowerState = NFA_DM_PWR_STATE_UNKNOWN;
      mCurrLevel = level;
      break;

    default:
      LOG(ERROR) << StringPrintf("%s: not handled", fn);
      break;
  }
  mMutex.unlock();
}

/*******************************************************************************
**
** Function:        getLevel
**
** Description:     Get the current power level of the controller.
**
** Returns:         Power level.
**
*******************************************************************************/
PowerSwitch::PowerLevel PowerSwitch::getLevel() {
  PowerLevel level = UNKNOWN_LEVEL;
  mMutex.lock();
  level = mCurrLevel;
  mMutex.unlock();
  return level;
}

/*******************************************************************************
**
** Function:        setLevel
**
** Description:     Set the controller's power level.
**                  level: power level.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool PowerSwitch::setLevel(PowerLevel newLevel) {
  static const char fn[] = "PowerSwitch::setLevel";
  bool retval = false;

  mMutex.lock();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: level=%s (%u)", fn, powerLevelToString(newLevel), newLevel);
  if (mCurrLevel == newLevel) {
    retval = true;
    goto TheEnd;
  }

  if (mCurrLevel == UNKNOWN_LEVEL) {
    LOG(ERROR) << StringPrintf("%s: unknown power level", fn);
    goto TheEnd;
  }

  if ((mCurrLevel == LOW_POWER && newLevel == FULL_POWER) ||
      (mCurrLevel == FULL_POWER && newLevel == LOW_POWER)) {
    mMutex.unlock();
    SyncEventGuard g(gDeactivatedEvent);
    if (gActivated) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: wait for deactivation", fn);
      gDeactivatedEvent.wait();
    }
    mMutex.lock();
  }

  switch (newLevel) {
    case FULL_POWER:
      if (mCurrDeviceMgtPowerState == NFA_DM_PWR_MODE_OFF_SLEEP)
        retval = setPowerOffSleepState(false);
      break;

    case LOW_POWER:
    case POWER_OFF:
      if (isPowerOffSleepFeatureEnabled())
        retval = setPowerOffSleepState(true);
      else if (mDesiredScreenOffPowerState ==
               1)  //.conf file desires full-power
      {
        mCurrLevel = FULL_POWER;
        retval = true;
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("%s: not handled", fn);
      break;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: actual power level=%s", fn, powerLevelToString(mCurrLevel));

TheEnd:
  mMutex.unlock();
  return retval;
}

bool PowerSwitch::setScreenOffPowerState(ScreenOffPowerState newState) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("PowerSwitch::setScreenOffPowerState: level=%s (%u)",
                      screenOffPowerStateToString(newState), newState);

  mMutex.lock();
  mDesiredScreenOffPowerState = (int)newState;
  mMutex.unlock();

  return true;
}

/*******************************************************************************
**
** Function:        setModeOff
**
** Description:     Set a mode to be deactive.
**
** Returns:         True if any mode is still active.
**
*******************************************************************************/
bool PowerSwitch::setModeOff(PowerActivity deactivated) {
  bool retVal = false;

  mMutex.lock();
  mCurrActivity &= ~deactivated;
  retVal = mCurrActivity != 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "PowerSwitch::setModeOff(deactivated=0x%x) : mCurrActivity=0x%x",
      deactivated, mCurrActivity);
  mMutex.unlock();
  return retVal;
}

/*******************************************************************************
**
** Function:        setModeOn
**
** Description:     Set a mode to be active.
**
** Returns:         True if any mode is active.
**
*******************************************************************************/
bool PowerSwitch::setModeOn(PowerActivity activated) {
  bool retVal = false;

  mMutex.lock();
  mCurrActivity |= activated;
  retVal = mCurrActivity != 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "PowerSwitch::setModeOn(activated=0x%x) : mCurrActivity=0x%x", activated,
      mCurrActivity);
  mMutex.unlock();
  return retVal;
}

/*******************************************************************************
**
** Function:        setPowerOffSleepState
**
** Description:     Adjust controller's power-off-sleep state.
**                  sleep: whether to enter sleep state.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool PowerSwitch::setPowerOffSleepState(bool sleep) {
  static const char fn[] = "PowerSwitch::setPowerOffSleepState";
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; sleep=%u", fn, sleep);
  tNFA_STATUS stat = NFA_STATUS_FAILED;
  bool retval = false;

  if (sleep)  // enter power-off-sleep state
  {
    // make sure the current power state is ON
    if (mCurrDeviceMgtPowerState != NFA_DM_PWR_MODE_OFF_SLEEP) {
      SyncEventGuard guard(mPowerStateEvent);
      mExpectedDeviceMgtPowerState =
          NFA_DM_PWR_MODE_OFF_SLEEP;  // if power adjustment is ok, then this is
                                      // the expected state
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: try power off", fn);
      stat = NFA_PowerOffSleepMode(TRUE);
      if (stat == NFA_STATUS_OK) {
        mPowerStateEvent.wait();
        mCurrLevel = LOW_POWER;
      } else {
        LOG(ERROR) << StringPrintf("%s: API fail; stat=0x%X", fn, stat);
        goto TheEnd;
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "%s: power is not ON; curr device mgt power state=%s (%u)", fn,
          deviceMgtPowerStateToString(mCurrDeviceMgtPowerState),
          mCurrDeviceMgtPowerState);
      goto TheEnd;
    }
  } else  // exit power-off-sleep state
  {
    // make sure the current power state is OFF
    if (mCurrDeviceMgtPowerState != NFA_DM_PWR_MODE_FULL) {
      SyncEventGuard guard(mPowerStateEvent);
      mCurrDeviceMgtPowerState = NFA_DM_PWR_STATE_UNKNOWN;
      mExpectedDeviceMgtPowerState =
          NFA_DM_PWR_MODE_FULL;  // if power adjustment is ok, then this is the
                                 // expected state
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: try full power", fn);
      stat = NFA_PowerOffSleepMode(FALSE);
      if (stat == NFA_STATUS_OK) {
        mPowerStateEvent.wait();
        if (mCurrDeviceMgtPowerState != NFA_DM_PWR_MODE_FULL) {
          LOG(ERROR) << StringPrintf(
              "%s: unable to full power; curr device mgt power stat=%s (%u)",
              fn, deviceMgtPowerStateToString(mCurrDeviceMgtPowerState),
              mCurrDeviceMgtPowerState);
          goto TheEnd;
        }
        android::doStartupConfig();
        mCurrLevel = FULL_POWER;
      } else {
        LOG(ERROR) << StringPrintf("%s: API fail; stat=0x%X", fn, stat);
        goto TheEnd;
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "%s: not in power-off state; curr device mgt power state=%s (%u)", fn,
          deviceMgtPowerStateToString(mCurrDeviceMgtPowerState),
          mCurrDeviceMgtPowerState);
      goto TheEnd;
    }
  }

  retval = true;
TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; return %u", fn, retval);
  return retval;
}

/*******************************************************************************
**
** Function:        deviceMgtPowerStateToString
**
** Description:     Decode power level to a string.
**                  deviceMgtPowerState: power level.
**
** Returns:         Text representation of power level.
**
*******************************************************************************/
const char* PowerSwitch::deviceMgtPowerStateToString(
    uint8_t deviceMgtPowerState) {
  switch (deviceMgtPowerState) {
    case NFA_DM_PWR_MODE_FULL:
      return "DM-FULL";
    case NFA_DM_PWR_MODE_OFF_SLEEP:
      return "DM-OFF";
    default:
      return "DM-unknown????";
  }
}

/*******************************************************************************
**
** Function:        powerLevelToString
**
** Description:     Decode power level to a string.
**                  level: power level.
**
** Returns:         Text representation of power level.
**
*******************************************************************************/
const char* PowerSwitch::powerLevelToString(PowerLevel level) {
  switch (level) {
    case UNKNOWN_LEVEL:
      return "PS-UNKNOWN";
    case FULL_POWER:
      return "PS-FULL";
    case LOW_POWER:
      return "PS-LOW-POWER";
    case POWER_OFF:
      return "PS-POWER-OFF";
    default:
      return "PS-unknown????";
  }
}

/*******************************************************************************
**
** Function:        screenOffPowerStateToString
**
** Description:     Decode power level to a string.
**                  level: power level.
**
** Returns:         Text representation of power level.
**
*******************************************************************************/
const char* PowerSwitch::screenOffPowerStateToString(
    ScreenOffPowerState state) {
  switch (state) {
    case POWER_STATE_OFF:
      return "SOPS-POWER_OFF";
    case POWER_STATE_FULL:
      return "SOPS-FULL";
    case POWER_STATE_CARD_EMULATION:
      return "SOPS-CARD_EMULATION";
    default:
      return "SOPS-unknown????";
  }
}

/*******************************************************************************
**
** Function:        abort
**
** Description:     Abort and unblock currrent operation.
**
** Returns:         None
**
*******************************************************************************/
void PowerSwitch::abort() {
  static const char fn[] = "PowerSwitch::abort";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  SyncEventGuard guard(mPowerStateEvent);
  mPowerStateEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        deviceManagementCallback
**
** Description:     Callback function for the stack.
**                  event: event ID.
**                  eventData: event's data.
**
** Returns:         None
**
*******************************************************************************/
void PowerSwitch::deviceManagementCallback(uint8_t event,
                                           tNFA_DM_CBACK_DATA* eventData) {
  static const char fn[] = "PowerSwitch::deviceManagementCallback";

  switch (event) {
    case NFA_DM_PWR_MODE_CHANGE_EVT: {
      tNFA_DM_PWR_MODE_CHANGE& power_mode = eventData->power_mode;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_PWR_MODE_CHANGE_EVT; status=0x%X; device mgt power "
          "state=%s (0x%X)",
          fn, power_mode.status,
          sPowerSwitch.deviceMgtPowerStateToString(power_mode.power_mode),
          power_mode.power_mode);
      SyncEventGuard guard(sPowerSwitch.mPowerStateEvent);
      if (power_mode.status == NFA_STATUS_OK) {
        // the event data does not contain the newly configured power mode,
        // so this code assigns the expected value
        sPowerSwitch.mCurrDeviceMgtPowerState =
            sPowerSwitch.mExpectedDeviceMgtPowerState;
      }
      sPowerSwitch.mPowerStateEvent.notifyOne();
    } break;
  }
}

/*******************************************************************************
**
** Function:        isPowerOffSleepFeatureEnabled
**
** Description:     Whether power-off-sleep feature is enabled in .conf file.
**
** Returns:         True if feature is enabled.
**
*******************************************************************************/
bool PowerSwitch::isPowerOffSleepFeatureEnabled() {
  return mDesiredScreenOffPowerState == 0;
}
