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
#pragma once
#include "SyncEvent.h"
#include "nfa_api.h"

/*****************************************************************************
**
**  Name:           PowerSwitch
**
**  Description:    Adjust the controller's power states.
**
*****************************************************************************/
class PowerSwitch {
 public:
  /*******************************************************************************
  **
  ** Description:     UNKNOWN_LEVEL: power level is unknown because the stack is
  *off.
  **                  FULL_POWER: controller is in full-power state.
  **                  LOW_POWER: controller is in lower-power state.
  **
  *******************************************************************************/
  enum PowerLevel { UNKNOWN_LEVEL, FULL_POWER, LOW_POWER, POWER_OFF };

  /*******************************************************************************
  **
  ** Description:     POWER_STATE_OFF: power level is OFF when screen is off.
  **                  POWER_STATE_FULL: controller is in full-power state when
  *screen is off.
  **                                    after a period of inactivity, controller
  *goes into snooze
  **                                    mode.
  **                  POWER_STATE_CARD_EMULATION: screen-off card-emulation
  **                                              (CE4/CE3/CE1 modes are used),
  **
  *******************************************************************************/
  enum ScreenOffPowerState {
    POWER_STATE_OFF = 0,
    POWER_STATE_FULL = 1,
    POWER_STATE_CARD_EMULATION = 2
  };

  /*******************************************************************************
  **
  ** Description:     DISCOVERY: Discovery is enabled
  **                  SE_ROUTING: Routing to SE is enabled.
  **                  SE_CONNECTED: SE is connected.
  **
  *******************************************************************************/
  typedef int PowerActivity;
  static const PowerActivity DISCOVERY;
  static const PowerActivity SE_ROUTING;
  static const PowerActivity SE_CONNECTED;
  static const PowerActivity HOST_ROUTING;

  /*******************************************************************************
  **
  ** Description:     Platform Power Level, copied from
  *NativeNfcBrcmPowerMode.java.
  **                  UNKNOWN_LEVEL: power level is unknown.
  **                  POWER_OFF: The phone is turned OFF
  **                  SCREEN_OFF: The phone is turned ON but screen is OFF
  **                  SCREEN_ON_LOCKED: The phone is turned ON, screen is ON but
  *user locked
  **                  SCREEN_ON_UNLOCKED: The phone is turned ON, screen is ON,
  *and user unlocked
  **
  *******************************************************************************/
  static const int PLATFORM_UNKNOWN_LEVEL = 0;
  static const int PLATFORM_POWER_OFF = 1;
  static const int PLATFORM_SCREEN_OFF = 2;
  static const int PLATFORM_SCREEN_ON_LOCKED = 3;
  static const int PLATFORM_SCREEN_ON_UNLOCKED = 4;

  static const int VBAT_MONITOR_ENABLED = 1;
  static const int VBAT_MONITOR_PRIMARY_THRESHOLD = 5;
  static const int VBAT_MONITOR_SECONDARY_THRESHOLD = 8;
  /*******************************************************************************
  **
  ** Function:        PowerSwitch
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  PowerSwitch();

  /*******************************************************************************
  **
  ** Function:        ~PowerSwitch
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~PowerSwitch();

  /*******************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the singleton of this object.
  **
  ** Returns:         Reference to this object.
  **
  *******************************************************************************/
  static PowerSwitch& getInstance();

  /*******************************************************************************
  **
  ** Function:        initialize
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void initialize(PowerLevel level);

  /*******************************************************************************
  **
  ** Function:        getLevel
  **
  ** Description:     Get the current power level of the controller.
  **
  ** Returns:         Power level.
  **
  *******************************************************************************/
  PowerLevel getLevel();

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
  bool setLevel(PowerLevel level);

  /*******************************************************************************
  **
  ** Function:        setScreenOffPowerState
  **
  ** Description:     Set the controller's screen off power state.
  **                  state: the desired screen off power state.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool setScreenOffPowerState(ScreenOffPowerState state);

  /*******************************************************************************
  **
  ** Function:        setModeOff
  **
  ** Description:     Set a mode to be deactive.
  **
  ** Returns:         True if any mode is still active.
  **
  *******************************************************************************/
  bool setModeOff(PowerActivity deactivated);

  /*******************************************************************************
  **
  ** Function:        setModeOn
  **
  ** Description:     Set a mode to be active.
  **
  ** Returns:         True if any mode is active.
  **
  *******************************************************************************/
  bool setModeOn(PowerActivity activated);

  /*******************************************************************************
  **
  ** Function:        abort
  **
  ** Description:     Abort and unblock currrent operation.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void abort();

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
  static void deviceManagementCallback(uint8_t event,
                                       tNFA_DM_CBACK_DATA* eventData);

  /*******************************************************************************
  **
  ** Function:        isPowerOffSleepFeatureEnabled
  **
  ** Description:     Whether power-off-sleep feature is enabled in .conf file.
  **
  ** Returns:         True if feature is enabled.
  **
  *******************************************************************************/
  bool isPowerOffSleepFeatureEnabled();

 private:
  PowerLevel mCurrLevel;
  uint8_t mCurrDeviceMgtPowerState;  // device management power state; such as
                                     // NFA_BRCM_PWR_MODE_???
  uint8_t mExpectedDeviceMgtPowerState;  // device management power state; such
                                         // as NFA_BRCM_PWR_MODE_???
  int mDesiredScreenOffPowerState;  // read from .conf file; 0=power-off-sleep;
                                    // 1=full power; 2=CE4 power
  static PowerSwitch sPowerSwitch;  // singleton object
  static const uint8_t NFA_DM_PWR_STATE_UNKNOWN =
      -1;  // device management power state power state is unknown
  SyncEvent mPowerStateEvent;
  PowerActivity mCurrActivity;
  Mutex mMutex;

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
  bool setPowerOffSleepState(bool sleep);

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
  const char* deviceMgtPowerStateToString(uint8_t deviceMgtPowerState);

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
  const char* powerLevelToString(PowerLevel level);

  /*******************************************************************************
  **
  ** Function:        screenOffPowerStateToString
  **
  ** Description:     Decode screen-off power state to a string.
  **                  state: power state
  **
  ** Returns:         Text representation of screen-off power state.
  **
  *******************************************************************************/
  const char* screenOffPowerStateToString(ScreenOffPowerState state);
};
