/******************************************************************************
* 周期ハンドラ                                                                *
*                                                                             *
* File name : hdrcyc.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <machine.h>
#include "kernel.h"
#include "sh7237.h"
#include "define.h"
#include "hdrcyc.h"
#include "iodefine.h"
#include "define.h"
#include "seg.h"
#include "datastruct.h" 
#include "automachine.h"
#include "mnet.h"
#include "tskcmd.h"
#include "sub.h"
#include "macro.h"

//*************************************
// テーブル
//*************************************
#define DUTY_PTN  6


//*************************************
// 関数定義
//*************************************
static void CycSegCtrl( void );
static void CycInputChRead( void );										// 入力チャネル読込処理
static void InpRead( void );											// 入力情報読み込み処理
static void DpumpControl();


//*************************************
// テーブル
//*************************************

static const UB DUTY_TBL[DUTY_PTN] = {
	OFF, // 停止
	20,  // ON DUTY  20%   OFF DUTY 80%
	40,  // ON DUTY  40%   OFF DUTY 60%
	60,  // ON DUTY  60%   OFF DUTY 40%
	80,  // ON DUTY  80%   OFF DUTY 20%
	100  // ON DUTY 100%   OFF DUTY  0%
};

//*************************************
// データ定義
//*************************************
static	UH				uhIntervalTimer;
static	ST_DPUMP_CNT	Dpump;


/******************************************************************************
* 周期起床タスク
* 
* [概要]
* 
* 7seg表示、LED点灯、DipSW監視を制御
* 
******************************************************************************/
void HdrCyc( VP_INT exinf )
{
//	static BOOL initflg = FALSE;

	WdgStop();

	// "Unused variable" warning remove.
	exinf = E_SUCCESS;

	/* 1sec Interval */
	if( !(uhIntervalTimer % INTERVAL_1S) )
	{
		// SEG表示処理
		CycSegCtrl();
	}

	/* 100msec Interval */
	if( !(uhIntervalTimer % INTERVAL_100M) )
	{
	}

	/* 10msec Interval */
	if( !(uhIntervalTimer % INTERVAL_10M) )
	{
	}

	DpumpControl();
	
	/* インターバルタイマカウント更新*/
	uhIntervalTimer++;

	WdgStart();
}
/******************************************************************************
* 
* [概要]	入力チャネル読込処理
* [引数]	
* [戻値]	
* 
******************************************************************************/
static void CycInputChRead(void)
{
	InpRead();
}

/******************************************************************************
* 
* [概要]	7SEG表示を行う。
* [引数]	
* [戻値]	
* 
******************************************************************************/
static void CycSegCtrl(void)
{
	static	UH	uhBlink = 0;

	if( !uhBlink )
	{
		SegDisp(ALL_BLINK);
	}
	else
	{
		// 7SEG表示
		SegDisp(0);
	}

	uhBlink ^= 1;
}

/******************************************************************************
* 
* [概要]
* 読み込み処理
* 
******************************************************************************/
static void InpRead()
{
}


/******************************************************************************
* 
* [概要]
* [引数]
* [戻値]
* 
******************************************************************************/
void CycInit()
{
	int save_i;
	
	save_i = get_imask();
	set_imask(0x0f);

	PE.DR.BIT.B0 = OFF;
	PE.DR.BIT.B1 = OFF;
	
	Dpump.Req    = REQIDL;
	Dpump.OnPls  = ( DUTY_TBL[6] / INTERVAL_10M );
	Dpump.OfPls  = ( INTERVAL_10M -  Dpump.OnPls);
	Dpump.OnCnt  = CLR;
	Dpump.OfCnt  = CLR;
	Dpump.Sts    = CLR;
	Dpump.Duty   = OFF;
	
	set_imask(save_i);
}

/******************************************************************************
* 
* [概要]
* [引数]
* [戻値]
* 
******************************************************************************/
static void DpumpControl()
{
	if( Dpump.Req == REQACT )
	{
		if( Dpump.Sts == OFF )	// Off Pls
		{
			if( Dpump.OfCnt == CLR )
			{
				PE.DR.BIT.B0 = ON;
				PE.DR.BIT.B1 = ON;
			}
			
			Dpump.OfCnt++;
			if( Dpump.OfPls <= Dpump.OfCnt )
			{
				Dpump.OfCnt = CLR;
				Dpump.Sts   = ON;
			}
		}
		else 					// On Pls
		{
			if( Dpump.OnCnt == CLR )
			{
				PE.DR.BIT.B0 = ON;
				PE.DR.BIT.B1 = OFF;
			}
			Dpump.OnCnt++;
			if( Dpump.OnPls <= Dpump.OnCnt )
			{
				Dpump.OnCnt = CLR;
				Dpump.Sts   = OFF;
			}
		}
	}
	if( Dpump.Req == REQEND )
	{
		PE.DR.BIT.B0 = ON;
		PE.DR.BIT.B1 = ON;
		Dpump.Req = REQIDL;
	}
}

/******************************************************************************
* 
* [概要]
* [引数]
* [戻値]
* 
******************************************************************************/
void SetDpumpDuty( UB dat )
{
	int save_i;
	
	save_i = get_imask();
	set_imask(0x0f);

	if( dat <=  DUTY_PTN )
	{
		Dpump.Duty = DUTY_TBL[dat];
		
		if( Dpump.Duty == OFF )
		{
			Dpump.OnPls = CLR;
			Dpump.OfPls = CLR;
		}
		else
		{
			Dpump.OnPls = ( DUTY_TBL[dat] / INTERVAL_10M );
			Dpump.OfPls = ( INTERVAL_10M  - Dpump.OnPls  );
		}
	}
	
	set_imask(save_i);
}

/******************************************************************************
* 
* [概要]
* [引数]
* [戻値]
* 
******************************************************************************/
void DpumpStart()
{
	int save_i;
	
	save_i = get_imask();
	set_imask(0x0f);
	
	Dpump.Req   = REQACT;
	Dpump.OnCnt = CLR;
	Dpump.OfCnt = CLR;
	Dpump.Sts   = OFF;
	
	set_imask(save_i);
}

/******************************************************************************
* 
* [概要]
* [引数]
* [戻値]
* 
******************************************************************************/
void DpumpStop()
{
	int save_i;
	
	save_i = get_imask();
	set_imask(0x0f);
	
	Dpump.Req   = REQEND;
	Dpump.OnCnt = CLR;
	Dpump.OfCnt = CLR;
	Dpump.Sts   = OFF;
	
	set_imask(save_i);
}
