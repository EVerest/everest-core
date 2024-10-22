/*
* Copyright (C) 2012-2014 NXP Semiconductors
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
*/#include <log/log.h>

#include <phNxpConfig.h>
#include <phNxpLog.h>
#include <phNxpNciHal_dta.h>

/*********************** Global Variables *************************************/
static phNxpDta_Control_t nxpdta_ctrl = {0, 0, 0};
extern bool nfc_debug_enabled;
/*******************************************************************************
**
** Function         phNxpEnable_DtaMode
**
** Description      This function configures
**                  HAL in DTA mode
**
*******************************************************************************/
void phNxpEnable_DtaMode(uint16_t pattern_no) {
  nxpdta_ctrl.dta_ctrl_flag = false;
  nxpdta_ctrl.dta_t1t_flag = false;
  nxpdta_ctrl.dta_pattern_no = pattern_no;
  NXPLOG_NCIHAL_D(">>>>DTA - Mode is enabled");
  nxpdta_ctrl.dta_ctrl_flag = true;
}

/*******************************************************************************
**
** Function         phNxpDisable_DtaMode
**
** Description      This function disable DTA mode
**
*******************************************************************************/
void phNxpDisable_DtaMode(void) {
  nxpdta_ctrl.dta_ctrl_flag = false;
  nxpdta_ctrl.dta_t1t_flag = false;
  NXPLOG_NCIHAL_D(">>>>DTA - Mode is Disabled");
}

/******************************************************************************
 * Function         phNxpDta_IsEnable
 *
 * Description      This function checks the DTA mode is enable or not.
 *
 * Returns          It returns TRUE if DTA enabled otherwise FALSE
 *
 ******************************************************************************/
NFCSTATUS phNxpDta_IsEnable(void) { return nxpdta_ctrl.dta_ctrl_flag; }

/******************************************************************************
 * Function         phNxpDta_T1TEnable
 *
 * Description      This function  enables  DTA mode for T1T tag.
 *
 *
 ******************************************************************************/
void phNxpDta_T1TEnable(void) { nxpdta_ctrl.dta_t1t_flag = true; }
/******************************************************************************
 * Function         phNxpNHal_DtaUpdate
 *
 * Description      This function changes the command and responses specific
 *                  to make DTA application success
 *
 * Returns          It return NFCSTATUS_SUCCESS then continue with send else
 *                  sends NFCSTATUS_FAILED direct response is prepared and
 *                  do not send anything to NFCC.
 *
 ******************************************************************************/

