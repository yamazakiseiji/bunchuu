/******************************************************************************
* シリアル通信拡張用 送信タスク                                               *
*                                                                             *
* File name : tsktx2.c                                                        *
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
#include "tskcmd.h"

/***********************************************************************
*  関数定義
***********************************************************************/
static void Tx2Proc(T_CMD_MSG* mrcv);									// 拡張シリアル送信処理
static void Tx2Send( UB senddata );										// シリアル２データ送信


/***********************************************************************
* データ定義
***********************************************************************/




/******************************************************************************
* 拡張シリアル通信タスク
* 
* [概要]
* 拡張シリアル通信にてデータ送信を行う
* 
* 
******************************************************************************/
TASK TskTxEx( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	for( ;; )
	{
		// 送信メッセージの受信
		err = rcv_mbx( ID_TSK_TXEX, (T_MSG**)&msg );
		if( E_OK != err )
		{
			continue;
		}
		Tx2Proc(msg);

		// メモリ解放
		RelMpf( ID_MPF_TXEX, (VP)msg );
	}
}

/******************************************************************************
* 
* [概要]	拡張シリアル２通信送信処理
* 
* 
* 
******************************************************************************/
static void Tx2Proc( T_CMD_MSG* mrcv )
{
	UB			msg;
	INT			i;
	INT			siz;

	msg = mrcv->msgdata.msgid;
	
	switch(msg)
	{
		case ID_MSG_CMD:												// コマンドレスポンス
			siz = mrcv->msgdata.siz;									// サイズ取得
			for( i = 0; i < siz; i++ )									// サイズ分送信
			{
				Tx2Send( mrcv->msgdata.data[i] );
			}
			break;
		
		case ID_MSG_IMPPDAT:
		case ID_MSG_IMPSPED:
		case ID_MSG_IMPPOSI:
		case ID_MSG_IMPOPTN:
		case ID_MSG_EXPRUNL:
		case ID_MSG_EXPCOML:
			siz = mrcv->msgdata.siz;									// サイズ取得
			
			for( i = 0; i < siz; i++ )									// サイズ分送信
			{
				Tx2Send( MentePcSendbuf[i] );
			}
			break;
		
		case ID_MSG_ACK:												// ＡＣＫレスポンス
			Tx2Send( ACK );
			break;

		case ID_MSG_NAK:												// ＮＡＫレスポンス
		default:														// 規定外
			Tx2Send( NAK );
			break;
	}
}

/******************************************************************************
* 
* [概要]	シリアル２データ送信
* 
* 
* 
******************************************************************************/
static void Tx2Send( UB senddata )
{
	while( 0 == SCI2.SCSSR.BIT.TDRE )
	{
		sio_CheckError( &SCI2 );
	}
	SCI2.SCTDR = senddata;
	SCI2.SCSSR.BIT.TDRE = 0;

	rot_rdq( TPRI_SELF );
}

