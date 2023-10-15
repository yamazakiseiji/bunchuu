/******************************************************************************
* シリアル通信拡張用 受信タスクヘッダ                                         *
*                                                                             *
* File name : tskrx2.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKRX2_H_
#define _TSKRX2_H_


// プロトタイプ
extern TASK TskRxEx( void );

extern ER   sio2_init( INT act ) ;												// シリアルポート２初期化
extern void sio2_set( volatile struct st_sci* comm );							// シリアル２通信設定
extern void AlmComR2Dat( VP_INT exinf );										// コマンドリクエスト受信監視用アラームハンドラ
extern BOOL GetPortActive( UB pno );											// ポート状態取得



#endif  // _TSKRX2_H_
