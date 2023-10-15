/******************************************************************************
* LED・ブザー制御タスク                                                       *
*                                                                             *
* File name : tskled.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKLED_H_
#define _TSKLED_H_



/*************************************
 関数定義
 *************************************/
extern TASK	TskLed( void );													// ＩＯ監視
extern void AlternateOutProc();
extern BOOL AlternateOutDataSet( UB ch, UB ledpat, UB buzpat );

#endif  // _TSKLED_H_
