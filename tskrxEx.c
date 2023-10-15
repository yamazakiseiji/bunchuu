/******************************************************************************
* シリアル通信２ 受信タスク                                                   *
*                                                                             *
* File name : tskrxEx.c                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/

// （注）----------------------------------------------------------------------
//       シリアル０通信：ＰＣ制御に使用（固定）
//       シリアル１通信：ＥＥＰＲＯＭアクセスのためＳＰＩ通信に使用（固定）
//       シリアル２通信：タッチパネル通信
//       シリアル３通信：拡張用（ただしＨＵＤＩのためエミレータ使用中は使えない）
// （注）----------------------------------------------------------------------


#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "tskrxEx.h"
#include "datastruct.h"
#include "sub.h"
#include "macro.h"

//*************************************
// 定数定義
//*************************************
#define COM2_BUFSIZ			128											// 通信データ受信バッファサイズ
#define KINDID_MAX			256											// 

//*************************************
// 関数定義
//*************************************
static void rx2_init();													// 初期化
typedef	ER	(*RX2_FUNC)( UB data );										// コールファンクション
static	ER 	rx2_dmy( UB rxdata );										// ダミー
static	ER 	rx2_idle( UB rxdata );										// 待機状態
static	ER 	rx2_crrecv( UB rxdata );									// CR受信

static	ER 	rx2_etx( UB rxdata );										// 
static	ER 	rx2_sum1( UB rxdata );										// 
static	ER 	rx2_sum2( UB rxdata );										// 


//*************************************
// テーブル
//*************************************

// コールファンクション
RX2_FUNC	Rx2FuncTbl[3] = 
{
	&rx2_dmy     ,														// STS00
	&rx2_idle    ,														// STS01
	&rx2_crrecv  														// STS02
};

//*************************************
// データ定義
//*************************************
static	ST_SEQ2	seq2;													// 
static	UB		TplRecvData[512];										// 



/******************************************************************************
* 
* [概要]	シリアル通信（拡張用）タスク処理
* [引数]	なし
* [戻値]	なし
* 
******************************************************************************/
TASK TskRxEx( void )
{
	T_COM_CHAR  rxd;
    ER          err;
	UB          rxdat;
	volatile struct st_sci*  sci2;

	// 制御データ初期化
	rx2_init();

	//バーコードリーダ通信設定(Port2)
	sci2 = &SCI2;
	sci2->SCBRR          = 32;											// ボーレート       38400bps
	sci2->SCSMR.BIT.CHR  = 0;											// データ長         8bit
	sci2->SCSMR.BIT.STOP = 0;											// ストップビット   1bit
	sci2->SCSMR.BIT._PE  = 0;											// パリティ         なし
	sio2_set(sci2);

	//バーコードリーダ通信開始(Port2)
	sio2_init(SIO_OPEN);

	for( ; ; )
	{
		err = rcv_dtq( ID_DTQ_RXEX, &rxd.data );						// データ受信
		if( rxd.byte.type != COM_MTYP_CHR )								// エラー？
		{
			rx2_init();													// 初期化
			continue;
		}
		rxdat = rxd.byte.data;											// 受信データ取得
		err = Rx2FuncTbl[seq2.rstatus]( rxdat ) ;						// ファンクションテーブル実行
	}
}

