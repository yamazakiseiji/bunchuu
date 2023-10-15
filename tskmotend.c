/******************************************************************************
* ���[�^����I���Ď��^�X�N                                                    *
*                                                                             *
* File name : tskmotend.c                                                     *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "tskmotend.h"
#include "tskmotact.h"
#include "tskcmd.h"
#include "sub.h"
#include "mnet.h"
#include "tblchr.h"
#include "automachine.h"


//*************************************
// �����֐�
//*************************************
static  void MotEndInit();												// �E�F�C�g�v������
static  void MotEndInitSet( UB mm );									// ����p�f�[�^�����l�Z�b�g
static  void MotEndDlyTmo();											// �f�B���C�^�C���A�E�g�Ď�
static  void MotEndChk();												// �R�}���h���슮���`�F�b�N
static  void MotEndMulti( UB mm );										// 
static  void MotEndSigle( UB mm );										// 
static  void MotEndProc   ( T_CMD_MSG* mrcv );							// �E�F�C�g�v������
static  void CmdRcvMsg     ( T_CMD_MSG* mrcv );							// ���b�Z�[�W��M����
static  ER   MotEndAnalyze( T_CMD_MSG* mrcv );							// �R�}���h�̉��
static  ER   MotEndDummy     ( T_CMD_MSG* mrcv );						// 
static  ER   MotEndRequest   ( T_CMD_MSG* mrcv );						// ���[�^����O�̃E�F�C�g���N�G�X�g
static  ER   MotEndRequestW  ( T_CMD_MSG* mrcv );						// ���[�^����O�̃E�F�C�g���N�G�X�g
static  ER   MotEndPDELAY    ( T_CMD_MSG* mrcv );						// PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�


/***********************************************************************
* �e�[�u��
***********************************************************************/




//*************************************
// �f�[�^��`
//*************************************
static	ST_MOTWAIT			MotEnd[(DVMAX+1)];							// �E�F�C�g����f�[�^
static	ST_MOTOR_STATUS		sts;										// 
static	ST_MOTOR_STATUS		sts2;										// 


