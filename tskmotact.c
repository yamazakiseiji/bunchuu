/******************************************************************************
* ���[�^����^�X�N                                                            *
*                                                                             *
* File name : tskmot.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
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


//*************************************
// �����֐�
//*************************************
static void MotActInit();						// ���[�^��ԏ�����
static ER MotActProc ( T_CMD_MSG* mrcv );
static ER MotActDummy( T_CMD_MSG* mrcv );
static ER MotActMove ( T_CMD_MSG* mrcv );
static ER MotActMAP  ( T_CMD_MSG* mrcv );
static ER MotActSTOP ( T_CMD_MSG* mrcv );


//*************************************
// �f�[�^��`
//*************************************
static	UB 					bufwk[CMD_BUFSIZ];							// 
static	ST_MOTOR_STATUS		sts;										// 


/******************************************************************************
* 
* [�T�v]	���[�^����^�X�N
* [����]	
* [�ߒl]	
* 
******************************************************************************/
TASK TskMotAct( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotActInit();

	for( ;; )
	{
        // �R�}���h�҂�
		err = rcv_mbx( ID_MBX_MOTACT, (T_MSG**)&msg );

		// ���[�^���쏈��
		err = MotActProc(msg);

		// ���������
		RelMpf( ID_MPF_MOTACT, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	���[�^��ԏ�����
* [����]	
* [�ߒl]	
* 
******************************************************************************/
static void MotActInit()
{
	memset( &bufwk,   CLR,    sizeof( bufwk ) );
	memset( &sts,     CLR,    sizeof( ST_MOTOR_STATUS ) );
}
/***********************************************************************
* 
* [�T�v]	�R�}���h�̉��
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static ER MotActProc( T_CMD_MSG* mrcv )
{
	ER		err = E_SUCCESS;
	UB		msg;

	msg = mrcv->msgdata.msgid;

	if( ( msg >= ID_MSG_HELLO ) && ( msg <= ID_MSG_BASE_CMD_MAX	 ))
	{
		// �R�[���t�@���N�V����
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );
		memcpy( &bufwk[0], &mrcv->msgdata.data[0],CMD_BUFSIZ );

		switch( msg )
		{
			case ID_MSG_ABSMV :
			case ID_MSG_LIMIT :
			case ID_MSG_MAP   :
			case ID_MSG_RELMV :
				MotActMove( mrcv );
				break;

			case ID_MSG_ABORT :
			case ID_MSG_STOP  :
				MotActSTOP( mrcv );
				break;
			default:
				break;
		}
	}

	return( err );
}

/***********************************************************************
* 
* [�T�v]	�_�~�[���[�`��
* [����]	cmd:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static ER MotActDummy( T_CMD_MSG* mrcv )
{
	// �K�v�ł���΃G���[�o��
	return(E_SYS_FNCCAL);
}
static 	UB		prid1;
static 	UB		bcid;
static 	UB		msg;

/***********************************************************************
*
* [�T�v]  �ړ�����[ ABSMV,RELMV, LIMIT, MAP ]
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static ER MotActMove  ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm = 0;
	UB		mmpare = 0;
	ST_ABSMV		absmv;
	ST_LIMIT		limit;
	ST_RELMV		relmv;
	ST_CNTLDATA		msgdat;												// ���s�^�X�N����p�ް�

	memset( &absmv, CLR,  sizeof( ST_ABSMV ) );							// ������
	prid1 = mrcv->msgdata.prid1;										// 
	bcid = mrcv->msgdata.bcid;											// �a�b�h�c�擾
	msg = mrcv->msgdata.msgid;											// ���b�Z�[�W�h�c�擾
	mm     = mrcv->msgdata.inf;											// ���[�^�擾
	mmpare = mrcv->msgdata.inf2;										// ���[�^�擾

	switch(msg)
	{
	case ID_MSG_ABSMV:
		memcpy( &absmv, &bufwk[0], sizeof(ST_ABSMV) );					// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)

		result = motor_ABSMV( absmv.mm, absmv.pppp, absmv.st );			// ��Βl�ړ�
		mm = absmv.mm;

		break;

	case ID_MSG_LIMIT:
		memcpy( &limit, &bufwk[0], sizeof(ST_LIMIT) );

		result = motor_LIMIT(
								limit.mm,
								limit.dr,
								limit.md,
								limit.sp
							);											// ���~�b�g�ʒu�ړ�
		mm = limit.mm;
		break;

	case ID_MSG_RELMV:
		memcpy( &relmv, &bufwk[0], sizeof(ST_RELMV) );

		result = motor_RELMV( relmv.mm, relmv.pppp, relmv.st );			// ���Βl�ړ�
		mm = relmv.mm;
		break;

	case ID_MSG_MAP:
		memcpy( &absmv, &bufwk[0], sizeof(ST_ABSMV) );					// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)

		result = motor_ABSMV( absmv.mm, absmv.pppp, absmv.st );			// ��Βl�ړ�
//		mm     = mrcv->msgdata.inf;										// ���[�^�擾
//		mmpare = mrcv->msgdata.inf2;									// ���[�^�擾
		break;
	}

	// �G���[���肩
	if( result != E_SUCCESS )
	{
		motor_STATUS( mm, &sts );										// ��Ԏ擾

		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// �p�����[�^��
							STRTBL_MSG[msg], 							// �R�}���h������
							result, 									// �G���[�R�[�h

							(INT)mm,									// ���[�^�P�ԍ�
							sts.pos, 									// �p���X�J�E���g�P
							sts.enc,									// �G���R�[�_�P
							sts.cw_limit, 								// CW���~�b�g�Z���T(0:OFF,1:ON)
							sts.ccw_limit, 								// CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 											// ���[�^�Q�ԍ�
							0, 											// �p���X�J�E���g�Q
							0,											// �G���R�[�_�Q
							0, 											// CW���~�b�g�Z���T(0:OFF,1:ON)
							0			 								// CCW���~�b�g�Z���T(0:OFF,1:ON)
						);

	    // �I���ʒm
		CmdEndResponse	(
							ID_TSK_MOTACT,
							prid1,
							bcid,
							msg,
							mm,
							NO_DATA,
							&msgdat
						);
	}
	else
	{
		result = mbx_send	(
								ID_TSK_MOTACT,
								ID_MPF_MOTEND,
								ID_MBX_MOTEND,
								prid1,
								bcid,
								msg,
								mm,
								mmpare,
								&bufwk[0],
								CMD_BUFSIZ
							);
	}

	return(result);
}
/***********************************************************************
*
* [�T�v]  STOP/ABORT    �p���X���[�^�����E����~
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static ER MotActSTOP   ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
 	UB		prid1;
 	UB		bcid;
 	UB		msg; 
	ST_STOP stop;
	ST_CNTLDATA			msgdat;									// ���b�Z�[�W�f�[�^

	
	memset( &stop, CLR,  sizeof( ST_STOP ) );					// ������
	prid1 = mrcv->msgdata.prid1;								// 
	bcid = mrcv->msgdata.bcid;									// 
	msg = mrcv->msgdata.msgid;									// ���b�Z�[�W�h�c�擾

	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &stop, &bufwk[0], sizeof(ST_STOP) );

	if( msg == ID_MSG_ABORT )
	{
		result = motor_ABORT( stop.mm );						// ����~
	}
	else
	{
		result = motor_STOP( stop.mm );							// ������~
	}

	// �G���[���肩
	if( result != E_SUCCESS )
	{
		motor_STATUS( stop.mm, &sts );							// ��Ԏ擾

		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&msgdat,
							PARAM7, 							// �p�����[�^��
							STRTBL_MSG[msg], 					// �R�}���h������
							result, 							// �G���[�R�[�h

							stop.mm, 							// ���[�^�P�ԍ�
							sts.pos, 							// �p���X�J�E���g�P
							sts.enc,							// �G���R�[�_�P
							sts.cw_limit, 						// CW���~�b�g�Z���T(0:OFF,1:ON)
							sts.ccw_limit, 						// CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 									// ���[�^�Q�ԍ�
							0, 									// �p���X�J�E���g�Q
							0,									// �G���R�[�_�Q
							0, 									// CW���~�b�g�Z���T(0:OFF,1:ON)
							0			 						// CCW���~�b�g�Z���T(0:OFF,1:ON)
						);

		
		// �I���ʒm(��TskCmd)
		CmdEndResponse	(
							ID_TSK_MOTACT,
							prid1,
							bcid,
							msg,
							stop.mm,
							NO_DATA,
							&msgdat
						);
	}
	else
	{
		result = mbx_send	(
								ID_TSK_MOTACT,
								ID_MPF_MOTEND,
								ID_MBX_MOTEND,
								prid1,
								bcid,
								msg,
								stop.mm,
								NO_DATA,
								&bufwk[0],
								CMD_BUFSIZ
							);
	}
	return(result);
}
