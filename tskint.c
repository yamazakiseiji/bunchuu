/******************************************************************************
* ＩＮＴコマンド監視タスク                                                    *
*                                                                             *
* File name : tskint.c                                                        *
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
#include "tskint.h"
#include "automachine.h"
#include "tblchr.h"
#include "mnet.h"
#include "tskcmd.h"


//*************************************
// 関数定義
//*************************************
static void InttskInit();
static void IntInfoSet(T_CMD_MSG* mrcv);
static void IntProc();
static void IntTrgAction(ST_INTCNTL *cntl);
static BOOL MotStopChk( UB pm, UB trg );
static ER IntTrgChk(ST_INTCNTL *cntl);


//*************************************
// テーブル
//*************************************


//*************************************
// データ定義
//*************************************
static	UB					ChDataOld[MAX_CH+1];						// 2020/2/12 １はじまり対応
static	ST_INTCNTL			stCntl[MAX_CH+1];							// 2020/2/12 １はじまり対応
static	ST_CNTLDATA			rspdat;

/***********************************************************************
* ｘｘｘ
* 
* [概要]
* ｘｘｘ
* 
***********************************************************************/
TASK TskInt( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	InttskInit();														// ＩＮＴコマンド情報初期化処理
	
	for( ;; )
    {
        // コマンド待ち
        err = trcv_mbx( ID_MBX_INT, (T_MSG**)&msg, DLY_1M );

    	if( err == E_TMOUT )
		{
			// INT監視処理
	    	IntProc();
			continue;
		}
    	else
    	{
			// INT情報セット
	    	IntInfoSet(msg);
    	}
    	
		RelMpf( ID_MPF_INT, (VP)msg );									// メモリ解放
    }
}

