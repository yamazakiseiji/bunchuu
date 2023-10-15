/******************************************************************************
* ���̓Z���T���j�b�g����^�X�N                                                *
*                                                                             *
* File name : tskpunit.c                                                      *
*                                                                             *
* Copyright (c) 2023 NICHIRYO Co., Ltd.                                       *
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
#include "tskpunit.h"


//*************************************
// define
//*************************************
#define MOTIONNET_STATUS_INPUT_CHG_INTR		0x04// ���͕ω����荞��
#define MOTIONNET_STATUS_DATA_TRNS_COMP		0x01// �f�[�^�ʐM����
#define MOTIONNET_CMD_TX_FLAG_RST			0x02// ���M�t���O���Z�b�g
#define MOTIONNET_CMD_MSG_TX				0x01// ���b�Z�[�W���M�R�}���h
#define MOTIONNET_TX_DATA_NOTHING			0x00// ��̃f�[�^
#define MOTIONNET_INTR_FLAG					0x01// ���荞�݃t���O
#define MOTIONNET_INPUT_CHG_INTR_ENABLE		0x01// ���͕ω����荞�݋���

#define MNET_CMD_VALVEOUT_INI				0x00// �d���ُ������w��
#define MNET_CMD_PRESSURE_INI				0x01// ���͒l�������w��
#define MNET_CMD_PRESSURE_REN				0x02// ���͒l�擾�i�A���j
#define MNET_CMD_PRESSURE_TAN				0x03// ���͒l�擾�i�P�Ɓj
#define MNET_CMD_PRESSURE_CAN				0x04// ���͒l�擾�L�����Z��
#define MNET_CMD_VALVEOUT_REQ				0x05// �d���ُo�͎w��


#define	MOTIONNET_ADDR						0x0C008000	

enum NmMotionnetState{
	NmMotionnetState_Norequest = 0,
	NmMotionnetState_Init,
	NmMotionnetState_IntrWait1,
	NmMotionnetState_IntrWait2,
	NmMotionnetState_IntrWait3,
	NmMotionnetState_IntrWait4,
	NmMotionnetState_IntrWait5
};

//*************************************
// �����֐�
//*************************************
static void PresUnitInit();
static void PresUnitProc();
static void PresUnitProcReq( T_CMD_MSG* mrcv );
void	check_pressure_motionnet_initialize( void );
void	check_pressure_motionnet_handle( unsigned char kind );
void	check_pressure_motionnet_default( void );
void PressDataGet( unsigned short *buf );
unsigned char set_punit_cmd( unsigned char kind );

//*************************************
// �f�[�^��`
//*************************************
struct StMotionnet		*pMotionnetPunit;
static unsigned short	MotionnetState;
static unsigned char	msg_req;
static ST_VOUT			vout;
static T_MSG_DAT		reqbk;
static unsigned short	presdat[NOZZLE_MAX+1];
static unsigned char	renmsg;
static unsigned short	renflag;
static unsigned short	rencnt;
static unsigned short	actflag;

/******************************************************************************
* 
* [�T�v]	���͐���^�X�N
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
TASK TskPunit( void )
{
	ER			errflg;
	T_CMD_MSG* 	msg;

	check_pressure_motionnet_initialize();

	for( ;; )
	{
        errflg = trcv_mbx( ID_MBX_PUNIT, (T_MSG**)&msg, DLY_10M );

		if( errflg == E_TMOUT )
		{
			PresUnitProc();
		}
		else
		{
			PresUnitProcReq(msg);
		}
		// ���������
		RelMpf( ID_MPF_PUNIT, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	������
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static void PresUnitInit()
{
	MotionnetState = NmMotionnetState_Norequest;
	memset( &reqbk.tskid, CLR, sizeof( T_MSG_DAT ) );
	memset( &vout,    CLR,    sizeof( ST_VOUT ));
	memset( &presdat, CLR,    NOZZLE_MAX+1       );
	msg_req = CLR;
	actflag = CLR;
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���N�G�X�g
* [����]	mrcv:���b�Z�[�W�d��
* [�ߒl]	�|
* 
***********************************************************************/
static void PresUnitProcReq( T_CMD_MSG* mrcv )
{
	msg_req = mrcv->msgdata.msgid;
	memcpy( &reqbk.tskid, &mrcv->msgdata.tskid, sizeof(T_MSG_DAT) );
	actflag = CLR;

	switch( msg_req )
	{
	case ID_MSG_VALVEOUT:
	case ID_MSG_VALVEOUT_HS:
		memcpy( &vout.target, &reqbk.data[0],sizeof(ST_VOUT) );
	
	// �������� ���� ��������
	
	
	case ID_MSG_VALVEINI:
	case ID_MSG_VALVEINI_HS:
	case ID_MSG_PRESSINI:
	case ID_MSG_PRESSINI_HS:
	case ID_MSG_PRESSREN:
	case ID_MSG_PRESSREN_HS:
	case ID_MSG_PRESSTAN:
	case ID_MSG_PRESSTAN_HS:
	case ID_MSG_PRESSCAN:
	case ID_MSG_PRESSCAN_HS:
		MotionnetState = NmMotionnetState_Init;
		PresUnitProc();
		break;
		
	default:
		break;
	}
}

