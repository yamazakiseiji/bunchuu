/******************************************************************************
* �V���A���ʐM�Q ��M�^�X�N                                                   *
*                                                                             *
* File name : tskrxEx.c                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/

// �i���j----------------------------------------------------------------------
//       �V���A���O�ʐM�F�o�b����Ɏg�p�i�Œ�j
//       �V���A���P�ʐM�F�d�d�o�q�n�l�A�N�Z�X�̂��߂r�o�h�ʐM�Ɏg�p�i�Œ�j
//       �V���A���Q�ʐM�F�^�b�`�p�l���ʐM
//       �V���A���R�ʐM�F�g���p�i�������g�t�c�h�̂��߃G�~���[�^�g�p���͎g���Ȃ��j
// �i���j----------------------------------------------------------------------


#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "tskrxEx.h"
#include "datastruct.h"
#include "sub.h"
#include "macro.h"

//*************************************
// �萔��`
//*************************************
#define COM2_BUFSIZ			128											// �ʐM�f�[�^��M�o�b�t�@�T�C�Y
#define KINDID_MAX			256											// 

//*************************************
// �֐���`
//*************************************
static void rx2_init();													// ������
typedef	ER	(*RX2_FUNC)( UB data );										// �R�[���t�@���N�V����
static	ER 	rx2_dmy( UB rxdata );										// �_�~�[
static	ER 	rx2_idle( UB rxdata );										// �ҋ@���
static	ER 	rx2_crrecv( UB rxdata );									// CR��M

static	ER 	rx2_etx( UB rxdata );										// 
static	ER 	rx2_sum1( UB rxdata );										// 
static	ER 	rx2_sum2( UB rxdata );										// 


//*************************************
// �e�[�u��
//*************************************

// �R�[���t�@���N�V����
RX2_FUNC	Rx2FuncTbl[3] = 
{
	&rx2_dmy     ,														// STS00
	&rx2_idle    ,														// STS01
	&rx2_crrecv  														// STS02
};

//*************************************
// �f�[�^��`
//*************************************
static	ST_SEQ2	seq2;													// 
static	UB		TplRecvData[512];										// 



/******************************************************************************
* 
* [�T�v]	�V���A���ʐM�i�g���p�j�^�X�N����
* [����]	�Ȃ�
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
TASK TskRxEx( void )
{
	T_COM_CHAR  rxd;
    ER          err;
	UB          rxdat;
	volatile struct st_sci*  sci2;

	// ����f�[�^������
	rx2_init();

	//�o�[�R�[�h���[�_�ʐM�ݒ�(Port2)
	sci2 = &SCI2;
	sci2->SCBRR          = 32;											// �{�[���[�g       38400bps
	sci2->SCSMR.BIT.CHR  = 0;											// �f�[�^��         8bit
	sci2->SCSMR.BIT.STOP = 0;											// �X�g�b�v�r�b�g   1bit
	sci2->SCSMR.BIT._PE  = 0;											// �p���e�B         �Ȃ�
	sio2_set(sci2);

	//�o�[�R�[�h���[�_�ʐM�J�n(Port2)
	sio2_init(SIO_OPEN);

	for( ; ; )
	{
		err = rcv_dtq( ID_DTQ_RXEX, &rxd.data );						// �f�[�^��M
		if( rxd.byte.type != COM_MTYP_CHR )								// �G���[�H
		{
			rx2_init();													// ������
			continue;
		}
		rxdat = rxd.byte.data;											// ��M�f�[�^�擾
		err = Rx2FuncTbl[seq2.rstatus]( rxdat ) ;						// �t�@���N�V�����e�[�u�����s
	}
}

/******************************************************************************
* 
* [�T�v]	�V���A���|�[�g�Q������
* [����]	act:OPEN/CLOSE�w��
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
ER sio2_init(INT act)
{
	// SCI2�ݒ�
	if( act == SIO_OPEN )
	{
		if( SCI2.SCSCR.BIT.TE )		return( E_COM_P_OPEN );
		if( SCI2.SCSCR.BIT.RE )		return( E_COM_P_OPEN );

		SCI2.SCSCR.BIT.RIE		= 1;									// ��M�f�[�^�t�����荞�݋���
		SCI2.SCSCR.BIT.TE		= 1;									// ���M����
		SCI2.SCSCR.BIT.RE		= 1;									// ��M����
	}
	else
	{
		SCI2.SCSCR.BIT.RIE		= 0;									// ��M�f�[�^�t�����荞�݋���
		SCI2.SCSCR.BIT.TE		= 0;									// ���M�֎~
		SCI2.SCSCR.BIT.RE		= 0;									// ��M�֎~
	}

	SCI2.SCSCR.BIT.TIE		= 0;										// ���M�f�[�^�G���v�e�B���荞�݋֎~
	SCI2.SCSCR.BIT.TEIE		= 0;										// ���M�I�����荞�݋֎~
	INTC.IPR16.BIT._SCI2	= INT_PRI_SCI2;								// �D�揇��

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [�T�v]	�V���A���Q�ʐM�ݒ�
* [����]	comm:�V���A���ݒ�f�[�^�A�h���X
* [�ߒl]	�Ȃ�
* 
******************************************************************************/
void sio2_set( volatile struct st_sci* comm )
{
	SCI2.SCSCR.BIT.RIE		= 0;										// ��M�f�[�^�t�����荞�݋���
	SCI2.SCSCR.BIT.TIE		= 0;										// ���M�f�[�^�G���v�e�B���荞�݋֎~
	SCI2.SCSCR.BIT.TEIE		= 0;										// ���M�I�����荞�݋֎~
	SCI2.SCSCR.BIT.MPIE		= 0;										// 
	SCI2.SCSCR.BIT.TE		= 0;										// ���M�֎~
	SCI2.SCSCR.BIT.RE		= 0;										// ��M�֎~

	
	// SCI2�ݒ�
	SCI2.SCSMR.BIT.CA    = 0;											// ����������
	SCI2.SCSMR.BIT.CHR   = comm->SCSMR.BIT.CHR;							// �f�[�^�r�b�g���i�V�A�W�j
	SCI2.SCSMR.BIT._PE   = comm->SCSMR.BIT._PE;							// �p���e�B�iNON,EVE,ODD�j
	SCI2.SCSMR.BIT.STOP  = comm->SCSMR.BIT.STOP;						// �X�g�b�v�r�b�g(NON,1,2)
	SCI2.SCSMR.BIT.CKS   = 0;											// �N���b�N�Z���N�g
	SCI2.SCBRR           = comm->SCBRR;									// �{�[���[�g(9600,14400,19200,38400)
}

