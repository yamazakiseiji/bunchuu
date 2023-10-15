/******************************************************************************
* モータウェイト制御タスクヘッダ                                              *
*                                                                             *
* File name : tskmotwait.c                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKMOTEND_H_
#define _TSKMOTEND_H_


// タスク
extern TASK TskMotEnd( void );
extern UB GetMotWait( UB mot  );	// モータウェイト状態取得


#endif // _TSKMOTEND_H_