/***********************************************************************
* 
* [�T�v]	���N�G�X�g����
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void PresUnitProc()
{
	switch( msg_req )
	{
#if 0
	case ID_MSG_VALVEINI:
	case ID_MSG_VALVEINI_HS:
		check_pressure_motionnet_handle( MNET_CMD_VALVEOUT_INI );
		break;

	case ID_MSG_PRESSINI:
	case ID_MSG_PRESSINI_HS:
		check_pressure_motionnet_handle( MNET_CMD_PRESSURE_INI );
		break;
		
	case ID_MSG_PRESSREN:
	case ID_MSG_PRESSREN_HS:
		check_pressure_motionnet_handle( MNET_CMD_PRESSURE_REN );
		break;
		
	case ID_MSG_PRESSTAN:
	case ID_MSG_PRESSTAN_HS:
		check_pressure_motionnet_handle( MNET_CMD_PRESSURE_TAN );
		break;
		
	case ID_MSG_PRESSCAN:
	case ID_MSG_PRESSCAN_HS:
		check_pressure_motionnet_handle( MNET_CMD_PRESSURE_CAN );
		break;
		
	case ID_MSG_VALVEOUT:
	case ID_MSG_VALVEOUT_HS:
		check_pressure_motionnet_handle( MNET_CMD_VALVEOUT_REQ );
		break;
#endif
	case ID_MSG_VALVEINI:
	case ID_MSG_VALVEINI_HS:
	case ID_MSG_PRESSINI:
	case ID_MSG_PRESSINI_HS:
	case ID_MSG_PRESSREN:
	case ID_MSG_PRESSREN_HS:
	case ID_MSG_PRESSTAN:
	case ID_MSG_PRESSTAN_HS:
	case ID_MSG_PRESSCAN:
	case ID_MSG_PRESSCAN_HS:
	case ID_MSG_VALVEOUT:
	case ID_MSG_VALVEOUT_HS:
		check_pressure_motionnet_handle( msg_req );
		break;
	default:
		if ( renflag == ON ) 
		{
			rencnt++;
			if (( rencnt >= 20 ) &&( actflag == CLR))
			{
				actflag = SET;
				MotionnetState = NmMotionnetState_Init;
			}
		
			if( actflag == SET )
			{
				rencnt = 0;
				check_pressure_motionnet_handle( renmsg );
			}
		}
		break;
	}
}

//==============================================================================
//	Function	: check_pressure_motionnet_initialize
//	Description	: ���̓Z���T���Motionnet�����̏�����
//	Parameters	: �Ȃ�
//	Return		: �Ȃ�
//	Note		: 
//	Global		: 
//==============================================================================
void	check_pressure_motionnet_initialize( void )
{
	unsigned long   setValue;
	
	pMotionnetPunit      = (struct StMotionnet *)MOTIONNET_ADDR;
	
	// Motionnet������������
	// �f�o�C�X�ԍ�0�̐ݒ�
	pMotionnetPunit->deviceInformation[0] = 0x8B;	// �f�o�C�X�g�p�A�f�[�^�f�o�C�X�AP0�`3:�������o
	
	// �T�C�N���b�N�ʐM���J�n����
	pMotionnetPunit->commandStatus0 = (unsigned char)( MOTIONNET_CMD_CYCLIC_START >> 0 );
	pMotionnetPunit->commandStatus1 = (unsigned char)( MOTIONNET_CMD_CYCLIC_START >> 8 );

	MotionnetState = NmMotionnetState_Norequest;
	
	// �O�̂��߂̑҂�
//	wait_ms( 100 );
}

static	unsigned char wk1;
static	unsigned char wk2;
static	unsigned char wk3;
static	unsigned char wk4;
	
static		unsigned char	Rxdata[4];
	
void	check_pressure_motionnet_handle( unsigned char kind )
{
	ER	result = E_SUCCESS;
	unsigned char	cyclicError;
	unsigned char	IntrFlg;
	unsigned char	loop;
	unsigned char	Status;
	unsigned char	cmd;

//	pMotionnet = (struct StMotionnet *)MOTIONNET_ADDR;
	
	// �����̑O�Ɋ��ɃG���[���������Ă�����N���A����
	cyclicError = pMotionnetPunit->cyclicCommErrorFlg[0];
	pMotionnetPunit->cyclicCommErrorFlg[0] = cyclicError;

	
// �V���~���[�V�������[�h
#if DEBUG_SIM_PRESS != 0
	
	Rxdata[0] = 0x00;
	Rxdata[1] = 0x00;

	
	if(
		( kind == ID_MSG_PRESSREN    )||
		( kind == ID_MSG_PRESSREN_HS )||
		( kind == ID_MSG_PRESSTAN    )||
		( kind == ID_MSG_PRESSTAN_HS )
	)
	{
		for( loop = 0; loop < NOZZLE_MAX+1; loop++ )
		{
			presdat[loop] = loop * 10;
		}
		
		result = mbx_send(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_CMD,          // mpfid
							ID_MBX_CMD,          // mbxid
							reqbk.prid1,         // prid1
							reqbk.bcid,          // bcid
							kind,                // msgid
							NO_DATA,             // sts
							NO_DATA,             // sts2
							(UB *)&presdat[0],   // data
							sizeof( presdat )    // siz
						);
	}

	if(
		( kind == ID_MSG_VALVEOUT    )||
		( kind == ID_MSG_VALVEOUT_HS )||
		( kind == ID_MSG_VALVEINI    )||
		( kind == ID_MSG_VALVEINI_HS )||
		( kind == ID_MSG_PRESSINI    )||
		( kind == ID_MSG_PRESSINI_HS )||
		( kind == ID_MSG_PRESSCAN    )||
		( kind == ID_MSG_PRESSCAN_HS )
	)
	{
		result = mbx_send(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_CMD,          // mpfid
							ID_MBX_CMD,          // mbxid
							reqbk.prid1,         // prid1
							reqbk.bcid,          // bcid
							kind,                // msgid
							NO_DATA,             // sts
							NO_DATA,             // sts2
							(UB *)&Rxdata[0],    // data
							sizeof( Rxdata )     // siz
						);
	}
	msg_req = CLR;
	return;
#endif
	
	
	switch ( MotionnetState )
	{
		case NmMotionnetState_Norequest://0
			msg_req = CLR;
			break;
		
		case NmMotionnetState_Init://1
//			if ( PE.DR.BIT.B4 == 0 ) {
				IntrFlg = pMotionnetPunit->inputChangeIntrFlg[0];											// ���荞�݃t���O�擾
				pMotionnetPunit->inputChangeIntrFlg[0] = IntrFlg;											// �擾�������荞�݃t���O���������ނ��ƂŃt���O���N���A
			
				pMotionnetPunit->fifo0 = MOTIONNET_CMD_MSG_TX;												// �@0001h ���b�Z�[�W���M�R�}���h��FIFO�ɏ�������
				pMotionnetPunit->fifo1 = MOTIONNET_TX_DATA_NOTHING;											// �V

				cmd = set_punit_cmd( kind );																// ���b�Z�[�WID��著�M�R�}���h���Z�b�g����
				pMotionnetPunit->fifo0 = cmd;																// �A���M�pFIFO�Ƀ��b�Z�[�W����������   3 or 2
				pMotionnetPunit->fifo1 = MOTIONNET_TX_DATA_NOTHING;											// �V
	
				if(
					( kind == ID_MSG_VALVEOUT    )||
					( kind == ID_MSG_VALVEOUT_HS )
				)
				{
					wk1 = (unsigned char)((vout.target )     & 0xff);
					wk2 = (unsigned char)((vout.target >> 8) & 0xff);
					
					pMotionnetPunit->fifo0 = wk1;
					pMotionnetPunit->fifo1 = wk2;
					
					
					wk3 = (unsigned char)((vout.valout )     & 0xff);
					wk4 = (unsigned char)((vout.valout >> 8) & 0xff);
					pMotionnetPunit->fifo0 = wk3;
					pMotionnetPunit->fifo1 = wk4;
				}
				if (
					( kind == ID_MSG_PRESSREN    )||
					( kind == ID_MSG_PRESSREN_HS )
				)
				{
					if ( renflag == OFF ) {
						renmsg = kind;
						renflag = ON;
						rencnt = 0;
					}
				}

				if (
					( kind == ID_MSG_PRESSCAN    )||
					( kind == ID_MSG_PRESSCAN_HS )
				)
				{
					renflag = OFF;
					rencnt = 0;
				}
			
				pMotionnetPunit->commandStatus0 = (unsigned char)( MOTIONNET_CMD_DATA_START >> 0 );			// �B�f�[�^�ʐM�R�}���h(4000h)�̏�������
				pMotionnetPunit->commandStatus1 = (unsigned char)( MOTIONNET_CMD_DATA_START >> 8 );			// �V
				pMotionnetPunit->inputChangeIntrSettting[0] = MOTIONNET_INPUT_CHG_INTR_ENABLE;				// ���͊��荞�ݕω�����
				MotionnetState = NmMotionnetState_IntrWait1;
	dly_tsk(30);
//			}
			break;

		case NmMotionnetState_IntrWait1://2
//			if ( PE.DR.BIT.B4 == 0 ) {
					Status = pMotionnetPunit->commandStatus0;													// �X�e�[�^�X���擾

				if ( Status & MOTIONNET_STATUS_INPUT_CHG_INTR ){										// 0x04	
					if ( Status & MOTIONNET_STATUS_DATA_TRNS_COMP ){									// �f�[�^�ʐM���� 0x01
						IntrFlg = pMotionnetPunit->inputChangeIntrFlg[0];									// ���荞�݃t���O�擾
						if ( IntrFlg == MOTIONNET_INTR_FLAG ){											// ���荞�݃t���O����0x01
							pMotionnetPunit->inputChangeIntrFlg[0] = IntrFlg;								// �擾�������荞�݃t���O���������ނ��ƂŃt���O���N���A
							pMotionnetPunit->fifo0 = MOTIONNET_CMD_MSG_TX;									// 0001h ���b�Z�[�W���M�R�}���h��FIFO�ɏ�������
							pMotionnetPunit->fifo1 = MOTIONNET_TX_DATA_NOTHING;								// 0001h ���b�Z�[�W���M�R�}���h��FIFO�ɏ�������

							pMotionnetPunit->commandStatus0 = (unsigned char)( (MOTIONNET_CMD_DATA_START) >> 0 );// �f�[�^�ʐM�R�}���h(4000h)�̏�������
							pMotionnetPunit->commandStatus1 = (unsigned char)( (MOTIONNET_CMD_DATA_START)  >> 8 );	// �f�[�^�ʐM�R�}���h(4000h)�̏�������
							pMotionnetPunit->inputChangeIntrSettting[0] = MOTIONNET_INPUT_CHG_INTR_ENABLE;	// ���͊��荞�ݕω�����
							MotionnetState = NmMotionnetState_IntrWait2;								// �J��
						}
					}
				}
//				else
//				{
//				}
//			}
			break;

		case NmMotionnetState_IntrWait2:
//			if ( PE.DR.BIT.B4 == 0 ) {
				Status = pMotionnetPunit->commandStatus0;													// �X�e�[�^�X���擾
				if ( Status & MOTIONNET_STATUS_DATA_TRNS_COMP ){										// �f�[�^�ʐM����0x01
					if(
						( kind == ID_MSG_PRESSREN    )||
						( kind == ID_MSG_PRESSREN_HS )||
						( kind == ID_MSG_PRESSTAN    )||
						( kind == ID_MSG_PRESSTAN_HS )
					)
					{
						for( loop = 0; loop < (NOZZLE_MAX+1);  loop++ )
						{
							wk1  = pMotionnetPunit->fifo0;
							wk2  = pMotionnetPunit->fifo1;
							
							
//							presdat[loop] = wk2 + wk1 * 0x100;
							presdat[loop] = wk1 + wk2 * 0x100;
							
							
						}
					}
					else
					{
						Rxdata[0]  = pMotionnetPunit->fifo1;
						Rxdata[1]  = pMotionnetPunit->fifo0;
																// �b�l�c�̃��X�|���X
						Rxdata[2]  = pMotionnetPunit->fifo1;
						Rxdata[3]  = pMotionnetPunit->fifo0;
																// ����/���s�̃��X�|���X

					}

					
					pMotionnetPunit->fifo0 = MOTIONNET_CMD_TX_FLAG_RST;										// 0002h ���M�������t���O���Z�b�g��������
					pMotionnetPunit->fifo1 = MOTIONNET_TX_DATA_NOTHING;										// 0002h ���M�������t���O���Z�b�g��������

//					pMotionnetPunit->fifo0 = Rxdata[0];															// ��M�f�[�^�𑗐M
//					pMotionnetPunit->fifo1 = Rxdata[1];										// ��M�f�[�^�𑗐M
//					pMotionnetPunit->fifo1 = MOTIONNET_TX_DATA_NOTHING;										// ��M�f�[�^�𑗐M
					pMotionnetPunit->commandStatus0 = (unsigned char)( (MOTIONNET_CMD_DATA_START) >> 0 );		// �f�[�^�ʐM�R�}���h(4000h)�̏�������
					pMotionnetPunit->commandStatus1 = (unsigned char)( (MOTIONNET_CMD_DATA_START) >> 8 );		// �f�[�^�ʐM�R�}���h(4000h)�̏�������
					pMotionnetPunit->inputChangeIntrSettting[0] = MOTIONNET_INPUT_CHG_INTR_ENABLE;			// ���͊��荞�ݕω�����
					MotionnetState = NmMotionnetState_IntrWait3;										// �J��
				}
//			}
			break;

		case NmMotionnetState_IntrWait3:
//			if ( PE.DR.BIT.B4 == 0 ) {
				Status = pMotionnetPunit->commandStatus0;													// �X�e�[�^�X���擾
				if ( Status & MOTIONNET_STATUS_DATA_TRNS_COMP ){										// �f�[�^�ʐM����
					MotionnetState = NmMotionnetState_Norequest;										// �J��
					actflag = CLR;

					if(
						( kind == ID_MSG_PRESSREN_HS )||
						( kind == ID_MSG_PRESSTAN_HS )
					)
					{
						result = mbx_send(
											ID_TSK_PUNIT,        // tskid
											ID_MPF_CMD,          // mpfid
											ID_MBX_CMD,          // mbxid
											reqbk.prid1,         // prid1
											reqbk.bcid,          // bcid
											kind,                // msgid
											NO_DATA,             // sts
											NO_DATA,             // sts2
											(UB *)&presdat[0],   // data
											sizeof( presdat )    // siz
										);
					}

					if(
						( kind == ID_MSG_VALVEOUT    )||
						( kind == ID_MSG_VALVEOUT_HS )||
						( kind == ID_MSG_VALVEINI    )||
						( kind == ID_MSG_VALVEINI_HS )||
						( kind == ID_MSG_PRESSINI    )||
						( kind == ID_MSG_PRESSINI_HS )||
						( kind == ID_MSG_PRESSCAN    )||
						( kind == ID_MSG_PRESSCAN_HS )
					)
					{
						result = mbx_send(
											ID_TSK_PUNIT,        // tskid
											ID_MPF_CMD,          // mpfid
											ID_MBX_CMD,          // mbxid
											reqbk.prid1,         // prid1
											reqbk.bcid,          // bcid
											kind,                // msgid
											NO_DATA,             // sts
											NO_DATA,             // sts2
											(UB *)&Rxdata[0],    // data
											sizeof( Rxdata )     // siz
										);
					}
				}
				msg_req = CLR;
//			}
			break;
	}
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
void PressDataGet( unsigned short *buf )
{
	unsigned char	loop;

	for( loop = 0; loop < (NOZZLE_MAX+1);  loop++ )
	{
		*buf = presdat[loop];
		*buf++;
	}
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
unsigned char set_punit_cmd( unsigned char kind )
{
	unsigned char cmd;
	switch( kind )
	{

	case ID_MSG_VALVEINI:
	case ID_MSG_VALVEINI_HS:
		cmd = MNET_CMD_VALVEOUT_INI;
		break;

	case ID_MSG_PRESSINI:
	case ID_MSG_PRESSINI_HS:
		cmd = MNET_CMD_PRESSURE_INI;
		break;
		
	case ID_MSG_PRESSREN:
	case ID_MSG_PRESSREN_HS:
		cmd = MNET_CMD_PRESSURE_REN;
		break;
		
	case ID_MSG_PRESSTAN:
	case ID_MSG_PRESSTAN_HS:
		cmd = MNET_CMD_PRESSURE_TAN;
		break;
		
	case ID_MSG_PRESSCAN:
	case ID_MSG_PRESSCAN_HS:
		cmd = MNET_CMD_PRESSURE_CAN;
		break;
		
	case ID_MSG_VALVEOUT:
	case ID_MSG_VALVEOUT_HS:
		cmd = MNET_CMD_VALVEOUT_REQ;
		break;

	default:
		break;
	}
	return cmd;
}