/***********************************************************************
* 
* [概要]	ＩＮＴコマンド情報初期化処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void InttskInit()
{
	INT		Loop;

	memset( &stCntl,      DEFDAT,  sizeof( stCntl ) );
	memset( &ChDataOld,   DEFDAT,  sizeof( ChDataOld ) );

	for( Loop = MIN_CH; Loop <= INMAX; Loop++ )	// 2020/2/12 １はじまり対応
	{
		stCntl[Loop].intinf.nn = Loop;
	}
}

/***********************************************************************
* 
* [概要]	ＩＮＴコマンド情報設定処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void IntInfoSet(T_CMD_MSG* mrcv)
{
	UB		ch;

	ch  = mrcv->msgdata.inf;

	stCntl[ch].tskid = mrcv->msgdata.tskid;
	stCntl[ch].prid1  = mrcv->msgdata.prid1;
	stCntl[ch].bcid  = mrcv->msgdata.bcid;
	stCntl[ch].msgid = mrcv->msgdata.msgid;
	
	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &stCntl[ch].intinf, &mrcv->msgdata.data[0], sizeof(ST_INTINF) );
}

/***********************************************************************
* 
* [概要]	ＩＮＴコマンド監視
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void IntProc()
{
	INT		Loop;
	
	for( Loop = MIN_CH; Loop <= MAX_CH; Loop++ )	// 2020/2/12 １はじまり対応
	{
		// トリガ成立か？
		if( IntTrgChk(&stCntl[Loop]) != E_CONTINUE )
		{
			// トリガ成立時の処理実行
			IntTrgAction( &stCntl[Loop] );
		}
	}
}
/***********************************************************************
* 
* [概要]	ＩＮＴコマンドトリガ成立時の処理実行
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void IntTrgAction(ST_INTCNTL *cntl)
{
	UB inpHL;
	INT		Loop;
	INT		SubLoop;
	INT		chloop;
	UB		chno;
	UB		motstop;
	UB		outdat;
	UB		outch;
	UB		chkbit;
	BOOL	stopflg;

	if( cntl->intinf.nn == DEFDAT )		return;

	chloop = (MAX_CH / 8) + 1;

	// トリガ成立時の処理
	for( Loop = 0; Loop < chloop; Loop++ )
	{
		chkbit = 0x01;
		motstop = cntl->intinf.mmm[Loop] ;
		outdat  = cntl->intinf.dat[Loop] ;
		outch   = cntl->intinf.msk[Loop] ;

		for( SubLoop = 0; SubLoop < 8; SubLoop++ )
		{
			// CH番号
			chno = ( Loop * 8 ) + SubLoop + 1;
			stopflg = MotStopChk( chno, cntl->intinf.trg ) ;
			
			// モータ停止( = 1 )
			if(( motstop & chkbit ))
			{
				// 停止対象方向のモータか？
				if( stopflg == TRUE )
				{
					motor_ABORT( chno );
				}
			}

			// 出力対象( = 1 )
			if(( outch & chkbit ))
			{
				// 出力データＨ
				if(( outdat  & chkbit ))
				{
					pio_BOUT( chno, TRUE );
				}
				// 出力データＬ
				else
				{
					pio_BOUT( chno, FALSE );
				}
			}

			chkbit = ( chkbit << 1 );
		}
	}

	// ブロックコマンドによる内部実行時はクリア
	if( cntl->bcid != NO_DATA )
	{
		// トリガ条件成立したのでトリガクリア
		cntl->intinf.trg = TRG_NM;
	}

	// 状態取得
	inpHL = pio_BIN( cntl->intinf.nn );
	
	// レスポンス送信
	// 応答メッセージ作成
	CmdMakeResponseInt( &rspdat,
						PARAM3, 								// パラメータ数
						STRTBL_MSG[ID_MSG_INTRPT], 				// コマンド文字列
						cntl->intinf.nn,						// 

						inpHL									// 
					);

	// 基本コマンドの場合は以下の処理
	if( cntl->bcid == NO_DATA )
	{
		SendResponse( &rspdat );

		// 
		// CMDTSKに Interruptによる停止を通知する必要あり、関連する管理データをクリアするため
		// 

		// 停止モータ情報をコピー
		memcpy( &rspdat.rsp.buf[0], &cntl->intinf.mmm[0], 8 );
		
		// 応答メッセージ送信
		CmdEndResponse	(
							ID_TSK_INT,
							cntl->prid1,
							NO_DATA,
							cntl->msgid,
							cntl->intinf.nn,
							NO_DATA,
							&rspdat
						);
	}
	// ＢＣコマンドの展開系のコマンドではない場合は以下の処理
	else
	{
	    // 応答メッセージ送信
		CmdEndResponse	(
							ID_TSK_INT,
							cntl->prid1,
							cntl->bcid,
							cntl->msgid,
							cntl->intinf.nn,
							NO_DATA,
							&rspdat
						);
	}



}

/***********************************************************************
* 
* [概要]	モータ停止チェック
* [引数]	pm:モータ、trg:トリガ
* [戻値]	TRUE:停止対象、FALSE:停止非対象
* 
***********************************************************************/
static BOOL MotStopChk( UB pm, UB trg )
{
	BOOL result = FALSE;
 	RSTS rsts;
 	UB	 sdir;

	// RSTS取得
	GetMotorRegister((UH)pm, RRG_RSTS, &rsts.UNID.data );

	// 動作方向取得
	sdir = ( rsts.UNID.BIT.SDIR )?  MINUS : PLUS;

	switch(trg)
	{
		case TRG_NM:                                                    // 0  N ：通常入力設定(割込設定解除) 
			// 対象なし
			break;
		case TRG_HD:                                                    // 1  HD：High入力状態の間、CW/CCW両方向共に一時停止
		case TRG_LD:                                                    // 4  LD：Low入力状態の間、CW/CCW両方向共に一時停止
		case TRG_UD:                                                    // 7  UD：立ち上がりエッジ検出時、CW/CCW両方向共に一時停止
		case TRG_DD:                                                    // 10 DD：立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
		case TRG_ED:                                                    // 13 ED：立ち上がり／立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
			result = TRUE;
			break;
		case TRG_HP:                                                    // 2  H+：High入力状態の間、CW方向のみ一時停止
		case TRG_LP:                                                    // 5  L+：Low入力状態の間、CW方向のみ一時停止
		case TRG_UP:                                                    // 8  U+：立ち上がりエッジ検出時、CW方向のみ一時停止
		case TRG_DP:                                                    // 11 D+：立ち下がりエッジ検出時、CW方向のみ一時停止
		case TRG_EP:                                                    // 14 E+：立ち上がり／立ち下がりエッジ検出時、CW方向のみ一時停止
			if( sdir == PLUS )	result = TRUE;
			break;
		case TRG_HM:                                                    // 3  H-：High入力状態の間、CCW方向のみ一時停止
		case TRG_LM:                                                    // 6  L-：Low入力状態の間、CCW方向のみ一時停止
		case TRG_UM:                                                    // 9  U-：立ち上がりエッジ検出時、CCW方向のみ一時停止
		case TRG_DM:                                                    // 12 D-：立ち下がりエッジ検出時、CCW方向のみ一時停止
		case TRG_EM:                                                    // 15 E-：立ち上がり／立ち下がりエッジ検出時、CCW方向のみ一時停止
			if( sdir == MINUS )	result = TRUE;
			break;
	}
	return( result);
}

