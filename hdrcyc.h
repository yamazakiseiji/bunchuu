/******************************************************************************
* �����n���h��                                                                *
*                                                                             *
* File name : hdrcyc.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _HDRCYC_H_
#define _HDRCYC_H_

extern void HdrCyc( VP_INT exinf );							// �����n���h��
//extern void AlternateOutProc();								// ���ݏo�͏���
//extern BOOL AlternateOutSet( UB ch, UB ledpat, UB buzpat );	// ���ݏo�͐ݒ�
//extern BOOL AlternateOutGet( UB ch, BLINK_CTRL* blkdat);	// ���ݏo�͎擾

extern void CycInit();
extern void SetDpumpDuty( UB dat );
extern void DpumpStart();
extern void DpumpStop();

#endif  // _HDRCYC_H_

