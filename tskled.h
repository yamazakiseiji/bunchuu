/******************************************************************************
* LED�E�u�U�[����^�X�N                                                       *
*                                                                             *
* File name : tskled.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKLED_H_
#define _TSKLED_H_



/*************************************
 �֐���`
 *************************************/
extern TASK	TskLed( void );													// �h�n�Ď�
extern void AlternateOutProc();
extern BOOL AlternateOutDataSet( UB ch, UB ledpat, UB buzpat );

#endif  // _TSKLED_H_