/******************************************************************************
* 
* [�T�v]	���[�^����I���Ď��^�X�N
* [����]	
* [�ߒl]	
* 
******************************************************************************/
TASK TskMotEnd( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotEndInit();

	for( ;; )
	{
        // �R�}���h�҂�
        err = trcv_mbx( ID_MBX_MOTEND, (T_MSG**)&msg, DLY_10M );

		if( err == E_TMOUT )
		{
			MotEndDlyTmo();
			MotEndChk();
			continue;
		}
		else
		{
			MotEndProc(msg);
		}
		// ���������
		RelMpf( ID_MPF_MOTEND, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	���[�^�E�F�C�g������
* [����]	
* [�ߒl]	
* 
******************************************************************************/
static void MotEndInit()
{
	UB Loop;

	memset( &MotEnd, CLR, sizeof( MotEnd ) );

	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		MotEndInitSet(Loop);
//		MotEnd[Loop].req     = DEFDAT;									// default
//		MotEnd[Loop].motpare = DEFDAT;									// default
//		MotEnd[Loop].prid1   = DEFDAT;									// default		
//		MotEnd[Loop].bcid    = DEFDAT;									// default		
//		MotEnd[Loop].msgid   = DEFDAT;									// default
//		MotEnd[Loop].inf     = DEFDAT;									// default
//		MotEnd[Loop].inf2    = DEFDAT;									// default
//		MotEnd[Loop].tim     = 10/DLY_10M;								// default
//		memset( &MotEnd[Loop].msgdat[0], CLR, CMD_BUFSIZ );
	}
}

/******************************************************************************
* 
* [�T�v]	����p�f�[�^�����l�Z�b�g
* [����]	���[�^�ԍ�
* [�ߒl]	�|
* 
******************************************************************************/
static void MotEndInitSet( UB mm )
{
	MotEnd[mm].req     = DEFDAT;									// default
	MotEnd[mm].motpare = DEFDAT;									// default
	MotEnd[mm].prid1   = DEFDAT;									// default		
	MotEnd[mm].bcid    = DEFDAT;									// default		
	MotEnd[mm].msgid   = DEFDAT;									// default
	MotEnd[mm].inf     = DEFDAT;									// default
	MotEnd[mm].inf2    = DEFDAT;									// default
	MotEnd[mm].tim     = 10/DLY_10M;								// default
	memset( &MotEnd[mm].msgdat[0], CLR, CMD_BUFSIZ );
}

/***********************************************************************
* 
* [�T�v]	�f�B���C�^�C���A�E�g�Ď�
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndDlyTmo()
{
	UB Loop;
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		switch( MotEnd[Loop].req )
		{
		case REQRDY:
			// Busy�L��Busy����
			if( motor_IsBusy(Loop) == FALSE )
			{
				// �㎥OFF���邩����
				if( GetPowerSts( Loop ) == FALSE )
				{
					// WAIT�Ȃ�
					if( MotEnd[Loop].tim == 0 )
					{
						MotEnd[Loop].req = REQEND;						// �I������
					}
					else
					{
						MotEnd[Loop].req = REQACT;						// �f�B���C����
					}
					motor_POWER( Loop, CLR );
				}
				else
				{
					MotEnd[Loop].req = REQEND;							// �f�B���C�v���I��
				}
			}
			else
			{
			}
			break;

		case REQACT:
			MotEnd[Loop].cnt --;										// �f�N�������g

			if( MotEnd[Loop].cnt <= 0 )
			{
				MotEnd[Loop].req = REQEND;								// �f�B���C�v���I��
			}
			break;

		default:
			break;
		}
	}
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���슮���`�F�b�N
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndChk()
{
	UB Loop;
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		if( MotEnd[Loop].msgid == ID_MSG_MAP )
		{
			MotEndMulti( Loop );
		}
		else
		{
			MotEndSigle(Loop);
		}
	}
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���슮����̃��X�|���X�����i�V���O���쓮�j
* [����]	mm:���[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndSigle( UB mm )
{
	ER  result = E_SUCCESS;
	ST_CNTLDATA			msgdat;										// ���b�Z�[�W�f�[�^

	// �f�B���C�I��������
	if( MotEnd[mm].req == REQEND )
	{
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );

		// ��Ԏ擾
		result = motor_STATUS( mm, &sts );

		// ���~�b�g�m�F
		if(
			( MotEnd[mm].msgid == ID_MSG_ABSMV )||
			( MotEnd[mm].msgid == ID_MSG_RELMV )||
			( MotEnd[mm].msgid == ID_MSG_MAP   ) )
		{
			// ���~�b�g���H
			if(( sts.cw_limit )||( sts.ccw_limit ))
			{
				result = ( E_LMT_ERR | mm );
			}
		}
		
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// �p�����[�^��
							STRTBL_MSG[MotEnd[mm].msgid], 				// �R�}���h������
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
							0											// CCW���~�b�g�Z���T(0:OFF,1:ON)
						);

	    // �������b�Z�[�W���M
		CmdEndResponse	(
							ID_TSK_MOTEND,                              // ���ID
							MotEnd[mm].prid1,                           // PROCID
							MotEnd[mm].bcid,                            // BCID
							MotEnd[mm].msgid,                           // ү����ID
							MotEnd[mm].inf,                             // ���1
							MotEnd[mm].inf2,                            // ���2
							&msgdat                                     // DATA
						);
		
		MotEndInitSet(mm);
	}
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���슮����̃��X�|���X�����i�}���`�쓮�j
* [����]	mm:���[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndMulti( UB mm )
{
	ER  result = E_SUCCESS;
	ST_CNTLDATA			msgdat;										// ���b�Z�[�W�f�[�^

	// �f�B���C�I��������
	if(
		( MotEnd[mm].req == REQEND )
	)
	{
		memset( &sts,   CLR,  sizeof( ST_MOTOR_STATUS ) );

		// ��Ԏ擾
		result = motor_STATUS( mm, &sts );

		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&msgdat,
							PARAM7, 									// �p�����[�^��
							STRTBL_MSG[MotEnd[mm].msgid], 				// �R�}���h������
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
							0											// CCW���~�b�g�Z���T(0:OFF,1:ON)
						);

	    // �������b�Z�[�W���M
		CmdEndResponse	(
							ID_TSK_MOTEND,
							MotEnd[mm].prid1,  
							MotEnd[mm].bcid,
							MotEnd[mm].msgid,
// 05/23					mm,
// 05/23					MotEnd[mm].motpare,
							MotEnd[mm].inf,                             // ���1
							MotEnd[mm].inf2,                            // ���2
							&msgdat
						);
		
		MotEndInitSet(mm);
	}
}

/***********************************************************************
* 
* [�T�v]	�E�F�C�g�v������
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_CMD:	// TSK_CMD->
	case ID_TSK_MOTACT:	// TSK_MOTACT->
	case ID_TSK_INT:	// TSK_INT->
		CmdRcvMsg(mrcv);
		break;
	case ID_TSK_RX0:	// TSK_RX0->
	case ID_TSK_TX0:	// TSK_TX0->
	case ID_TSK_RXEX:	// TSK_RX2->
	case ID_TSK_TXEX:
	case ID_TSK_EEP:	// TSK_EEP->
	default:
		break;
	}
}
/***********************************************************************
* 
* [�T�v]	���b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdRcvMsg( T_CMD_MSG* mrcv )
{
	ER			err;

	err = MotEndAnalyze( mrcv );
}
/***********************************************************************
* 
* [�T�v]	�R�}���h�̉��
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static ER MotEndAnalyze( T_CMD_MSG* mrcv )
{
	ER			err = E_SUCCESS;
	UB			msgid;

	// ���b�Z�[�W�h�c�擾
	msgid = mrcv->msgdata.msgid;

	if( ( msgid >= ID_MSG_HELLO ) && ( msgid <= ID_MSG_BASE_CMD_MAX	 ))
	{
		switch( msgid )
		{
			case ID_MSG_INT   :
			case ID_MSG_ABSMV :
			case ID_MSG_ABORT :
			case ID_MSG_LIMIT :
			case ID_MSG_MAP   :
			case ID_MSG_RELMV :
			case ID_MSG_STOP  :
				MotEndRequest( mrcv );
				break;
			case ID_MSG_PDELAY:
				MotEndPDELAY( mrcv );
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
* [����]	msg:���b�Z�[�WID�Amm1:���[�^�Amm2:���[�^�Adata:���b�Z�[�W�f�[�^
* [�ߒl]	�G���[�R�[�h
* 
***********************************************************************/
static ER MotEndDummy( T_CMD_MSG* mrcv )
{
	memset( mrcv, CLR,  sizeof( T_CMD_MSG ) );
	
	// �K�v�ł���΃G���[�o��
	return(E_SYS_FNCCAL);
}
/***********************************************************************
* 
* [�T�v]	���[�^����O�̃E�F�C�g���N�G�X�g
* [����]	msg:���b�Z�[�WID�Amm1:���[�^�Amm2:���[�^�Adata:���b�Z�[�W�f�[�^
* [�ߒl]	�G���[�R�[�h
* 
***********************************************************************/
static ER MotEndRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm;
	UB		mm2;

	// ���[�^�擾
	mm = mrcv->msgdata.inf;
	mm2 = mrcv->msgdata.inf2;

	MotEnd[mm].prid1 = mrcv->msgdata.prid1;							// procid�Z�b�g
	MotEnd[mm].bcid  = mrcv->msgdata.bcid;							// BCID�Z�b�g
	MotEnd[mm].msgid = mrcv->msgdata.msgid;							// ���b�Z�[�W�h�c�Z�b�g
	MotEnd[mm].inf   = mrcv->msgdata.inf;							// 
	MotEnd[mm].inf2  = mrcv->msgdata.inf2;							// 

	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &MotEnd[mm].msgdat[0], &mrcv->msgdata.data[0], CMD_BUFSIZ );

	MotEnd[mm].req = REQRDY;											// ���[�^���쒆�̂��߃f�B���C�A�N�V�������f�B
	MotEnd[mm].cnt = MotEnd[mm].tim;

	// ���[�^�y�A�Z�b�g
	MotEnd[mm].motpare = mm2;
	
	return(result);
}

/***********************************************************************
* 
* [�T�v]	���[�^�E�F�C�g��Ԏ擾
* [����]	���[�^�ԍ�
* [�ߒl]	�|
* 
***********************************************************************/
UB GetMotEnd( UB mot  )
{
	return( MotEnd[mot].req );
}

/***********************************************************************
*
* [�T�v]  PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static ER MotEndPDELAY ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	ST_PDELAY pdelay;

	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &pdelay, &mrcv->msgdata.data[0], sizeof(ST_PDELAY) );

	// �f�B���C���Ԑݒ�
	MotEnd[pdelay.mm].tim = ( pdelay.tttt / 10 );

	return(result);
}
