/******************************************************************************
* モータウェイト制御タスク                                                    *
*                                                                             *
* File name : tskmotwait.c                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "tskmotwait.h"
#include "tskcmd.h"
#include "sub.h"
#include "automachine.h"


//*************************************
// 関数定義
//*************************************

static  void MotWaitInit();												// ウェイト要求処理
static  void MotWaitChk();												// 
static  void MotWaitProc   ( T_CMD_MSG* mrcv );							// ウェイト要求処理
static  void CmdRcvMsg     ( T_CMD_MSG* mrcv );							// メッセージ受信処理
static  ER   MotWaitAnalyze( T_CMD_MSG* mrcv );							// コマンドの解析
static  ER   MotWaitDummy     ( T_CMD_MSG* mrcv );						// 
static  ER   MotWaitRequest   ( T_CMD_MSG* mrcv );						// モータ動作前のウェイトリクエスト
static  ER   MotWaitRequestW  ( T_CMD_MSG* mrcv );						// モータ動作前のウェイトリクエスト
static  ER   MotWaitPDELAY    ( T_CMD_MSG* mrcv );						// PDELAY  パルスモータのパワー制御待ち時間設定
static  ER   MotWaitTimeRequest( T_CMD_MSG* mrcv );
static void MotWaitTimeAllInit();
static void MotWaitTimeInit(UB bcid);


//*************************************
// テーブル
//*************************************



//*************************************
// データ定義
//*************************************
static	ST_MOTWAIT			MotWait[(DVMAX+1)];							// ウェイト制御データ
static	ST_MOTWAIT			WaitTime[BCCTRL_MAX];							// ウェイト制御データ
//static	INT WaitTime[BCCTRL_MAX];										// 指定時間ＷＡＩＴ用
static	INT IntervalCounter;
static	UB  inf2_dbg;

/******************************************************************************
* 
* [概要]	モータ制御タスク
* [引数]	
* [戻値]	
* 
******************************************************************************/
TASK TskMotWait( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotWaitInit();
	MotWaitTimeAllInit();
	
	for( ;; )
	{
        // コマンド待ち
        err = trcv_mbx( ID_MBX_MOTWAIT, (T_MSG**)&msg, DLY_10M );

		if( err == E_TMOUT )
		{
			MotWaitChk();
			continue;
		}
		else
		{
			MotWaitProc(msg);
		}
		// メモリ解放
		RelMpf( ID_MPF_MOTWAIT, (VP)msg );
	}
}
/******************************************************************************
* 
* [概要]	モータウェイト初期化
* [引数]	
* [戻値]	
* 
******************************************************************************/
static void MotWaitInit()
{
	INT Loop;

	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		
		MotWait[Loop].req = CLR;			// default
		MotWait[Loop].tskid = DEFDAT;		// default
		MotWait[Loop].prid1 = DEFDAT;		// default
		MotWait[Loop].bcid  = DEFDAT;		// default
		MotWait[Loop].msgid = DEFDAT;		// default
		MotWait[Loop].tim = 100/DLY_10M;	// default
		MotWait[Loop].cnt = CLR;			// default

	}
	IntervalCounter = 0;
}
static void MotWaitTimeAllInit()
{
	INT Loop;
	
	for( Loop = 0; Loop < BCCTRL_MAX; Loop++ )
	{
		MotWaitTimeInit(Loop);
	}
	
	IntervalCounter = 0;
}
static void MotWaitTimeInit(UB bcid)
{
	WaitTime[bcid].req = CLR;			// default
	WaitTime[bcid].tskid = DEFDAT;		// default
	WaitTime[bcid].prid1 = DEFDAT;		// default
	WaitTime[bcid].bcid  = DEFDAT;		// default
	WaitTime[bcid].msgid = DEFDAT;		// default
	WaitTime[bcid].tim = CLR;			// default
	WaitTime[bcid].cnt = CLR;			// default
}

