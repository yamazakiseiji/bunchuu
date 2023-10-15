/******************************************************************************
* �o�b�ʐM���M�^�X�N(SIO0)                                                    *
*                                                                             *
* File name : tsktx0.c                                                        *
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
#include "machine.h"//debug
#include "tskcmd.h"
#include "tblchr.h"


//*************************************
// �֐���`
//*************************************
static void Tx0Proc(T_CMD_MSG* mrcv);
static void Tx0Send( UB senddata );
static void Tx0Init();
static void Tx0Proc2();
static void Tx0RunLSend();


//*************************************
// �e�[�u��
//*************************************


//*************************************
// �f�[�^��`
//*************************************
static UB			Sendbuff[640];							// ���M�o�b�t�@
static UB			wkbuff[512];							// ���M�o�b�t�@

/******************************************************************************
* 
* [�T�v]	�o�b�ʐM���M�^�X�N
* 			���u����o�b�փf�[�^���M���s��
* 
* 
******************************************************************************/
TASK TskTx0( void )
{
	T_CMD_MSG*  msg;
	ER          err;
	UB          msgid;
	INT         siz;

	Tx0Init();

	for( ;; )
	{
		// ���M���b�Z�[�W�̎�M
		err = rcv_mbx( ID_MBX_TX0, (T_MSG**)&msg );
		if( E_OK != err )
		{
			continue;
		}

		msgid = msg->msgdata.msgid;
		
		switch( msgid )
		{
			case ID_MSG_VALVEINI_HS:
			case ID_MSG_PRESSINI_HS:
			case ID_MSG_PRESSREN_HS:
			case ID_MSG_PRESSTAN_HS:
		    case ID_MSG_PRESSCAN_HS:
		    case ID_MSG_VALVEOUT_HS:
		    case ID_MSG_PRESSURE_HS:
				siz = GetPsnsData(&Sendbuff[0]);
				Tx0Proc2( siz );
				break;
			
		    case ID_MSG_EXPRUNL:
				Tx0RunLSend();
				break;
			
			default:
				Tx0Proc( msg );
				break;
		}

		// ���������
		RelMpf( ID_MPF_TX0, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	
* 
* 
* 
******************************************************************************/
static void Tx0RunLSend()
{
	INT siz;
	INT len;
	UB  bcc;
	UB  bcch;
	UB  bccl;

	memset( &wkbuff[0],    CLR,  sizeof( wkbuff ) );
	memset( &Sendbuff[0],  CLR,  sizeof( Sendbuff ) );
	
	GetRunData(&wkbuff[0]);
	sprintf( &Sendbuff[1], "%s,%04X,%s" , STRTBL_MSG[ID_MSG_EXPRUNL], E_SUCCESS, &wkbuff[8] );
	Sendbuff[0] = STX;
	
	len = strlen(&Sendbuff);

	Sendbuff[len] = ETX;

	bcc = CalcBCC( &Sendbuff[1], len);				// BCC�v�Z
	DectoHexASC( bcc, &bcch, &bccl );				// 10�i����16�i����ASC�ϊ�

	Sendbuff[(len+1)] = bcch;						// BCC1
	Sendbuff[(len+2)] = bccl;						// BCC2
	
	siz = len+4;
	
	Tx0Proc2( siz );
}

/******************************************************************************
* 
* [�T�v]	�o�b�ʐM���M
* 
* 
* 
******************************************************************************/
static void Tx0Init()
{
}

/******************************************************************************
* 
* [�T�v]	PC�ʐM���M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
static void Tx0Proc(T_CMD_MSG* mrcv)
{
	INT i;
	INT siz;
	UB  msg;
	UB  bcc;
	UB  bcch;
	UB  bccl;

	msg = mrcv->msgdata.msgid;
	switch(msg)
	{
		case ID_MSG_CMD:												// �R�}���h���X�|���X
		case ID_MSG_EEPREAD:											// 
		case ID_MSG_RUNLOGERR:
			siz = mrcv->msgdata.siz;									// �T�C�Y�擾
			for( i = 0; i < siz; i++ )									// �T�C�Y�����M
			{
				Tx0Send( mrcv->msgdata.data[i] );
			}
			break;
		
		case ID_MSG_EEPWRIT:											// 
			break;

		case ID_MSG_ACK:												// �`�b�j���X�|���X
			Tx0Send( ACK );
			break;
		
		case ID_MSG_NAK:												// �m�`�j���X�|���X
		default:														// �K��O
			Tx0Send( NAK );
			break;
	}
}

/******************************************************************************
* 
* [�T�v]	PC�ʐM���M����
* [����]	���M�T�C�Y
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
static void Tx0Proc2( INT siz )
{
	INT			i;
	
	for( i = 0; i < siz; i++ )									// �T�C�Y�����M
	{
		Tx0Send( Sendbuff[i] );
	}
}
/******************************************************************************
* 
* [�T�v]	�f�[�^���M
* [����]	���M�f�[�^
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
static void Tx0Send( UB senddata )
{
   int save_i;

    // �����݋֎~����
//x    save_i = get_imask();
//x	set_imask(0x0f);

//	rx_disable();

	while( 0 == SCI0.SCSSR.BIT.TDRE )
	{
		sio_CheckError( &SCI0 );
	}
	SCI0.SCTDR = senddata;
	SCI0.SCSSR.BIT.TDRE = 0;

	rot_rdq( TPRI_SELF );

//	rx_enable();
//x	set_imask(save_i);
}




