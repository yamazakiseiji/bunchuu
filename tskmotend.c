/******************************************************************************
* モータ動作終了監視タスク                                                    *
*                                                                             *
* File name : tskmotend.c                                                     *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "tskmotend.h"
#include "tskmotact.h"
#include "tskcmd.h"
#include "sub.h"
#include "mnet.h"
#include "tblchr.h"
#include "automachine.h"


//*************************************
// 内部関数
//*************************************
static  void MotEndInit();												// ウェイト要求処理
static  void MotEndInitSet( UB mm );									// 制御用データ初期値セット
static  void MotEndDlyTmo();											// ディレイタイムアウト監視
static  void MotEndChk();												// コマンド動作完了チェック
static  void MotEndMulti( UB mm );										// 
static  void MotEndSigle( UB mm );										// 
static  void MotEndProc   ( T_CMD_MSG* mrcv );							// ウェイト要求処理
static  void CmdRcvMsg     ( T_CMD_MSG* mrcv );							// メッセージ受信処理
static  ER   MotEndAnalyze( T_CMD_MSG* mrcv );							// コマンドの解析
static  ER   MotEndDummy     ( T_CMD_MSG* mrcv );						// 
static  ER   MotEndRequest   ( T_CMD_MSG* mrcv );						// モータ動作前のウェイトリクエスト
static  ER   MotEndRequestW  ( T_CMD_MSG* mrcv );						// モータ動作前のウェイトリクエスト
static  ER   MotEndPDELAY    ( T_CMD_MSG* mrcv );						// PDELAY  パルスモータのパワー制御待ち時間設定


/***********************************************************************
* テーブル
***********************************************************************/




//*************************************
// データ定義
//*************************************
static	ST_MOTWAIT			MotEnd[(DVMAX+1)];							// ウェイト制御データ
static	ST_MOTOR_STATUS		sts;										// 
static	ST_MOTOR_STATUS		sts2;										// 


/******************************************************************************
* 
* [概要]	モータ動作終了監視タスク
* [引数]	
* [戻値]	
* 
******************************************************************************/
TASK TskMotEnd( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotEndInit();

	for( ;; )
	{
        // コマンド待ち
        err = trcv_mbx( ID_MBX_MOTEND, (T_MSG**)&msg, DLY_10M );

		if( err == E_TMOUT )
		{
			MotEndDlyTmo();
			MotEndChk();
			continue;
		}
		else
		{
			MotEndProc(msg);
		}
		// メモリ解放
		RelMpf( ID_MPF_MOTEND, (VP)msg );
	}
}
/******************************************************************************
* 
* [概要]	モータウェイト初期化
* [引数]	
* [戻値]	
* 
******************************************************************************/
static void MotEndInit()
{
	UB Loop;

	memset( &MotEnd, CLR, sizeof( MotEnd ) );

	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		MotEndInitSet(Loop);
//		MotEnd[Loop].req     = DEFDAT;									// default
//		MotEnd[Loop].motpare = DEFDAT;									// default
//		MotEnd[Loop].prid1   = DEFDAT;									// default		
//		MotEnd[Loop].bcid    = DEFDAT;									// default		
//		MotEnd[Loop].msgid   = DEFDAT;									// default
//		MotEnd[Loop].inf     = DEFDAT;									// default
//		MotEnd[Loop].inf2    = DEFDAT;									// default
//		MotEnd[Loop].tim     = 10/DLY_10M;								// default
//		memset( &MotEnd[Loop].msgdat[0], CLR, CMD_BUFSIZ );
	}
}

/******************************************************************************
* 
* [概要]	制御用データ初期値セット
* [引数]	モータ番号
* [戻値]	－
* 
******************************************************************************/
static void MotEndInitSet( UB mm )
{
	MotEnd[mm].req     = DEFDAT;									// default
	MotEnd[mm].motpare = DEFDAT;									// default
	MotEnd[mm].prid1   = DEFDAT;									// default		
	MotEnd[mm].bcid    = DEFDAT;									// default		
	MotEnd[mm].msgid   = DEFDAT;									// default
	MotEnd[mm].inf     = DEFDAT;									// default
	MotEnd[mm].inf2    = DEFDAT;									// default
	MotEnd[mm].tim     = 10/DLY_10M;								// default
	memset( &MotEnd[mm].msgdat[0], CLR, CMD_BUFSIZ );
}