NFCSTATUS phNxpNHal_DtaUpdate(uint16_t* cmd_len, uint8_t* p_cmd_data,
                              uint16_t* rsp_len, uint8_t* p_rsp_data) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if (nxpdta_ctrl.dta_ctrl_flag == true) {
    // Workaround for DTA, block the set config command with general bytes */
    if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
        p_cmd_data[2] == 0x17 && p_cmd_data[3] == 0x01 &&
        p_cmd_data[4] == 0x29 && p_cmd_data[5] == 0x14) {
      *rsp_len = 5;
      NXPLOG_NCIHAL_D(">>>>DTA - Block set config command");
      phNxpNciHal_print_packet("DTASEND", p_cmd_data, *cmd_len);

      p_rsp_data[0] = 0x40;
      p_rsp_data[1] = 0x02;
      p_rsp_data[2] = 0x02;
      p_rsp_data[3] = 0x00;
      p_rsp_data[4] = 0x00;

      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 5);

      status = NFCSTATUS_FAILED;
      NXPLOG_NCIHAL_D(
          "Going through DTA workaround - Block set config command END");

    } else if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x08 &&
               p_cmd_data[2] == 0x04 && p_cmd_data[3] == 0xFF &&
               p_cmd_data[4] == 0xFF) {
      NXPLOG_NCIHAL_D(">>>>DTA Change Felica system code");
      *rsp_len = 4;
      p_rsp_data[0] = 0x41;
      p_rsp_data[1] = 0x08;
      p_rsp_data[2] = 0x01;
      p_rsp_data[3] = 0x00;
      status = NFCSTATUS_FAILED;

      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 4);
    } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
               p_cmd_data[2] == 0x10 && p_cmd_data[3] == 0x05 &&
               p_cmd_data[10] == 0x32 && p_cmd_data[12] == 0x00) {
      NXPLOG_NCIHAL_D(">>>>DTA Update LA_SEL_INFO param");

      p_cmd_data[12] = 0x40;
      p_cmd_data[18] = 0x02;
      status = NFCSTATUS_SUCCESS;
    } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
               p_cmd_data[2] == 0x0D && p_cmd_data[3] == 0x04 &&
               p_cmd_data[10] == 0x32 && p_cmd_data[12] == 0x00) {
      NXPLOG_NCIHAL_D(">>>>DTA Blocking dirty set config");
      *rsp_len = 5;
      p_rsp_data[0] = 0x40;
      p_rsp_data[1] = 0x02;
      p_rsp_data[2] = 0x02;
      p_rsp_data[3] = 0x00;
      p_rsp_data[4] = 0x00;
      status = NFCSTATUS_FAILED;
      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 5);
    } else if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x03) {
      NXPLOG_NCIHAL_D(">>>>DTA Add NFC-F listen tech params");
      p_cmd_data[2] += 6;
      p_cmd_data[3] += 3;
      p_cmd_data[*cmd_len] = 0x80;
      p_cmd_data[*cmd_len + 1] = 0x01;
      p_cmd_data[*cmd_len + 2] = 0x82;
      p_cmd_data[*cmd_len + 3] = 0x01;
      p_cmd_data[*cmd_len + 4] = 0x85;
      p_cmd_data[*cmd_len + 5] = 0x01;

      *cmd_len += 6;
      status = NFCSTATUS_SUCCESS;
    } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
               p_cmd_data[2] == 0x0D && p_cmd_data[3] == 0x04 &&
               p_cmd_data[10] == 0x32 && p_cmd_data[12] == 0x20 &&
               nxpdta_ctrl.dta_pattern_no == 0x1000) {
      NXPLOG_NCIHAL_D(">>>>DTA Blocking dirty set config for analog testing");
      *rsp_len = 5;
      p_rsp_data[0] = 0x40;
      p_rsp_data[1] = 0x02;
      p_rsp_data[2] = 0x02;
      p_rsp_data[3] = 0x00;
      p_rsp_data[4] = 0x00;
      status = NFCSTATUS_FAILED;
      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 5);
    } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
               p_cmd_data[2] == 0x0D && p_cmd_data[3] == 0x04 &&
               p_cmd_data[4] == 0x32 && p_cmd_data[5] == 0x01 &&
               p_cmd_data[6] == 0x00) {
      NXPLOG_NCIHAL_D(">>>>DTA Blocking dirty set config");
      *rsp_len = 5;
      p_rsp_data[0] = 0x40;
      p_rsp_data[1] = 0x02;
      p_rsp_data[2] = 0x02;
      p_rsp_data[3] = 0x00;
      p_rsp_data[4] = 0x00;
      status = NFCSTATUS_FAILED;
      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 5);
    } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
               p_cmd_data[2] == 0x04 && p_cmd_data[3] == 0x01 &&
               p_cmd_data[4] == 0x50 && p_cmd_data[5] == 0x01 &&
               p_cmd_data[6] == 0x00 && nxpdta_ctrl.dta_pattern_no == 0x1000) {
      NXPLOG_NCIHAL_D(">>>>DTA Blocking dirty set config for analog testing");
      *rsp_len = 5;
      p_rsp_data[0] = 0x40;
      p_rsp_data[1] = 0x02;
      p_rsp_data[2] = 0x02;
      p_rsp_data[3] = 0x00;
      p_rsp_data[4] = 0x00;
      status = NFCSTATUS_FAILED;
      phNxpNciHal_print_packet("DTARECV", p_rsp_data, 5);
    } else {
    }
    if (nxpdta_ctrl.dta_t1t_flag == true) {
      if (p_cmd_data[2] == 0x07 && p_cmd_data[3] == 0x78 &&
          p_cmd_data[4] == 0x00 && p_cmd_data[5] == 0x00) {
        /*if (nxpdta_ctrl.dta_pattern_no == 0)
        {
          NXPLOG_NCIHAL_D(">>>>DTA - T1T modification block RID command Custom
        Response (pattern 0)");
          phNxpNciHal_print_packet("DTASEND", p_cmd_data, *cmd_len);
          *rsp_len = 10;
          p_rsp_data[0] = 0x00;
          p_rsp_data[1] = 0x00;
          p_rsp_data[2] = 0x07;
          p_rsp_data[3] = 0x12;
          p_rsp_data[4] = 0x49;
          p_rsp_data[5] = 0x00;
          p_rsp_data[6] = 0x00;
          p_rsp_data[7] = 0x00;
          p_rsp_data[8] = 0x00;
          p_rsp_data[9] = 0x00;

          status = NFCSTATUS_FAILED;

          phNxpNciHal_print_packet("DTARECV", p_rsp_data, *rsp_len);
        }
        else
        {*/
        NXPLOG_NCIHAL_D("Change RID command's UID echo bytes to 0");

        nxpdta_ctrl.dta_t1t_flag = false;
        p_cmd_data[6] = 0x00;
        p_cmd_data[7] = 0x00;
        p_cmd_data[8] = 0x00;
        p_cmd_data[9] = 0x00;
        status = NFCSTATUS_SUCCESS;
        /*}*/
      }
    }
  }
  return status;
}
