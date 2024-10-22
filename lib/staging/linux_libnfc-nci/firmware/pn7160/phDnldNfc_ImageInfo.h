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
#ifndef PHDNLDNFC_IMAGEINFO_H /* */
#define PHDNLDNFC_IMAGEINFO_H/* */

#define ANDROID
#ifndef ANDROID
#define DECLDIR __declspec(dllexport)
#endif

/* PN547 Download Write Frames sequence */
extern uint8_t gphDnldNfc_DlSequence[];

#ifdef ANDROID
extern uint8_t*  gphDnldNfc_DlSeq;
extern uint16_t gphDnldNfc_DlSeqSz;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ANDROID
DECLDIR uint8_t*  gphDnldNfc_DlSeq;
DECLDIR uint16_t gphDnldNfc_DlSeqSz;
#endif
#ifdef __cplusplus
}
#endif

#endif /* PHDNLDNFC_IMAGEINFO_H */
