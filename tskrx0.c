/******************************************************************************
* ＰＣ通信受信タスク(SIO0)                                                    *
*                                                                             *
* File name : tskrx0.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "tskrx0.h"
#include "datastruct.h"
#include "sub.h"
#include "tblchr.h"
#include "macro.h"
#include "automachine.h"
#include "mnet.h"
#include "tskeep.h"


//*************************************
// 関数定義
//*************************************
static void	sio_init( void );
static void rx0_init(void);											// 初期化処理
static ER 	rx0_dmy(UB rxdata);
static ER 	rx0_idle(UB rxdata);									// 受信処理 待機中
static ER 	rx0_cmdrecv(UB rxdata);									// コマンド+データ(ETXまで)受信処理
static ER 	rx0_bcc1recv(UB rxdata);								// BCC1受信処理
static ER 	rx0_bcc2recv(UB rxdata);								// BCC2受信処理
static UB	rx0_ComChk( UB *cmd );
static ER	rx0_BCdatset( UB msg );
typedef ER	(*RX0_FUNC)(UB data);
static ER	rx0_SetIMPPDAT();
static ER	rx0_SetEXPSPED();
static ER	rx0_SetEXPPOSI();
static ER	rx0_SetEXPOPTN();
static ER	rx0_SetEXPCLOG();
static ER	rx0_SetIMPSPED();
static ER	rx0_SetIMPPOSI();
static ER	rx0_SetIMPOPTN();
static ER	rx0_SetEXPPDAT();


UB Get_pcnt();
UB Get_pno();
void Set_pno( UB pno );
void Set_pcnt( UB pcnt );
//void ImportCommDataSet( const char* str , UB* imp );










void rx_enable( void );
void rx_disable( void );
//static void RpMakeSendM( UB parno, INT pardat );

//void Rcvset_spddat( const char* str );
//void Rcvset_spdptn( const char* str );

//*************************************
// テーブル
//*************************************

// コールファンクション
RX0_FUNC	Rx0FuncTbl[5] = 
{
	&rx0_dmy     ,														// STS00
	&rx0_idle    ,														// STS01
	&rx0_cmdrecv ,														// STS02
	&rx0_bcc1recv,														// STS03
	&rx0_bcc2recv														// STS04
};

//*************************************
// データ定義
//*************************************
//#define	SRAM_START_ADDRESS			0x0C000000	// 0C00 0000 - 0C07 FFFF

static	ST_SEQ seq;
static	UB RecvData[CMD_BUFSIZ];
static	const char* RecvArgs[MAX_ARGS];
static	ST_PROTCOL			*pProcAdr;
static	ST_PROTCOL_INFO		prinf;
static	UB wkbuf[32];
static	UB potcol_no;
static	UB potcol_cnt;


/***********************************************************************
* 
* [概要]	ＰＣ通信受信タスク
* 			ＰＣからデータ受信を行う。（ＰＣ→装置）
* [引数]	-
* [戻値]	-
* 
***********************************************************************/
TASK TskRx0( void )
{
	T_COM_CHAR  rxd;
    ER          err;
	UB          rxdat;

	sio_init();															// シリアル通信制御データ初期化
	rx0_init();															// 初期化

	for( ; ; )
	{
		err = rcv_dtq( ID_DTQ_RXD, &rxd.data );							// データ受信
		if( rxd.byte.type != COM_MTYP_CHR )								// エラー？
		{
			rx0_init();													// 初期化
			continue;
		}

		rxdat = rxd.byte.data;											// 受信データ取得
		
		err = Rx0FuncTbl[seq.rstatus]( rxdat ) ;						// ファンクションテーブル実行
	}
}
/***********************************************************************
* 
* [概要]	シリアルポート初期化
* [引数]	-
* [戻値]	-
* 
***********************************************************************/
static void sio_init( void )
{
	// SCI0設定(ＰＣ通信用)
	SCI0.SCSMR.BIT.CA		= 0;										// 調歩同期式
	SCI0.SCSMR.BIT.CHR		= 0;										// ８ビット長
	SCI0.SCSMR.BIT._PE		= 0;										// パリティなし
	SCI0.SCSMR.BIT.STOP		= 0;										// １ストップビット
	SCI0.SCSMR.BIT.CKS		= 0;										// PΦ(40MHz)
	SCI0.SCBRR				= 32;										// 38400bps

	SCI0.SCSCR.BIT.TIE		= 0;										// 送信データエンプティ割り込み禁止
	SCI0.SCSCR.BIT.RIE		= 1;										// 受信データフル割り込み許可
	SCI0.SCSCR.BIT.TEIE		= 0;										// 送信終了割り込み禁止
	SCI0.SCSCR.BIT.TE		= 1;										// 送信許可
	SCI0.SCSCR.BIT.RE		= 1;										// 受信許可
	INTC.IPR16.BIT._SCI0	= INT_PRI_SCI0;								// 優先順位

	stp_alm( ID_ALM_R0DAT );												// アラームハンドラ停止

}

