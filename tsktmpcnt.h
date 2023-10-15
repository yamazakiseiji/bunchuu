/******************************************************************************
* 制御タスク                                                *
*                                                                             *
* File name : tsktmp.c                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKTMPCNT_H_
#define _TSKTMPCNT_H_


// タスク
extern TASK TskTmpCnt( void );

extern void TmpDataSet( unsigned char tmpdat );
extern unsigned char TmpDataGet();

#endif // _TSKMOTWAIT_H_
