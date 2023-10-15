/******************************************************************************
* �����n���h��                                                                *
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
// �e�[�u��
//*************************************
#define DUTY_PTN  6


//*************************************
// �֐���`
//*************************************
static void CycSegCtrl( void );
static void CycInputChRead( void );										// ���̓`���l���Ǎ�����
static void InpRead( void );											// ���͏��ǂݍ��ݏ���
static void DpumpControl();


//*************************************
// �e�[�u��
//*************************************

static const UB DUTY_TBL[DUTY_PTN] = {
	OFF, // ��~
	20,  // ON DUTY  20%   OFF DUTY 80%
	40,  // ON DUTY  40%   OFF DUTY 60%
	60,  // ON DUTY  60%   OFF DUTY 40%
	80,  // ON DUTY  80%   OFF DUTY 20%
	100  // ON DUTY 100%   OFF DUTY  0%
};

//*************************************
// �f�[�^��`
//*************************************
static	UH				uhIntervalTimer;
static	ST_DPUMP_CNT	Dpump;


/******************************************************************************
* �����N���^�X�N
* 
* [�T�v]
* 
* 7seg�\���ALED�_���ADipSW�Ď��𐧌�
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
		// SEG�\������
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
	
	/* �C���^�[�o���^�C�}�J�E���g�X�V*/
	uhIntervalTimer++;

	WdgStart();
}
/******************************************************************************
* 
* [�T�v]	���̓`���l���Ǎ�����
* [����]	
* [�ߒl]	
* 
******************************************************************************/
static void CycInputChRead(void)
{
	InpRead();
}

/******************************************************************************
* 
* [�T�v]	7SEG�\�����s���B
* [����]	
* [�ߒl]	
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
		// 7SEG�\��
		SegDisp(0);
	}

	uhBlink ^= 1;
}

/******************************************************************************
* 
* [�T�v]
* �ǂݍ��ݏ���
* 
******************************************************************************/
static void InpRead()
{
}


/******************************************************************************
* 
* [�T�v]
* [����]
* [�ߒl]
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
* [�T�v]
* [����]
* [�ߒl]
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
* [�T�v]
* [����]
* [�ߒl]
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
* [�T�v]
* [����]
* [�ߒl]
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
* [�T�v]
* [����]
* [�ߒl]
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