void rx_enable( void )
{
	SCI0.SCSCR.BIT.RIE		= 1;										// 受信データフル割り込み許可
	SCI0.SCSCR.BIT.RE		= 1;										// 受信許可
}
void rx_disable( void )
{
	SCI0.SCSCR.BIT.RIE		= 0;										// 受信データフル割り込み許可
	SCI0.SCSCR.BIT.RE		= 0;										// 受信許可
}
/***********************************************************************
* 
* [概要]	初期化処理
* [引数]	
* [戻値]	
* 
***********************************************************************/
static void rx0_init(void)
{
	memset( &seq, 0x00, sizeof( seq ) );
	seq.rstatus = STS01;
	stp_alm( ID_ALM_R0DAT );												// アラームハンドラ停止
}
/***********************************************************************
* 
* [概要]	ダミー
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER rx0_dmy(UB rxdata)
{
    ER          err = E_SUCCESS;
	rxdata = 0;
	return( err );
}
/***********************************************************************
* 
* [概要]	受信処理 待機中
* [引数]	受信データ
* [戻値]	
* 
***********************************************************************/
static ER rx0_idle(UB rxdata)
{
    ER          err = E_SUCCESS;
	ID			msgid;
	
	switch( rxdata )
	{
		case STX:
			memset( &RecvData[0], 0x00, sizeof(RecvData) );

			seq.type = RCVCMD;
			seq.rstatus = STS02;
			seq.rbuff[0] = rxdata;
			seq.wpnt = 1;
			seq.sizmax = 0;
			sta_alm( ID_ALM_R0DAT, TMOUT_RCV );							// 受信タイマスタート
			break;
		case ACK:
		case NAK:
			if( rxdata == ACK )
			{
				msgid = ID_MSG_ACK;
			}
			else
			{
				msgid = ID_MSG_NAK;
			}
			memset( &RecvData[0], 0x00, sizeof(RecvData) );

			err = mbx_send	(
								ID_TSK_RX0,                             // tskid
								ID_MPF_CMD,                             // memory
								ID_MBX_CMD,                             // mailbox
								NO_DATA,                                // prid1
								NO_DATA,                                // bcid
								msgid,                                  // msg
								0,                                      // sts
								0,                                      // sts2
								&RecvData[0],                           // data
								CMD_BUFSIZ                              // data siz
							);
			rx0_init();													// 初期化
			break;
		default:
			rx0_init();													// 初期化
			break;
	}

	return( err );
}
/***********************************************************************
* 
* [概要]	コマンド+データ(ETXまで)受信
* [引数]	受信データ
* [戻値]	状態
* 
***********************************************************************/
static ER rx0_cmdrecv(UB rxdata)
{
	ER	err = E_SUCCESS;

	if(seq.wpnt >= 1800)
	{
		err = E_SYS_POLGET;
	}
	else
	{
		seq.rbuff[seq.wpnt++] = rxdata;
	}

	if( rxdata == ETX )
	{
		seq.rstatus++;
	}

	return( err );
}
/***********************************************************************
* 
* [概要]	BCC1受信
* [引数]	受信データ
* [戻値]	状態
* 
***********************************************************************/
static ER rx0_bcc1recv(UB rxdata)
{
	ER	err = E_SUCCESS;

	seq.rbuff[seq.wpnt++] = rxdata;
	seq.bcc[0] = rxdata;
	seq.rstatus++;

	return( err );
}

