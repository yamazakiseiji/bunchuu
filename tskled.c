/******************************************************************************
* パトライト・ＬＥＤ・ブザー制御タスク                                        *
*                                                                             *
* File name : tskled.c                                                        *
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
#include "tskled.h"
#include "automachine.h"


//*************************************
// 関数定義
//*************************************
static void AlternateOutInit();					// 制御データ初期化
static void AlternateOutProc();					// 交互出力監視（パトライト・ＬＥＤ・ブザー）
static void AlternateOutReq(T_CMD_MSG* mrcv);	// 交互出力要求処理
static void PatLightOut( UB col );				// パトライト出力

//*************************************
// テーブル
//*************************************

// =================================
// 補足．●＝ＬＥＤ
// =================================
//     Ch.0  サンプルラックロック1
//     Ch.1  サンプルラックロック2
//     Ch.2  サンプルラックロック3
//     Ch.3  サンプルラックロック4
//     Ch.4  精製ラックロック1
//     Ch.5  精製ラックロック2
//     Ch.6  精製ラックロック3
//     Ch.7  開栓ハンド
//     Ch.8  チューブ把持
//     Ch.9  搬送ハンド
//     Ch.10 空気弁
// ●  Ch.11 照明LED
// ●  Ch.12 パトライト赤
// ●  Ch.13 パトライト緑
// ●  Ch.14 パトライト青
// ●  Ch.15 ブザー1
// ●  Ch.16 サンプルラックLED1
// ●  Ch.17 サンプルラックLED2
// ●  Ch.18 サンプルラックLED3
// ●  Ch.19 サンプルラックLED4
//     Ch.20 液面検知開始1
//     Ch.21 液面検知開始2
//     Ch.22 吸引開始1
//     Ch.23 吸引開始2
// ●  Ch.24 精製ラックLED1
// ●  Ch.25 精製ラックLED2
// ●  Ch.26 精製ラックLED3
//     Ch.27 reserve
//     Ch.28 ドロップキャッチャCW
//     Ch.29 ドロップキャッチャCCW


// ﾊﾟﾄﾗｲﾄ点滅時間(100msec単位)
#define SYNCTIM1	7
#define SYNCTIM2	5
#define SYNCTIM3	3
#define PATRBWTIM	21
#define ACTTWAITIM	1			// 実行前ＷＡＩＴ

// // #define C_RED			1		// 1:赤
// // #define C_GRN			2		// 2:緑
// // #define C_BLU			3		// 3:青
// // #define C_YEL			4		// 4:黄
// // #define C_PAP			5		// 5:紫
// // #define C_SKY			6		// 6:水
// // #define C_WHT			7		// 7:白
// // #define C_RBW			8		// 8:虹(パトライト色動作確認)

// ブザーパターン１周期(＝ベース周期100msec×20)
#define BUZ_INTERVAL	30


// ブザーパターンテーブル
const char BUZ_PAT_TBL[8][BUZ_INTERVAL] =
{
   // 0msec                     1000msec
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},//  0:無音
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//  1:ピー
	{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},//  2:ピーッピーッ(ON:700 msec,OFF:300 msec)
	{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},//  3:ピーッピーッ(ON:500 msec,OFF:500 msec)
	{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},//  4:ピーッピーッ(ON:300 msec,OFF:700 msec)
	{ 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0},//  5:ピピッピピッ(ON:300 msec,OFF:300 msec)
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},//  6:ピーッピーッ(ON:1000msec,OFF:500msec)
	{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},//  7:ピーッピピッ
};


//*************************************
// データ定義
//*************************************

static UB			ledreq[MAX_CH+1];			// ＣＨ別ＬＥＤ情報// 2020/2/12 １はじまり対応
static ST_BZCNT		bzcnt;					// パトライト・ブザー制御データ
static ST_LEDON		ledon;					// ＬＥＤ制御データ
static ST_LED_STS	buzsts;					// ブザー情報
static ST_LED_STS	patsyc;					// 光り物シンクロ用
static UB			rbwcnt;
static UB			blptn;

/***********************************************************************
* ｘｘｘ
* 
* [概要]
* ｘｘｘ
* 
* 
***********************************************************************/
TASK TskLed( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	// 初期化
	AlternateOutInit();
	
	for( ;; )
    {
		// メッセージ指示待ち
		err = trcv_mbx( ID_MBX_LED, (T_MSG**)&msg, DLY_100M );

    	if( err == E_TMOUT )
		{
			// 交互出力監視
			AlternateOutProc();
			continue;
		}
    	else
    	{
			// メッセージ処理
    		AlternateOutReq(msg);
    	}
    	
    	// メモリ解放
		RelMpf( ID_MPF_LED, (VP)msg );
    }
}
/***********************************************************************
* 
* [概要]	制御データ初期化
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void AlternateOutInit()
{
	memset( &ledreq[0], DEFDAT, (MAX_CH+1) );// 2020/2/12 １はじまり対応
	memset( &bzcnt,     DEFDAT, sizeof( ST_BZCNT )   );
	memset( &ledon,     DEFDAT, sizeof( ST_LEDON )   );
	memset( &buzsts,    DEFDAT, sizeof( ST_LED_STS ) );
	memset( &patsyc,    CLR,    sizeof( ST_LED_STS ) );
}

/***********************************************************************
* 
* [概要]	交互出力監視（パトライト・ＬＥＤ・ブザー）
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void AlternateOutProc()
{
	INT	Loop;
	UB  synctim;

	// パトライト点滅監視
	patsyc.cnt++;
	if( patsyc.cnt >= SYNCTIM1 )
	{
		patsyc.sts ^= LEDON;
		patsyc.cnt =  CLR;
	}
	
	
	// ------------------------
	// ﾊﾟﾄﾗｲﾄ出力
	// ------------------------
	switch( bzcnt.Act )
	{
	// 消灯
	case PATOF:
		// 念のためもう一回出力
		PatLightOut( C_CLR );
		bzcnt.Act = DEFDAT;
		break;

	// 点灯
	case PATON:
		// 念のためもう一回出力
		PatLightOut( bzcnt.Colptn );
		bzcnt.Act = DEFDAT;
		break;
	
	// 点滅
	case PATBL1:
	case PATBL2:
	case PATBL3:
		if( patsyc.sts == LEDON )
		{
			PatLightOut( bzcnt.Colptn );
		}
		else
		{
			PatLightOut( C_CLR );
		}
		break;
	// 指示なし
	default:
		break;
	}


	// ------------------------
	// ﾌﾞｻﾞｰ出力
	// ------------------------
	switch( bzcnt.Buzptn )
	{
	// 停止
	case BUZOF:
	// ブザー1(ピー)
	case BUZ01:
		// 念のためもう一回出力
		buzsts.sts = bzcnt.Buzptn;
		pio_BOUT( O_BUZZER1, bzcnt.Buzptn );
		bzcnt.Buzptn = DEFDAT;
		break;
	case BUZ02:	//  2:
	case BUZ03:	//  3:
	case BUZ04:	//  4:
	case BUZ05:	//  5:
	case BUZ06:	//  6:
	case BUZ07:	//  7:
		if( buzsts.cnt >= BUZ_INTERVAL )	buzsts.cnt = 0;

		buzsts.sts = BUZ_PAT_TBL[bzcnt.Buzptn][buzsts.cnt];
		pio_BOUT( O_BUZZER1, buzsts.sts );
		buzsts.cnt++;
		break;
	// 指示なし
	default:
		break;
	}


	// ------------------------
	// LED出力
	// ------------------------
	for( Loop = MIN_CH ; Loop <= MAX_CH ; Loop++ )// 2020/2/12 １はじまり対応
	{
		switch( ledreq[Loop] )
		{
		// 消灯
		case LEDOF:
		// 点灯
		case LEDON:
			pio_BOUT( Loop, ledreq[Loop] );
			ledreq[Loop] = DEFDAT;
			break;
		
		// 点滅
		case LEDBL:
			pio_BOUT( Loop, patsyc.sts );
			break;

		// 指示なし
		default:
			break;
		}
	}
}

/***********************************************************************
* 
* [概要]	交互出力要求処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void AlternateOutReq(T_CMD_MSG* mrcv)
{
	UB msgid;
#if 1
	msgid = mrcv->msgdata.msgid;
	switch( msgid )
	{
	// ------------------------
	// ﾊﾟﾄﾗｲﾄ出力ﾘｸｴｽﾄ
	// ------------------------
	case ID_MSG_CBZCNT:
		memcpy( &bzcnt, &mrcv->msgdata.data[0],sizeof(ST_BZCNT) );
		
		// パトライト制御
		switch( bzcnt.Act )
		{
		case PATON:			// 1:点灯
			PatLightOut( bzcnt.Colptn );
			break;
		case PATBL1:		// 2:点滅
		case PATBL2:
		case PATBL3:
			// 点滅前に一瞬消灯させるため下へ続く
			// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
			
		case PATOF:			// 0:消灯
			PatLightOut( C_CLR );
			rbwcnt = 0;
			break;
		default:
			// 何もしない
			break;
		}


		// ブザー制御
		switch( bzcnt.Buzptn )
		{
		// 停止
		case BUZOF:
			buzsts.sts = OFF;
			pio_BOUT( O_BUZZER1, OFF );
			break;

		// ブザー1(ピー)
		case BUZ01:
			buzsts.sts = ON;
			pio_BOUT( O_BUZZER1, ON  );
			break;

		case BUZ02:	//
		case BUZ03:	//
		case BUZ04:	//
		case BUZ05:	//
		case BUZ06:	//
		case BUZ07:	//
			buzsts.cnt = 0;
			buzsts.sts = OFF;
			pio_BOUT( O_BUZZER1, OFF );
			// 鳴動前に一瞬停止させるため下へ続く
			// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
			break;

		default:
			// 何もしない
			break;
		}
		break;


	// ------------------------
	// LED出力ﾘｸｴｽﾄ
	// ------------------------
	case ID_MSG_CLEDON:
		memcpy( &ledon, &mrcv->msgdata.data[0],sizeof(ST_LEDON) );
		
		if( ledon.nn > MAX_CH )		break;

		ledreq[ledon.nn] = ledon.act;

		switch( ledon.act )
		{
			case LEDOF:			// 0:消灯
			case LEDON:			// 1:点灯
				pio_BOUT( ledon.nn, ledon.act  );
				break;

			case LEDBL:			// 2:点滅
				pio_BOUT( ledon.nn, patsyc.sts );
				break;
			default:
				break;
		}
		break;

	// 規定外
	default:
		break;
	}
#endif
}

/***********************************************************************
* 
* [概要]	パトライト出力処理
*
*    補足.  赤＋緑     → 黄
*           赤＋青     → 紫
*           緑＋青     → 水色
*           赤＋緑＋青 → 白
*
* [引数]	色ﾊﾟﾀｰﾝ
* [戻値]	－
* 
***********************************************************************/
static void PatLightOut( UB col )
{
	switch( col )
	{
	case C_GRN:			// 緑
		//           R   G    B
		pio_PATOUT( OFF, ON, OFF );
		break;
	case C_YEL:			// 黄
		pio_PATOUT( ON, ON, OFF );
		break;
	case C_RED:			// 赤
		pio_PATOUT( ON, OFF, OFF );
		break;
	case C_CLR:			// 消灯
		pio_PATOUT( OFF, OFF, OFF );
		break;
	}
}

