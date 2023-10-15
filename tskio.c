/******************************************************************************
* 入出力監視タスク                                                            *
*                                                                             *
* File name : tskio.c                                                         *
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
#include "tskio.h"
#include "automachine.h"


//*************************************
// 関数定義
//*************************************
static void CountSet(T_CMD_MSG* mrcv);
static void CountProc();


//*************************************
// テーブル
//*************************************


//*************************************
// データ定義
//*************************************
// カウント監視
static	ST_IOCOUNT			iocnt[MAX_CH+1];// 2020/2/12 １はじまり対応



/***********************************************************************
* ｘｘｘ
* 
* [概要]
* ｘｘｘ
* 
* 
***********************************************************************/
TASK TskIo( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	memset( &iocnt,  CLR,  sizeof( iocnt ) );

	for( ;; )
    {
        // コマンド待ち
        err = trcv_mbx( ID_MBX_IO, (T_MSG**)&msg, DLY_1M );

    	if( err == E_TMOUT )
		{
			// カウント処理
	    	CountProc(msg);
			continue;
		}
    	else
    	{
			// メッセージ処理
	    	CountSet(msg);
    	}
    	
		RelMpf( ID_MPF_IO, (VP)msg );// メモリ解放
    }
}

/***********************************************************************
* 
* [概要]	カウント設定処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void CountSet(T_CMD_MSG* mrcv)
{
	UB		msg;
	UB		ch;

	msg = mrcv->msgdata.msgid;
	ch  = mrcv->msgdata.inf;

	if( msg == ID_MSG_CNT_START )
	{
		iocnt[ch].sts   = SET;
		iocnt[ch].cnt   = CLR;

		iocnt[ch].now = GetInputDat(ch);
		iocnt[ch].old = iocnt[ch].now;
	}
	
	if( msg == ID_MSG_CNT_STOP )
	{
		iocnt[ch].sts   = CLR;
	}
}

/***********************************************************************
* 
* [概要]	カウント設定処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void CountProc()
{
	INT		Loop;

// 負荷が大きいので実装入力ＣＨのみ、監視
//	for( Loop = 0; Loop < MAX_CH; Loop++ )
	for( Loop = MIN_CH; Loop <= INMAX; Loop++ )
	{
		// カウント対象？
		if( iocnt[Loop].sts == SET )
		{
			iocnt[Loop].old = iocnt[Loop].now;
			iocnt[Loop].now = GetInputDat(Loop);

			// Ｌ→Ｈ（前回Ｌ、現在Ｈ）
			if( (!iocnt[Loop].old)&&( iocnt[Loop].now ) )
			{
				iocnt[Loop].cnt++;
			}
		}
	}
}

/***********************************************************************
* 
* [概要]	カウント数取得
* [引数]	ch:入力ch
* [戻値]	－
* 
***********************************************************************/
UINT GetCount(UB ch)
{
	UINT	count = CLR;

	if(( ch >= MIN_CH )&&( ch <= MAX_CH )) 	// 2020/2/12 １はじまり対応
	{
		count = iocnt[ch].cnt;
	}
	
	return( count );
}