/***********************************************************************
* 
* [概要]	BCC2受信
* [引数]	受信データ
* [戻値]	状態
* 
***********************************************************************/
static ER rx0_bcc2recv(UB rxdata)
{
	UB  anum;
	UB	msgid = CLR;
	UB	prid1 = CLR;
	UB	bcid = NO_DATA;
	UB	ubInf1 = CLR;
	INT msglen = CLR;
	INT ubCalSiz;
	UB	ubCalBcc;
	UB	ubRcvBcc;
	ER	err = E_CONTINUE;

	seq.rbuff[seq.wpnt++] = rxdata;
	seq.bcc[1] = rxdata;
	stp_alm( ID_ALM_R0DAT );											// 受信タイマ停止

	if( seq.wpnt > 3 )
	{
		ubCalSiz  = (seq.wpnt-3);										// サイズ取得

		/*******************/
		/* BCC一致チェック */
		/*******************/
		ubCalBcc = CalcBCC( &seq.rbuff[1], ubCalSiz );					// BCC計算
		ubRcvBcc = AsctoHex( seq.bcc[0] ) * 0x10 + AsctoHex( seq.bcc[1] ) ;		// ASC→HEX変換
		
		if( ubCalBcc == ubRcvBcc )
		{
			err = E_SUCCESS;
		}
		else
		{
			err = E_COM_BCCERR;
		}

		/********************/
		/* コマンドチェック */
		/********************/
		if( err == E_SUCCESS )
		{
			msgid = rx0_ComChk( &seq.rbuff[1] ) ;
			// 規定外
			if(  msgid == NO_DATA )
			{
				err = E_COM_CMDDEF;
			}
			else
			{
				memset( &RecvData,    CLR,  sizeof( RecvData ) );
				msglen = seq.wpnt-4;
				memcpy( &RecvData[0], &seq.rbuff[1], msglen );
				

				if(
					( msgid == ID_MSG_IMPPDAT )||
					( msgid == ID_MSG_IMPSPED )||
					( msgid == ID_MSG_IMPPOSI )||
					( msgid == ID_MSG_IMPOPTN )||
					( msgid == ID_MSG_EXPRUNL )||
					( msgid == ID_MSG_EXPCOML )
				)
				{
					memset( &RecvArgs,    CLR,  sizeof( RecvArgs ) );
					anum = SplitOnly( (char *)&seq.rbuff[1], RecvArgs );
					err = rx0_BCdatset( msgid );
				}
				else
				{
//					ToUpper( (char*)&RecvData );
//					anum = Split( (char *)&RecvData[0], RecvArgs );
				}
			}
		}
	}

	/**********************************************************/
	/* 状態別対応 ※電文として問題ある場合にNAKを返信する     */
	/**********************************************************/
	switch( err )
	{
		// 成功・完了
		case E_SUCCESS:
			SendACK();													// ACK送信

			err = mbx_send	(
								ID_TSK_RX0,								// 送信元タスクＩＤ
								ID_MPF_CMD,								// メモリプールＩＤ
								ID_MBX_CMD,								// メールボックスＩＤ
								prid1,
								bcid,									// ブロックコマンド管理ＩＤ(受信タスクでセット)
								msgid,									// メッセージＩＤ
								ubInf1, 								// inf1
								0, 										// inf2
								&RecvData[0],							// 受信データ
								msglen									// 受信サイズ
							);
		
			rx0_init();													// 初期化
			break;

		// システムエラー(ＢＣＣエラー)
		case E_COM_BCCERR:
		// 未定義コマンド
		case E_COM_CMDDEF:
		default:
			rx0_init();													// 初期化
			err = E_SUCCESS;
			break;

		// 継続中
		case E_CONTINUE:
			break;
	}
	
	return( err );
}
/***********************************************************************
* 
* [概要]	コマンドリクエスト受信監視用アラームハンドラ
* 		
* 
* 
***********************************************************************/
void AlmComR0Dat( VP_INT exinf )
{
    T_COM_CHAR  dat;

	exinf = 0;
    dat.byte.type = COM_MTYP_RTMO;
    dat.byte.data = 0;
    SndDtq( ID_DTQ_RXD, dat.data );
}
/***********************************************************************
* 
* [概要]	ACKの送信
* [引数]	-
* [戻値]	err:状態
* 
***********************************************************************/
ER SendACK( void )
{
    ER          err;

	err = mbx_send	(
						ID_TSK_RX0,    // tskid
						ID_MPF_TX0,    // memory
						ID_MBX_TX0,    // mailbox
						NO_DATA,       // prid1
						NO_DATA,       // bcid
						ID_MSG_ACK,    // msg
						0,             // sts
						0,             // sts2
						&RecvData[0],  // data
						CMD_BUFSIZ     // data siz
						);

	return( err );
}

