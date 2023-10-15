/******************************************************************************
* モータウェイト制御タスクヘッダ                                              *
*                                                                             *
* File name : tskmotwait.c                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKMOTWAIT_H_
#define _TSKMOTWAIT_H_


// タスク
extern TASK TskMotWait( void );
extern UB GetMotWait( UB mot  );	// モータウェイト状態取得


#endif // _TSKMOTWAIT_H_
