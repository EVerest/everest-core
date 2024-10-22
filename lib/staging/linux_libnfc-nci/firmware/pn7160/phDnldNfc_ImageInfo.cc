/******************************************************************************
*
* Copyright 2021 NXP.
* NXP Confidential. This software is owned or controlled by NXP and may only be
* used strictly in accordance with the applicable license terms. By expressly
* accepting such terms or by downloading, installing, activating and/or
* otherwise using the software, you are agreeing that you have read, and that
* you agree to comply with and are bound by, such license terms. If you do not
* agree to be bound by the applicable license terms, then you may not retain,
* install, activate or otherwise use the software.
*
******************************************************************************/
#include <phNfcTypes.h>
#include <phDnldNfc_ImageInfo.h>

/* PN547 New Mw Version to be UPDATED for every new Release made */

#define DLL_EXPORT
#ifdef ANDROID
uint8_t* gphDnldNfc_DlSeq  = &gphDnldNfc_DlSequence[0];
#else
DECLDIR uint8_t* gphDnldNfc_DlSeq  = &gphDnldNfc_DlSequence[0];
#endif