/***********************************************************************
* 
* [�T�v]	�R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��
* [����]	exinf:OS�g�����
* [�ߒl]	�Ȃ�
* 
***********************************************************************/
void AlmComR2Dat( VP_INT exinf )
{
	VP_INT dmyint;
	
	dmyint = exinf;														// �ǂݎ̂�
	
	rx2_init();															// ������
}
/***********************************************************************
* 
* [�T�v]	�|�[�g��Ԏ擾
* [����]	�Ȃ�
* [�ߒl]	TRUE:�I�[�v���AFALSE:�N���[�Y
* 
***********************************************************************/
BOOL GetPortActive()
{
	BOOL	actflg = FALSE;

	if(( SCI2.SCSCR.BIT.TE )&&( SCI2.SCSCR.BIT.RE ))
	{
		actflg = TRUE;
	}

	return( actflg );
}

/***********************************************************************
* 
* [�T�v]	�_�~�[
* [����]	rxdata:��M�f�[�^
* [�ߒl]	���
* 
***********************************************************************/
static ER rx2_dmy( UB rxdata )
{
	UB dmy;
    ER          err = E_SUCCESS;
	
	dmy = rxdata;	// �ǂݎ̂�
	return( err );
}

/***********************************************************************
* 
* [�T�v]	������
* [����]	�Ȃ�
* [�ߒl]	�Ȃ�
* 
***********************************************************************/
static void rx2_init()
{
	stp_alm( ID_ALM_R2DAT );

	memset( &seq2, 0x00, sizeof( seq2 ) );
	
	seq2.rstatus = STS01;
}

/***********************************************************************
* 
* [�T�v]	�A�C�h�����O
* [����]	�Ȃ�
* [�ߒl]	�Ȃ�
* 
***********************************************************************/
static ER rx2_idle( UB rxdata )
{
    ER  err = E_SUCCESS;

	switch( rxdata )
	{
		case STX:
			memset( &TplRecvData[0], 0x00, sizeof(TplRecvData) );

			seq2.type = RCVCMD;
			seq2.rstatus = STS02;
			TplRecvData[0] = rxdata;
			seq2.wpnt = 1;
			sta_alm( ID_ALM_R2DAT, TMOUT_RCV );
			break;
		case ACK:
			err = mbx_send	(
								ID_TSK_RXEX,
								ID_MPF_CMD,
								ID_MBX_CMD,
								NO_DATA,
								NO_DATA,
								ID_MSG_ACK,
								0, 
								0, 
								&rxdata,
								sizeof( rxdata )
							);
			rx2_init();
			break;
		case NAK:
			err = mbx_send	(
								ID_TSK_RXEX,
								ID_MPF_CMD,
								ID_MBX_CMD,
								NO_DATA,
								NO_DATA,
								ID_MSG_NAK,
								0, 
								0, 
								&rxdata,
								sizeof( rxdata )
							);
			rx2_init();
			break;
		default:
			rx2_init();
			break;
	}

	return( err );
}
static 	UB	ubDatSiz  ;
static 	UB	ubCalSiz  ;
static 	UB	ubCalSum  ;
static 	UB	ubRcvSum  ;
static 	UB	ubRcvSum1 ;
static 	UB	ubRcvSum2 ;
static 	UB	ubKindId  ;

