/******************************************************************************
* シリアル通信２ 受信タスク                                                   *
*                                                                             *
* File name : tskrx2.c                                                        *
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
#include "tskrx2.h"
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

// 識別IDチェック用テーブル
const KINDID_CHECK[KINDID_MAX] = {
//           0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F  
/* 00 */	DIS,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* 10 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,
/* 20 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,
/* 30 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,
/* 40 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,
/* 50 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* 60 */	ENA,ENA,ENA,ENA,ENA,ENA,ENA,ENA,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* 70 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* 80 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* 90 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* A0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* B0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* C0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* D0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* E0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
/* F0 */	DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,DIS,
};

//*************************************
// データ定義
//*************************************
static	ST_SEQ2	seq2;													// 
static	UB		RecvData2[COM2_BUFSIZ];									// 



/******************************************************************************
* 
* [概要]	シリアル通信（拡張用）タスク処理
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
TASK TskRx2( void )
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
		err = rcv_dtq( ID_DTQ_RXEX, &rxd.data );							// データ受信
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
* [戻値]	－
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
* [戻値]	－
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
* [戻値]	－
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
* [引数]	pno:ポート番号
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
* [引数]	pno:ポート番号、rxdata:受信データ
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
    ER          err = E_SUCCESS;

	switch( rxdata )
	{
		case STX:
			memset( &RecvData2[0], 0x00, sizeof(RecvData2) );

			seq2.type = RCVCMD;
			seq2.rstatus = STS02;
			seq2.rbuff[0] = rxdata;
			seq2.wpnt = 1;
			seq2.sizmax = 0;
			sta_alm( ID_ALM_R2DAT, TMOUT_RCV );
			break;
		case ACK:
			memset( &RecvData2[0], 0x00, sizeof(RecvData2) );
			err = mbx_send	(
								ID_TSK_RX0,         // tskid
								ID_MPF_CMD,         // mpfid
								ID_MBX_CMD,         // mbxid
								NO_DATA,            // prid1
								NO_DATA,            // bcid
								ID_MSG_ACK,         // msgid
								0,                  // sts
								0,                  // sts2
								&RecvData2[0],      // data
								COM2_BUFSIZ         // siz
							);
			rx2_init();
			break;
		case NAK:
			memset( &RecvData2[0], 0x00, sizeof(RecvData2) );
			err = mbx_send	(
								ID_TSK_RX0,         // tskid
								ID_MPF_CMD,         // mpfid
								ID_MBX_CMD,         // mbxid
								NO_DATA,            // prid1
								NO_DATA,            // bcid
								ID_MSG_NAK,         // msgid
								0,                  // sts
								0,                  // sts2
								&RecvData2[0],      // data
								COM2_BUFSIZ         // siz
							);
			rx2_init();
			break;
		default:
			rx2_init();
			break;
	}

	return( err );
}

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
	UB	ubDatSiz  = CLR;
	UB	ubCalSiz  = CLR;
	UB	ubCalSum  = CLR;
	UB	ubRcvSum  = CLR;
	UB	ubRcvSum1 = CLR;
	UB	ubRcvSum2 = CLR;
	UB	ubKindId  = CLR;

	
	if(seq2.wpnt < COM2_BUFSIZ)
	{
		seq2.rbuff[seq2.wpnt++] = rxdata;

		if( rxdata == CRCHR )
		{
			seq2.rstatus++;
			
			if( seq2.wpnt >= 9 )										// STX A$ 識別ID ETX SUM1 SUM2 CR  最低でも9byte有る
			{
				ubDatSiz  = seq2.wpnt - 5;
				ubCalSiz  = seq2.wpnt - 4;
				ubRcvSum1 = seq2.rbuff[seq2.wpnt - 2];
				ubRcvSum2 = seq2.rbuff[seq2.wpnt - 1];
				
				ubCalSum = CalcBCC( &seq2.rbuff[1], ubCalSiz );			// STXの次～ETX
				ubRcvSum = AsctoHex( ubRcvSum1 ) * 0x10 + AsctoHex( ubRcvSum2 );	// 
				
				if( ubCalSum == ubRcvSum )
				{
					ubKindId = AsctoHex( seq2.rbuff[3]  ) * 0x10 + AsctoHex( seq2.rbuff[4] );	// 識別ID取得
					if( KINDID_CHECK[ubKindId] == ENA )
					{
						err = E_SUCCESS;
						memset( &RecvData2,    CLR,  sizeof( RecvData2 ) );
						memcpy( &RecvData2[0], &seq2.rbuff[1], ubDatSiz );
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
			SendACK();													// ACK送信

			err = mbx_send	(
								ID_TSK_RXEX,							// tskid 送信元タスクＩＤ     
								ID_MPF_CMD,								// mpfid メモリプールＩＤ
								ID_MBX_CMD,								// mbxid メールボックスＩＤ
								0,										// prid1 
								0,										// bcid  
								ubKindId,								// msgid メッセージＩＤ
								0, 										// sts   inf1
								0, 										// sts2  inf2
								&RecvData2[0],							// data  受信データ
								ubDatSiz								// siz   受信サイズ
							);
		
			rx2_init();													// 初期化
			break;

		// システムエラー(ＢＣＣエラー)
		case E_COM_BCCERR:

		// 未定義コマンド
		case E_COM_CMDDEF:

		// 上記以外
		default:
			rx2_init();													// 初期化
			err = E_SUCCESS;
			break;

		// 継続中
		case E_CONTINUE:
			break;
	}
	
	return( err );

}
