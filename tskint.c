/******************************************************************************
* �h�m�s�R�}���h�Ď��^�X�N                                                    *
*                                                                             *
* File name : tskint.c                                                        *
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
#include "tskint.h"
#include "automachine.h"
#include "tblchr.h"
#include "mnet.h"
#include "tskcmd.h"


//*************************************
// �֐���`
//*************************************
static void InttskInit();
static void IntInfoSet(T_CMD_MSG* mrcv);
static void IntProc();
static void IntTrgAction(ST_INTCNTL *cntl);
static BOOL MotStopChk( UB pm, UB trg );
static ER IntTrgChk(ST_INTCNTL *cntl);


//*************************************
// �e�[�u��
//*************************************


//*************************************
// �f�[�^��`
//*************************************
static	UB					ChDataOld[MAX_CH+1];						// 2020/2/12 �P�͂��܂�Ή�
static	ST_INTCNTL			stCntl[MAX_CH+1];							// 2020/2/12 �P�͂��܂�Ή�
static	ST_CNTLDATA			rspdat;

/***********************************************************************
* ������
* 
* [�T�v]
* ������
* 
***********************************************************************/
TASK TskInt( void )
{
    T_CMD_MSG*  msg;
	ER          err;

	InttskInit();														// �h�m�s�R�}���h��񏉊�������
	
	for( ;; )
    {
        // �R�}���h�҂�
        err = trcv_mbx( ID_MBX_INT, (T_MSG**)&msg, DLY_1M );

    	if( err == E_TMOUT )
		{
			// INT�Ď�����
	    	IntProc();
			continue;
		}
    	else
    	{
			// INT���Z�b�g
	    	IntInfoSet(msg);
    	}
    	
		RelMpf( ID_MPF_INT, (VP)msg );									// ���������
    }
}

