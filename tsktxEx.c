/******************************************************************************
* �V���A���ʐM�g���p ���M�^�X�N                                               *
*                                                                             *
* File name : tsktx2.c                                                        *
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
#include "tskcmd.h"

/***********************************************************************
*  �֐���`
***********************************************************************/
static void Tx2Proc(T_CMD_MSG* mrcv);									// �g���V���A�����M����
static void Tx2Send( UB senddata );										// �V���A���Q�f�[�^���M


/***********************************************************************
* �f�[�^��`
***********************************************************************/




/******************************************************************************
* �g���V���A���ʐM�^�X�N
* 
* [�T�v]
* �g���V���A���ʐM�ɂăf�[�^���M���s��
* 
* 
******************************************************************************/
TASK TskTxEx( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	for( ;; )
	{
		// ���M���b�Z�[�W�̎�M
		err = rcv_mbx( ID_TSK_TXEX, (T_MSG**)&msg );
		if( E_OK != err )
		{
			continue;
		}
		Tx2Proc(msg);

		// ���������
		RelMpf( ID_MPF_TXEX, (VP)msg );
	}
}

/******************************************************************************
* 
* [�T�v]	�g���V���A���Q�ʐM���M����
* 
* 
* 
******************************************************************************/
static void Tx2Proc( T_CMD_MSG* mrcv )
{
	UB			msg;
	INT			i;
	INT			siz;

	msg = mrcv->msgdata.msgid;
	
	switch(msg)
	{
		case ID_MSG_CMD:												// �R�}���h���X�|���X
			siz = mrcv->msgdata.siz;									// �T�C�Y�擾
			for( i = 0; i < siz; i++ )									// �T�C�Y�����M
			{
				Tx2Send( mrcv->msgdata.data[i] );
			}
			break;
		
		case ID_MSG_IMPPDAT:
		case ID_MSG_IMPSPED:
		case ID_MSG_IMPPOSI:
		case ID_MSG_IMPOPTN:
		case ID_MSG_EXPRUNL:
		case ID_MSG_EXPCOML:
			siz = mrcv->msgdata.siz;									// �T�C�Y�擾
			
			for( i = 0; i < siz; i++ )									// �T�C�Y�����M
			{
				Tx2Send( MentePcSendbuf[i] );
			}
			break;
		
		case ID_MSG_ACK:												// �`�b�j���X�|���X
			Tx2Send( ACK );
			break;

		case ID_MSG_NAK:												// �m�`�j���X�|���X
		default:														// �K��O
			Tx2Send( NAK );
			break;
	}
}

/******************************************************************************
* 
* [�T�v]	�V���A���Q�f�[�^���M
* 
* 
* 
******************************************************************************/
static void Tx2Send( UB senddata )
{
	while( 0 == SCI2.SCSSR.BIT.TDRE )
	{
		sio_CheckError( &SCI2 );
	}
	SCI2.SCTDR = senddata;
	SCI2.SCSSR.BIT.TDRE = 0;

	rot_rdq( TPRI_SELF );
}