/***********************************************************************
* 
* [概要]	ＩＮＴコマンド監視
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static ER IntTrgChk(ST_INTCNTL *cntl)
{
	UB sts;
	ER result = E_CONTINUE;


	// 未接続
	if( cntl->intinf.nn == DEFDAT )		return( result );

	// 状態取得
	sts = pio_BIN( cntl->intinf.nn );

	// 最初の一発目
	if( ChDataOld[cntl->intinf.nn] == DEFDAT )
	{
		ChDataOld[cntl->intinf.nn] = sts;
		return( result );
	}

	switch(cntl->intinf.trg)
	{
		case TRG_HD:                                                    // HD  ：High入力状態の間、CW/CCW両方向共に一時停止
		case TRG_HP:                                                    // H+  ：High入力状態の間、CW方向のみ一時停止
		case TRG_HM:                                                    // H-  ：High入力状態の間、CCW方向のみ一時停止
			if(( ChDataOld[cntl->intinf.nn] == SET )&&( sts == SET ))
			{
				result = E_SUCCESS;
			}
			break;
		case TRG_LD:                                                    // LD  ：Low入力状態の間、CW/CCW両方向共に一時停止
		case TRG_LP:                                                    // L+  ：Low入力状態の間、CW方向のみ一時停止
		case TRG_LM:                                                    // L-  ：Low入力状態の間、CCW方向のみ一時停止
			if(( ChDataOld[cntl->intinf.nn] == CLR )&&( sts == CLR ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_UD:                                                    // UD  ：立ち上がりエッジ検出時、CW/CCW両方向共に一時停止
		case TRG_UP:                                                    // U+  ：立ち上がりエッジ検出時、CW方向のみ一時停止
		case TRG_UM:                                                    // U-  ：立ち上がりエッジ検出時、CCW方向のみ一時停止
			if(( ChDataOld[cntl->intinf.nn] == CLR )&&( sts == SET ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_DD:                                                    // DD  ：立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
		case TRG_DP:                                                    // D+  ：立ち下がりエッジ検出時、CW方向のみ一時停止
		case TRG_DM:                                                    // D-  ：立ち下がりエッジ検出時、CCW方向のみ一時停止
			if(( ChDataOld[cntl->intinf.nn] == SET )&&( sts == CLR ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_ED:                                                    // ED  ：立ち上がり／立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
		case TRG_EP:                                                    // E+  ：立ち上がり／立ち下がりエッジ検出時、CW方向のみ一時停止
		case TRG_EM:                                                    // E-  ：立ち上がり／立ち下がりエッジ検出時、CCW方向のみ一時停止
			if( (ChDataOld[cntl->intinf.nn]^sts) )
			{
				result = E_SUCCESS;
			}
			break;
		case TRG_NM:                                                    // N   ：通常入力設定(割込設定解除) 
		default:
			break;
	}

	ChDataOld[cntl->intinf.nn] = sts;

	return( result );
}

