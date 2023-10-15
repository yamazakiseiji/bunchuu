/******************************************************************************
* 周期ハンドラ                                                                *
*                                                                             *
* File name : hdrcyc.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _HDRCYC_H_
#define _HDRCYC_H_

extern void HdrCyc( VP_INT exinf );							// 周期ハンドラ
//extern void AlternateOutProc();								// 交互出力処理
//extern BOOL AlternateOutSet( UB ch, UB ledpat, UB buzpat );	// 交互出力設定
//extern BOOL AlternateOutGet( UB ch, BLINK_CTRL* blkdat);	// 交互出力取得

extern void CycInit();
extern void SetDpumpDuty( UB dat );
extern void DpumpStart();
extern void DpumpStop();

#endif  // _HDRCYC_H_