/***********************************************************************
* 
* [概要]	ウェイト監視
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void MotWaitChk()
{
	UB Loop;
	ER result;
	ID mpfid;
	ID mbxid;

	// ＤＥＬＡＹ時間経過したかチェック
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		if( MotWait[Loop].req == SET )
		{
			MotWait[Loop].cnt --;										// デクリメント

			if( MotWait[Loop].cnt <= 0 )
			{
inf2_dbg = MotWait[Loop].motpare;
				// 動作指示
				result = mbx_send(	MotWait[Loop].tskid,				//  tskid
							ID_MPF_MOTACT,								//  mpfid
							ID_MBX_MOTACT,								//  mbxid
							MotWait[Loop].prid1,						//  prid1
							MotWait[Loop].bcid,							//  bcid
							MotWait[Loop].msgid,						//  msgid
							Loop,										//  sts
							MotWait[Loop].motpare,						//  sts2
							&MotWait[Loop].msgdat[0],					//  data
							CMD_BUFSIZ                                  //  siz
						);
				// 監視データクリア
				MotWait[Loop].req = CLR;			// default
				MotWait[Loop].cnt = CLR;			// default
				MotWait[Loop].msgid = DEFDAT;		// default
				MotWait[Loop].tim = 100/DLY_10M;		// default
			}
		}
	}
	// 指定時間待ったかチェック
	for( Loop = 0; Loop < BCCTRL_MAX; Loop++ )
	{
		// 待ち時間あり？
		if( WaitTime[Loop].tim )
		{
			WaitTime[Loop].tim--;

			// 待ち終了
			if( WaitTime[Loop].tim == CLR )
			{	
				if( WaitTime[Loop].tskid == ID_TSK_TMPCNT )
				{
					mpfid = ID_MPF_TMPCNT;
					mbxid = ID_MBX_TMPCNT;
				}
				else
				{
					mpfid = ID_MPF_CMD;
					mbxid = ID_MBX_CMD;
				}
				
				// メッセージ通知
				result = mbx_send(
									ID_TSK_MOTWAIT,
									mpfid,
									mbxid,
									WaitTime[Loop].prid1,
									Loop,
									ID_MSG_TIMWAIT,
									NO_DATA,
									NO_DATA,
									&Loop,
									sizeof( Loop )
								);
				MotWaitTimeInit(Loop);
			}
		}
	}
}

/***********************************************************************
* 
* [概要]	ウェイト要求処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void MotWaitProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_CMD:
	case ID_TSK_INT:
	case ID_TSK_TMPCNT:
		CmdRcvMsg(mrcv);
		break;
	case ID_TSK_RX0:	// TSK_RX0->
	case ID_TSK_TX0:	// TSK_TX0->
	case ID_TSK_RXEX:	// TSK_RX2->
	case ID_TSK_TXEX:
	case ID_TSK_EEP:	// TSK_EEP->
	default:
		break;
	}
}
/***********************************************************************
* 
* [概要]	メッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void CmdRcvMsg( T_CMD_MSG* mrcv )
{
	ER			err;

	err = MotWaitAnalyze( mrcv );
}
/***********************************************************************
* 
* [概要]	コマンドの解析
* [引数]	data:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static ER MotWaitAnalyze( T_CMD_MSG* mrcv )
{
	ER			err = E_SUCCESS;
	UB			msgid;

	// メッセージＩＤ取得
	msgid = mrcv->msgdata.msgid;

	if(
		(( msgid >= ID_MSG_HELLO ) && ( msgid <= ID_MSG_BASE_CMD_MAX))||
		( msgid == ID_MSG_TIMWAIT)
	)
	{
		// コールファンクション
		switch( msgid )
		{
			case ID_MSG_ABSMV :
			case ID_MSG_ABORT :
			case ID_MSG_LIMIT :
			case ID_MSG_RELMV :
			case ID_MSG_STOP  :
				MotWaitRequest( mrcv );
				break;
			case ID_MSG_PDELAY:
				MotWaitPDELAY( mrcv );
				break;
			case ID_MSG_MAP   :
				MotWaitRequestW( mrcv );
				break;
			case ID_MSG_TIMWAIT :
				MotWaitTimeRequest( mrcv );
				break;
			default:
				break;
		}
		
	}

	return( err );
}

/***********************************************************************
* 
* [概要]	ダミールーチン
* [引数]	msg:メッセージID、mm1:モータ、mm2:モータ、data:メッセージデータ
* [戻値]	エラーコード
* 
***********************************************************************/
static ER MotWaitDummy( T_CMD_MSG* mrcv )
{
	// 必要であればエラー出力
	return(E_SYS_FNCCAL);
}

/***********************************************************************
* 
* [概要]	モータ動作前のウェイトリクエスト
* [引数]	msg:メッセージID、mm1:モータ、mm2:モータ、data:メッセージデータ
* [戻値]	エラーコード
* 
***********************************************************************/
static ER MotWaitRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm;

	// １軸目
	mm = mrcv->msgdata.inf;												// モータ取得
	MotWait[mm].tskid = mrcv->msgdata.tskid;							// タスクＩＤセット
	MotWait[mm].prid1 = mrcv->msgdata.prid1;							// プロセスＩＤ
	MotWait[mm].bcid  = mrcv->msgdata.bcid;								// ＢＣＩＤセット
	MotWait[mm].msgid = mrcv->msgdata.msgid;							// メッセージＩＤセット
	MotWait[mm].req   = SET;												// ディレイ開始
	MotWait[mm].cnt   = MotWait[mm].tim;								// ディレイ時間設定
	MotWait[mm].motpare = mrcv->msgdata.inf2;							// モータペアセット
	memcpy( &MotWait[mm].msgdat[0],&mrcv->msgdata.data[0],CMD_BUFSIZ );	// データコピー

inf2_dbg = mrcv->msgdata.inf2;

	motor_POWER( mm, SET );	

	// １軸目
	if( MotWait[mm].tim == 0 )
	{
		// 動作指示
		mbx_send(	MotWait[mm].tskid,									// tskid
					ID_MPF_MOTACT,										// mpfid
					ID_MBX_MOTACT,										// mbxid
					MotWait[mm].prid1,									// prid1
					MotWait[mm].bcid,									// bcid
					MotWait[mm].msgid,									// msgid
					mm,													// sts
					MotWait[mm].motpare,								// sts2
					&MotWait[mm].msgdat[0],								// data
					CMD_BUFSIZ                                          // siz
				);
	}
	return(result);
}

