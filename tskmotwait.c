/******************************************************************************
* ���[�^�E�F�C�g����^�X�N                                                    *
*                                                                             *
* File name : tskmotwait.c                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "sh7237.h"
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "tskmotwait.h"
#include "tskcmd.h"
#include "sub.h"
#include "automachine.h"


//*************************************
// �֐���`
//*************************************

static  void MotWaitInit();												// �E�F�C�g�v������
static  void MotWaitChk();												// 
static  void MotWaitProc   ( T_CMD_MSG* mrcv );							// �E�F�C�g�v������
static  void CmdRcvMsg     ( T_CMD_MSG* mrcv );							// ���b�Z�[�W��M����
static  ER   MotWaitAnalyze( T_CMD_MSG* mrcv );							// �R�}���h�̉��
static  ER   MotWaitDummy     ( T_CMD_MSG* mrcv );						// 
static  ER   MotWaitRequest   ( T_CMD_MSG* mrcv );						// ���[�^����O�̃E�F�C�g���N�G�X�g
static  ER   MotWaitRequestW  ( T_CMD_MSG* mrcv );						// ���[�^����O�̃E�F�C�g���N�G�X�g
static  ER   MotWaitPDELAY    ( T_CMD_MSG* mrcv );						// PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�
static  ER   MotWaitTimeRequest( T_CMD_MSG* mrcv );
static void MotWaitTimeAllInit();
static void MotWaitTimeInit(UB bcid);


//*************************************
// �e�[�u��
//*************************************



//*************************************
// �f�[�^��`
//*************************************
static	ST_MOTWAIT			MotWait[(DVMAX+1)];							// �E�F�C�g����f�[�^
static	ST_MOTWAIT			WaitTime[BCCTRL_MAX];							// �E�F�C�g����f�[�^
//static	INT WaitTime[BCCTRL_MAX];										// �w�莞�Ԃv�`�h�s�p
static	INT IntervalCounter;
static	UB  inf2_dbg;

/******************************************************************************
* 
* [�T�v]	���[�^����^�X�N
* [����]	
* [�ߒl]	
* 
******************************************************************************/
TASK TskMotWait( void )
{
	T_CMD_MSG* 	msg;
	ER			err;

	MotWaitInit();
	MotWaitTimeAllInit();
	
	for( ;; )
	{
        // �R�}���h�҂�
        err = trcv_mbx( ID_MBX_MOTWAIT, (T_MSG**)&msg, DLY_10M );

		if( err == E_TMOUT )
		{
			MotWaitChk();
			continue;
		}
		else
		{
			MotWaitProc(msg);
		}
		// ���������
		RelMpf( ID_MPF_MOTWAIT, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	���[�^�E�F�C�g������
* [����]	
* [�ߒl]	
* 
******************************************************************************/
static void MotWaitInit()
{
	INT Loop;

	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		
		MotWait[Loop].req = CLR;			// default
		MotWait[Loop].tskid = DEFDAT;		// default
		MotWait[Loop].prid1 = DEFDAT;		// default
		MotWait[Loop].bcid  = DEFDAT;		// default
		MotWait[Loop].msgid = DEFDAT;		// default
		MotWait[Loop].tim = 100/DLY_10M;	// default
		MotWait[Loop].cnt = CLR;			// default

	}
	IntervalCounter = 0;
}
static void MotWaitTimeAllInit()
{
	INT Loop;
	
	for( Loop = 0; Loop < BCCTRL_MAX; Loop++ )
	{
		MotWaitTimeInit(Loop);
	}
	
	IntervalCounter = 0;
}
static void MotWaitTimeInit(UB bcid)
{
	WaitTime[bcid].req = CLR;			// default
	WaitTime[bcid].tskid = DEFDAT;		// default
	WaitTime[bcid].prid1 = DEFDAT;		// default
	WaitTime[bcid].bcid  = DEFDAT;		// default
	WaitTime[bcid].msgid = DEFDAT;		// default
	WaitTime[bcid].tim = CLR;			// default
	WaitTime[bcid].cnt = CLR;			// default
}

/***********************************************************************
* 
* [�T�v]	�E�F�C�g�Ď�
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void MotWaitChk()
{
	UB Loop;
	ER result;
	ID mpfid;
	ID mbxid;

	// �c�d�k�`�x���Ԍo�߂������`�F�b�N
	for( Loop = DVMIN; Loop <= DVMAX; Loop++  )
	{
		if( MotWait[Loop].req == SET )
		{
			MotWait[Loop].cnt --;										// �f�N�������g

			if( MotWait[Loop].cnt <= 0 )
			{
inf2_dbg = MotWait[Loop].motpare;
				// ����w��
				result = mbx_send(	MotWait[Loop].tskid,				//  tskid
							ID_MPF_MOTACT,								//  mpfid
							ID_MBX_MOTACT,								//  mbxid
							MotWait[Loop].prid1,						//  prid1
							MotWait[Loop].bcid,							//  bcid
							MotWait[Loop].msgid,						//  msgid
							Loop,										//  sts
							MotWait[Loop].motpare,						//  sts2
							&MotWait[Loop].msgdat[0],					//  data
							CMD_BUFSIZ                                  //  siz
						);
				// �Ď��f�[�^�N���A
				MotWait[Loop].req = CLR;			// default
				MotWait[Loop].cnt = CLR;			// default
				MotWait[Loop].msgid = DEFDAT;		// default
				MotWait[Loop].tim = 100/DLY_10M;		// default
			}
		}
	}
	// �w�莞�ԑ҂������`�F�b�N
	for( Loop = 0; Loop < BCCTRL_MAX; Loop++ )
	{
		// �҂����Ԃ���H
		if( WaitTime[Loop].tim )
		{
			WaitTime[Loop].tim--;

			// �҂��I��
			if( WaitTime[Loop].tim == CLR )
			{	
				if( WaitTime[Loop].tskid == ID_TSK_TMPCNT )
				{
					mpfid = ID_MPF_TMPCNT;
					mbxid = ID_MBX_TMPCNT;
				}
				else
				{
					mpfid = ID_MPF_CMD;
					mbxid = ID_MBX_CMD;
				}
				
				// ���b�Z�[�W�ʒm
				result = mbx_send(
									ID_TSK_MOTWAIT,
									mpfid,
									mbxid,
									WaitTime[Loop].prid1,
									Loop,
									ID_MSG_TIMWAIT,
									NO_DATA,
									NO_DATA,
									&Loop,
									sizeof( Loop )
								);
				MotWaitTimeInit(Loop);
			}
		}
	}
}

/***********************************************************************
* 
* [�T�v]	�E�F�C�g�v������
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotWaitProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_CMD:
	case ID_TSK_INT:
	case ID_TSK_TMPCNT:
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

	err = MotWaitAnalyze( mrcv );
}
/***********************************************************************
* 
* [�T�v]	�R�}���h�̉��
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static ER MotWaitAnalyze( T_CMD_MSG* mrcv )
{
	ER			err = E_SUCCESS;
	UB			msgid;

	// ���b�Z�[�W�h�c�擾
	msgid = mrcv->msgdata.msgid;

	if(
		(( msgid >= ID_MSG_HELLO ) && ( msgid <= ID_MSG_BASE_CMD_MAX))||
		( msgid == ID_MSG_TIMWAIT)
	)
	{
		// �R�[���t�@���N�V����
		switch( msgid )
		{
			case ID_MSG_ABSMV :
			case ID_MSG_ABORT :
			case ID_MSG_LIMIT :
			case ID_MSG_RELMV :
			case ID_MSG_STOP  :
				MotWaitRequest( mrcv );
				break;
			case ID_MSG_PDELAY:
				MotWaitPDELAY( mrcv );
				break;
			case ID_MSG_MAP   :
				MotWaitRequestW( mrcv );
				break;
			case ID_MSG_TIMWAIT :
				MotWaitTimeRequest( mrcv );
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
static ER MotWaitDummy( T_CMD_MSG* mrcv )
{
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
static ER MotWaitRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		mm;

	// �P����
	mm = mrcv->msgdata.inf;												// ���[�^�擾
	MotWait[mm].tskid = mrcv->msgdata.tskid;							// �^�X�N�h�c�Z�b�g
	MotWait[mm].prid1 = mrcv->msgdata.prid1;							// �v���Z�X�h�c
	MotWait[mm].bcid  = mrcv->msgdata.bcid;								// �a�b�h�c�Z�b�g
	MotWait[mm].msgid = mrcv->msgdata.msgid;							// ���b�Z�[�W�h�c�Z�b�g
	MotWait[mm].req   = SET;												// �f�B���C�J�n
	MotWait[mm].cnt   = MotWait[mm].tim;								// �f�B���C���Ԑݒ�
	MotWait[mm].motpare = mrcv->msgdata.inf2;							// ���[�^�y�A�Z�b�g
	memcpy( &MotWait[mm].msgdat[0],&mrcv->msgdata.data[0],CMD_BUFSIZ );	// �f�[�^�R�s�[

inf2_dbg = mrcv->msgdata.inf2;

	motor_POWER( mm, SET );	

	// �P����
	if( MotWait[mm].tim == 0 )
	{
		// ����w��
		mbx_send(	MotWait[mm].tskid,									// tskid
					ID_MPF_MOTACT,										// mpfid
					ID_MBX_MOTACT,										// mbxid
					MotWait[mm].prid1,									// prid1
					MotWait[mm].bcid,									// bcid
					MotWait[mm].msgid,									// msgid
					mm,													// sts
					MotWait[mm].motpare,								// sts2
					&MotWait[mm].msgdat[0],								// data
					CMD_BUFSIZ                                          // siz
				);
	}
	return(result);
}

/***********************************************************************
* 
* [�T�v]	���[�^����O�̃E�F�C�g���N�G�X�g
* [����]	msg:���b�Z�[�WID�Amm1:���[�^�Amm2:���[�^�Adata:���b�Z�[�W�f�[�^
* [�ߒl]	�G���[�R�[�h
* 
***********************************************************************/
static ER MotWaitRequestW( T_CMD_MSG* mrcv )
{
	UB		mm;
	UB		mm2;
	ER		result = E_SUCCESS;
	
	// �P����
	mm = mrcv->msgdata.inf;												// ���[�^�擾
	mm2 = mrcv->msgdata.inf2;											// ���[�^�擾
	MotWait[mm].tskid = mrcv->msgdata.tskid;							// �^�X�N�h�c�Z�b�g
//	MotWait[mm].tskid = ID_TSK_MOTWAIT;									// �^�X�N�h�c
	MotWait[mm].prid1 = mrcv->msgdata.prid1;							// �v���Z�X�h�c
	MotWait[mm].bcid = mrcv->msgdata.bcid;								// �a�b�h�c
	MotWait[mm].msgid = mrcv->msgdata.msgid;							// ���b�Z�[�W�h�c
	MotWait[mm].req = SET;												// �f�B���C�J�n
	MotWait[mm].cnt = MotWait[mm].tim;									// �f�B���C���Ԑݒ�
	MotWait[mm].motpare = mm2;											// ���[�^�y�A�Z�b�g
	memcpy( &MotWait[mm].msgdat[0],&mrcv->msgdata.data[0],CMD_BUFSIZ );	// �f�[�^�R�s�[

	motor_POWER( mm, SET );

	// WAIT����
	if( MotWait[mm].tim == 0 )
	{
		// ����w��
		mbx_send(	MotWait[mm].tskid,									// tskid
					ID_MPF_MOTACT,										// mpfid
					ID_MBX_MOTACT,										// mbxid
					MotWait[mm].prid1,									// prid1
					MotWait[mm].bcid,									// bcid
					MotWait[mm].msgid,									// msgid
					mm,													// sts
					mm2,												// sts2
					&MotWait[mm].msgdat[0],								// data
					CMD_BUFSIZ											// siz
				);
	}
	return(result);
}

/***********************************************************************
* 
* [�T�v]	�w�莞�Ԃv�`�h�s
* [����]	mrcv:���b�Z�[�W�f�[�^
* [�ߒl]	�G���[�R�[�h
* 
***********************************************************************/
static ER MotWaitTimeRequest( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	UB		bcid;
	UB		prid1;
	UB		msgid;
	INT		waittim = CLR;
	ID		mpfid;
	ID		mbxid;
	
	bcid = mrcv->msgdata.bcid;											// �a�b�h�c�Z�b�g

	if( bcid < BCCTRL_MAX )
	{
		prid1 = mrcv->msgdata.prid1;									// 
		msgid = mrcv->msgdata.msgid;
		// �w��v�`�h�s���Ԏ擾
		memcpy( &waittim, &mrcv->msgdata.data[0],sizeof( INT ) );
		
		// WAIT����
		if( waittim == 0 )
		{
			if( mrcv->msgdata.tskid == ID_TSK_TMPCNT )
			{
				mpfid = ID_MPF_TMPCNT;
				mbxid = ID_MBX_TMPCNT;
			}
			else
			{
				mpfid = ID_MPF_CMD;
				mbxid = ID_MBX_CMD;
			}
			
			// ���b�Z�[�W�ʒm
			result = mbx_send(
								ID_TSK_MOTWAIT,   // tskid
								mpfid,            // mpfid
								mbxid,            // mbxid
								prid1,            // prid1
								bcid,             // bcid
								ID_MSG_TIMWAIT,   // msgid
								NO_DATA,          // sts
								NO_DATA,          // sts2
								&bcid,            // data
								sizeof( bcid )    // siz
							);
		}
		else
		{
			// �w��v�`�h�s���ԃZ�b�g
			WaitTime[bcid].tskid = mrcv->msgdata.tskid;
			WaitTime[bcid].prid1 = prid1;
			WaitTime[bcid].bcid  = bcid;
			WaitTime[bcid].msgid = msgid;
			WaitTime[bcid].motpare  = 0;
			WaitTime[bcid].tim   = waittim;
			WaitTime[bcid].cnt   = waittim;
		}
	}
	
	return(result);
}
/***********************************************************************
* 
* [�T�v]	���[�^�E�F�C�g��Ԏ擾
* [����]	���[�^�ԍ�
* [�ߒl]	�|
* 
***********************************************************************/
UB GetMotWait( UB mot  )
{
	return( MotWait[mot].req );
}

/***********************************************************************
*
* [�T�v]  PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static ER MotWaitPDELAY ( T_CMD_MSG* mrcv )
{
	ER		result = E_SUCCESS;
	ST_PDELAY pdelay;

	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &pdelay, &mrcv->msgdata.data[0], sizeof(ST_PDELAY) );

	// �f�B���C���Ԑݒ�
	MotWait[pdelay.mm].tim = ( pdelay.tttt / DLY_10M );

	return(result);
}