/***********************************************************************
* 
* [概要]	ディレイタイムアウト監視
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void MotEndDlyTmo()
{
	UB Loop;
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		switch( MotEnd[Loop].req )
		{
		case REQRDY:
			// Busy有→Busy解除
			if( motor_IsBusy(Loop) == FALSE )
			{
				// 励磁OFFするか判定
				if( GetPowerSts( Loop ) == FALSE )
				{
					// WAITなし
					if( MotEnd[Loop].tim == 0 )
					{
						MotEnd[Loop].req = REQEND;						// 終了動作
					}
					else
					{
						MotEnd[Loop].req = REQACT;						// ディレイ動作
					}
					motor_POWER( Loop, CLR );
				}
				else
				{
					MotEnd[Loop].req = REQEND;							// ディレイ要求終了
				}
			}
			else
			{
			}
			break;

		case REQACT:
			MotEnd[Loop].cnt --;										// デクリメント

			if( MotEnd[Loop].cnt <= 0 )
			{
				MotEnd[Loop].req = REQEND;								// ディレイ要求終了
			}
			break;

		default:
			break;
		}
	}
}
/***********************************************************************
* 
* [概要]	コマンド動作完了チェック
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void MotEndChk()
{
	UB Loop;
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		if( MotEnd[Loop].msgid == ID_MSG_MAP )
		{
			MotEndMulti( Loop );
		}
		else
		{
			MotEndSigle(Loop);
		}
	}
}
/***********************************************************************
* 
* [概要]	コマンド動作完了後のレスポンス処理（シングル駆動）
* [引数]	mm:モータ
* [戻値]	－
* 
***********************************************************************/
static void MotEndSigle( UB mm )
{
	ER  result = E_SUCCESS;
	ST_CNTLDATA			msgdat;										// メッセージデータ

	// ディレイ終了したか
	if( MotEnd[mm].req == REQEND )
	{
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );

		// 状態取得
		result = motor_STATUS( mm, &sts );

		// リミット確認
		if(
			( MotEnd[mm].msgid == ID_MSG_ABSMV )||
			( MotEnd[mm].msgid == ID_MSG_RELMV )||
			( MotEnd[mm].msgid == ID_MSG_MAP   ) )
		{
			// リミットか？
			if(( sts.cw_limit )||( sts.ccw_limit ))
			{
				result = ( E_LMT_ERR | mm );
			}
		}
		
		// 応答メッセージ作成
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// パラメータ数
							STRTBL_MSG[MotEnd[mm].msgid], 				// コマンド文字列
							result, 									// エラーコード

							(INT)mm,									// モータ１番号
							sts.pos, 									// パルスカウント１
							sts.enc,									// エンコーダ１
							sts.cw_limit, 								// CWリミットセンサ(0:OFF,1:ON)
							sts.ccw_limit, 								// CCWリミットセンサ(0:OFF,1:ON)

							0, 											// モータ２番号
							0, 											// パルスカウント２
							0,											// エンコーダ２
							0, 											// CWリミットセンサ(0:OFF,1:ON)
							0											// CCWリミットセンサ(0:OFF,1:ON)
						);

	    // 応答メッセージ送信
		CmdEndResponse	(
							ID_TSK_MOTEND,                              // ﾀｽｸID
							MotEnd[mm].prid1,                           // PROCID
							MotEnd[mm].bcid,                            // BCID
							MotEnd[mm].msgid,                           // ﾒｯｾｰｼﾞID
							MotEnd[mm].inf,                             // 情報1
							MotEnd[mm].inf2,                            // 情報2
							&msgdat                                     // DATA
						);
		
		MotEndInitSet(mm);
	}
}
/***********************************************************************
* 
* [概要]	コマンド動作完了後のレスポンス処理（マルチ駆動）
* [引数]	mm:モータ
* [戻値]	－
* 
***********************************************************************/
static void MotEndMulti( UB mm )
{
	ER  result = E_SUCCESS;
	ST_CNTLDATA			msgdat;										// メッセージデータ

	// ディレイ終了したか
	if(
		( MotEnd[mm].req == REQEND )
	)
	{
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );

		// 状態取得
		result = motor_STATUS( mm, &sts );

		// 応答メッセージ作成
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// パラメータ数
							STRTBL_MSG[MotEnd[mm].msgid], 				// コマンド文字列
							result, 									// エラーコード

							(INT)mm,									// モータ１番号
							sts.pos, 									// パルスカウント１
							sts.enc,									// エンコーダ１
							sts.cw_limit, 								// CWリミットセンサ(0:OFF,1:ON)
							sts.ccw_limit, 								// CCWリミットセンサ(0:OFF,1:ON)

							0, 											// モータ２番号
							0, 											// パルスカウント２
							0,											// エンコーダ２
							0, 											// CWリミットセンサ(0:OFF,1:ON)
							0											// CCWリミットセンサ(0:OFF,1:ON)
						);

	    // 応答メッセージ送信
		CmdEndResponse	(
							ID_TSK_MOTEND,
							MotEnd[mm].prid1,  
							MotEnd[mm].bcid,
							MotEnd[mm].msgid,
// 05/23					mm,
// 05/23					MotEnd[mm].motpare,
							MotEnd[mm].inf,                             // 情報1
							MotEnd[mm].inf2,                            // 情報2
							&msgdat
						);
		
		MotEndInitSet(mm);
	}
}