/***********************************************************************
* 
* [概要]	NAKの送信
* [引数]	-
* [戻値]	err:状態
* 
***********************************************************************/
ER SendNAK( void )
{
    ER          err;

	err = mbx_send	(
						ID_TSK_RX0,     // tskid
						ID_MPF_TX0,     // memory
						ID_MBX_TX0,     // mailbox
						NO_DATA,        // prid1
						NO_DATA,        // bcid
						ID_MSG_NAK,     // msg
						0,              // sts
						0,              // sts2
						&RecvData[0],   // data
						CMD_BUFSIZ      // data siz
					);

	return( err );
}

/***********************************************************************
* 
* [概要]	
* 		
* 
* 
***********************************************************************/
static UB rx0_ComChk( UB *cmd )
{
	UB Loop;
	INT Len;

	for( Loop = 0; Loop < ID_MSG_BASE_CMD_MAX; Loop++ )
	{
		Len = strlen( &STRTBL_MSG[Loop][0] );
		ToUppern( (char*)cmd, Len );

		if( !Strncmp( cmd, &STRTBL_MSG[Loop][0],Len ))
		{
			if( ( cmd[Len] == ',' )||( cmd[Len] == ETX ) )
			{
				return( Loop );
			}
		}
	}
	
	return( NO_DATA );
}
/***********************************************************************
* 
* [概要]パラメータ数が多くプールでは引き渡せないコマンドについては
* 		ここでデータセットしておく
* 
* 
***********************************************************************/
static ER rx0_BCdatset( UB msg )
{
	ER result = E_SUCCESS;

	switch( msg )
	{
	case ID_MSG_IMPPDAT:													// 
		result = rx0_SetIMPPDAT();
		break;
	case ID_MSG_IMPSPED:													// 
		result = rx0_SetIMPSPED();
		break;
	case ID_MSG_IMPPOSI:													// 
		result = rx0_SetIMPPOSI();
		break;
	case ID_MSG_IMPOPTN:													// 
		result = rx0_SetIMPOPTN();
		break;
	case ID_MSG_EXPRUNL:													// 
//		result = rx0_SetEXPRUNL();
		break;
	case ID_MSG_EXPCOML:													// 
//		result = rx0_SetEXPCOML();
		break;
//	case ID_MSG_EXPPOSI:													// 
//		result = rx0_SetEXPPOSI();
//		break;
//	case ID_MSG_EXPOPTN:													// 
//		result = rx0_SetEXPOPTN();
//		break;
//	case ID_MSG_EXPCLOG:													// 
//		result = rx0_SetEXPCLOG();
//		break;
	}
	return( result );
}

