/******************************************************************************
* シリアル通信拡張用 ハンドラヘッダ                                           *
*                                                                             *
* File name : hdrsioEx.h                                                      *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _HDRSIOEX_H_
#define _HDRSIOEX_H_

// 割り込みサービスルーチン
extern void int_sio2_eri(INT portno);
extern void int_sio2_rxi(INT portno);


#endif // _HDRSIOEX_H_
