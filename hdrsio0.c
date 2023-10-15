/******************************************************************************
* ＰＣ通信ハンドラ(SIO0)                                                      *
*                                                                             *
* File name : hdrsio0.c                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "hdrsio0.h"
#include "datastruct.h"
#include "sub.h"

//static B dbgcnt;
//static B dbgbuf[32];

/*-----------------------------------------------------------------------------
  機能：受信エラー割り込みサービスルーチン
 ----------------------------------------------------------------------------*/
void int_sio0_eri(void)
{
	T_COM_CHAR  dat;

	dat.byte.type = COM_MTYP_ERR;
	if( SCI0.SCSSR.BIT.ORER )			// オーバーランエラー
	{
		dat.byte.type = COM_MTYP_ORN;
		SCI0.SCSSR.BIT.ORER = 0;
	}
	if( SCI0.SCSSR.BIT.FER )			// フレーミングエラー
	{
		dat.byte.type = COM_MTYP_FER;
		SCI0.SCSSR.BIT.FER = 0;
	}
	if( SCI0.SCSSR.BIT.PER )			// パリティーエラー
	{
		dat.byte.type = COM_MTYP_PER;
		SCI0.SCSSR.BIT.PER = 0;
	}
	// エラー送信
	dat.byte.data = SCI0.SCRDR;			// データは読み捨て
	SndDtq( ID_DTQ_RXD, dat.data );

	SCI0.SCSSR.BIT.RDRF = 0;
}

/*-----------------------------------------------------------------------------
  機能：受信データフル割り込みサービスルーチン
 ----------------------------------------------------------------------------*/
void int_sio0_rxi(void)
{
    T_COM_CHAR  dat;

    dat.byte.type = COM_MTYP_CHR;
    dat.byte.data = SCI0.SCRDR;
	
//	if( dbgcnt > 32 )	dbgcnt = 0;
//	dbgbuf[dbgcnt] = dat.byte.data;
//	dbgcnt++;
    SndDtq( ID_DTQ_RXD, dat.data );

    SCI0.SCSSR.BIT.RDRF = 0;

}