/***********************************************************************
* 
* [概要]	モータ動作前のウェイトリクエスト
* [引数]	msg:メッセージID、mm1:モータ、mm2:モータ、data:メッセージデータ
* [戻値]	エラーコード
* 
***********************************************************************/
static ER MotWaitRequestW( T_CMD_MSG* mrcv )
{
	UB		mm;
	UB		mm2;
	ER		result = E_SUCCESS;
	
	// １軸目
	mm = mrcv->msgdata.inf;												// モータ取得
	mm2 = mrcv->msgdata.inf2;											// モータ取得
	MotWait[mm].tskid = mrcv->msgdata.tskid;							// タスクＩＤセット
//	MotWait[mm].tskid = ID_TSK_MOTWAIT;									// タスクＩＤ
	MotWait[mm].prid1 = mrcv->msgdata.prid1;							// プロセスＩＤ
	MotWait[mm].bcid = mrcv->msgdata.bcid;								// ＢＣＩＤ
	MotWait[mm].msgid = mrcv->msgdata.msgid;							// メッセージＩＤ
	MotWait[mm].req = SET;												// ディレイ開始
	MotWait[mm].cnt = MotWait[mm].tim;									// ディレイ時間設定
	MotWait[mm].motpare = mm2;											// モータペアセット
	memcpy( &MotWait[mm].msgdat[0],&mrcv->msgdata.data[0],CMD_BUFSIZ );	// データコピー

	motor_POWER( mm, SET );

	// WAIT無し
	if( MotWait[mm].tim == 0 )
	{
		// 動作指示
		mbx_send(	MotWait[mm].tskid,									// tskid
					ID_MPF_MOTACT,										// mpfid
					ID_MBX_MOTACT,										// mbxid
					MotWait[mm].prid1,									// prid1
					MotWait[mm].bcid,									// bcid
					MotWait[mm].msgid,									// msgid
					mm,													// sts
					mm2,												// sts2
					&MotWait[mm].msgdat[0],								// data
					CMD_BUFSIZ											// siz
				);
	}
	return(result);
}

/***********************************************************************
* 
* [概要]	指定時間ＷＡＩＴ
* [引数]	mrcv:メッセージデータ
* [戻値]	エラーコード
* 
***********************************************************************/
static ER MotWaitTimeRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		bcid;
	UB		prid1;
	UB		msgid;
	INT		waittim = CLR;
	ID		mpfid;
	ID		mbxid;
	
	bcid = mrcv->msgdata.bcid;											// ＢＣＩＤセット

	if( bcid < BCCTRL_MAX )
	{
		prid1 = mrcv->msgdata.prid1;									// 
		msgid = mrcv->msgdata.msgid;
		// 指定ＷＡＩＴ時間取得
		memcpy( &waittim, &mrcv->msgdata.data[0],sizeof( INT ) );
		
		// WAIT無し
		if( waittim == 0 )
		{
			if( mrcv->msgdata.tskid == ID_TSK_TMPCNT )
			{
				mpfid = ID_MPF_TMPCNT;
				mbxid = ID_MBX_TMPCNT;
			}
			else
			{
				mpfid = ID_MPF_CMD;
				mbxid = ID_MBX_CMD;
			}
			
			// メッセージ通知
			result = mbx_send(
								ID_TSK_MOTWAIT,   // tskid
								mpfid,            // mpfid
								mbxid,            // mbxid
								prid1,            // prid1
								bcid,             // bcid
								ID_MSG_TIMWAIT,   // msgid
								NO_DATA,          // sts
								NO_DATA,          // sts2
								&bcid,            // data
								sizeof( bcid )    // siz
							);
		}
		else
		{
			// 指定ＷＡＩＴ時間セット
			WaitTime[bcid].tskid = mrcv->msgdata.tskid;
			WaitTime[bcid].prid1 = prid1;
			WaitTime[bcid].bcid  = bcid;
			WaitTime[bcid].msgid = msgid;
			WaitTime[bcid].motpare  = 0;
			WaitTime[bcid].tim   = waittim;
			WaitTime[bcid].cnt   = waittim;
		}
	}
	
	return(result);
}
/***********************************************************************
* 
* [概要]	モータウェイト状態取得
* [引数]	モータ番号
* [戻値]	－
* 
***********************************************************************/
UB GetMotWait( UB mot  )
{
	return( MotWait[mot].req );
}

/***********************************************************************
*
* [概要]  PDELAY  パルスモータのパワー制御待ち時間設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static ER MotWaitPDELAY ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	ST_PDELAY pdelay;

	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &pdelay, &mrcv->msgdata.data[0], sizeof(ST_PDELAY) );

	// ディレイ時間設定
	MotWait[pdelay.mm].tim = ( pdelay.tttt / DLY_10M );

	return(result);
}
