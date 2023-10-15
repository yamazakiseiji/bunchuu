/******************************************************************************
* �����퐧��^�X�N                                                            *
*                                                                             *
* File name : tsktmp.c                                                        *
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
#include "sub.h"
#include "automachine.h"
#include "tsktmpcnt.h"
#include "tblchr.h"

//*************************************
// define
//*************************************
#define	FZ110				1					// 0:RS232C,1:RS485
#define	SCI_16C554A_ADDR	0x0C00F000			// �V���A���i16C554_A�j�A�h���X
#define	SCI_16C554B_ADDR	0x0C00F008			// �V���A���i16C554_B�j�A�h���X
#define	SCI_16C554C_ADDR	0x0C00F010			// �V���A���i16C554_C�j�A�h���X
#define	SCI_16C554D_ADDR	0x0C00F018			// �V���A���i16C554_D�j�A�h���X
#define	TMPDATA_SIZE		8					// 
#define	BUFF_MAX			16					// 
#define	SEND_SIZE			8					// 
#define	RECV_SIZE			8					// 
//#define	SLAVE_ADR			0x00				// 
#define	SLAVE_ADR			0x01				// // Leagic�ǉ�2023.8.22
#define	FC_TMPGET			0x03				// 
#define	FC_TMPSET			0x06				// 
#define	RECV_TOUT			1000				// 
#define	RTS_WAIT			2					// 
#define	DAT_WAIT			10					// 
#define	TMPGET_RCVSIZE		7					// 
#define	TMPSET_RCVSIZE		8					// 
//#define TIMEOUT_RCV			1000000				// // Leagic�ǉ�2023.8.25
#define TIMEOUT_RCV			2000000				// // Leagic�ǉ�2023.8.25
												// ��500000�Ŏ����܂������ʐMNG�ł����B



//*************************************
// �����֐�
//*************************************
static void TmpCntInit();
static void check_main_serial_initialize( void );
static void TmpCntrlReq( T_CMD_MSG* mrcv );
static ER   TmpRecvProc( unsigned char msgid, unsigned short *temp );
static void TmpSendDataMake( unsigned char msgid,unsigned char dat );
static ER   TmpDataSend( unsigned char *buf, unsigned char siz );
static ER   TmpDataRecv( unsigned char *buf, unsigned char *siz );
       void TmpDataSet( unsigned char tmpdat );
unsigned char TmpDataGet();
static unsigned short CrcCal(unsigned char *buf,unsigned char siz);


//*************************************
// �f�[�^��`
//*************************************
typedef struct {
	unsigned char dat;
	unsigned char step;
	unsigned char req;
	unsigned char dmy;
}ST_TMPDAT;

static ST_TMPDAT tmp;
static unsigned char	txbuf[BUFF_MAX];
static unsigned char	rxbuf[BUFF_MAX+100];
static unsigned char	rxcnt;
static unsigned char	tempdat_bk[2];// [0]new,[1]old
static	UB 				bufwk[CMD_BUFSIZ];							// 

volatile struct	St16550	*p16C554;
	T_CMD_MSG* 	msg;

/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
TASK TskTmpCnt( void )
{
	ER			err;

	TmpCntInit();
	
	for( ;; )
	{
        err = rcv_mbx( ID_MBX_TMPCNT, (T_MSG**)&msg );
    	if( E_OK != err )		continue;

		TmpCntrlReq(msg);

		// ���������
		RelMpf( ID_MPF_TMPCNT, (VP)msg );
	}
}
/******************************************************************************
* 
* [�T�v]	������
* [����]	
* [�ߒl]	
* 
******************************************************************************/
static void TmpCntInit()
{
	memset( &tmp.dat,   CLR,    sizeof( ST_TMPDAT ) );
	check_main_serial_initialize();
	
}
//==============================================================================
//	Function	: check_main_serial_initialize
//	Description	: ���C����̃V���A�������̏�����
//	Parameters	: �Ȃ�
//	Return		: �Ȃ�
//	Note		: 
//	Global		: 
//==============================================================================
void	check_main_serial_initialize( void )
{
#if FZ110 == 0
	//----------------------------------
	// �E16C554A
	//----------------------------------
	p16C554 = (struct St16550 *)SCI_16C554A_ADDR;
	p16C554->REG3.LCR = 0x80 | 0x1B;			// ���C���R���g���[�� with �f�o�C�U
//	p16C554->REG0.DLL = 0x20;					// �{�[���[�g : 4.9152MHz / 9600  / 16 = 32
//	p16C554->REG0.DLL = 0x10;					// �{�[���[�g : 4.9152MHz / 19200 / 16 = 16
	p16C554->REG0.DLL = 0x08;					// �{�[���[�g : 4.9152MHz / 38400 / 16 = 8
	p16C554->REG1.DLH = 0x00;					// STOP = 1
	p16C554->REG3.LCR = 0x00 | 0x1B;			// ���C���R���g���[��
	p16C554->REG2.FCR = 0x00;					// FIFO�f�B�Z�[�u��
	p16C554->REG1.IER = 0x00;					// ���荞�ݖ��g�p

	p16C554->REG4.BIT.RTS = 0;
	dly_tsk(RTS_WAIT);
	
#else
	//----------------------------------
	// �E16C554D
	//----------------------------------
	p16C554 = (struct St16550 *)SCI_16C554D_ADDR;
	p16C554->REG3.LCR = 0x80 | 0x1B;			// ���C���R���g���[�� with �f�o�C�U
//	p16C554->REG0.DLL = 0x20;					// �{�[���[�g : 4.9152MHz / 9600 / 16 = 32
	p16C554->REG0.DLL = 0x08;					// �{�[���[�g : 4.9152MHz / 38400 / 16 = 8
	p16C554->REG1.DLH = 0x00;					// STOP = 1
	p16C554->REG3.LCR = 0x00 | 0x1B;			// ���C���R���g���[��
	p16C554->REG2.FCR = 0x00;					// FIFO�f�B�Z�[�u��
	p16C554->REG1.IER = 0x00;					// ���荞�ݖ��g�p
	
	p16C554->REG4.BIT.RTS = 0;
	dly_tsk(RTS_WAIT);
#endif
}

