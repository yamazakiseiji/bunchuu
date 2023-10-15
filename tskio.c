/******************************************************************************
* ���o�͊Ď��^�X�N                                                            *
*                                                                             *
* File name : tskio.c                                                         *
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
#include "tskio.h"
#include "automachine.h"


//*************************************
// �֐���`
//*************************************
static void CountSet(T_CMD_MSG* mrcv);
static void CountProc();


//*************************************
// �e�[�u��
//*************************************


//*************************************
// �f�[�^��`
//*************************************
// �J�E���g�Ď�
static	ST_IOCOUNT			iocnt[MAX_CH+1];// 2020/2/12 �P�͂��܂�Ή�



/***********************************************************************
* ������
* 
* [�T�v]
* ������
* 
* 
***********************************************************************/
TASK TskIo( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	memset( &iocnt,  CLR,  sizeof( iocnt ) );

	for( ;; )
    {
        // �R�}���h�҂�
        err = trcv_mbx( ID_MBX_IO, (T_MSG**)&msg, DLY_1M );

    	if( err == E_TMOUT )
		{
			// �J�E���g����
	    	CountProc(msg);
			continue;
		}
    	else
    	{
			// ���b�Z�[�W����
	    	CountSet(msg);
    	}
    	
		RelMpf( ID_MPF_IO, (VP)msg );// ���������
    }
}

/***********************************************************************
* 
* [�T�v]	�J�E���g�ݒ菈��
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void CountSet(T_CMD_MSG* mrcv)
{
	UB		msg;
	UB		ch;

	msg = mrcv->msgdata.msgid;
	ch  = mrcv->msgdata.inf;

	if( msg == ID_MSG_CNT_START )
	{
		iocnt[ch].sts   = SET;
		iocnt[ch].cnt   = CLR;

		iocnt[ch].now = GetInputDat(ch);
		iocnt[ch].old = iocnt[ch].now;
	}
	
	if( msg == ID_MSG_CNT_STOP )
	{
		iocnt[ch].sts   = CLR;
	}
}

/***********************************************************************
* 
* [�T�v]	�J�E���g�ݒ菈��
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void CountProc()
{
	INT		Loop;

// ���ׂ��傫���̂Ŏ������͂b�g�̂݁A�Ď�
//	for( Loop = 0; Loop < MAX_CH; Loop++ )
	for( Loop = MIN_CH; Loop <= INMAX; Loop++ )
	{
		// �J�E���g�ΏہH
		if( iocnt[Loop].sts == SET )
		{
			iocnt[Loop].old = iocnt[Loop].now;
			iocnt[Loop].now = GetInputDat(Loop);

			// �k���g�i�O��k�A���݂g�j
			if( (!iocnt[Loop].old)&&( iocnt[Loop].now ) )
			{
				iocnt[Loop].cnt++;
			}
		}
	}
}

/***********************************************************************
* 
* [�T�v]	�J�E���g���擾
* [����]	ch:����ch
* [�ߒl]	�|
* 
***********************************************************************/
UINT GetCount(UB ch)
{
	UINT	count = CLR;

	if(( ch >= MIN_CH )&&( ch <= MAX_CH )) 	// 2020/2/12 �P�͂��܂�Ή�
	{
		count = iocnt[ch].cnt;
	}
	
	return( count );
}