/***********************************************************************
* 
* [概要]  プロトコル情報    →内蔵RAM展開→タッチパネルへ通知
*         プロトコルタイトル→内蔵RAM展開→タッチパネルへ通知
*         プロトコル        →SRAM展開   →EEPROMバックアップ→タッチパネルへ通知
* 
***********************************************************************/
static ER		rx0_SetIMPPDAT()
{
	ER	result = E_SUCCESS;
	UB  pno;
	UB  pcnt;
	UB  loop;
	char* rcvpnt;
	
	
	pno = Rcvset_pinf( RecvArgs[1] );

	if( ( pno > 0 )&&( pno <= PROTMAX ) )
	{
		SetRcvData( &seq.rbuff[9], (seq.wpnt-12));

		pcnt = Get_pcnt();
		
		
		pProcAdr = (ST_PROTCOL *)( SRAM_START_ADDRESS + sizeof( ST_PROTCOL )*( pno - 1) );
		memset( pProcAdr, CLR, sizeof( ST_PROTCOL ) );
		
		
		for( loop = 0; loop < pcnt; loop++ )
		{
			Rcvset_prot( RecvArgs[(3+loop)] , pno , loop );
		}
	}
	else
	{
		result = E_RECV_FAIL;
	}
	return( result );
}
static	UB h_dat;
static	UB l_dat;
static	UB pno;
/***********************************************************************
* 
*  [機能]	文字列を数値にする
*  [引数]	str     数字文字
*           rad     進数( 10 or 16 )
*           byte    バイト数(1/2/4)
*           sig     符号(0:符号なし、1:符号あり)
*  [戻値]   数値
* 
***********************************************************************/
UB Rcvset_pinf( const char* str )
{
	UB cnt;
	UB prc;
	char* rcvpnt;

	rcvpnt = str;

	h_dat = rcvpnt[0];
	l_dat = rcvpnt[1];
	pno = AsctoHex(h_dat) * 0x10 + AsctoHex(l_dat);
	
	h_dat = rcvpnt[2];
	l_dat = rcvpnt[3];
	cnt = AsctoHex(h_dat) * 0x10 + AsctoHex(l_dat);

	h_dat = rcvpnt[4];
	l_dat = rcvpnt[5];
	prc = AsctoHex(h_dat) * 0x10 + AsctoHex(l_dat);
    
	if( ( pno > 0 )&&( pno <= PROTMAX ) )
	{
		Set_pno( pno );
		Set_pcnt( cnt );
		
		prinf.Dat[(pno-1)].Cnt = cnt;
		prinf.Dat[(pno-1)].Prc = prc;
		prinf.Dat[(pno-1)].Dmy = CLR;
	}
	
	return( pno );
}
void Rcvset_prot( const char* str , UB pno , UB adr )
{

	if( ( pno > 0 )&&( pno <= PROTMAX ) )
	{
		pProcAdr = (ST_PROTCOL *)( SRAM_START_ADDRESS + sizeof( ST_PROTCOL )*( pno - 1) +  adr * sizeof( ST_PROTCOL_STEP ) );
		
		
		//          s1:コピー先、      s2:コピー元
		StrHexcpyn( (char * )&wkbuf[0], (char * )str , 20);
		memcpy( pProcAdr, &wkbuf[0], sizeof( ST_PROTCOL_STEP ) );
		
		
//x		memcpy( pProcAdr, str, sizeof( ST_PROTCOL_STEP ) );
		
		
		
	}
}

//UB Get_pcnt( UB pno )
//{
//	UB cnt = CLR;
//	
//	if( ( pno > 0 )&&( pno <= PROTMAX ) )
//	{
//		cnt = prinf.Dat[(pno-1)].Cnt;
//	}
//	return( cnt );
//}
UB Get_pno()
{
	return( potcol_no );
}
UB Get_pcnt()
{
	return( potcol_cnt );
}
void Set_pno( UB pno )
{
	potcol_no = pno;
}
void Set_pcnt( UB pcnt )
{
	potcol_cnt = pcnt;
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER		rx0_SetIMPSPED()
{
	Rcvset_spddat( RecvArgs[1] );
	
	return( E_SUCCESS );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER		rx0_SetIMPPOSI()
{
	Rcvset_posdat( RecvArgs[1] );

	return( E_SUCCESS );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER		rx0_SetIMPOPTN()
{
	return( E_SUCCESS );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER		rx0_SetEXPRUNL()
{
	return( E_SUCCESS );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER		rx0_SetEXPCOML()
{
	return( E_SUCCESS );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
//static ER		rx0_SetEXPPOSI()
//{
//	return( E_SUCCESS );
//}
//
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
//static ER		rx0_SetEXPOPTN()
//{
//	return( E_SUCCESS );
//}
//
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
//static ER		rx0_SetEXPCLOG()
//{
//	return( E_SUCCESS );
//}
//
