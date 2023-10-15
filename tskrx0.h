/******************************************************************************
* ＰＣ通信受信タスク(SIO0)ヘッダ                                              *
*                                                                             *
* File name : tskrx0.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKRX0_H_
#define _TSKRX0_H_



/*************************************
 関数定義
 *************************************/
extern TASK	TskRx0( void );
extern ER	SendACK( void );
extern ER	SendNAK( void );
extern ER	Send001( void );//debug
extern ER	Send002( void );//debug
extern ER	Send003( void );//debug

extern void AlmComR0Dat( VP_INT exinf );

extern void rx_enable( void );
extern void rx_disable( void );

extern UB Get_pno();
extern UB Get_pcnt();
extern void Set_pno( UB pno );
extern void Set_pcnt( UB pcnt );

#endif  // _TSKRX0_H_