/***********************************************************************
* 
* [�T�v]	�h�m�s�R�}���h��񏉊�������
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void InttskInit()
{
	INT		Loop;

	memset( &stCntl,      DEFDAT,  sizeof( stCntl ) );
	memset( &ChDataOld,   DEFDAT,  sizeof( ChDataOld ) );

	for( Loop = MIN_CH; Loop <= INMAX; Loop++ )	// 2020/2/12 �P�͂��܂�Ή�
	{
		stCntl[Loop].intinf.nn = Loop;
	}
}

/***********************************************************************
* 
* [�T�v]	�h�m�s�R�}���h���ݒ菈��
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void IntInfoSet(T_CMD_MSG* mrcv)
{
	UB		ch;

	ch  = mrcv->msgdata.inf;

	stCntl[ch].tskid = mrcv->msgdata.tskid;
	stCntl[ch].prid1  = mrcv->msgdata.prid1;
	stCntl[ch].bcid  = mrcv->msgdata.bcid;
	stCntl[ch].msgid = mrcv->msgdata.msgid;
	
	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &stCntl[ch].intinf, &mrcv->msgdata.data[0], sizeof(ST_INTINF) );
}

/***********************************************************************
* 
* [�T�v]	�h�m�s�R�}���h�Ď�
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void IntProc()
{
	INT		Loop;
	
	for( Loop = MIN_CH; Loop <= MAX_CH; Loop++ )	// 2020/2/12 �P�͂��܂�Ή�
	{
		// �g���K�������H
		if( IntTrgChk(&stCntl[Loop]) != E_CONTINUE )
		{
			// �g���K�������̏������s
			IntTrgAction( &stCntl[Loop] );
		}
	}
}
/***********************************************************************
* 
* [�T�v]	�h�m�s�R�}���h�g���K�������̏������s
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void IntTrgAction(ST_INTCNTL *cntl)
{
	UB inpHL;
	INT		Loop;
	INT		SubLoop;
	INT		chloop;
	UB		chno;
	UB		motstop;
	UB		outdat;
	UB		outch;
	UB		chkbit;
	BOOL	stopflg;

	if( cntl->intinf.nn == DEFDAT )		return;

	chloop = (MAX_CH / 8) + 1;

	// �g���K�������̏���
	for( Loop = 0; Loop < chloop; Loop++ )
	{
		chkbit = 0x01;
		motstop = cntl->intinf.mmm[Loop] ;
		outdat  = cntl->intinf.dat[Loop] ;
		outch   = cntl->intinf.msk[Loop] ;

		for( SubLoop = 0; SubLoop < 8; SubLoop++ )
		{
			// CH�ԍ�
			chno = ( Loop * 8 ) + SubLoop + 1;
			stopflg = MotStopChk( chno, cntl->intinf.trg ) ;
			
			// ���[�^��~( = 1 )
			if(( motstop & chkbit ))
			{
				// ��~�Ώە����̃��[�^���H
				if( stopflg == TRUE )
				{
					motor_ABORT( chno );
				}
			}

			// �o�͑Ώ�( = 1 )
			if(( outch & chkbit ))
			{
				// �o�̓f�[�^�g
				if(( outdat  & chkbit ))
				{
					pio_BOUT( chno, TRUE );
				}
				// �o�̓f�[�^�k
				else
				{
					pio_BOUT( chno, FALSE );
				}
			}

			chkbit = ( chkbit << 1 );
		}
	}

	// �u���b�N�R�}���h�ɂ��������s���̓N���A
	if( cntl->bcid != NO_DATA )
	{
		// �g���K�������������̂Ńg���K�N���A
		cntl->intinf.trg = TRG_NM;
	}

	// ��Ԏ擾
	inpHL = pio_BIN( cntl->intinf.nn );
	
	// ���X�|���X���M
	// �������b�Z�[�W�쐬
	CmdMakeResponseInt( &rspdat,
						PARAM3, 								// �p�����[�^��
						STRTBL_MSG[ID_MSG_INTRPT], 				// �R�}���h������
						cntl->intinf.nn,						// 

						inpHL									// 
					);

	// ��{�R�}���h�̏ꍇ�͈ȉ��̏���
	if( cntl->bcid == NO_DATA )
	{
		SendResponse( &rspdat );

		// 
		// CMDTSK�� Interrupt�ɂ���~��ʒm����K�v����A�֘A����Ǘ��f�[�^���N���A���邽��
		// 

		// ��~���[�^�����R�s�[
		memcpy( &rspdat.rsp.buf[0], &cntl->intinf.mmm[0], 8 );
		
		// �������b�Z�[�W���M
		CmdEndResponse	(
							ID_TSK_INT,
							cntl->prid1,
							NO_DATA,
							cntl->msgid,
							cntl->intinf.nn,
							NO_DATA,
							&rspdat
						);
	}
	// �a�b�R�}���h�̓W�J�n�̃R�}���h�ł͂Ȃ��ꍇ�͈ȉ��̏���
	else
	{
	    // �������b�Z�[�W���M
		CmdEndResponse	(
							ID_TSK_INT,
							cntl->prid1,
							cntl->bcid,
							cntl->msgid,
							cntl->intinf.nn,
							NO_DATA,
							&rspdat
						);
	}



}

/***********************************************************************
* 
* [�T�v]	���[�^��~�`�F�b�N
* [����]	pm:���[�^�Atrg:�g���K
* [�ߒl]	TRUE:��~�ΏہAFALSE:��~��Ώ�
* 
***********************************************************************/
static BOOL MotStopChk( UB pm, UB trg )
{
	BOOL result = FALSE;
 	RSTS rsts;
 	UB	 sdir;

	// RSTS�擾
	GetMotorRegister((UH)pm, RRG_RSTS, &rsts.UNID.data );

	// ��������擾
	sdir = ( rsts.UNID.BIT.SDIR )?  MINUS : PLUS;

	switch(trg)
	{
		case TRG_NM:                                                    // 0  N �F�ʏ���͐ݒ�(�����ݒ����) 
			// �ΏۂȂ�
			break;
		case TRG_HD:                                                    // 1  HD�FHigh���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
		case TRG_LD:                                                    // 4  LD�FLow���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
		case TRG_UD:                                                    // 7  UD�F�����オ��G�b�W���o���ACW/CCW���������Ɉꎞ��~
		case TRG_DD:                                                    // 10 DD�F����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
		case TRG_ED:                                                    // 13 ED�F�����オ��^����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
			result = TRUE;
			break;
		case TRG_HP:                                                    // 2  H+�FHigh���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
		case TRG_LP:                                                    // 5  L+�FLow���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
		case TRG_UP:                                                    // 8  U+�F�����オ��G�b�W���o���ACW�����݈̂ꎞ��~
		case TRG_DP:                                                    // 11 D+�F����������G�b�W���o���ACW�����݈̂ꎞ��~
		case TRG_EP:                                                    // 14 E+�F�����オ��^����������G�b�W���o���ACW�����݈̂ꎞ��~
			if( sdir == PLUS )	result = TRUE;
			break;
		case TRG_HM:                                                    // 3  H-�FHigh���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
		case TRG_LM:                                                    // 6  L-�FLow���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
		case TRG_UM:                                                    // 9  U-�F�����オ��G�b�W���o���ACCW�����݈̂ꎞ��~
		case TRG_DM:                                                    // 12 D-�F����������G�b�W���o���ACCW�����݈̂ꎞ��~
		case TRG_EM:                                                    // 15 E-�F�����オ��^����������G�b�W���o���ACCW�����݈̂ꎞ��~
			if( sdir == MINUS )	result = TRUE;
			break;
	}
	return( result);
}