/***********************************************************************
* 
* [�T�v]	�R�}���h���N�G�X�g
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static void TmpCntrlReq( T_CMD_MSG* mrcv )
{
	ER result = E_RECV_NONE;
	ST_CNTLDATA   dbgdata;
	unsigned char prid1;
	unsigned char bcid;
	unsigned char msgid;
	unsigned char inf;
	unsigned char inf2;
	unsigned char dat;
	unsigned short tempdat = CLR;
	INT waittim;
	
	prid1  = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	msgid = mrcv->msgdata.msgid;
	inf   = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	
	switch(msgid)
	{
	case ID_MSG_TMPSET:
	case ID_MSG_TMPSET_HS:
	case ID_MSG_TMPGET:
	case ID_MSG_TMPGET_HS:
		// ��x�A���x�f�[�^�擾
		TmpSendDataMake( ID_MSG_TMPGET, 0 );
		result = TmpRecvProc( ID_MSG_TMPGET, &tempdat );
		
		if(
			( msgid == ID_MSG_TMPSET )    ||
			( msgid == ID_MSG_TMPSET_HS )
		)
		{
			dat  = mrcv->msgdata.data[0];
			TmpSendDataMake( msgid, dat );
			result = TmpRecvProc( msgid, &tempdat );
		}
		
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&dbgdata,
							PARAM3, 									// �p�����[�^��
							STRTBL_MSG[msgid], 							// �R�}���h������
							result, 									// �G���[�R�[�h

							tempdat,									// 
							0, 											// 
							0,											// 
							0, 											// 
							0, 											// 

							0, 											// 
							0, 											// 
							0,											// 
							0, 											// 
							0											// 
						);
		
	    // �������b�Z�[�W���M
		CmdEndResponse	(
							ID_TSK_TMPCNT,                              // ���ID
							prid1,                                      // PROCID
							bcid,                                       // BCID
							msgid,                                      // ү����ID
							inf,                                        // ���1
							inf2,                                       // ���2
							&dbgdata                                    // DATA
						);
		break;
		
		
	case ID_MSG_TMPGETR:
		TmpSendDataMake( ID_MSG_TMPGET, 0 );
		result = TmpRecvProc( ID_MSG_TMPGET, &tempdat );
		break;
		
		
	default:
		break;
	}
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���N�G�X�g
* [����]	
* [�ߒl]	���x�f�[�^
* [���l]	
*                            0  1  2  3  4  5  6  7
*   ID_MSG_TMPGE     [SEND] 01 03 00 00 00 02 C4 0B
*   ID_MSG_TMPGE     [RECV] 01 03 04 00 1B 8A 3D        ��[3][4]���x
* 
*   ID_MSG_TMPSET    [SEND] 01 06 00 2C 00 1C 49 CA
*   ID_MSG_TMPSET    [RECV] 01 06 00 2C 00 1C 49 CA
* 
***********************************************************************/
static ER TmpRecvProc( unsigned char msgid, unsigned short *temp )
{
	ER result = E_CONTINUE;
	unsigned int timeoutflg = CLR;
	unsigned char rxdat = CLR;
	unsigned char i;
	unsigned char size;

	memset( &rxbuf[0], CLR, sizeof( rxbuf ) );
	
	// Leagic�ǉ�2023.8.25(Start)
	result = TmpDataRecv( &rxbuf[0], &size );
	
	/* �f�[�^���e�`�F�b�N */
	if(
			( msgid == ID_MSG_TMPGET )    ||
			( msgid == ID_MSG_TMPGETR )   ||
			( msgid == ID_MSG_TMPGET_HS ) ||
			( msgid == ID_MSG_TMPSET )    ||
			( msgid == ID_MSG_TMPSET_HS ) 
		){
		for( i = 0; i < size; i++ ){
			switch( i ){
				case 0:// �X���[�u�A�h���X
					if( rxbuf[i] != SLAVE_ADR ){
			 			result = E_TEMP_FAIL;
					}
				break;
				case 1:// �t�@���N�V�����R�[�h
					if( ( rxbuf[i] != FC_TMPGET ) && ( rxbuf[i] != FC_TMPSET ) ){
						result = E_TEMP_FAIL;
					}
				break;
				default:
					/* DO NOTHING */
				break;
			}
			if( result != E_SUCCESS ){
					/* ���ʂ����s�̏ꍇ���菈���𒆒f���� */
				break;
			}
		}
	}
	/* �f�[�^�T�C�Y�`�F�b�N */
	/* �f�[�^���e�������Ă����ꍇ�̂ݎ��s���� */
	if( result == E_SUCCESS ){
		if(
			( msgid == ID_MSG_TMPSET )||
			( msgid == ID_MSG_TMPSET_HS )
			)
			{
				if( size == TMPSET_RCVSIZE )
				{
					// ��CRC�`�F�b�N�L�q�B���͖������ɐ���

					*temp  = 0x0000;
					result = E_SUCCESS;
				}
				else{
					result = E_TEMP_FAIL;
				}
			}
		if(
			( msgid == ID_MSG_TMPGET ) ||
			( msgid == ID_MSG_TMPGETR )||
			( msgid == ID_MSG_TMPGET_HS )
			)
			{
				if( size == TMPGET_RCVSIZE )
				{
					// ��CRC�`�F�b�N�L�q�B���͖������ɐ���
					
					*temp = ( rxbuf[3] * 0x100 ) + rxbuf[4];
					result = E_SUCCESS;
				}
				else{
					result = E_TEMP_FAIL;
				}
				
				if( result == E_SUCCESS )
				{
					TmpDataSet(rxbuf[4]);
				}
			}
		
	}
	// Leagic�ǉ�2023.8.25(End)
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static void TmpSendDataMake( unsigned char msgid,unsigned char dat )
{
	switch(msgid)
	{
	case ID_MSG_TMPSET:
	case ID_MSG_TMPSET_HS:
		txbuf[0] = 0x01;// �X���[�u�A�h���X
		txbuf[1] = 0x06;// �t�@���N�V�����R�[�h
		txbuf[2] = 0x00;// ���W�X�^�A�h���X
		txbuf[3] = 0x2c;// ���W�X�^�A�h���X
		txbuf[4] = 0x00;// �����݃f�[�^
		txbuf[5] = dat; // �����݃f�[�^
		break;
		
	case ID_MSG_TMPGET:
	case ID_MSG_TMPGET_HS:
		txbuf[0] = 0x01;// �X���[�u�A�h���X
		txbuf[1] = 0x03;// �t�@���N�V�����R�[�h
		txbuf[2] = 0x00;// ���W�X�^�A�h���X
		txbuf[3] = 0x00;// ���W�X�^�A�h���X
		txbuf[4] = 0x00;// ��
		txbuf[5] = 0x01;// ��
		break;
		
	default:
		return;
	}

	CrcCal( &txbuf[0], 6 );
	TmpDataSend(&txbuf[0], SEND_SIZE);
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER TmpDataSend( unsigned char *buf, unsigned char siz )
{
	ER result = E_SUCCESS;
	unsigned char loop;
	unsigned char THRE;							// Leagic�ǉ�2023.8.22
	
#if FZ110 == 0
	p16C554 = (struct St16550 *)SCI_16C554A_ADDR;//RS232C
#else
	p16C554 = (struct St16550 *)SCI_16C554D_ADDR;//RS485
#endif
	p16C554->REG4.BIT.RTS = 1;
	dly_tsk(RTS_WAIT);
	
	// Leagic�ǉ�2023.8.22(Start)
	for( loop = 0; loop < siz; loop++ )
	{
		THRE = p16C554->REG5.BIT.THRE;
		if( THRE == 1 ){
			p16C554->REG0.THR = *buf;
			buf++;
		}
		else{
			loop--;
		}
	}
	dly_tsk(10);
	// Leagic�ǉ�2023.8.22(End)
	
	p16C554->REG4.BIT.RTS = 0;
	dly_tsk(RTS_WAIT);

	return( result );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER TmpDataRecv( unsigned char *buf, unsigned char *siz )
{
	ER result = E_SUCCESS;
	unsigned char DR;
	unsigned char RDRF;
	unsigned long timeoutflg = CLR;
	unsigned char size = CLR;
#if 0
#ifdef DEBUG_TEMP_DEV
	rxcnt = 0;

	rxbuf[rxcnt++] = 0x01;
	rxbuf[rxcnt++] = 0x03;
	rxbuf[rxcnt++] = 0x04;
	rxbuf[rxcnt++] = 0x00;
	rxbuf[rxcnt++] = 0x1B;
	rxbuf[rxcnt++] = 0x8A;
	*buf = 0x3D;
	
	return( E_SUCCESS );
#endif
#endif

	
	
	*buf = CLR;

#if FZ110 == 0
	p16C554 = (struct St16550 *)SCI_16C554A_ADDR;//RS232C
#else
	p16C554 = (struct St16550 *)SCI_16C554D_ADDR;//RS485
#endif

	// Leagic�ǉ�2023.8.25(Start)
	while( 1 ){
		DR = p16C554->REG5.BIT.DR;
		if(!DR){
			timeoutflg++;
			if( timeoutflg >= TIMEOUT_RCV ){
				result = E_SUCCESS;
				*siz = size; 
				break;
			}
		}
		else{
			timeoutflg = CLR;
			*buf  = p16C554->REG0.RBR;
			buf++;
			size++;
		}
	}
	// Leagic�ǉ�2023.8.25(End)

	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
void TmpDataSet( unsigned char tmpdat )
{
	tempdat_bk[1] = tempdat_bk[0];
	tempdat_bk[0] = tmpdat;
	
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
unsigned char TmpDataGet()
{
	return( tempdat_bk[0] );
}

/***********************************************************************
* 
* [�T�v]	CRC�v�Z
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static unsigned short CrcCal(unsigned char *buf,unsigned char siz)
{
	unsigned short CRC= 0xffff;
	unsigned char  crch;
	unsigned char  crcl;
	unsigned short next;
	unsigned short carry;
	unsigned short n;
	unsigned char  loop;

	for( loop = 0; loop < siz; loop++ )
	{
		next = (unsigned short) *buf;
		CRC ^= next;
		for (n = 0; n < 8; n++) 
		{
			carry = CRC & 1;
			CRC >>= 1;
			if (carry) 
			{
				CRC ^= 0xA001;
			}
		}
		buf++;
	}
	crch = (unsigned char)(CRC / 256);
	crcl = (unsigned char)(CRC % 256);
	
	*buf = crcl;
	buf++;
	*buf = crch;

	return CRC;
}
