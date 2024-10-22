/*
 * Copyright (C) 2010-2014 NXP Semiconductors
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
 * OSAL Implementation for Timers.
 */

#include <phNfcCommon.h>
#include <phNfcTypes.h>
#include <phNxpLog.h>
#include <phNxpNciHal.h>
#include <phOsalNfc_Timer.h>
#include <signal.h>

#define PH_NFC_MAX_TIMER (5U)
static phOsalNfc_TimerHandle_t apTimerInfo[PH_NFC_MAX_TIMER];

extern phNxpNciHal_Control_t nxpncihal_ctrl;

/*
 * Defines the base address for generating timerid.
 */
#define PH_NFC_TIMER_BASE_ADDRESS (100U)

/*
 *  Defines the value for invalid timerid returned during timeSetEvent
 */
#define PH_NFC_TIMER_ID_ZERO (0x00)

/*
 * Invalid timer ID type. This ID used indicate timer creation is failed */
#define PH_NFC_TIMER_ID_INVALID (0xFFFF)

/* Forward declarations */
static void phOsalNfc_PostTimerMsg(phLibNfc_Message_t* pMsg);
static void phOsalNfc_DeferredCall(void* pParams);
static void phOsalNfc_Timer_Expired(union sigval sv);

/*
 *************************** Function Definitions ******************************
 */

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Create
**
** Description      Creates a timer which shall call back the specified function
**                  when the timer expires. Fails if OSAL module is not
**                  initialized or timers are already occupied
**
** Parameters       None
**
** Returns          TimerId
**                  TimerId value of PH_OSALNFC_TIMER_ID_INVALID indicates that
**                  timer is not created
**
*******************************************************************************/
uint32_t phOsalNfc_Timer_Create(void) {
  /* dwTimerId is also used as an index at which timer object can be stored */
  uint32_t dwTimerId = PH_OSALNFC_TIMER_ID_INVALID;
  static struct sigevent se;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  /* Timer needs to be initialized for timer usage */

  se.sigev_notify = SIGEV_THREAD;
  se.sigev_notify_function = phOsalNfc_Timer_Expired;
  se.sigev_notify_attributes = NULL;
  dwTimerId = phUtilNfc_CheckForAvailableTimer();

  /* Check whether timers are available, if yes create a timer handle structure
   */
  if ((PH_NFC_TIMER_ID_ZERO != dwTimerId) && (dwTimerId <= PH_NFC_MAX_TIMER)) {
    pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwTimerId - 1];
    /* Build the Timer Id to be returned to Caller Function */
    dwTimerId += PH_NFC_TIMER_BASE_ADDRESS;
    se.sigev_value.sival_int = (int)dwTimerId;
    /* Create POSIX timer */
    if (timer_create(CLOCK_REALTIME, &se, &(pTimerHandle->hTimerHandle)) ==
        -1) {
      dwTimerId = PH_NFC_TIMER_ID_INVALID;
    } else {
      /* Set the state to indicate timer is ready */
      pTimerHandle->eState = eTimerIdle;
      /* Store the Timer Id which shall act as flag during check for timer
       * availability */
      pTimerHandle->TimerId = dwTimerId;
    }
  } else {
    dwTimerId = PH_NFC_TIMER_ID_INVALID;
  }

  /* Timer ID invalid can be due to Uninitialized state,Non availability of
   * Timer */
  return dwTimerId;
}

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Start
**
** Description      Starts the requested, already created, timer.
**                  If the timer is already running, timer stops and restarts
**                  with the new timeout value and new callback function in case
**                  any ??????
**                  Creates a timer which shall call back the specified function
**                  when the timer expires
**
** Parameters       dwTimerId - valid timer ID obtained during timer creation
**                  dwRegTimeCnt - requested timeout in milliseconds
**                  pApplication_callback - application callback interface to be
**                                          called when timer expires
**                  pContext - caller context, to be passed to the application
**                             callback function
**
** Returns          NFC status:
**                  NFCSTATUS_SUCCESS - the operation was successful
**                  NFCSTATUS_NOT_INITIALISED - OSAL Module is not initialized
**                  NFCSTATUS_INVALID_PARAMETER - invalid parameter passed to
**                                                the function
**                  PH_OSALNFC_TIMER_START_ERROR - timer could not be created
**                                                 due to system error
**
*******************************************************************************/
NFCSTATUS phOsalNfc_Timer_Start(uint32_t dwTimerId, uint32_t dwRegTimeCnt,
                                pphOsalNfc_TimerCallbck_t pApplication_callback,
                                void* pContext) {
  NFCSTATUS wStartStatus = NFCSTATUS_SUCCESS;

  struct itimerspec its;
  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  /* Retrieve the index at which the timer handle structure is stored */
  dwIndex = dwTimerId - PH_NFC_TIMER_BASE_ADDRESS - 0x01;
  pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
  /* OSAL Module needs to be initialized for timer usage */
  /* Check whether the handle provided by user is valid */
  if ((dwIndex < PH_NFC_MAX_TIMER) && (0x00 != pTimerHandle->TimerId) &&
      (NULL != pApplication_callback)) {
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = dwRegTimeCnt / 1000;
    its.it_value.tv_nsec = 1000000 * (dwRegTimeCnt % 1000);
    if (its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) {
      /* This would inadvertently stop the timer*/
      its.it_value.tv_nsec = 1;
    }
    pTimerHandle->Application_callback = pApplication_callback;
    pTimerHandle->pContext = pContext;
    pTimerHandle->eState = eTimerRunning;
    /* Arm the timer */
    if ((timer_settime(pTimerHandle->hTimerHandle, 0, &its, NULL)) == -1) {
      wStartStatus = PHNFCSTVAL(CID_NFC_OSAL, PH_OSALNFC_TIMER_START_ERROR);
    }
  } else {
    wStartStatus = PHNFCSTVAL(CID_NFC_OSAL, NFCSTATUS_INVALID_PARAMETER);
  }

  return wStartStatus;
}

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Stop
**
** Description      Stops already started timer
**                  Allows to stop running timer. In case timer is stopped,
**                  timer callback will not be notified any more
**
** Parameters       dwTimerId - valid timer ID obtained during timer creation
**
** Returns          NFC status:
**                  NFCSTATUS_SUCCESS - the operation was successful
**                  NFCSTATUS_NOT_INITIALISED - OSAL Module is not initialized
**                  NFCSTATUS_INVALID_PARAMETER - invalid parameter passed to
**                                                the function
**                  PH_OSALNFC_TIMER_STOP_ERROR - timer could not be stopped due
**                                                to system error
**
*******************************************************************************/
NFCSTATUS phOsalNfc_Timer_Stop(uint32_t dwTimerId) {
  NFCSTATUS wStopStatus = NFCSTATUS_SUCCESS;
  static struct itimerspec its = {{0, 0}, {0, 0}};

  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  dwIndex = dwTimerId - PH_NFC_TIMER_BASE_ADDRESS - 0x01;
  pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
  /* OSAL Module and Timer needs to be initialized for timer usage */
  /* Check whether the TimerId provided by user is valid */
  if ((dwIndex < PH_NFC_MAX_TIMER) && (0x00 != pTimerHandle->TimerId) &&
      (pTimerHandle->eState != eTimerIdle)) {
    /* Stop the timer only if the callback has not been invoked */
    if (pTimerHandle->eState == eTimerRunning) {
      if ((timer_settime(pTimerHandle->hTimerHandle, 0, &its, NULL)) == -1) {
        wStopStatus = PHNFCSTVAL(CID_NFC_OSAL, PH_OSALNFC_TIMER_STOP_ERROR);
      } else {
        /* Change the state of timer to Stopped */
        pTimerHandle->eState = eTimerStopped;
      }
    }
  } else {
    wStopStatus = PHNFCSTVAL(CID_NFC_OSAL, NFCSTATUS_INVALID_PARAMETER);
  }

  return wStopStatus;
}

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Delete
**
** Description      Deletes previously created timer
**                  Allows to delete previously created timer. In case timer is
**                  running, it is first stopped and then deleted
**
** Parameters       dwTimerId - valid timer ID obtained during timer creation
**
** Returns          NFC status:
**                  NFCSTATUS_SUCCESS - the operation was successful
**                  NFCSTATUS_NOT_INITIALISED - OSAL Module is not initialized
**                  NFCSTATUS_INVALID_PARAMETER - invalid parameter passed to
**                                                the function
**                  PH_OSALNFC_TIMER_DELETE_ERROR - timer could not be stopped
**                                                  due to system error
**
*******************************************************************************/
NFCSTATUS phOsalNfc_Timer_Delete(uint32_t dwTimerId) {
  NFCSTATUS wDeleteStatus = NFCSTATUS_SUCCESS;

  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  dwIndex = dwTimerId - PH_NFC_TIMER_BASE_ADDRESS - 0x01;
  pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
  /* OSAL Module and Timer needs to be initialized for timer usage */

  /* Check whether the TimerId passed by user is valid and Deregistering of
   * timer is successful */
  if ((dwIndex < PH_NFC_MAX_TIMER) && (0x00 != pTimerHandle->TimerId) &&
      (NFCSTATUS_SUCCESS == phOsalNfc_CheckTimerPresence(pTimerHandle))) {
    /* Cancel the timer before deleting */
    if (timer_delete(pTimerHandle->hTimerHandle) == -1) {
      wDeleteStatus = PHNFCSTVAL(CID_NFC_OSAL, PH_OSALNFC_TIMER_DELETE_ERROR);
    }
    /* Clear Timer structure used to store timer related data */
    memset(pTimerHandle, (uint8_t)0x00, sizeof(phOsalNfc_TimerHandle_t));
  } else {
    wDeleteStatus = PHNFCSTVAL(CID_NFC_OSAL, NFCSTATUS_INVALID_PARAMETER);
  }
  return wDeleteStatus;
}

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Cleanup
**
** Description      Deletes all previously created timers
**                  Allows to delete previously created timers. In case timer is
**                  running, it is first stopped and then deleted
**
** Parameters       None
**
** Returns          None
**
*******************************************************************************/
void phOsalNfc_Timer_Cleanup(void) {
  /* Delete all timers */
  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  for (dwIndex = 0; dwIndex < PH_NFC_MAX_TIMER; dwIndex++) {
    pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
    /* OSAL Module and Timer needs to be initialized for timer usage */

    /* Check whether the TimerId passed by user is valid and Deregistering of
     * timer is successful */
    if ((0x00 != pTimerHandle->TimerId) &&
        (NFCSTATUS_SUCCESS == phOsalNfc_CheckTimerPresence(pTimerHandle))) {
      /* Cancel the timer before deleting */
      if (timer_delete(pTimerHandle->hTimerHandle) == -1) {
        NXPLOG_TML_E("timer %d delete error!", dwIndex);
      }
      /* Clear Timer structure used to store timer related data */
      memset(pTimerHandle, (uint8_t)0x00, sizeof(phOsalNfc_TimerHandle_t));
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phOsalNfc_DeferredCall
**
** Description      Invokes the timer callback function after timer expiration.
**                  Shall invoke the callback function registered by the timer
**                  caller function
**
** Parameters       pParams - parameters indicating the ID of the timer
**
** Returns          None                -
**
*******************************************************************************/
static void phOsalNfc_DeferredCall(void* pParams) {
  /* Retrieve the timer id from the parameter */
  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;
  if (NULL != pParams) {
    /* Retrieve the index at which the timer handle structure is stored */
    dwIndex = (uintptr_t)pParams - PH_NFC_TIMER_BASE_ADDRESS - 0x01;
    pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
    if (pTimerHandle->Application_callback != NULL) {
      /* Invoke the callback function with osal Timer ID */
      pTimerHandle->Application_callback((uintptr_t)pParams,
                                         pTimerHandle->pContext);
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phOsalNfc_PostTimerMsg
**
** Description      Posts message on the user thread
**                  Shall be invoked upon expiration of a timer
**                  Shall post message on user thread through which timer
**                  callback function shall be invoked
**
** Parameters       pMsg - pointer to the message structure posted on user
**                         thread
**
** Returns          None
**
*******************************************************************************/
static void phOsalNfc_PostTimerMsg(phLibNfc_Message_t* pMsg) {
  (void)phDal4Nfc_msgsnd(
      nxpncihal_ctrl.gDrvCfg
          .nClientId /*gpphOsalNfc_Context->dwCallbackThreadID*/,
      pMsg, 0);

  return;
}

/*******************************************************************************
**
** Function         phOsalNfc_Timer_Expired
**
** Description      posts message upon expiration of timer
**                  Shall be invoked when any one timer is expired
**                  Shall post message on user thread to invoke respective
**                  callback function provided by the caller of Timer function
**
** Returns          None
**
*******************************************************************************/
static void phOsalNfc_Timer_Expired(union sigval sv) {
  uint32_t dwIndex;
  phOsalNfc_TimerHandle_t* pTimerHandle;

  dwIndex = ((uint32_t)(sv.sival_int)) - PH_NFC_TIMER_BASE_ADDRESS - 0x01;
  pTimerHandle = (phOsalNfc_TimerHandle_t*)&apTimerInfo[dwIndex];
  /* Timer is stopped when callback function is invoked */
  pTimerHandle->eState = eTimerStopped;

  pTimerHandle->tDeferedCallInfo.pDeferedCall = &phOsalNfc_DeferredCall;
  pTimerHandle->tDeferedCallInfo.pParam = (void*)((intptr_t)(sv.sival_int));

  pTimerHandle->tOsalMessage.eMsgType = PH_LIBNFC_DEFERREDCALL_MSG;
  pTimerHandle->tOsalMessage.pMsgData = (void*)&pTimerHandle->tDeferedCallInfo;

  /* Post a message on the queue to invoke the function */
  phOsalNfc_PostTimerMsg((phLibNfc_Message_t*)&pTimerHandle->tOsalMessage);

  return;
}

/*******************************************************************************
**
** Function         phUtilNfc_CheckForAvailableTimer
**
** Description      Find an available timer id
**
** Parameters       void
**
** Returns          Available timer id
**
*******************************************************************************/
uint32_t phUtilNfc_CheckForAvailableTimer(void) {
  /* Variable used to store the index at which the object structure details
     can be stored. Initialize it as not available. */
  uint32_t dwIndex = 0x00;
  uint32_t dwRetval = 0x00;

  /* Check whether Timer object can be created */
  for (dwIndex = 0x00; ((dwIndex < PH_NFC_MAX_TIMER) && (0x00 == dwRetval));
       dwIndex++) {
    if (!(apTimerInfo[dwIndex].TimerId)) {
      dwRetval = (dwIndex + 0x01);
    }
  }

  return (dwRetval);
}

/*******************************************************************************
**
** Function         phOsalNfc_CheckTimerPresence
**
** Description      Checks the requested timer is present or not
**
** Parameters       pObjectHandle - timer context
**
** Returns          NFCSTATUS_SUCCESS if found
**                  Other value if not found
**
*******************************************************************************/
NFCSTATUS phOsalNfc_CheckTimerPresence(void* pObjectHandle) {
  uint32_t dwIndex;
  NFCSTATUS wRegisterStatus = NFCSTATUS_INVALID_PARAMETER;

  for (dwIndex = 0x00;
       ((dwIndex < PH_NFC_MAX_TIMER) && (wRegisterStatus != NFCSTATUS_SUCCESS));
       dwIndex++) {
    /* For Timer, check whether the requested handle is present or not */
    if (((&apTimerInfo[dwIndex]) == (phOsalNfc_TimerHandle_t*)pObjectHandle) &&
        (apTimerInfo[dwIndex].TimerId)) {
      wRegisterStatus = NFCSTATUS_SUCCESS;
    }
  }
  return wRegisterStatus;
}
