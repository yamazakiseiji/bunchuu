/******************************************************************************
* ＰＣ通信送信タスク(SIO0)                                                    *
*                                                                             *
* File name : tsktx0.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "sub.h"
#include "machine.h"//debug
#include "tskcmd.h"
#include "tblchr.h"


//*************************************
// 関数定義
//*************************************
static void Tx0Proc(T_CMD_MSG* mrcv);
static void Tx0Send( UB senddata );
static void Tx0Init();
static void Tx0Proc2();
static void Tx0RunLSend();


//*************************************
// テーブル
//*************************************


//*************************************
// データ定義
//*************************************
static UB			Sendbuff[640];							// 送信バッファ
static UB			wkbuff[512];							// 送信バッファ

/******************************************************************************
* 
* [概要]	ＰＣ通信送信タスク
* 			装置からＰＣへデータ送信を行う
* 
* 
******************************************************************************/
TASK TskTx0( void )
{
	T_CMD_MSG*  msg;
	ER          err;
	UB          msgid;
	INT         siz;

	Tx0Init();

	for( ;; )
	{
		// 送信メッセージの受信
		err = rcv_mbx( ID_MBX_TX0, (T_MSG**)&msg );
		if( E_OK != err )
		{
			continue;
		}

		msgid = msg->msgdata.msgid;
		
		switch( msgid )
		{
			case ID_MSG_VALVEINI_HS:
			case ID_MSG_PRESSINI_HS:
			case ID_MSG_PRESSREN_HS:
			case ID_MSG_PRESSTAN_HS:
		    case ID_MSG_PRESSCAN_HS:
		    case ID_MSG_VALVEOUT_HS:
		    case ID_MSG_PRESSURE_HS:
				siz = GetPsnsData(&Sendbuff[0]);
				Tx0Proc2( siz );
				break;
			
		    case ID_MSG_EXPRUNL:
				Tx0RunLSend();
				break;
			
			default:
				Tx0Proc( msg );
				break;
		}

		// メモリ解放
		RelMpf( ID_MPF_TX0, (VP)msg );
	}
}
/******************************************************************************
* 
* [概要]	
* 
* 
* 
******************************************************************************/
static void Tx0RunLSend()
{
	INT siz;
	INT len;
	UB  bcc;
	UB  bcch;
	UB  bccl;

	memset( &wkbuff[0],    CLR,  sizeof( wkbuff ) );
	memset( &Sendbuff[0],  CLR,  sizeof( Sendbuff ) );
	
	GetRunData(&wkbuff[0]);
	sprintf( &Sendbuff[1], "%s,%04X,%s" , STRTBL_MSG[ID_MSG_EXPRUNL], E_SUCCESS, &wkbuff[8] );
	Sendbuff[0] = STX;
	
	len = strlen(&Sendbuff);

	Sendbuff[len] = ETX;

	bcc = CalcBCC( &Sendbuff[1], len);				// BCC計算
	DectoHexASC( bcc, &bcch, &bccl );				// 10進数→16進数→ASC変換

	Sendbuff[(len+1)] = bcch;						// BCC1
	Sendbuff[(len+2)] = bccl;						// BCC2
	
	siz = len+4;
	
	Tx0Proc2( siz );
}

/******************************************************************************
* 
* [概要]	ＰＣ通信送信
* 
* 
* 
******************************************************************************/
static void Tx0Init()
{
}

/******************************************************************************
* 
* [概要]	PC通信送信処理
* [引数]	メッセージデータ
* [戻値]	なし
* 
******************************************************************************/
static void Tx0Proc(T_CMD_MSG* mrcv)
{
	INT i;
	INT siz;
	UB  msg;
	UB  bcc;
	UB  bcch;
	UB  bccl;

	msg = mrcv->msgdata.msgid;
	switch(msg)
	{
		case ID_MSG_CMD:												// コマンドレスポンス
		case ID_MSG_EEPREAD:											// 
		case ID_MSG_RUNLOGERR:
			siz = mrcv->msgdata.siz;									// サイズ取得
			for( i = 0; i < siz; i++ )									// サイズ分送信
			{
				Tx0Send( mrcv->msgdata.data[i] );
			}
			break;
		
		case ID_MSG_EEPWRIT:											// 
			break;

		case ID_MSG_ACK:												// ＡＣＫレスポンス
			Tx0Send( ACK );
			break;
		
		case ID_MSG_NAK:												// ＮＡＫレスポンス
		default:														// 規定外
			Tx0Send( NAK );
			break;
	}
}

/******************************************************************************
* 
* [概要]	PC通信送信処理
* [引数]	送信サイズ
* [戻値]	なし
* 
******************************************************************************/
static void Tx0Proc2( INT siz )
{
	INT			i;
	
	for( i = 0; i < siz; i++ )									// サイズ分送信
	{
		Tx0Send( Sendbuff[i] );
	}
}
/******************************************************************************
* 
* [概要]	データ送信
* [引数]	送信データ
* [戻値]	なし
* 
******************************************************************************/
static void Tx0Send( UB senddata )
{
   int save_i;

    // 割込み禁止処理
//x    save_i = get_imask();
//x	set_imask(0x0f);

//	rx_disable();

	while( 0 == SCI0.SCSSR.BIT.TDRE )
	{
		sio_CheckError( &SCI0 );
	}
	SCI0.SCTDR = senddata;
	SCI0.SCSSR.BIT.TDRE = 0;

	rot_rdq( TPRI_SELF );

//	rx_enable();
//x	set_imask(save_i);
}