/***********************************************************************
* 
* [概要]	ウェイト要求処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void MotEndProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_CMD:	// TSK_CMD->
	case ID_TSK_MOTACT:	// TSK_MOTACT->
	case ID_TSK_INT:	// TSK_INT->
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

	err = MotEndAnalyze( mrcv );
}
/***********************************************************************
* 
* [概要]	コマンドの解析
* [引数]	data:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static ER MotEndAnalyze( T_CMD_MSG* mrcv )
{
	ER			err = E_SUCCESS;
	UB			msgid;

	// メッセージＩＤ取得
	msgid = mrcv->msgdata.msgid;

	if( ( msgid >= ID_MSG_HELLO ) && ( msgid <= ID_MSG_BASE_CMD_MAX	 ))
	{
		switch( msgid )
		{
			case ID_MSG_INT   :
			case ID_MSG_ABSMV :
			case ID_MSG_ABORT :
			case ID_MSG_LIMIT :
			case ID_MSG_MAP   :
			case ID_MSG_RELMV :
			case ID_MSG_STOP  :
				MotEndRequest( mrcv );
				break;
			case ID_MSG_PDELAY:
				MotEndPDELAY( mrcv );
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
static ER MotEndDummy( T_CMD_MSG* mrcv )
{
	memset( mrcv, CLR,  sizeof( T_CMD_MSG ) );
	
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
static ER MotEndRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm;
	UB		mm2;

	// モータ取得
	mm = mrcv->msgdata.inf;
	mm2 = mrcv->msgdata.inf2;

	MotEnd[mm].prid1 = mrcv->msgdata.prid1;							// procidセット
	MotEnd[mm].bcid  = mrcv->msgdata.bcid;							// BCIDセット
	MotEnd[mm].msgid = mrcv->msgdata.msgid;							// メッセージＩＤセット
	MotEnd[mm].inf   = mrcv->msgdata.inf;							// 
	MotEnd[mm].inf2  = mrcv->msgdata.inf2;							// 

	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &MotEnd[mm].msgdat[0], &mrcv->msgdata.data[0], CMD_BUFSIZ );

	MotEnd[mm].req = REQRDY;											// モータ動作中のためディレイアクションレディ
	MotEnd[mm].cnt = MotEnd[mm].tim;

	// モータペアセット
	MotEnd[mm].motpare = mm2;
	
	return(result);
}

/***********************************************************************
* 
* [概要]	モータウェイト状態取得
* [引数]	モータ番号
* [戻値]	－
* 
***********************************************************************/
UB GetMotEnd( UB mot  )
{
	return( MotEnd[mot].req );
}

/***********************************************************************
*
* [概要]  PDELAY  パルスモータのパワー制御待ち時間設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static ER MotEndPDELAY ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	ST_PDELAY pdelay;

	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &pdelay, &mrcv->msgdata.data[0], sizeof(ST_PDELAY) );

	// ディレイ時間設定
	MotEnd[pdelay.mm].tim = ( pdelay.tttt / 10 );

	return(result);
}
