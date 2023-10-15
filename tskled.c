/******************************************************************************
* �p�g���C�g�E�k�d�c�E�u�U�[����^�X�N                                        *
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
// �֐���`
//*************************************
static void AlternateOutInit();					// ����f�[�^������
static void AlternateOutProc();					// ���ݏo�͊Ď��i�p�g���C�g�E�k�d�c�E�u�U�[�j
static void AlternateOutReq(T_CMD_MSG* mrcv);	// ���ݏo�͗v������
static void PatLightOut( UB col );				// �p�g���C�g�o��

//*************************************
// �e�[�u��
//*************************************

// =================================
// �⑫�D�����k�d�c
// =================================
//     Ch.0  �T���v�����b�N���b�N1
//     Ch.1  �T���v�����b�N���b�N2
//     Ch.2  �T���v�����b�N���b�N3
//     Ch.3  �T���v�����b�N���b�N4
//     Ch.4  �������b�N���b�N1
//     Ch.5  �������b�N���b�N2
//     Ch.6  �������b�N���b�N3
//     Ch.7  �J���n���h
//     Ch.8  �`���[�u�c��
//     Ch.9  �����n���h
//     Ch.10 ��C��
// ��  Ch.11 �Ɩ�LED
// ��  Ch.12 �p�g���C�g��
// ��  Ch.13 �p�g���C�g��
// ��  Ch.14 �p�g���C�g��
// ��  Ch.15 �u�U�[1
// ��  Ch.16 �T���v�����b�NLED1
// ��  Ch.17 �T���v�����b�NLED2
// ��  Ch.18 �T���v�����b�NLED3
// ��  Ch.19 �T���v�����b�NLED4
//     Ch.20 �t�ʌ��m�J�n1
//     Ch.21 �t�ʌ��m�J�n2
//     Ch.22 �z���J�n1
//     Ch.23 �z���J�n2
// ��  Ch.24 �������b�NLED1
// ��  Ch.25 �������b�NLED2
// ��  Ch.26 �������b�NLED3
//     Ch.27 reserve
//     Ch.28 �h���b�v�L���b�`��CW
//     Ch.29 �h���b�v�L���b�`��CCW


// ���ײē_�Ŏ���(100msec�P��)
#define SYNCTIM1	7
#define SYNCTIM2	5
#define SYNCTIM3	3
#define PATRBWTIM	21
#define ACTTWAITIM	1			// ���s�O�v�`�h�s

// // #define C_RED			1		// 1:��
// // #define C_GRN			2		// 2:��
// // #define C_BLU			3		// 3:��
// // #define C_YEL			4		// 4:��
// // #define C_PAP			5		// 5:��
// // #define C_SKY			6		// 6:��
// // #define C_WHT			7		// 7:��
// // #define C_RBW			8		// 8:��(�p�g���C�g�F����m�F)

// �u�U�[�p�^�[���P����(���x�[�X����100msec�~20)
#define BUZ_INTERVAL	30


// �u�U�[�p�^�[���e�[�u��
const char BUZ_PAT_TBL[8][BUZ_INTERVAL] =
{
   // 0msec                     1000msec
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},//  0:����
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//  1:�s�[
	{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},//  2:�s�[�b�s�[�b(ON:700 msec,OFF:300 msec)
	{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},//  3:�s�[�b�s�[�b(ON:500 msec,OFF:500 msec)
	{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},//  4:�s�[�b�s�[�b(ON:300 msec,OFF:700 msec)
	{ 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0},//  5:�s�s�b�s�s�b(ON:300 msec,OFF:300 msec)
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},//  6:�s�[�b�s�[�b(ON:1000msec,OFF:500msec)
	{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},//  7:�s�[�b�s�s�b
};


//*************************************
// �f�[�^��`
//*************************************

static UB			ledreq[MAX_CH+1];			// �b�g�ʂk�d�c���// 2020/2/12 �P�͂��܂�Ή�
static ST_BZCNT		bzcnt;					// �p�g���C�g�E�u�U�[����f�[�^
static ST_LEDON		ledon;					// �k�d�c����f�[�^
static ST_LED_STS	buzsts;					// �u�U�[���
static ST_LED_STS	patsyc;					// ���蕨�V���N���p
static UB			rbwcnt;
static UB			blptn;

/***********************************************************************
* ������
* 
* [�T�v]
* ������
* 
* 
***********************************************************************/
TASK TskLed( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	// ������
	AlternateOutInit();
	
	for( ;; )
    {
		// ���b�Z�[�W�w���҂�
		err = trcv_mbx( ID_MBX_LED, (T_MSG**)&msg, DLY_100M );

    	if( err == E_TMOUT )
		{
			// ���ݏo�͊Ď�
			AlternateOutProc();
			continue;
		}
    	else
    	{
			// ���b�Z�[�W����
    		AlternateOutReq(msg);
    	}
    	
    	// ���������
		RelMpf( ID_MPF_LED, (VP)msg );
    }
}
/***********************************************************************
* 
* [�T�v]	����f�[�^������
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void AlternateOutInit()
{
	memset( &ledreq[0], DEFDAT, (MAX_CH+1) );// 2020/2/12 �P�͂��܂�Ή�
	memset( &bzcnt,     DEFDAT, sizeof( ST_BZCNT )   );
	memset( &ledon,     DEFDAT, sizeof( ST_LEDON )   );
	memset( &buzsts,    DEFDAT, sizeof( ST_LED_STS ) );
	memset( &patsyc,    CLR,    sizeof( ST_LED_STS ) );
}

/***********************************************************************
* 
* [�T�v]	���ݏo�͊Ď��i�p�g���C�g�E�k�d�c�E�u�U�[�j
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void AlternateOutProc()
{
	INT	Loop;
	UB  synctim;

	// �p�g���C�g�_�ŊĎ�
	patsyc.cnt++;
	if( patsyc.cnt >= SYNCTIM1 )
	{
		patsyc.sts ^= LEDON;
		patsyc.cnt =  CLR;
	}
	
	
	// ------------------------
	// ���ײďo��
	// ------------------------
	switch( bzcnt.Act )
	{
	// ����
	case PATOF:
		// �O�̂��߂������o��
		PatLightOut( C_CLR );
		bzcnt.Act = DEFDAT;
		break;

	// �_��
	case PATON:
		// �O�̂��߂������o��
		PatLightOut( bzcnt.Colptn );
		bzcnt.Act = DEFDAT;
		break;
	
	// �_��
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
	// �w���Ȃ�
	default:
		break;
	}


	// ------------------------
	// �޻ް�o��
	// ------------------------
	switch( bzcnt.Buzptn )
	{
	// ��~
	case BUZOF:
	// �u�U�[1(�s�[)
	case BUZ01:
		// �O�̂��߂������o��
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
	// �w���Ȃ�
	default:
		break;
	}


	// ------------------------
	// LED�o��
	// ------------------------
	for( Loop = MIN_CH ; Loop <= MAX_CH ; Loop++ )// 2020/2/12 �P�͂��܂�Ή�
	{
		switch( ledreq[Loop] )
		{
		// ����
		case LEDOF:
		// �_��
		case LEDON:
			pio_BOUT( Loop, ledreq[Loop] );
			ledreq[Loop] = DEFDAT;
			break;
		
		// �_��
		case LEDBL:
			pio_BOUT( Loop, patsyc.sts );
			break;

		// �w���Ȃ�
		default:
			break;
		}
	}
}

/***********************************************************************
* 
* [�T�v]	���ݏo�͗v������
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
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
	// ���ײďo��ظ���
	// ------------------------
	case ID_MSG_CBZCNT:
		memcpy( &bzcnt, &mrcv->msgdata.data[0],sizeof(ST_BZCNT) );
		
		// �p�g���C�g����
		switch( bzcnt.Act )
		{
		case PATON:			// 1:�_��
			PatLightOut( bzcnt.Colptn );
			break;
		case PATBL1:		// 2:�_��
		case PATBL2:
		case PATBL3:
			// �_�őO�Ɉ�u���������邽�߉��֑���
			// ����������������������������������
			
		case PATOF:			// 0:����
			PatLightOut( C_CLR );
			rbwcnt = 0;
			break;
		default:
			// �������Ȃ�
			break;
		}


		// �u�U�[����
		switch( bzcnt.Buzptn )
		{
		// ��~
		case BUZOF:
			buzsts.sts = OFF;
			pio_BOUT( O_BUZZER1, OFF );
			break;

		// �u�U�[1(�s�[)
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
			// ���O�Ɉ�u��~�����邽�߉��֑���
			// ����������������������������������
			break;

		default:
			// �������Ȃ�
			break;
		}
		break;


	// ------------------------
	// LED�o��ظ���
	// ------------------------
	case ID_MSG_CLEDON:
		memcpy( &ledon, &mrcv->msgdata.data[0],sizeof(ST_LEDON) );
		
		if( ledon.nn > MAX_CH )		break;

		ledreq[ledon.nn] = ledon.act;

		switch( ledon.act )
		{
			case LEDOF:			// 0:����
			case LEDON:			// 1:�_��
				pio_BOUT( ledon.nn, ledon.act  );
				break;

			case LEDBL:			// 2:�_��
				pio_BOUT( ledon.nn, patsyc.sts );
				break;
			default:
				break;
		}
		break;

	// �K��O
	default:
		break;
	}
#endif
}

/***********************************************************************
* 
* [�T�v]	�p�g���C�g�o�͏���
*
*    �⑫.  �ԁ{��     �� ��
*           �ԁ{��     �� ��
*           �΁{��     �� ���F
*           �ԁ{�΁{�� �� ��
*
* [����]	�F�����
* [�ߒl]	�|
* 
***********************************************************************/
static void PatLightOut( UB col )
{
	switch( col )
	{
	case C_GRN:			// ��
		//           R   G    B
		pio_PATOUT( OFF, ON, OFF );
		break;
	case C_YEL:			// ��
		pio_PATOUT( ON, ON, OFF );
		break;
	case C_RED:			// ��
		pio_PATOUT( ON, OFF, OFF );
		break;
	case C_CLR:			// ����
		pio_PATOUT( OFF, OFF, OFF );
		break;
	}
}