/******************************************************************************
* 
* [概要]	シリアルポート２初期化
* [引数]	act:OPEN/CLOSE指定
* [戻値]	なし
* 
******************************************************************************/
ER sio2_init(INT act)
{
	// SCI2設定
	if( act == SIO_OPEN )
	{
		if( SCI2.SCSCR.BIT.TE )		return( E_COM_P_OPEN );
		if( SCI2.SCSCR.BIT.RE )		return( E_COM_P_OPEN );

		SCI2.SCSCR.BIT.RIE		= 1;									// 受信データフル割り込み許可
		SCI2.SCSCR.BIT.TE		= 1;									// 送信許可
		SCI2.SCSCR.BIT.RE		= 1;									// 受信許可
	}
	else
	{
		SCI2.SCSCR.BIT.RIE		= 0;									// 受信データフル割り込み許可
		SCI2.SCSCR.BIT.TE		= 0;									// 送信禁止
		SCI2.SCSCR.BIT.RE		= 0;									// 受信禁止
	}

	SCI2.SCSCR.BIT.TIE		= 0;										// 送信データエンプティ割り込み禁止
	SCI2.SCSCR.BIT.TEIE		= 0;										// 送信終了割り込み禁止
	INTC.IPR16.BIT._SCI2	= INT_PRI_SCI2;								// 優先順位

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [概要]	シリアル２通信設定
* [引数]	comm:シリアル設定データアドレス
* [戻値]	なし
* 
******************************************************************************/
void sio2_set( volatile struct st_sci* comm )
{
	SCI2.SCSCR.BIT.RIE		= 0;										// 受信データフル割り込み許可
	SCI2.SCSCR.BIT.TIE		= 0;										// 送信データエンプティ割り込み禁止
	SCI2.SCSCR.BIT.TEIE		= 0;										// 送信終了割り込み禁止
	SCI2.SCSCR.BIT.MPIE		= 0;										// 
	SCI2.SCSCR.BIT.TE		= 0;										// 送信禁止
	SCI2.SCSCR.BIT.RE		= 0;										// 受信禁止

	
	// SCI2設定
	SCI2.SCSMR.BIT.CA    = 0;											// 調歩同期式
	SCI2.SCSMR.BIT.CHR   = comm->SCSMR.BIT.CHR;							// データビット長（７、８）
	SCI2.SCSMR.BIT._PE   = comm->SCSMR.BIT._PE;							// パリティ（NON,EVE,ODD）
	SCI2.SCSMR.BIT.STOP  = comm->SCSMR.BIT.STOP;						// ストップビット(NON,1,2)
	SCI2.SCSMR.BIT.CKS   = 0;											// クロックセレクト
	SCI2.SCBRR           = comm->SCBRR;									// ボーレート(9600,14400,19200,38400)
}

/***********************************************************************
* 
* [概要]	コマンドリクエスト受信監視用アラームハンドラ
* [引数]	exinf:OS拡張情報
* [戻値]	なし
* 
***********************************************************************/
void AlmComR2Dat( VP_INT exinf )
{
	VP_INT dmyint;
	
	dmyint = exinf;														// 読み捨て
	
	rx2_init();															// 初期化
}
/***********************************************************************
* 
* [概要]	ポート状態取得
* [引数]	なし
* [戻値]	TRUE:オープン、FALSE:クローズ
* 
***********************************************************************/
BOOL GetPortActive()
{
	BOOL	actflg = FALSE;

	if(( SCI2.SCSCR.BIT.TE )&&( SCI2.SCSCR.BIT.RE ))
	{
		actflg = TRUE;
	}

	return( actflg );
}

/***********************************************************************
* 
* [概要]	ダミー
* [引数]	rxdata:受信データ
* [戻値]	状態
* 
***********************************************************************/
static ER rx2_dmy( UB rxdata )
{
	UB dmy;
    ER          err = E_SUCCESS;
	
	dmy = rxdata;	// 読み捨て
	return( err );
}

/***********************************************************************
* 
* [概要]	初期化
* [引数]	なし
* [戻値]	なし
* 
***********************************************************************/
static void rx2_init()
{
	stp_alm( ID_ALM_R2DAT );

	memset( &seq2, 0x00, sizeof( seq2 ) );
	
	seq2.rstatus = STS01;
}

/***********************************************************************
* 
* [概要]	アイドリング
* [引数]	なし
* [戻値]	なし
* 
***********************************************************************/
static ER rx2_idle( UB rxdata )
{
    ER  err = E_SUCCESS;

	switch( rxdata )
	{
		case STX:
			memset( &TplRecvData[0], 0x00, sizeof(TplRecvData) );

			seq2.type = RCVCMD;
			seq2.rstatus = STS02;
			TplRecvData[0] = rxdata;
			seq2.wpnt = 1;
			sta_alm( ID_ALM_R2DAT, TMOUT_RCV );
			break;
		case ACK:
			err = mbx_send	(
								ID_TSK_RXEX,
								ID_MPF_CMD,
								ID_MBX_CMD,
								NO_DATA,
								NO_DATA,
								ID_MSG_ACK,
								0, 
								0, 
								&rxdata,
								sizeof( rxdata )
							);
			rx2_init();
			break;
		case NAK:
			err = mbx_send	(
								ID_TSK_RXEX,
								ID_MPF_CMD,
								ID_MBX_CMD,
								NO_DATA,
								NO_DATA,
								ID_MSG_NAK,
								0, 
								0, 
								&rxdata,
								sizeof( rxdata )
							);
			rx2_init();
			break;
		default:
			rx2_init();
			break;
	}

	return( err );
}
static 	UB	ubDatSiz  ;
static 	UB	ubCalSiz  ;
static 	UB	ubCalSum  ;
static 	UB	ubRcvSum  ;
static 	UB	ubRcvSum1 ;
static 	UB	ubRcvSum2 ;
static 	UB	ubKindId  ;

/***********************************************************************
* 
* [概要]	改行コード（CR）まで受信処理
* [引数]	なし
* [戻値]	なし
* 
***********************************************************************/
static ER rx2_crrecv( UB rxdata )
{
	ER	err       = E_CONTINUE;
#if 0
	UB	ubDatSiz  = CLR;
	UB	ubCalSiz  = CLR;
	UB	ubCalSum  = CLR;
	UB	ubRcvSum  = CLR;
	UB	ubRcvSum1 = CLR;
	UB	ubRcvSum2 = CLR;
	UB	ubKindId  = CLR;
#endif
	
	if(seq2.wpnt < COM2_BUFSIZ)
	{
		TplRecvData[seq2.wpnt++] = rxdata;

		if( rxdata == LFCHR )
		{
			seq2.rstatus++;
			
			if( seq2.wpnt >= 10 )										// STX A$ 識別ID ETX SUM1 SUM2 CR LF  最低でも10byte有る
			{
				ubDatSiz  = seq2.wpnt - 6;
				ubRcvSum1 = AsctoHex( TplRecvData[seq2.wpnt - 4] );
				ubRcvSum2 = AsctoHex( TplRecvData[seq2.wpnt - 3] );
				
				ubCalSum = CalcBCC( &TplRecvData[1], ubDatSiz );		// STXの次～ETX
				ubRcvSum = ubRcvSum1 * 0x10 + ubRcvSum2 ;				// 
				
				if( ubCalSum == ubRcvSum )
				{
					ubKindId = AsctoHex( TplRecvData[3]  ) * 0x10 + AsctoHex( TplRecvData[4] );	// 識別ID取得

					if(( ubKindId >= ID_TPL_FWVER )&&( ubKindId <= ID_TPL_PRINF ))// ざっくりチェック
					{
						err = E_SUCCESS;
						SetRcvData( &TplRecvData[1],  ubDatSiz );
					}
					else
					{
						err = E_COM_CMDDEF;								// コマンド未定義
					}
				}
				else
				{
					err = E_COM_BCCERR;									// BCC不一致
				}
			}
			else
			{
				err = E_PAR_SHORTG;										// パラメータ過不足
			}
		}
	}
	else
	{
		err = E_PAR_OUTRNG;												// パラメータ範囲外
	}

	switch( err )
	{
		// 成功・完了
		case E_SUCCESS:
//			SendACK();													// ACK送信
			err = mbx_send	(
								ID_TSK_RXEX,							// 送信元タスクＩＤ
								ID_MPF_CMD,								// メモリプールＩＤ
								ID_MBX_CMD,								// メールボックスＩＤ
								ubKindId,								// プロセスＩＤ
								NO_DATA,								// bcid
								NO_DATA,								// msgid
								0, 										// inf1
								0, 										// inf2
								&TplRecvData[0],						// 受信データ
								ubDatSiz								// 受信サイズ
							);
		
			rx2_init();													// 初期化
			break;

		// システムエラー(ＢＣＣエラー)
		case E_COM_BCCERR:

		// 未定義コマンド
		case E_COM_CMDDEF:

		// 上記以外
		default:
			if( ubKindId == ID_TPL_EXRUN )
			{
				err = mbx_send	(
									ID_TSK_RXEX,							// 送信元タスクＩＤ
									ID_MPF_CMD,								// メモリプールＩＤ
									ID_MBX_CMD,								// メールボックスＩＤ
									ubKindId,								// プロセスＩＤ
									NO_DATA,								// bcid
									NO_DATA,								// msgid
									0, 										// inf1
									0, 										// inf2
									(UB *)&err,									// 受信データ
									sizeof(err)								// 受信サイズ
								);
			}
			else
			{
				rx2_init();													// 初期化
				err = E_SUCCESS;
			}
			break;

		// 継続中
		case E_CONTINUE:
			break;
	}
	
	return( err );

}