/***********************************************************************
* 
* [�T�v]	�h�m�s�R�}���h�Ď�
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static ER IntTrgChk(ST_INTCNTL *cntl)
{
	UB sts;
	ER result = E_CONTINUE;


	// ���ڑ�
	if( cntl->intinf.nn == DEFDAT )		return( result );

	// ��Ԏ擾
	sts = pio_BIN( cntl->intinf.nn );

	// �ŏ��̈ꔭ��
	if( ChDataOld[cntl->intinf.nn] == DEFDAT )
	{
		ChDataOld[cntl->intinf.nn] = sts;
		return( result );
	}

	switch(cntl->intinf.trg)
	{
		case TRG_HD:                                                    // HD  �FHigh���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
		case TRG_HP:                                                    // H+  �FHigh���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
		case TRG_HM:                                                    // H-  �FHigh���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
			if(( ChDataOld[cntl->intinf.nn] == SET )&&( sts == SET ))
			{
				result = E_SUCCESS;
			}
			break;
		case TRG_LD:                                                    // LD  �FLow���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
		case TRG_LP:                                                    // L+  �FLow���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
		case TRG_LM:                                                    // L-  �FLow���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
			if(( ChDataOld[cntl->intinf.nn] == CLR )&&( sts == CLR ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_UD:                                                    // UD  �F�����オ��G�b�W���o���ACW/CCW���������Ɉꎞ��~
		case TRG_UP:                                                    // U+  �F�����オ��G�b�W���o���ACW�����݈̂ꎞ��~
		case TRG_UM:                                                    // U-  �F�����オ��G�b�W���o���ACCW�����݈̂ꎞ��~
			if(( ChDataOld[cntl->intinf.nn] == CLR )&&( sts == SET ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_DD:                                                    // DD  �F����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
		case TRG_DP:                                                    // D+  �F����������G�b�W���o���ACW�����݈̂ꎞ��~
		case TRG_DM:                                                    // D-  �F����������G�b�W���o���ACCW�����݈̂ꎞ��~
			if(( ChDataOld[cntl->intinf.nn] == SET )&&( sts == CLR ))
			{
				result = E_SUCCESS;
			}
			break;

		case TRG_ED:                                                    // ED  �F�����オ��^����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
		case TRG_EP:                                                    // E+  �F�����オ��^����������G�b�W���o���ACW�����݈̂ꎞ��~
		case TRG_EM:                                                    // E-  �F�����オ��^����������G�b�W���o���ACCW�����݈̂ꎞ��~
			if( (ChDataOld[cntl->intinf.nn]^sts) )
			{
				result = E_SUCCESS;
			}
			break;
		case TRG_NM:                                                    // N   �F�ʏ���͐ݒ�(�����ݒ����) 
		default:
			break;
	}

	ChDataOld[cntl->intinf.nn] = sts;

	return( result );
}

