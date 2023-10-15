/******************************************************************************
* モータ制御タスク                                                            *
*                                                                             *
* File name : tskmot.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "tskcmd.h"
#include "tskmotact.h"
#include "mnet.h"
#include "sub.h"
#include "tblchr.h"
#include "automachine.h"


//*************************************
// 内部関数
//*************************************
static void MotActInit();						// モータ状態初期化
static ER MotActProc ( T_CMD_MSG* mrcv );
static ER MotActDummy( T_CMD_MSG* mrcv );
static ER MotActMove ( T_CMD_MSG* mrcv );
static ER MotActMAP  ( T_CMD_MSG* mrcv );
static ER MotActSTOP ( T_CMD_MSG* mrcv );


//*************************************
// データ定義
//*************************************
static	UB 					bufwk[CMD_BUFSIZ];							// 
static	ST_MOTOR_STATUS		sts;										// 


/******************************************************************************
* 
* [概要]	モータ制御タスク
* [引数]	
* [戻値]	
* 
******************************************************************************/
TASK TskMotAct( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotActInit();

	for( ;; )
	{
        // コマンド待ち
		err = rcv_mbx( ID_MBX_MOTACT, (T_MSG**)&msg );

		// モータ動作処理
		err = MotActProc(msg);

		// メモリ解放
		RelMpf( ID_MPF_MOTACT, (VP)msg );
	}
}
/******************************************************************************
* 
* [概要]	モータ状態初期化
* [引数]	
* [戻値]	
* 
******************************************************************************/
static void MotActInit()
{
	memset( &bufwk,   CLR,    sizeof( bufwk ) );
	memset( &sts,     CLR,    sizeof( ST_MOTOR_STATUS ) );
}
/***********************************************************************
* 
* [概要]	コマンドの解析
* [引数]	data:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static ER MotActProc( T_CMD_MSG* mrcv )
{
	ER		err = E_SUCCESS;
	UB		msg;

	msg = mrcv->msgdata.msgid;

	if( ( msg >= ID_MSG_HELLO ) && ( msg <= ID_MSG_BASE_CMD_MAX	 ))
	{
		// コールファンクション
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );
		memcpy( &bufwk[0], &mrcv->msgdata.data[0],CMD_BUFSIZ );

		switch( msg )
		{
			case ID_MSG_ABSMV :
			case ID_MSG_LIMIT :
			case ID_MSG_MAP   :
			case ID_MSG_RELMV :
				MotActMove( mrcv );
				break;

			case ID_MSG_ABORT :
			case ID_MSG_STOP  :
				MotActSTOP( mrcv );
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
* [引数]	cmd:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static ER MotActDummy( T_CMD_MSG* mrcv )
{
	// 必要であればエラー出力
	return(E_SYS_FNCCAL);
}
static 	UB		prid1;
static 	UB		bcid;
static 	UB		msg;

/***********************************************************************
*
* [概要]  移動動作[ ABSMV,RELMV, LIMIT, MAP ]
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static ER MotActMove  ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm = 0;
	UB		mmpare = 0;
	ST_ABSMV		absmv;
	ST_LIMIT		limit;
	ST_RELMV		relmv;
	ST_CNTLDATA		msgdat;												// 実行タスク制御用ﾃﾞｰﾀ

	memset( &absmv, CLR,  sizeof( ST_ABSMV ) );							// 初期化
	prid1 = mrcv->msgdata.prid1;										// 
	bcid = mrcv->msgdata.bcid;											// ＢＣＩＤ取得
	msg = mrcv->msgdata.msgid;											// メッセージＩＤ取得
	mm     = mrcv->msgdata.inf;											// モータ取得
	mmpare = mrcv->msgdata.inf2;										// モータ取得

	switch(msg)
	{
	case ID_MSG_ABSMV:
		memcpy( &absmv, &bufwk[0], sizeof(ST_ABSMV) );					// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)

		result = motor_ABSMV( absmv.mm, absmv.pppp, absmv.st );			// 絶対値移動
		mm = absmv.mm;

		break;

	case ID_MSG_LIMIT:
		memcpy( &limit, &bufwk[0], sizeof(ST_LIMIT) );

		result = motor_LIMIT(
								limit.mm,
								limit.dr,
								limit.md,
								limit.sp
							);											// リミット位置移動
		mm = limit.mm;
		break;

	case ID_MSG_RELMV:
		memcpy( &relmv, &bufwk[0], sizeof(ST_RELMV) );

		result = motor_RELMV( relmv.mm, relmv.pppp, relmv.st );			// 相対値移動
		mm = relmv.mm;
		break;

	case ID_MSG_MAP:
		memcpy( &absmv, &bufwk[0], sizeof(ST_ABSMV) );					// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)

		result = motor_ABSMV( absmv.mm, absmv.pppp, absmv.st );			// 絶対値移動
//		mm     = mrcv->msgdata.inf;										// モータ取得
//		mmpare = mrcv->msgdata.inf2;									// モータ取得
		break;
	}

	// エラーありか
	if( result != E_SUCCESS )
	{
		motor_STATUS( mm, &sts );										// 状態取得

		// 応答メッセージ作成
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// パラメータ数
							STRTBL_MSG[msg], 							// コマンド文字列
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
							0			 								// CCWリミットセンサ(0:OFF,1:ON)
						);

	    // 終了通知
		CmdEndResponse	(
							ID_TSK_MOTACT,
							prid1,
							bcid,
							msg,
							mm,
							NO_DATA,
							&msgdat
						);
	}
	else
	{
		result = mbx_send	(
								ID_TSK_MOTACT,
								ID_MPF_MOTEND,
								ID_MBX_MOTEND,
								prid1,
								bcid,
								msg,
								mm,
								mmpare,
								&bufwk[0],
								CMD_BUFSIZ
							);
	}

	return(result);
}
/***********************************************************************
*
* [概要]  STOP/ABORT    パルスモータ減速・即停止
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static ER MotActSTOP   ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
 	UB		prid1;
 	UB		bcid;
 	UB		msg; 
	ST_STOP stop;
	ST_CNTLDATA			msgdat;									// メッセージデータ

	
	memset( &stop, CLR,  sizeof( ST_STOP ) );					// 初期化
	prid1 = mrcv->msgdata.prid1;								// 
	bcid = mrcv->msgdata.bcid;									// 
	msg = mrcv->msgdata.msgid;									// メッセージＩＤ取得

	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &stop, &bufwk[0], sizeof(ST_STOP) );

	if( msg == ID_MSG_ABORT )
	{
		result = motor_ABORT( stop.mm );						// 即停止
	}
	else
	{
		result = motor_STOP( stop.mm );							// 減速停止
	}

	// エラーありか
	if( result != E_SUCCESS )
	{
		motor_STATUS( stop.mm, &sts );							// 状態取得

		// 応答メッセージ作成
		CmdMakeResponse( 	&msgdat,
							PARAM7, 							// パラメータ数
							STRTBL_MSG[msg], 					// コマンド文字列
							result, 							// エラーコード

							stop.mm, 							// モータ１番号
							sts.pos, 							// パルスカウント１
							sts.enc,							// エンコーダ１
							sts.cw_limit, 						// CWリミットセンサ(0:OFF,1:ON)
							sts.ccw_limit, 						// CCWリミットセンサ(0:OFF,1:ON)

							0, 									// モータ２番号
							0, 									// パルスカウント２
							0,									// エンコーダ２
							0, 									// CWリミットセンサ(0:OFF,1:ON)
							0			 						// CCWリミットセンサ(0:OFF,1:ON)
						);

		
		// 終了通知(→TskCmd)
		CmdEndResponse	(
							ID_TSK_MOTACT,
							prid1,
							bcid,
							msg,
							stop.mm,
							NO_DATA,
							&msgdat
						);
	}
	else
	{
		result = mbx_send	(
								ID_TSK_MOTACT,
								ID_MPF_MOTEND,
								ID_MBX_MOTEND,
								prid1,
								bcid,
								msg,
								stop.mm,
								NO_DATA,
								&bufwk[0],
								CMD_BUFSIZ
							);
	}
	return(result);
}