/***********************************************************************
* 
* [�T�v]	���s�R�[�h�iCR�j�܂Ŏ�M����
* [����]	�Ȃ�
* [�ߒl]	�Ȃ�
* 
***********************************************************************/
static ER rx2_crrecv( UB rxdata )
{
	ER	err       = E_CONTINUE;
#if 0
	UB	ubDatSiz  = CLR;
	UB	ubCalSiz  = CLR;
	UB	ubCalSum  = CLR;
	UB	ubRcvSum  = CLR;
	UB	ubRcvSum1 = CLR;
	UB	ubRcvSum2 = CLR;
	UB	ubKindId  = CLR;
#endif
	
	if(seq2.wpnt < COM2_BUFSIZ)
	{
		TplRecvData[seq2.wpnt++] = rxdata;

		if( rxdata == LFCHR )
		{
			seq2.rstatus++;
			
			if( seq2.wpnt >= 10 )										// STX A$ ����ID ETX SUM1 SUM2 CR LF  �Œ�ł�10byte�L��
			{
				ubDatSiz  = seq2.wpnt - 6;
				ubRcvSum1 = AsctoHex( TplRecvData[seq2.wpnt - 4] );
				ubRcvSum2 = AsctoHex( TplRecvData[seq2.wpnt - 3] );
				
				ubCalSum = CalcBCC( &TplRecvData[1], ubDatSiz );		// STX�̎��`ETX
				ubRcvSum = ubRcvSum1 * 0x10 + ubRcvSum2 ;				// 
				
				if( ubCalSum == ubRcvSum )
				{
					ubKindId = AsctoHex( TplRecvData[3]  ) * 0x10 + AsctoHex( TplRecvData[4] );	// ����ID�擾

					if(( ubKindId >= ID_TPL_FWVER )&&( ubKindId <= ID_TPL_PRINF ))// ��������`�F�b�N
					{
						err = E_SUCCESS;
						SetRcvData( &TplRecvData[1],  ubDatSiz );
					}
					else
					{
						err = E_COM_CMDDEF;								// �R�}���h����`
					}
				}
				else
				{
					err = E_COM_BCCERR;									// BCC�s��v
				}
			}
			else
			{
				err = E_PAR_SHORTG;										// �p�����[�^�ߕs��
			}
		}
	}
	else
	{
		err = E_PAR_OUTRNG;												// �p�����[�^�͈͊O
	}

	switch( err )
	{
		// �����E����
		case E_SUCCESS:
//			SendACK();													// ACK���M
			err = mbx_send	(
								ID_TSK_RXEX,							// ���M���^�X�N�h�c
								ID_MPF_CMD,								// �������v�[���h�c
								ID_MBX_CMD,								// ���[���{�b�N�X�h�c
								ubKindId,								// �v���Z�X�h�c
								NO_DATA,								// bcid
								NO_DATA,								// msgid
								0, 										// inf1
								0, 										// inf2
								&TplRecvData[0],						// ��M�f�[�^
								ubDatSiz								// ��M�T�C�Y
							);
		
			rx2_init();													// ������
			break;

		// �V�X�e���G���[(�a�b�b�G���[)
		case E_COM_BCCERR:

		// ����`�R�}���h
		case E_COM_CMDDEF:

		// ��L�ȊO
		default:
			if( ubKindId == ID_TPL_EXRUN )
			{
				err = mbx_send	(
									ID_TSK_RXEX,							// ���M���^�X�N�h�c
									ID_MPF_CMD,								// �������v�[���h�c
									ID_MBX_CMD,								// ���[���{�b�N�X�h�c
									ubKindId,								// �v���Z�X�h�c
									NO_DATA,								// bcid
									NO_DATA,								// msgid
									0, 										// inf1
									0, 										// inf2
									(UB *)&err,									// ��M�f�[�^
									sizeof(err)								// ��M�T�C�Y
								);
			}
			else
			{
				rx2_init();													// ������
				err = E_SUCCESS;
			}
			break;

		// �p����
		case E_CONTINUE:
			break;
	}
	
	return( err );

}
