/******************************************************************************
* ���[�V�����l�b�g�Ǘ��^�X�N                                                  *
*                                                                             *
* File name : mnet.c                                                          *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "datastruct.h" 
#include "mnet.h"
#include "sub.h"
#include "automachine.h"
#include "macro.h"


// *****************************************************
//  �R���p�C���X�C�b�`
// *****************************************************
#define WAIT_TYPE_IFB	1	// ��`:IFB�t���O�Ď�
#define WAIT_END_CHK	1	// ��`:�����҂�ٰ��
#define WAIT_TYPE_RW	1	// ��`:RW�f�B���C

#define DEBUG_INFO				1		// �� 1 = DEBUG�p, 0 = ���i�p


// *****************************************************
//  �֐���`
// *****************************************************
ER	MntInit( void );
BOOL pio_Init();
BOOL motor_Init(void);

void pio_ALLIN(UB *data);
UB pio_BIN(UB ch);
void pio_BOUT(UB ch, UB sw);
void pio_PATOUT(UB sw1, UB sw2, UB sw3);
ER motor_POWER(UB pm, UB sw);
ER motor_LIMIT(UB mm, UB dr, UB md, INT sp );
ER motor_SCAN(UB pm, UH dir);
ER motor_SET(UB pm, ST_MOTOR_PARAM mp);
ER motor_PSET(UB pm, INT cnt, INT enc, INT def);
ER motor_ABSMV(UB pm, INT pos, UB status);
ER motor_RELMV(UB pm, INT pos, UB status);
ER motor_STOP(UB pm);
ER motor_ABORT(UB pm);
ER motor_STATUS(UB pm, ST_MOTOR_STATUS *status);

static BOOL WriteMemory(INT addr, UH data);
static BOOL WriteMemoryInt(INT addr, INT data);
static BOOL ReadMemory(INT addr,  UH *data);
static BOOL SetMainCommand(UH command);
static BOOL GetMainStatus(MNETStatus* status);

// PCD4611 ---
static void WriteMemoryPCD4611(UB data);
static void WriteMemoryIntPCD4611(INT data);
static INT  ReadMemoryPCD4611();
// PCD4611 ---

 BOOL SetMotorRegisterInt(UB dev_no, UH reg, INT value);//��
//static BOOL SetMotorRegisterInt(UB dev_no, UH reg, INT value);
static BOOL SetMotorRegister(UB dev_no, UH reg, UH value);
       BOOL GetMotorStatus(UB dev_no, MotorStatus* status);
static BOOL GetMotorSubStatus(UB dev_no, MNETSubStatus* substs);
static BOOL SetMainRegister(UH reg, UH data);
       BOOL IsExist(UB dev_no);
static BOOL SetMotorCommand(UB dev_no, UH command);
       BOOL	GetMotorRegister(UB dev_no, UH reg, INT* value);
static ER   motor_SetAllRegister(UB pm);
static ER   motor_CalcMotorReg(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg);
static ER motor_CalcMotorRegPCD4611(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg);
static void motor_RegistCommand(UB pm, UH cmd);
       void g_Sleep(INT usec);
static void ClearInterrupt(void);
static BOOL IfbIdleWait(void);	// �h�e�a�A�C�h�����O�҂�
static BOOL ChkIFB( void );		// �h�e�a��Ԏ擾
static BOOL EndIdleWait(void);	// �����҂�
static BOOL ChkEnd( void );		// �G���h��Ԏ擾
static BOOL motor_InitController(void);
static UB ChkRbsy( void );
static UB ChkDbsy( void );
static UB ChkSbsy( void );



static ER SetRenv1Fu( UB dev_no );				// RENV1 �ݒ�( BCD-4020FUA , BCD-4020FU )
static ER SetRenv1PCD4611( UB dev_no );			// 
static ER SetRenv2Fu( UB dev_no, UB teibai );	// RENV2 �ݒ�( BCD-4020FUA , BCD-4020FU )
static ER SetRenv2PCD4611( UB dev_no, UB teibai );
static ER SetRenv3Fu( UB dev_no, UB enc );		// RENV3 �ݒ�( BCD-4020FUA , BCD-4020FU )
static ER SetRmdFu( UB dev_no, UB mod );		// RMD   �ݒ�( BCD-4020FUA , BCD-4020FU )
static ER SetRmv( UB dev_no, INT pos );			// RMV   �ݒ�
static ER SetRmvPCD4611( UB dev_no, INT pos );
void	check_main_pulsemotor_initialize( );

#ifdef DEBUG_INFO
static void DebugMnetInfo( UB mm );
#endif
static ER SetRenv3PCD4611( UB dev_no, UB enc );
static ER SetRmdPCD4611( UB dev_no, UB mod );


/***********************************************************************
* �\����
***********************************************************************/



/***********************************************************************
* �e�[�u��
***********************************************************************/

static const INT MOTORSPD_TBL[10] = {
	1999,
	 999,
	 399,
	 199,
	  99,
	  39,
	  19,
	   9,
	   3,
	   2
};
static const INT MOTORSPD_TBL_PCD4611[10] = {
	3999,// 0.3
	2399,// 0.5
	1199,// 1
	 599,// 2
	 239,// 5
	  59,// 10
	  29,// 20
	  14,// 50
	   5,// 100
	   2 // 200
};

// LIMIT�R�}���h���̃��[�h�I��
static const UB LMMOD_TBL[2][3] = {
//    �K��O   CW      CCW
	{ MOD20  , MOD20,  MOD28 },	// GENTEN_POS
	{ MOD22  , MOD22,  MOD2A }	// NUKEDA_POS
};

/***********************************************************************
* �f�[�^��`
***********************************************************************/
volatile struct	StMotionnet	*pMotionnet;
volatile struct	StPulseMotor	*pPulseMotor;


static	ST_PM_INFO		_pmInfo[20];			// �p���X���[�^���z��
static	BOOL 			Initialized = FALSE;	// 
static 	INT	factor;
static 	INT	param;
static 	INT	RMG;
static 	INT	RFL;
static 	INT	RFH;
static 	INT	RUR;
static 	INT	RUS;
static 	INT	RDS;

#define	PULSEMOTOR_CMD_CUN1R	0x20			// �J�E���^���Z�b�g�iCOUNTER1�j
#define	PULSEMOTOR_CMD_CUN2R	0x21			// �J�E���^���Z�b�g�iCOUNTER2�j
#define	PULSEMOTOR_CMD_STOP		0x49			// ����~
#define	PULSEMOTOR_CMD_STAFL	0x50			// FL�ᑬ�X�^�[�g
#define	PULSEMOTOR_CMD_WPRFL	0x81			// [W] �����x
#define	PULSEMOTOR_CMD_WPRMG	0x85			// [W] ���x�{��
#define	PULSEMOTOR_CMD_WPRMD	0x87			// [W] ���샂�[�h
#define	PULSEMOTOR_CMD_WRENV1	0x9C			// [W] ���ݒ�1
#define	PULSEMOTOR_CMD_WRENV2	0x9D			// [W] ���ݒ�2
#define	PULSEMOTOR_CMD_RRENV1	0xDC			// [R] ���ݒ�1
#define	PULSEMOTOR_CMD_RCUN1	0xE3			// [R] COUNTER1�i�w�߈ʒu�J�E���^�j
#define	PULSEMOTOR_CMD_RCUN2	0xE4			// [R] COUNTER2�i�@�B�ʒu�J�E���^�j
#define	PULSEMOTOR_CMD_RREST	0xF2			// [R] �G���[���荞�ݗv���擾




/******************************************************************************
* 
* [�T�v]
* 
* 
* 
******************************************************************************/
ER	MntInit( void )
{
	BOOL	blnResult;
	UB		rtycnt = 0;
	int	setValue;

	pMotionnet = (struct StMotionnet *)MNET_BASE_ADDRESS;

	blnResult = SetMainCommand(MNET_CENTER_RESET);				//  100h

	check_main_pulsemotor_initialize();// PCD4611 �������ǉ�

	// BUSY�����܂ő҂�
	while( ChkRbsy() ){};
	
	blnResult = SetMainRegister(MNET_CENTER_WRENV0, 0x037F);	// 5500h
	blnResult = SetMainCommand(MNET_CMD_CLEAR_SEND_FIFO);		//  200h
	blnResult = SetMainCommand(MNET_CMD_CLEAR_RECV_FIFO);		//  300h
	blnResult = SetMainCommand(MNET_CENTER_SYSCOM);				// 1000h

	// BUSY�����܂ő҂�
	while( 1)
	{
		if( ChkDbsy() == CLR ) break;

		rtycnt++;
		dly_tsk(DLY_1M);

		if( rtycnt > SYS_RTY_OVER )
		{
			return( E_CON_SYSCOM );
		}
	};

	// �Z���^�[�ʐM�J�n
	rtycnt = 0;
	blnResult = SetMainCommand(MNET_CENTER_STA_IOCOM);			// 3000h
	while(1)
	{
		if( ChkSbsy() ) break;
		rtycnt++;
		dly_tsk(DLY_1M);

		// BUSY�������ꂸ
		if( rtycnt > SYS_RTY_OVER )
		{
			// �ʐM�ُ�Ɣ��f
			return( E_CON_IO_COM );
		}
	}
	
	
	pio_Init();
	motor_Init();

	return( E_SUCCESS );

}
//==============================================================================
//	Function	: check_main_pulsemotor_initialize
//	Description	: ���C����̃p���X���[�^�����̏�����
//	Parameters	: �Ȃ�
//	Return		: �Ȃ�
//	Note		: 
//	Global		: 
//==============================================================================
void	check_main_pulsemotor_initialize( )
{
//	int	setValue;
	
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRFL, _pmInfo[MT_HEAD_Y0].reg[R1] );	// RFL
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRFH, _pmInfo[MT_HEAD_Y0].reg[R2] );	// RFH
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRUR, _pmInfo[MT_HEAD_Y0].reg[R3] );	// RUR
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDR, 0                           );	// RDR
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRMG, _pmInfo[MT_HEAD_Y0].reg[R4] );	// RMG
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDP, 0                           );	// RDP
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDS, _pmInfo[MT_HEAD_Y0].reg[R9] );	// RDS
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRMG, 1199                        );	// RMG
#if 1
	SetMotorRegisterInt( MT_Y0, WRG_PRMG,   1199              );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRFL,  50                );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRFH,  300               );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRUR,  500               );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_RENV2, 0x00000005        );	// 
	WriteMemoryPCD4611 ( CMD_CUN1R );							// CUN1R : 
	WriteMemoryPCD4611 ( CMD_CUN2R );							// CUN2R : 
#endif

#if 0
	WriteMemoryIntPCD4611( 1199 );
	WriteMemoryPCD4611( WPCD_PRMG );		// PRMG : ���x�{���ݒ�

	WriteMemoryIntPCD4611( 50 );
	WriteMemoryPCD4611( WPCD_PRFL );		// PRFL : �����ݒ�

	WriteMemoryIntPCD4611( 300 );
	WriteMemoryPCD4611( WPCD_PRFH );		// PRFH : �����ݒ�

	WriteMemoryIntPCD4611( 500 );
	WriteMemoryPCD4611( WPCD_PRUR );		// PRUR : 

	WriteMemoryIntPCD4611( 500 );
	WriteMemoryPCD4611( WPCD_PRDR );		// PRDR : 

	WriteMemoryIntPCD4611( 0x00000005 );
	WriteMemoryPCD4611( WPCD_RENV2 );		// RENV2 : 
	
	WriteMemoryPCD4611( CMD_CUN1R );		// CUN1R : 
	WriteMemoryPCD4611( CMD_CUN2R );		// CUN2R : 
#endif

#if 0
	// PRMG : ���x�{���ݒ�
	setValue = 1199;							// �{�� = 1
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRMG;	// 0x85;

	// PRFL : �����ݒ�
	setValue = 50;								// �����ݒ� = 50[pps]
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRFL;	// 0x81;

	// PRFH
	setValue = 300;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRFH;	// 0x82;

	// PRUR
	setValue = 500;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRUR;	// 0x83;

	// PRDR
	setValue = 500;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRDR;	// 0x84;

	// RENV2 : ���ݒ�2
	setValue = 0x00000005;						// EA/EB: A/B��1���{, P1�`P0: �ėp�o��
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_RENV2;	// 0x9D
#endif
	
	// ������Ԃ̐ݒ�
	// �ėp�o�́i"P0", "P1"�j��"L"�ɂ��� ... HOLD�ŉ�
	pPulseMotor->unCmdSts.stWrite.OTPB = 0x00;
}



BOOL motor_Init(void)
{
	UB i;

	motor_InitController();

	for (i=DVMIN+1; i<=DVMAX; i++)
	{
		if (IsExist( i ))
		{
			_pmInfo[i].factor 			= 2;
			_pmInfo[i].pos 				= 0;
			_pmInfo[i].power 			= OFF;
			_pmInfo[i].paused 			= FALSE;
			_pmInfo[i].command.code 	= CMD_NOCOMMAND;
			_pmInfo[i].startParam 		= CMD_NOTYPE;

			_pmInfo[i].reg[R0]			= 0;		// Pls
			_pmInfo[i].reg[R1]			= 200;		// RFL
			_pmInfo[i].reg[R2]			= 800;		// RFH
			_pmInfo[i].reg[R3]			= 500;		// RUR
			_pmInfo[i].reg[R4]			= 500;
			_pmInfo[i].reg[R5]			= 0;
			_pmInfo[i].reg[R6]			= 0;
			_pmInfo[i].reg[R7]			= 0;

			motor_PowerUp(i, _pmInfo[i].power);
		}
	}

	return TRUE;
}
///<summary>
/// ���[�^�R���g���[��������������֐�
///</summary>
static BOOL motor_InitController(void)
{
	UB pm;
	UB enc;
	UB mnet_drv_typ;

	// ���C���f�o�C�X�̑S���荞�݂𖳌���
	if (!SetMainRegister(MNET_CENTER_WRENV0, 0x037F))				return FALSE;

	for (pm=DVMIN; pm<=DVMAX; pm++)
	{
		if (IsExist( pm ))											// �f�o�C�X�ڑ����Ă��邩(��0�F�ڑ�)
		{
			mnet_drv_typ = MNET_DRV_TYPE[pm];						// �f�o�C�X���擾

			switch( mnet_drv_typ )
			{
			case BCD4020FUA:
				if (!SetMotorCommand(pm, CMD_SRST))					continue;
				if( IsEncoderExist(pm) == TRUE )					enc = EC_3;
				else												enc = EC_N;

				if( SetRenv1Fu( pm ))								return FALSE;// RENV1�ݒ�
				if( SetRenv2Fu( pm, enc ))							return FALSE;// RENV2�ݒ�
				if( SetRenv3Fu( pm, enc ))							return FALSE;// RENV3�ݒ�
				break;

			case BCD4020FU:
				if (!SetMotorCommand(pm, CMD_SRST))					continue;
				if( IsEncoderExist(pm) == TRUE )					enc = EC_3;
				else												enc = EC_N;

				if( SetRenv1Fu( pm ))								return FALSE;// RENV1�ݒ�
				if( SetRenv2Fu( pm, enc ))							return FALSE;// RENV2�ݒ�
				break;

			case PCD4611:
//				if( SetRenv1PCD4611( pm ))							return FALSE;// RENV1�ݒ�
//				if( SetRenv2PCD4611( pm, enc ))						return FALSE;// RENV2�ݒ�
				break;

			default:
				// ���[�^�h���C�o�ȊO�́A�X�L�b�v
				continue;
				break;
			}
			// RIRQ
			SetMotorRegisterInt(pm, WRG_RIRQ, 0x00000000);
		}
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
// ���荞�݃n���h��

///<summary>
/// �R�}���h���s���ʂ��擾���܂�
///</summary>
void IRQ4_ISR()
{
#if 0	
	MNETStatus status;
	status.UNID.data = pMotionnet->commandStatus0 ;
	ClearInterrupt();
	INTC.IRQRR.BIT.IRQ4F = 0;
#endif
}

//������������������������������������������������������������������������������������������������������������������������
// 
// IO�n
// 
//������������������������������������������������������������������������������������������������������������������������
///<summary>
/// IO�g���[��������������֐�
///</summary>
BOOL pio_Init()
{
// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return TRUE;
#endif
	// ���C���f�o�C�X�̑S���荞�݂𖳌���
	SetMainRegister(MNET_CENTER_WRENV0, 0x037F);

	return TRUE;
}


/******************************************************************************
* 
* [�T�v]	�S���̓`���l���̏�Ԃ��擾����
* [����]	data    ���͒l�i�[��
* [�߂�l]	TRUE:�����AFALSE:���s
* 
******************************************************************************/
void pio_ALLIN(UB *data)
{
	INT Loopn;
	INT amari;
	INT syou;
	INT sftdat;
	UB  sftbit;
	UB  ret;

	sftbit = 0x01;

	pMotionnet = (struct StMotionnet *)MNET_BASE_ADDRESS;

	// ���⑫�j��������Ă��Ȃ����͌��O
 	// bit1    �`���l��2
	// �E
	// bit63   �`���l��64

	
	// ���⑫�j��������Ă��Ȃ����͌��O
	// Ch0�͖����̂��ߌ������Ȃ�
	for( Loopn = MIN_CH; Loopn <= MAX_CH; Loopn++ )	// 2020/2/12 �P�͂��܂�Ή�
	{
		sftbit = 0x01;

		syou    = ( (Loopn - MIN_CH) / 8 );			// �]��擾
		amari   = ( (Loopn - MIN_CH) % 8 );			// �]��擾
		sftdat  = ( sftbit << amari );				// �]����V�t�g

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
		ret = 0;
#else
		ret = GetInputDat( Loopn ) ;
#endif

		if( ret )
		{
			data[syou] |= sftdat;
		}
		else
		{
			data[syou] &= (~sftdat);
		}
	}
}

/******************************************************************************
* 
* [�T�v]	���̓`���l������M�����擾����
* [����]	ch      ���̓`���l��(�O�`)
* 			sw      [out]�`���l���̏��(TRUE:ON�AFALSE�FOFF)
* [�߂�l]	1:High�A0:Low
* 
******************************************************************************/
UB pio_BIN(UB ch)
{
	UB retdat = OFF;
	
// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	if( ch == 4 )
	{
		return(1);
	}
	else
	{
		return(0);
	}
#endif
	
	if( ( ch == I_DOORSW1 )||( ch == I_DOORSW2 ) )
	{
		
		
		
		
	}
	else
	{
		retdat = GetInputDat( ch );
	}
	
	
	return ( retdat );
}
/******************************************************************************
* 
* [�T�v]	�o�̓`���l���ւ̏o�̓f�[�^
* [����]	ch      �o�̓`���l��
* 			sw      ON:HIGH�o�́AOFF:LOW�o��
* [�߂�l]	�|
* 
******************************************************************************/
void pio_BOUT(UB ch, UB sw)
{
	INT waittim = WAITTIM;
	INT cnt;
	UB rdata = 0;
	UB wdata = 0;
	UB devno;
	UB portno;
	UB mskbit;

	if(( ch < MIN_CH )||( ch > MAX_CH )) return;	// 2020/2/12 �P�͂��܂�Ή�

	if( ch == O_DAIAPMP )
	{
//		switch( sw )
//		{
//		case 1:		//���]
//			PE.DR.BIT.B0 = 1;
//			PE.DR.BIT.B1 = 0;
//			break;
//		case 2:		//�t�]
//			PE.DR.BIT.B0 = 0;
//			PE.DR.BIT.B1 = 1;
//			break;
//		default:	//��~
//			PE.DR.BIT.B0 = 0;
//			PE.DR.BIT.B1 = 0;
//			break;
//		}
		
		switch( sw )
		{
		case 0:
			DpumpStop();
			break;
		default:
			SetDpumpDuty( sw );
			DpumpStart();
			break;
		}
	}
	else
	{
		// CLR���N�G�X�g
		devno  = OUTPUTCH_INFTBL[ch][0];
		portno = OUTPUTCH_INFTBL[ch][1];
		mskbit = OUTPUTCH_INFTBL[ch][2];
		
		// �o�͒l�쐬
		if(( devno != NONE )&&( portno != NONE ))
		{
	// �{���[�h
#if DEBUG_SIM_MODE == 0
			// �����݃f�[�^�x�[�X�擾
			MAC_READ( pMotionnet->portData[devno][portno], wdata );
#endif

			// SET���N�G�X�g
			if( sw )
			{
				wdata |= mskbit;
			}
			// CLR���N�G�X�g
			else
			{
				wdata &= (~mskbit);
			}

// �{���[�h
#if DEBUG_SIM_MODE == 0
			// �w��Ch�֏o��
			for( cnt = 0; cnt < READ_RTY_MAX; cnt++ )
			{
				MAC_WRIT( pMotionnet->portData[devno][portno], wdata  );
				g_Sleep(waittim);
				MAC_READ( pMotionnet->portData[devno][portno], rdata );
				if( wdata == rdata )	break;
			}
#endif
		}
	}
}

/******************************************************************************
* 
* [�T�v]	�p�g���C�g��p�o��
* [����]	sw      ON:HIGH�o�́AOFF:LOW�o��
* [�߂�l]	�|
* 
******************************************************************************/
void pio_PATOUT(UB sw1, UB sw2, UB sw3)
{
	UB data1 = 0;
	UB data2 = 0;
	UB mskbit = 0;
	UB devcno1 = 0;
	UB portno1 = 0;
	UB devcno2 = 0;
	UB portno2 = 0;

// �{���[�h
#if DEBUG_SIM_MODE == 0
	devcno1 = OUTPUTCH_INFTBL[O_PATLRED][0];
	portno1 = OUTPUTCH_INFTBL[O_PATLRED][1];
	MAC_READ( pMotionnet->portData[devcno1][portno1], data1 );
	
	devcno2 = OUTPUTCH_INFTBL[O_PATLGRN][0];
	portno2 = OUTPUTCH_INFTBL[O_PATLGRN][1];
	MAC_READ( pMotionnet->portData[devcno2][portno2], data2 );
	
#endif

	// ��
	mskbit  = OUTPUTCH_INFTBL[O_PATLRED][2];
	if( sw1 )
	{
		data1 |= mskbit;
	}
	else
	{
		data1 &= (~mskbit);
	}

	// ��
	mskbit  = OUTPUTCH_INFTBL[O_PATLGRN][2];
	if( sw2 )
	{
		data2 |= mskbit;
	}
	else
	{
		data2 &= (~mskbit);
	}
	
// �{���[�h
#if DEBUG_SIM_MODE == 0
	MAC_WRIT( pMotionnet->portData[devcno1][portno1], data1 );
	MAC_WRIT( pMotionnet->portData[devcno2][portno2], data2 );
#endif
}

//������������������������������������������������������������������������������������������������������������������������
// 
// ���[�^�n
// 
//������������������������������������������������������������������������������������������������������������������������
/*-----------------------------------------------------------------------------
  �@�\�F���x�̐ݒ�
  �@�\�Fno      ���[�^�ԍ�
        type    �P�F�r���A�O�F��`
        resol   ���x�{��
        lspd    ����[Hz]
        hspd    ����[Hz]
        tim     ����������
        sarea   �r���ϑ��̈�[Hz] ����`�̎��͖���
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_SET(UB pm, ST_MOTOR_PARAM mp)
{
	ER Result = E_SUCCESS ;
	PM_REG	pm_reg = {0};

	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}

	// PCD4611�ʐM�ڑ���
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		Result = motor_CalcMotorRegPCD4611(&mp, &pm_reg);
	}
	else
	{
		Result = motor_CalcMotorReg(&mp, &pm_reg);
	}

	if( Result != E_SUCCESS)								return( Result );
	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));	// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));	// �r�W�[�m�F

	_pmInfo[pm].reg[R1] = pm_reg.reg[R1];
	_pmInfo[pm].reg[R2] = pm_reg.reg[R2];
	_pmInfo[pm].reg[R3] = pm_reg.reg[R3];
	_pmInfo[pm].reg[R4] = pm_reg.reg[R4];
	_pmInfo[pm].reg[R5] = pm_reg.reg[R5];
	_pmInfo[pm].reg[R6] = pm_reg.reg[R6];

	Result =  motor_SetAllRegister(pm);
	if( Result != E_SUCCESS )								return( Result );

	return Result;
}

/******************************************************************************
* 
* [�T�v]	�A�h���X�J�E���^�̃��Z�b�g
* [����]	pm:���[�^�ԍ��Acnt:�J�E���g�ݒ�l�Aenc:�G���R�[�_�ݒ�l�Adv:���g�p�Adef:�΍��J�E���^
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
ER motor_PSET(UB pm, INT cnt, INT enc, INT def)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));	// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));	// �r�W�[�m�F

	_pmInfo[pm].pos = cnt;

	// PCD4611�ʐM�ڑ���
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		if (!SetMotorRegisterInt(pm, WRG_RCUN1, cnt))		return(( E_MNT_ACCESS + WRG_RCUN1 ));
	}
	else
	{
		if (!SetMotorRegisterInt(pm, WRG_RCUN1, cnt))		return(( E_MNT_ACCESS + WRG_RCUN1 ));
		if (!SetMotorRegisterInt(pm, WRG_RCUN2, enc))		return(( E_MNT_ACCESS + WRG_RCUN2 ));
		if (!SetMotorRegisterInt(pm, WRG_RCUN3, def))		return(( E_MNT_ACCESS + WRG_RCUN3 ));
	}

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  �@�\�F��΃A�h���X�ړ�
  �@�\�Fpm      ���[�^�ԍ�
        pos     �ړI�n�p���X
        status  ����w��
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_ABSMV(UB pm, INT pos, UB status)
{
	INT rcnt1 = 0;
	ER	Result = E_SUCCESS;
	UH	dir = (pos >=0 ? CW : CCW);

	if (pm < DVMIN || DVMAX < pm)			return((E_PAR_OUTRNG + PARAM1));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))						return(( E_CON_ERR + pm ));		// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(pm))					return(( E_BUSY + pm ));		// �r�W�[�m�F
	if (motor_IsLimit(pm, dir))				return( E_SUCCESS );			// ���݃��~�b�g���m�F

	motor_SetAllRegister(pm);												// Limit�ゾ��Limit���̑����̂܂܂ƂȂ邽�߁A�����Œʏ�̐ݒ葬�x�ɕύX
	SetMotorCommand (pm, CMD_INTRS);										// �����ݏo�̓��Z�b�g
	_pmInfo[pm].reg[R0]		= (INT)pos;

	_pmInfo[pm].startParam	= CMD_STAUD;									// �X�^�[�g�R�}���h����w��
	if( status == REQ_STAFH )	_pmInfo[pm].startParam	= CMD_STAFH;
	if( status == REQ_STAFL )	_pmInfo[pm].startParam	= CMD_STAFL;
	_pmInfo[pm].paused		= FALSE;

	
	// PCD4611�ʐM�ڑ���
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_RCUN1);
		rcnt1 = ReadMemoryPCD4611();

		SetMotorRegisterInt( MT_Y0, WRG_RMD, MOD41 );
		SetMotorRegisterInt( MT_Y0, WRG_RMV, (pos - rcnt1) );
		
		WriteMemoryPCD4611( CMD_STAUD );
	}
	// MNET_BCD4020FU(A)�ڑ���
	else
	{
		Result = SetRenv1Fu( pm );			if( Result != E_SUCCESS )	return( Result );	// RENV1�ݒ�
		Result = SetRmdFu( pm, MOD42 );		if( Result != E_SUCCESS )	return( Result );	// RMD�ݒ�
		Result = SetRmv( pm, pos );			if( Result != E_SUCCESS )	return( Result );	// RMV�ݒ�

	}
	motor_RegistCommand(pm, CMD_ABSMV);
	
	return( E_SUCCESS );
}
/*-----------------------------------------------------------------------------
  �@�\�F���΃A�h���X�ړ�
  �@�\�Fpm      ���[�^�ԍ�
        pos     �ړI�n�p���X
        status  ����w��
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_RELMV(UB pm, INT pos, UB status)
{
	unsigned long	setValue;
	UB	mod;
	ER	Result = E_SUCCESS;
	UH	dir = (pos >=0 ? CW : CCW);

	if (pm < DVMIN || DVMAX < pm)			return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if (!IsExist( pm ))						return(( E_CON_ERR + pm ));	// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(pm))					return(( E_BUSY + pm ));	// �r�W�[�m�F
	if (motor_IsLimit(pm, dir))				return( E_SUCCESS );		// ���݃��~�b�g���m�F

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}
	
	// PCD4611�ʐM�ڑ���
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryIntPCD4611( MOD41 );
		WriteMemoryPCD4611( WRG_RMD );

		WriteMemoryIntPCD4611( pos );
		WriteMemoryPCD4611( WRG_RMV );
		
		WriteMemoryPCD4611( CMD_STAUD );
	}
	// MNET_BCD4020FU(A)�ڑ���
	else
	{
		motor_SetAllRegister(pm);											// Limit�ゾ��Limit���̑����̂܂܂ƂȂ邽�߁A�����Œʏ�̐ݒ葬�x�ɕύX
		SetMotorCommand (pm, CMD_INTRS);									// �����ݏo�̓��Z�b�g
		_pmInfo[pm].reg[R0]		= (INT)pos;

		_pmInfo[pm].startParam	= CMD_STAUD;								// �X�^�[�g�R�}���h����w��
		if( status == REQ_STAFH )	_pmInfo[pm].startParam	= CMD_STAFH;
		if( status == REQ_STAFL )	_pmInfo[pm].startParam	= CMD_STAFL;
		
		_pmInfo[pm].paused		= FALSE;

		Result = SetRenv1Fu( pm );			if( Result != E_SUCCESS )	return( Result );	// RENV1�ݒ�
		Result = SetRmdFu( pm, MOD41 );		if( Result != E_SUCCESS )	return( Result );	// RMD�ݒ�
		Result = SetRmv( pm, pos );			if( Result != E_SUCCESS )	return( Result );	// RMV�ݒ�
		
		motor_RegistCommand(pm, CMD_RELMV);	
	}
	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  �@�\�F���~�b�g�ʒu�ړ�
  �@�\�Fno      ���[�^�ԍ�
        dir     ����(MOTOR_CW/MOTOR_CCW)
        spd     ���x(�葬)
        st      ���U�[�u
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_LIMIT(UB mm, UB dr, UB md, INT sp )
{
	ER	Result = E_SUCCESS;
	UB	mod;
	
	if (mm < DVMIN || DVMAX < mm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif

	if( MNET_DRV_TYPE[mm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( mm ))										return(( E_CON_ERR + mm ));	// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(mm))									return(( E_BUSY + mm ));	// �r�W�[�m�F

	// PCD4611�ʐM�ڑ���
	if( MNET_DRV_TYPE[mm] == PCD4611 )
	{
//		if( dr == CW )	mod = MOD00;
//		else			mod = MOD08;
		if( dr == CW )	mod = MOD10;
		else			mod = MOD18;

		WriteMemoryIntPCD4611( mod );
		WriteMemoryPCD4611( WRG_RMD );
		
		WriteMemoryPCD4611( CMD_STAFL );
	}
	// MNET_BCD4020FU(A)�ڑ���
	else
	{
		SetMotorCommand (mm, CMD_INTRS);			// �����ݏo�̓��Z�b�g

		_pmInfo[mm].reg[R0]		= 0xFFFFFF;
		_pmInfo[mm].startParam	= CMD_STAFL;
		_pmInfo[mm].paused		= FALSE;

		if( MNET_DRV_TYPE[mm] == PCD4611 )			// PCD4611�ʐM�ڑ���
		{
			if( dr == CW )	mod = MOD10;
			else			mod = MOD18;
			
			WriteMemoryIntPCD4611( mod );
			WriteMemoryPCD4611( WRG_RMD );
			WriteMemoryPCD4611( CMD_STAFL );
		}
		// MNET_BCD4020FU(A)�ڑ���
		else
		{
			Result = SetRenv1Fu( mm );							if( Result != E_SUCCESS )	return( Result );	// RENV1�ݒ�
			Result = SetRmdFu( mm, LMMOD_TBL[md][dr] );			if( Result != E_SUCCESS )	return( Result );	// RMD�ݒ�

			if (!SetMotorRegisterInt(mm, WRG_RFL, sp))			return(( E_MNT_ACCESS + WRG_RFL ));
			if (!SetMotorRegisterInt(mm, WRG_RFH, sp))			return(( E_MNT_ACCESS + WRG_RFH ));
			if (!SetMotorRegisterInt(mm, WRG_RMG, 199))			return(( E_MNT_ACCESS + WRG_RMG ));
			motor_RegistCommand(mm, CMD_LIMIT);
		}
	}
	return( E_SUCCESS );
}


/*-----------------------------------------------------------------------------
  �@�\�F������~
  �@�\�Fno      ���[�^�ԍ�
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_STOP(UB pm)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}
	

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// �f�o�C�X�ڑ��m�F
	motor_RegistCommand(pm, CMD_STOP);
	if( !SetMotorCommand(pm, CMD_SDSTP) )					return(( E_BUSY + pm ));

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  �@�\�F����~
  �@�\�Fno      ���[�^�ԍ�
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_ABORT(UB pm)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// �f�o�C�X�ڑ��m�F
	if( !SetMotorCommand(pm, CMD_STOP))						return(( E_BUSY + pm ));

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  �@�\�F�㎥��ON/OFF�ݒ�
  �@�\�Fno      ���[�^�ԍ�
        sw      �P�F�㎥ON�A�O�F�㎥OFF
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_POWER(UB pm, UB sw)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// �f�o�C�X�ڑ��m�F
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));		// �r�W�[�m�F
	if (!motor_PowerUp(pm, sw))								return(( E_CUR_ERR + pm ));		// �㎥

	_pmInfo[pm].power = sw;

	return ( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  �@�\�F���[�^��Ԃ̓ǂݏo��
  �@�\�Fno      ���[�^�ԍ�
  �ߒl�F�G���[�R�[�h
 ----------------------------------------------------------------------------*/
ER motor_STATUS(UB pm, ST_MOTOR_STATUS *status)
{
	INT pos  = 0;
	INT enc  = 0;
	INT dev  = 0;
	INT rsts = 0;
	INT rest = 0;
	INT rcnt1= 0;
	UB mnet_drv_typ;
	MNETSubStatus substs;
	
	// ���[�^�ԍ��͈͊O
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	// �X�e�[�^�X�Z�b�g
	status->busy      = 0;
	status->cw_limit  = 0;
	status->ccw_limit = 0;
	status->enc       = 0;
	status->dev       = 0;
	status->pos       = _pmInfo[pm].pos = (INT)pos;
	return (E_SUCCESS );
#endif

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));				// �f�o�C�X�ڑ��ُ�

	mnet_drv_typ = MNET_DRV_TYPE[pm];						// �f�o�C�X���擾

	if( mnet_drv_typ == PCD4611 )							// PCD4611�ʐM�ڑ���
	{
		WriteMemoryPCD4611(RRG_REST);
		rest = ReadMemoryPCD4611();

		status->cw_limit  = (rest & 0x00000001) ? 1 : 0;
		status->ccw_limit = (rest & 0x00000002) ? 1 : 0;
		if((rest & 0x00000004))		return( E_CON_HEAD_Y0 );
		
//		WriteMemoryPCD4611(RRG_RSTS);
//		pcd_rsts = ReadMemoryPCD4611();
		
		WriteMemoryPCD4611(RRG_RCUN1);
		rcnt1 = ReadMemoryPCD4611();
		
		// �X�e�[�^�X�Z�b�g
		status->busy      = motor_IsBusy(pm);
		status->cw_limit  = motor_IsLimit(pm , CW );
		status->ccw_limit = motor_IsLimit(pm , CCW);
		status->enc       = 0;
		status->dev       = 0;
		status->pos       = rcnt1;
	}
	else
	{
#ifdef DEBUG_INFO
		DebugMnetInfo( pm );
#endif

		// �G���[�����v���擾
		if (!GetMotorRegister(pm, RRG_REST, &rest))				return(( E_MNT_ACCESS + RRG_REST ));	// ���W�X�^�A�N�Z�X�ُ�

		if (!GetMotorRegister(pm, RRG_RSTS, &rsts))	// ��Ԏ擾
		{
			return(( E_MNT_ACCESS + RRG_RSTS ));	// ���W�X�^�A�N�Z�X�ُ�
		}
		
		if (!GetMotorRegister(pm, RRG_RCUN1, &pos))	// �ړ��ʎ擾
		{
			return(( E_MNT_ACCESS + RRG_RCUN1 ));	// ���W�X�^�A�N�Z�X�ُ�
		}
		
		if( IsEncoderExist(pm) == TRUE )			// �G�����R�[�_�t��
		{
			// �G���R�[�_�l�擾
			if (!GetMotorRegister(pm, RRG_RCUN2, &enc))			return(( E_MNT_ACCESS + RRG_RCUN2 ));	// ���W�X�^�A�N�Z�X�ُ�

			// �΍��l�擾
			if (!GetMotorRegister(pm, RRG_RCUN3, &dev))			return(( E_MNT_ACCESS + RRG_RCUN3 ));	// ���W�X�^�A�N�Z�X�ُ�

			// �E�����m
			if(( rest & 0x00000004))							return(( E_DTY_ERR | pm ));				// REST bit2(ESC3)

			// �E�����m
			if(( rsts & 0xf0000000) == 0)		// �Œ�l�O�G���A�ɏ������܂�Ă���ꍇ�́AFIFO�̓Ǎ����o���Ȃ������Ɣ��f���Ď����Ȃ�
			{
				if( (rsts & 0x00400000) )							
				{
//				return(( E_DTY_ERR | pm ));				// RSTS bit22(SCP3)
				}
			}
		}

		// �ʐM�G���[
		if( (rest & 0x00000400) )								return(( E_CON_ERR | pm ));				// REST bit10(ESNT)

		// �E�����m(�I�[�o�[���C�h���s�̂��ߒE���Ɣ��f)
		if( (rest & 0x00001000) )								return(( E_DTY_ERR | pm ));				// REST bit12(ESOR)

		// �X�e�[�^�X�Z�b�g
		status->busy      = motor_IsBusy(pm);
		status->cw_limit  = (rsts & 0x00000040) ? 1 : 0;
		status->ccw_limit = (rsts & 0x00000080) ? 1 : 0;
		status->enc       = enc;
		status->dev       = dev;
		status->pos       = _pmInfo[pm].pos = (INT)pos;
		
	}

	if( status->busy == TRUE )									return(( E_BUSY + pm ));				// �f�o�C�X�r�W�[

	return ( E_SUCCESS );
}
/******************************************************************************
* 
* [�T�v]	�G���R�[�_�ݒ�
* [����]	mm:���[�^�ԍ��Aec:���{�ݒ�l�Adtyp:���g�p�Adv:���g�p�Aso:�E�����o��ٽ
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
ER motor_ENCODER(UB mm, UB ec, UB dtyp, UB dv, INT so)
{
	RENV2		renv2;
	RENV4		renv4;
	RENV2PCD	renv2pcd;
	INT 		pos = 0;
	UB 			mnet_drv_typ;

	// ���[�^�ԍ��͈͊O
	if (mm < DVMIN || DVMAX < mm)									return(( E_PAR_OUTRNG + PARAM1 ));

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif

	if( MNET_DRV_TYPE[mm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( mm ))												return(( E_CON_ERR + mm ));
	if (motor_IsBusy(mm))											return(( E_BUSY + mm ));

	mnet_drv_typ = MNET_DRV_TYPE[mm];	// �f�o�C�X���擾

	if(
		( mnet_drv_typ == BCD4020FUA )||
		( mnet_drv_typ == BCD4020FU )
	)
	{
		if (!GetMotorRegister(mm, RRG_RENV2, &renv2.UNID.data))		return(( E_MNT_ACCESS + RRG_RENV2 ));// RENV2�Ǎ�
		if (!GetMotorRegister(mm, RRG_RENV4, &renv4.UNID.data))		return(( E_MNT_ACCESS + RRG_RENV4 ));// RENV4�Ǎ�
		
		// Encoder ���{�ݒ�
		switch( ec )
		{
			case EC_1:// �P���{
			default:
				renv2.UNID.BIT.EIM1 = 0;
				renv2.UNID.BIT.EIM0 = 0;
				break;
			case EC_2:// �Q���{
				renv2.UNID.BIT.EIM1 = 0;
				renv2.UNID.BIT.EIM0 = 1;
				break;
			case EC_3:// �S���{
				renv2.UNID.BIT.EIM1 = 1;
				renv2.UNID.BIT.EIM0 = 0;
				break;
		}
		if (!SetMotorRegisterInt(mm, WRG_RENV2, renv2.UNID.data))	return(( E_MNT_ACCESS + WRG_RENV2 ));

		// �E�����o�ݒ�
		if (!GetMotorRegister(mm, RRG_RCUN1, &pos))					return(( E_MNT_ACCESS + RRG_RCUN1 ));		// �J�E���^���킹
		if (!SetMotorRegisterInt(mm, WRG_RCUN2, pos))				return(( E_MNT_ACCESS + WRG_RCUN2 ));
		if( !SetMotorCommand(mm, CMD_CUN3R) )						return(( E_BUSY + mm ));					// COUNTER3���Z�b�g
		if (!SetMotorRegisterInt(mm, WRG_RCMP3, so))				return(( E_MNT_ACCESS + WRG_RCMP3 ));		// RCMP3�f�[�^
		if(( so )&&(ec != EC_N))	// �E���p���X�L���G���R�[�_�����ȊO
		{
			// �E�����o��ٽ��0�̏ꍇ�ARCMP3 �� ��r�J�E���^�ŏ�������
			renv4.UNID.BIT.C3S0 = 1;
			renv4.UNID.BIT.C3S1 = 0;
			renv4.UNID.BIT.C3S2 = 1;
			renv4.UNID.BIT.C3S3 = 0;
		}
		else
		{
			// �E�����o��ٽ��0�̏ꍇ�A��ɔ�r�����s�����ɂ��Ă���
			renv4.UNID.BIT.C3S0 = 0;
			renv4.UNID.BIT.C3S1 = 0;
			renv4.UNID.BIT.C3S2 = 0;
			renv4.UNID.BIT.C3S3 = 0;
		}
		if (!SetMotorRegisterInt(mm,WRG_RENV4,renv4.UNID.data))		return(( E_MNT_ACCESS + WRG_RENV4 ));		// �ݒ�
	}
	// PCD4611�ʐM�ڑ���
	else if( MNET_DRV_TYPE[mm] == PCD4611 )
	{
		// �{�Č��́A�G���R�[�h�Ȃ����ߏ����s�v
	}

	return( E_SUCCESS );
}



/******************************************************************************
* 
* [�T�v]	���[�^���x�{���擾
* [����]	���x�ԍ�
* [�ߒl]	�{���l
* 
******************************************************************************/
INT GetFactor( UB factor )
{
	INT Result = 0;

	if( ( factor >= 0)&&( factor <= 10 ))
	{
		Result = MOTORSPD_TBL[factor];
	}
	return( Result );
}
/******************************************************************************
* 
* [�T�v]	���[�^���x�{���擾
* [����]	���x�ԍ�
* [�ߒl]	�{���l
* 
******************************************************************************/
INT GetFactorPCD4611( UB factor )
{
	INT Result = 0;

	if( ( factor >= 0)&&( factor <= 10 ))
	{
		Result = MOTORSPD_TBL_PCD4611[factor];
	}
	return( Result );
}

// ����������������������������������������������������������������������������
// ����������������������������������������������������������������������������
// ����������������������������������������������������������������������������


/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[�Ƀf�[�^���������݂܂�
* [����]  addr�F�����݃A�h���X�Adata�F�����݃f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static BOOL WriteMemory(INT addr, UH data)
{
	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);
	ER	result = E_SUCCESS;

	// �o�X�o�R�ɂ��f�[�^�ʒm�̂��߃A�N�Z�X�p�x������
	// �ꍇ�A�f�[�^�����̉\�����łĂ��邽�ߔr������ǉ�
	MAC_WAI_SEM( ID_SEM_MEMRW );

	*p++ = (UB)(data >> 0);
	*p   = (UB)(data >> 8);

#ifdef WAIT_TYPE_IFB
	// TRUE:�r�W�[	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}
#endif

#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif

	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[�Ƀf�[�^���������݂܂�
* [����]  addr�F�����݃A�h���X�Adata�F�����݃f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static BOOL WriteMemoryInt(INT addr, INT data)
{	
	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);
	ER	result = E_SUCCESS;

	// �o�X�o�R�ɂ��f�[�^�ʒm�̂��߃A�N�Z�X�p�x������
	// �ꍇ�A�f�[�^�����̉\�����łĂ��邽�ߔr������ǉ�
	MAC_WAI_SEM( ID_SEM_MEMRW );

	*(p+0) = (UB)(data >> 0);
	*(p+1) = (UB)(data >> 8);
	*(p+0) = (UB)(data >> 16);
	*(p+1) = (UB)(data >> 24);

	// TRUE:�r�W�[	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}

#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif
	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[����f�[�^��ǂݏo���܂�
* [����]  addr�F�ǂݏo���A�h���X�Adata�F�ǂݏo���f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static BOOL ReadMemory(INT addr,  UH *data)
{
	UB	b0;
	UB	b1;
	ER	result = E_SUCCESS;

	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);

	// �o�X�o�R�ɂ��f�[�^�ʒm�̂��߃A�N�Z�X�p�x������
	// �ꍇ�A�f�[�^�����̉\�����łĂ��邽�ߔr������ǉ�
	MAC_WAI_SEM( ID_SEM_MEMRW );

	b0 = (UB)*p++;
	b1 = (UB)*p;
	*data      = (b1 << 8) | b0;

#ifdef WAIT_TYPE_IFB
	// TRUE:�r�W�[	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}
#endif
#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif
	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�ɃR�}���h���������݂܂�
* [����]  command�F�R�}���h
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static BOOL  SetMainCommand(UH command)
{
	return WriteMemory(MNET_COMMAND, command);
}
/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃X�e�[�^�X���擾���܂�
* [����]  status�FMNET��Ԋi�[��
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static BOOL GetMainStatus(MNETStatus* status)
{
	return ReadMemory(MNET_STATUS, &status->UNID.data);
}



// --- PCD4611 ---------------------------------------------------------------------------------------------

/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[�Ƀf�[�^���������݂܂�
* [����]  addr�F�����݃A�h���X�Adata�F�����݃f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static void WriteMemoryPCD4611(UB data)
{
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	pPulseMotor->unCmdSts.stWrite.COMB0 = (unsigned char)data;
}
/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[�Ƀf�[�^���������݂܂�
* [����]  addr�F�����݃A�h���X�Adata�F�����݃f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static void WriteMemoryIntPCD4611(INT data)
{	
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	pPulseMotor->BUFB0 = (unsigned char)( data >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( data >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( data >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( data >> 24 );
}

/***********************************************************************
*
* [�T�v]  �Z���^�[�f�o�C�X�̃������[����f�[�^��ǂݏo���܂�
* [����]  addr�F�ǂݏo���A�h���X�Adata�F�ǂݏo���f�[�^
* [�ߒl]  TRUE:�����AFALSE:���s
* 
***********************************************************************/
static INT ReadMemoryPCD4611()
{
	INT result = 0x00;
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	result |=	( (INT)( pPulseMotor->BUFB0 ) << 0 );
	result |=	( (INT)( pPulseMotor->BUFB1 ) << 8 );
	result |=	( (INT)( pPulseMotor->BUFB2 ) << 16 );
	result |=	( (INT)( pPulseMotor->BUFB3 ) << 24 );
	
	return result;
}
// --- PCD4611 ---------------------------------------------------------------------------------------------



///<summary>
/// ���C���f�o�C�X�̃��W�X�^�[�Ƀf�[�^���������݂܂�
///</summary>
static BOOL  SetMainRegister(UH reg, UH data)
{
	// FALSE:�����ݎ��s?
	if( WriteMemory(MNET_IO_BUFFER, data) == FALSE )
	{
		return FALSE;
	}
	return SetMainCommand(reg);
}


///<summary>
/// �w�胂�[�^�[�f�o�C�X�ɃR�}���h���������݂܂�
///</summary>
static BOOL   SetMotorCommand(UB dev_no, UH command)
{
	if( MNET_DRV_TYPE[dev_no] == PCD4611 )
	{
		WriteMemoryPCD4611( (UB)command );
	}
	else
	{
		WriteMemory(MNET_FIFO, command);								// ���W�X�^��������

		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + dev_no)) )	// FALSE:�R�}���h�Z�b�g���s?
		{
			return FALSE;
		}

#ifdef WAIT_END_CHK
		
		if( EndIdleWait()  == TRUE )									// TRUE:�r�W�[	
		{
			return FALSE;
		}
#endif
	}
	return TRUE;
}

///<summary>
/// �w�胂�[�^�[�f�o�C�X�̃��W�X�^�[�Ƀf�[�^���������݂܂�
///</summary>
static BOOL   SetMotorRegisterInt(UB dev_no, UH reg, INT value)
{
	if( MNET_DRV_TYPE[dev_no] == PCD4611 )
	{
		WriteMemoryIntPCD4611(value);
		WriteMemoryPCD4611((UB)reg);
	}
	else
	{
		WriteMemory   (MNET_FIFO, reg);		// ���W�X�^��������
		WriteMemoryInt(MNET_FIFO, value);	// �f�[�^��������
		
		if( EndIdleWait()  == TRUE )		// TRUE:�r�W�[�̂܂�
		{
			return ( FALSE );
		}
	}
	
	return TRUE;
}

///<summary>
/// �w�胂�[�^�[�f�o�C�X�̃��W�X�^�[�Ƀf�[�^���������݂܂�
///</summary>
static BOOL   SetMotorRegister(UB dev_no, UH reg, UH value)
{
	UB mnetno;

	mnetno = MNET_NO[dev_no];

	if( MNET_DRV_TYPE[mnetno] == PCD4611 )
	{
		WriteMemoryPCD4611(value);
		WriteMemoryPCD4611(reg);
	}
	else
	{
		WriteMemory   (MNET_FIFO, reg);		// ���W�X�^��������
		WriteMemoryInt(MNET_FIFO, value);	// �f�[�^��������

		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + mnetno)) )// FALSE:�R�}���h�Z�b�g���s?
		{
			return ( FALSE );
		}
		
		if( EndIdleWait()  == TRUE )		// TRUE:�r�W�[	
		{
			return ( FALSE );
		}
	}


	return TRUE;
}
///<summary>
/// �w�胂�[�^�[�f�o�C�X�̃��W�X�^�[����f�[�^��ǂݍ��݂܂�
///</summary>
// FALSE:�ُ�ATRUE:����
BOOL	GetMotorRegister(UB dev_no, UH reg, INT* value)
{
	INT	fifo_dat;
	UB mnetno;
	MNETStatus status;

	mnetno = MNET_NO[dev_no];

	if( MNET_DRV_TYPE[mnetno] == PCD4611 )
	{
		WriteMemoryPCD4611((UB)reg);
		*value  =  ReadMemoryPCD4611();
	}
	else
	{
		if (!SetMainCommand(MNET_CMD_CLEAR_RECV_FIFO))		return FALSE;

		// ���[�h�R�}���h���e�h�e�n�֏�������
		WriteMemory(MNET_FIFO, reg);
		// ���[�h�R�}���h���e�h�e�n�֏�������
		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + mnetno)) )
		{
			return ( FALSE );
		}
		// �X�e�[�^�X���[�h����
		while (TRUE)
		{
			// �X�e�[�^�X���[�h
			GetMainStatus(&status);

			// CEND = 1?
			if( status.UNID.BIT.CEND == 1 )
			{
				// EDTE = 0?
				if(status.UNID.BIT.EDTE == 0 )
				{
					// ����
					break;
				}
				else
				{
					// �ُ�
					return FALSE;
				}
			}
		}
		*value  = ((INT)(pMotionnet->fifo0) <<  0);
		*value |= ((INT)(pMotionnet->fifo1) <<  8);
#ifdef WAIT_TYPE_IFB
		// TRUE:�r�W�[	
		if( IfbIdleWait() == TRUE )
		{
			return FALSE;
		}
#endif

		fifo_dat  =  (pMotionnet->fifo0 & 0x000000ff);
		fifo_dat |= ((pMotionnet->fifo1 <<  8) & 0x0000ff00);
		g_Sleep(30);
		fifo_dat |= ((pMotionnet->fifo0 << 16) & 0x00ff0000);
		fifo_dat |= ((pMotionnet->fifo1 << 24) & 0xff000000);
		*value  =  fifo_dat;
#ifdef WAIT_END_CHK
		// TRUE:�r�W�[	
		if( EndIdleWait()  == TRUE )
		{
			return FALSE;
		}
#endif
	}

	
	return( TRUE );
}


///<summary>
/// �w�胂�[�^�[�f�o�C�X�̃X�e�[�^�X��ǂݍ��݂܂�( PCD4611�ȊO )
///</summary>
BOOL GetMotorStatus(UB dev_no, MotorStatus* status)
{
	UB Data_H = 0;
	UB Data_L = 0;
	UH Data_HL;

	if (!IsExist( dev_no ))
	{
		return FALSE;
	}
	
	//	struct
	//	{
	//		UH	reserve2: 7;	// ���0�ɂȂ�܂��B
	//		UH	SBSY	: 1;	// �p���X�o�͊J�n�łP�ɂȂ�܂��B�����~�łO�ɂȂ�܂��B�i���a�r�x�j
	//		UH	reserve1: 4;	// ���0�ɂȂ�܂��B
	//		UH	SEVT	: 1;	// �C�x���g���荞�ݔ����ɂ��P�ɂȂ�A�q�h�r�s�ǂݏo���łO�ɂȂ�܂��B
	//		UH	SERR	: 1;	// �G���[���荞�ݔ����ɂ��P�ɂȂ�A�q�d�r�s�ǂݏo���łO�ɂȂ�܂��B
	//		UH	SEND	: 1;	// �����~�ɂ�芄�荞�݂𔭐����A�P�ɂȂ�܂��B���荞�݃��Z�b�g�R�}���h(0008h)�łO�ɂȂ�܂��B
	//		UH	SINT	: 1;	// ���荞�ݔ����ɂ��P�ɂȂ�܂��B(�r�b�g�P,�Q,�R�̂����ꂩ���P)
	//	}BIT;
	MAC_READ( pMotionnet->portData[dev_no][0], Data_L )
	MAC_READ( pMotionnet->portData[dev_no][1], Data_H )

	status->UNID.data = ( ( Data_H * 0x100 ) + Data_L ) ;

	return TRUE;
}
/******************************************************************************
* 
* [�T�v]	�w��f�o�C�X�����݂��邩�`�F�b�N���܂�
* [����]	���[�^�[�ԍ�
* [�ߒl]	TRUE:�f�o�C�X�L�AFALSE:�f�o�C�X��
* 
******************************************************************************/
BOOL IsExist(UB dev_no)
{
	UB result = 0;
	UB mnetno ;

	// �ڑ�����𒴂��Ă���H
	if( ( dev_no > DVMAX )||( ( dev_no < DVMIN ) ))
	{
		return( FALSE );
	}

	switch( MNET_DRV_TYPE[dev_no] )
	{
	case BCD4020FU:
	case BCD4020FUA:
		mnetno = dev_no;
		break;

	case PCD4611:
		return TRUE;
		break;

	case M204DUM:
		mnetno = MNET_NO[dev_no]; 
		break;
	case NCPRSUNIT:
		return TRUE;
		break;
	}

// �V���~���[�V�������[�h
#if DEBUG_SIM_MODE != 0
	return( TRUE ) ;
#endif
	
	MAC_READ( pMotionnet->deviceInformation[mnetno], result );

	return result != 0;
}

static	INT pcd_rsts;

/******************************************************************************
* 
* [�T�v]	�p���X���[�^�����s��Ԃ�Ԃ�
* [����]	���[�^�[�ԍ�
* [�ߒl]	TRUE:���s���AFALSE:�A�C�h�����O
* 
******************************************************************************/
BOOL motor_IsBusy(UB pm)
{
	INT Loop;
	UH  wk = 0;
	MotorStatus	mstatus;

	if (!IsExist( pm ))
	{
		return FALSE;
	}
	
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_RSTS);
		pcd_rsts = ReadMemoryPCD4611();
		
		if(
			( (pcd_rsts & 0x0000000A) == 0x0000000A )||
			( (pcd_rsts & 0x0000000B) == 0x0000000B )||
			( (pcd_rsts & 0x0000000C) == 0x0000000C )||
			( (pcd_rsts & 0x0000000D) == 0x0000000D )
		)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		for( Loop = 0; Loop <= 3; Loop++ )
		{
			GetMotorStatus(pm, (MotorStatus* )&mstatus);

			if ( wk != mstatus.UNID.BIT.SBSY )
			{
				wk = mstatus.UNID.BIT.SBSY;
				Loop = 0;
			}
		}
		
		if( wk == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/******************************************************************************
* 
* [�T�v]	�p���X���[�^���w������̃��~�b�g�ʒu�̏ꍇTRUE��Ԃ��֐�
* [����]	���[�^�[�ԍ��A����
* [�ߒl]	TRUE:���~�b�g�AFALSE:���~�b�g�ȊO
* 
******************************************************************************/
BOOL motor_IsLimit(UB pm, UH dir)
{
	BOOL result = FALSE;
	RSTS rsts;
	INT  rest= 0;

	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_REST);
		rest = ReadMemoryPCD4611();
		
		if( dir == CW )
		{
			if((rest & 0x00000001))		return( TRUE );
		}
		if( dir == CCW )
		{
			if((rest & 0x00000002))		return( TRUE );
		}
	}
	else
	{
		if (!GetMotorRegister(pm, RRG_RSTS, &rsts.UNID.data))
		{
			return( result );
		}

		if( dir == CW )
		{
			if( rsts.UNID.BIT.SPEL )
			{
				result = TRUE;
			}
		}
		if( dir == CCW )
		{
			if( rsts.UNID.BIT.SMEL )
			{
				result = TRUE;
			}
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	�w��p���X���[�^�����~�b�g�ʒu�̏ꍇTRUE��Ԃ��֐�
* [����]	���[�^�[�ԍ�
* [�ߒl]	TRUE:���~�b�g�AFALSE:���~�b�g�ȊO
* 
******************************************************************************/
BOOL motor_IsLimitW(UB pm)
{
	if( motor_IsLimit(pm,CW) == TRUE )	return( TRUE );
	if( motor_IsLimit(pm,CCW) == TRUE )	return( TRUE );

	return( FALSE );
}
/******************************************************************************
* 
* [�T�v]	�G���R�[�_�L�����擾
* [����]	���[�^�[�ԍ�
* [�ߒl]	TRUE:�L�AFALSE:��
* 
******************************************************************************/
BOOL IsEncoderExist(UB pm)
{
	BOOL	result = FALSE;
	UB		index;
	UB		bitsfh = 0x00;
	UB		bitdat = 0x01;
	UB		pmwk;

	// ���[�^�͈͊O
	if( pm > DVMAX )	return( result );
	
	// bit0    �`���l��1
 	// bit1    �`���l��2
	// �E
	// bit63   �`���l��64
	if( pm == 0 )		return( result );
	pmwk = pm - 1;

	index  = ( pmwk / 8 );
	bitsfh = ( pmwk % 8 );
	bitdat = (bitdat << bitsfh);
	
	// �G���R�[�_�t��
	if( (ENCODER_EXIST[index] & bitdat) )
	{
		result = TRUE;
	}
	
	return( result );
}
/******************************************************************************
* 
* [�T�v]	�p���X���[�^��㎥
* [����]	���[�^�[�ԍ��A�㎥����w��
* [�ߒl]	TRUE:�����AFALSE:���s
* 
******************************************************************************/
BOOL motor_PowerUp(UB pm, UB sw)
{
	INT cnt;
	INT rcnt;
	UB DataCmp;
	UB DataSet;
	UB mnet_drv_typ;

	mnet_drv_typ = MNET_DRV_TYPE[pm];

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( TRUE );
	}

	if(
		( mnet_drv_typ != BCD4020FUA )&&
		( mnet_drv_typ != BCD4020FU )
	)
	{
		return FALSE;
	}
	
	MAC_READ( pMotionnet->portData[pm][2], DataSet );

	switch( mnet_drv_typ )
	{
	case BCD4020FUA:
		if (sw)
		{
			DataSet &= (~0x01);	// �㎥ON
		}
		else
		{
			DataSet |= 0x01;	// �㎥OFF
		}
		break;
	case BCD4020FU:
		if (sw)
		{
			DataSet |= 0x40;	// �㎥ON
		}
		else
		{
			DataSet &= (~0x40);	// �㎥OFF
		}
		break;
	}

//x		g_Sleep(20);
//x		g_Sleep(40);
//o		g_Sleep(50);
//o		g_Sleep(60);
//o		g_Sleep(500);
//o		g_Sleep(250);
//o		g_Sleep(120);
	for( cnt = 0; cnt < READ_RTY_MAX; cnt++ )
	{
		MAC_WRIT( pMotionnet->portData[pm][3], DataSet  );

		for( rcnt = 0; rcnt < READ_RTY_MAX; rcnt++ )
		{
			g_Sleep(WAITTIM);
			MAC_READ( pMotionnet->portData[pm][2], DataCmp );
			if( DataSet == DataCmp )
			{
				return TRUE;
			}
			else
			{
			}
		}
	}
	
	
	return FALSE;
}
///<summary>
/// �S�Ẵ��W�X�^�ɒl���������ފ֐�
///</summary>
ER motor_SetAllRegister(UB pm)
{
	RMD		rmd;

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( E_SUCCESS );
	}

	// �f�o�C�X:PCD4611
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

//		WriteMemoryPCD4611( CMD_CUN1R );		// CUN1R : 
//		if( !SetMotorRegisterInt(pm, WRG_RFA, 1                  ) )	return(( E_MNT_ACCESS + WRG_RFA ));// 9Bh
		SetMotorRegisterInt( pm, WRG_PRFL, _pmInfo[pm].reg[R1] );	// RFL
		SetMotorRegisterInt( pm, WRG_PRFH, _pmInfo[pm].reg[R2] );	// RFH
		SetMotorRegisterInt( pm, WRG_PRUR, _pmInfo[pm].reg[R3] );	// RUR
		SetMotorRegisterInt( pm, WRG_PRDR, 0                   );	// RDR
		SetMotorRegisterInt( pm, WRG_PRMG, _pmInfo[pm].reg[R4] );	// RMG
		SetMotorRegisterInt( pm, WRG_PRDP, 0                   );	// RDP
		SetMotorRegisterInt( pm, WRG_PRDS, _pmInfo[pm].reg[R9] );	// RDS
		SetMotorRegisterInt( pm, WRG_PRMG, 1199                );	// RMG

	}
	// �f�o�C�X:FU,FU-A
	else
	{
		if( !GetMotorRegister(pm, RRG_RMD, &rmd.UNID.data))				return(( E_MNT_ACCESS + RRG_RMD ));

		rmd.UNID.BIT.MSMD = (_pmInfo[pm].param.accel_type == ACCEL_STYPE ? 1 : 0);
		rmd.UNID.BIT.MINT = 1;
		if( !SetMotorRegisterInt(pm, WRG_RMD, rmd.UNID.data) )			return(( E_MNT_ACCESS + WRG_RMD ));// 97h
		if( !SetMotorRegisterInt(pm, WRG_RFL, _pmInfo[pm].reg[R1]) )	return(( E_MNT_ACCESS + WRG_RFL ));// 91h
		if( !SetMotorRegisterInt(pm, WRG_RFH, _pmInfo[pm].reg[R2]) )	return(( E_MNT_ACCESS + WRG_RFH ));// 92h
		if( !SetMotorRegister(pm,   WRG_RUR, _pmInfo[pm].reg[R3]) )		return(( E_MNT_ACCESS + WRG_RUR ));// 93h
		if( !SetMotorRegister(pm,   WRG_RDR, 0) )						return(( E_MNT_ACCESS + WRG_RDR ));// 94h
		if( !SetMotorRegisterInt(pm, WRG_RMG, _pmInfo[pm].reg[R4]) )	return(( E_MNT_ACCESS + WRG_RMG ));// 95h
		if( !SetMotorRegisterInt(pm, WRG_RDP, 0) )						return(( E_MNT_ACCESS + WRG_RDP ));// 96h
		if( !SetMotorRegisterInt(pm, WRG_RFA, 1) )						return(( E_MNT_ACCESS + WRG_RFA ));// 9Bh
		if( !SetMotorRegisterInt(pm, WRG_RUS, _pmInfo[pm].reg[R8]) )	return(( E_MNT_ACCESS + WRG_RUS ));// 99h
		if( !SetMotorRegisterInt(pm, WRG_RDS, _pmInfo[pm].reg[R9]) )	return(( E_MNT_ACCESS + WRG_RDS ));// 9Ah
	}

	return( E_SUCCESS );
}
///<summary>
/// ���[�^����p�����[�^����e���W�X�^�̒l���v�Z����֐�
///</summary>
static ER motor_CalcMotorReg(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg)
{
	if( mp->accel_type > 1 )		return(( E_PAR_OUTRNG + PARAM1 ));
	if( mp->FH == 0 )				return(( E_PAR_OUTRNG + PARAM2 ));
	if( mp->FL == 0 )				return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->FH <= mp->FL )			return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->utime <= 0)				return(( E_PAR_OUTRNG + PARAM4 ));

	factor	= mp->factor;
	param	= (mp->accel_type == ACCEL_LINEAR ? 5000 : 2500);
	RMG		= factor;
	RFL		= (mp->FL * (RMG + 1)) / 200;
	RFH		= (mp->FH * (RMG + 1)) / 200;
	RUR		= ((mp->utime * param) / (RFH - RFL)) - 1;
	RUS		= (mp->us * (RMG + 1)) / 200;
	RDS		= (mp->ds * (RMG + 1)) / 200;
	
	pm_reg->reg[R1]	= RFL;
	pm_reg->reg[R2]	= RFH;
	pm_reg->reg[R3]	= RUR;
	pm_reg->reg[R4]	= RMG;
	pm_reg->reg[R5]	= 0;
	pm_reg->reg[R6]	= 2;
	pm_reg->reg[R7]	= 0;
	pm_reg->reg[R8]	= RUS;
	pm_reg->reg[R9]	= RDS;

	return ( E_SUCCESS );
}
///<summary>
/// ���[�^����p�����[�^����e���W�X�^�̒l���v�Z����֐�
///</summary>
static ER motor_CalcMotorRegPCD4611(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg)
{
	if( mp->accel_type > 1 )		return(( E_PAR_OUTRNG + PARAM1 ));
	if( mp->FH == 0 )				return(( E_PAR_OUTRNG + PARAM2 ));
	if( mp->FL == 0 )				return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->FH <= mp->FL )			return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->utime <= 0)				return(( E_PAR_OUTRNG + PARAM4 ));

	factor	= MOTORSPD_TBL_PCD4611[2];
	param	= (mp->accel_type == ACCEL_LINEAR ? 5000 : 2500);
	RMG		= factor;
	RFL		= (mp->FL * (RMG + 1)) / 2441;						// 40,000,000/ 16384 = 2441
	RFH		= (mp->FH * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	RUR		= ((mp->utime * param) / (RFH - RFL)) - 1;
	RUS		= (mp->us * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	RDS		= (mp->ds * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	
	pm_reg->reg[R1]	= RFL;
	pm_reg->reg[R2]	= RFH;
	pm_reg->reg[R3]	= RUR;
	pm_reg->reg[R4]	= RMG;
	pm_reg->reg[R5]	= 0;
	pm_reg->reg[R6]	= 2;
	pm_reg->reg[R7]	= 0;
	pm_reg->reg[R8]	= RUS;
	pm_reg->reg[R9]	= RDS;

	return ( E_SUCCESS );
}
/////<summary>
/// �R�}���h���Z�b�g����֐�
///</summary>
static void motor_RegistCommand(UB pm, UH cmd)
{
	_pmInfo[pm].command.code     = cmd;
	_pmInfo[pm].command.status   = PSTAT_READY;

	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		// PCD4611�́A�����s�v�B�O�i�K�Ŏ��s�ς�
	}
	else
	{
		SetMotorCommand(pm, _pmInfo[pm].startParam);
	}
}
///<summary>
/// �w���s�ҋ@���܂�
///</summary>
static void g_Sleep(INT usec)
{
    INT i;
    
    i = 40000;          // 6.25 ns/instruction * 4 instructions * loop_count = 1ms = 1000000 ns
    i *= usec;
    i /= 1000;
    while (i--);        /* loop count */
}
///<summary>
/// ���荞�ݗv�����N���A���܂�
///</summary>
static void ClearInterrupt(void)
{
}

/******************************************************************************
* 
* [�T�v]	�h�e�a�A�C�h�����O�҂�
* [����]	�|
* [�ߒl]	IFB�r�b�g(TRUE = �r�W�[ , FALSE = �A�C�h�����O)
* 
******************************************************************************/
static BOOL IfbIdleWait(void)
{
	INT	loop;
	
	for( loop = 0; loop < MNET_CMD_RTY_MAX; loop++ )
	{
		if( ChkIFB() == TRUE )	return( FALSE );
		g_Sleep(5);
	}
	
	return( TRUE );
}
/******************************************************************************
* 
* [�T�v]	�h�e�a��Ԏ擾
* [����]	�|
* [�ߒl]	IFB�r�b�g(TRUE = 1 , FALSE = 0)
* 
******************************************************************************/
static BOOL ChkIFB( void )
{
	BOOL	result; 

	if( PB.DR.BIT.B19 )
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}

	return( result );
}
/******************************************************************************
* 
* [�T�v]	�����҂�
* [����]	�|
* [�ߒl]	TRUE = �r�W�[ , FALSE = �A�C�h�����O
* 
******************************************************************************/
static BOOL EndIdleWait(void)
{
	INT	loop;
	
	for( loop = 0; loop < MNET_CMD_RTY_MAX; loop++ )
	{
		if( ChkEnd() == TRUE )	return( FALSE );
		g_Sleep(5);
	}

	return( TRUE );
}
/******************************************************************************
* 
* [�T�v]	�G���h��Ԏ擾
* [����]	�|
* [�ߒl]	TRUE:�����AFALSE:������
* 
******************************************************************************/
static BOOL ChkEnd( void )
{
	BOOL	result; 
	MNETStatus status;

	GetMainStatus(&status);

	if (status.UNID.BIT.CEND)
	{
		if (status.UNID.BIT.EDTE)
		{
			result = FALSE;	//�G���[
		}
		else
		{
			result = TRUE;
		}
	}
	else
	{
		result = FALSE;
	}

	return( result );
}
/******************************************************************************
* 
* [�T�v]	RBSY��Ԏ擾
* [����]	�|
* [�ߒl]	1:ON�A0:OFF
* 
******************************************************************************/
static UB ChkRbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.RBSY ? SET : CLR );

	return( result );
}
/******************************************************************************
* 
* [�T�v]	DBSY��Ԏ擾
* [����]	�|
* [�ߒl]	1:ON�A0:OFF
* 
******************************************************************************/
static UB ChkDbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.DBSY ? SET : CLR );

	return( result );
}
/******************************************************************************
* 
* [�T�v]	SBSY��Ԏ擾
* [����]	�|
* [�ߒl]	1:ON�A0:OFF
* 
******************************************************************************/
static UB ChkSbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.SBSY ? SET : CLR );

	return( result );
}
static UB dev_nowk;
/******************************************************************************
* 
* [�T�v]	RENV1 �ݒ�( BCD-4020FUA , BCD-4020FU )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv1Fu( UB dev_no )
{
	RENV1		renv1;
	ER 			result = E_SUCCESS;
dev_nowk = dev_no;
	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv1, CLR, sizeof( RENV1 ) );

		renv1.UNID.BIT.PDTC = 1;

		//FU-A
		if( MNET_DRV_TYPE[dev_no] == BCD4020FUA )
		{
			// �����J�����g�_�E��
			renv1.UNID.BIT.CDWN = 1;
		}

		// 9Ch
		if (!SetMotorRegisterInt( dev_no, WRG_RENV1, renv1.UNID.data ) )
		{
			result = E_MNT_ACCESS + WRG_RENV1;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RENV2 �ݒ�( BCD-4020FUA , BCD-4020FU )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv2Fu( UB dev_no, UB teibai )
{
	ER 			result = E_SUCCESS;
	RENV2		renv2;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv2, CLR, sizeof( RENV2 ) );
		
		// P0M �㎥�o��
		renv2.UNID.BIT.P0M  = 1;
		
		if( MNET_DRV_TYPE[dev_no] == BCD4020FUA )
		{
			// P1M �ėp�o��
			renv2.UNID.BIT.P1M  = 1;

			// P2M �ėp�o��
			renv2.UNID.BIT.P2M  = 1;
		}
		else
		{
			// P6M �����J�����g�_�E��
			renv2.UNID.BIT.P6M  = 1;
		}
		
		// EOF
		renv2.UNID.BIT.E0FF = 0;

		// EA/EB���͎d�l
		switch( teibai )
		{
		// 1���{
		case EC_1:
			renv2.UNID.BIT.EIM0 = 0;
			renv2.UNID.BIT.EIM1 = 0;
			break;
		// 2���{
		case EC_2:
			renv2.UNID.BIT.EIM0 = 1;
			renv2.UNID.BIT.EIM1 = 0;
			break;
		// 4���{
		case EC_3:
		default:
			renv2.UNID.BIT.EIM0 = 0;
			renv2.UNID.BIT.EIM1 = 1;
			break;
		}
		
		// 9Dh
		if (!SetMotorRegisterInt( dev_no, WRG_RENV2, renv2.UNID.data ) )
		{
			result = E_MNT_ACCESS + WRG_RENV2;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RENV3 �ݒ�( BCD-4020FUA , BCD-4020FU )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv3Fu( UB dev_no, UB enc )
{
	ER 			result = E_SUCCESS;
	RENV3		renv3;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv3, CLR, sizeof( RENV3 ) );

		// ���_���A����ݒ�F���_���A�Q
		renv3.UNID.BIT.ORM0 = 0;
		renv3.UNID.BIT.ORM1 = 0;
		renv3.UNID.BIT.ORM2 = 1;
		renv3.UNID.BIT.ORM3 = 1;

		// COUNTER2�J�E���g�ݒ�FEA/EB����
		renv3.UNID.BIT.CI20 = 0;
		renv3.UNID.BIT.CI21 = 0;

		// COUNTER3�J�E���g�ݒ�F�o�̓p���X��EA/EB���́i�΍��j
		renv3.UNID.BIT.CI30 = 0;
		renv3.UNID.BIT.CI31 = 0;
		renv3.UNID.BIT.CI32 = 1;

		// 9Eh
		if (!SetMotorRegisterInt(dev_no, WRG_RENV3, renv3.UNID.data) )
		{
			result = E_MNT_ACCESS + WRG_RENV3;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RENV4 �ݒ�( BCD-4020FUA , BCD-4020FU )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv4Fu( UB dev_no, INT so )
{
	ER 			result = E_SUCCESS;
	RENV4		renv4;
	UB 			sts;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv4, CLR, sizeof( RENV4 ) );
		
		sts = ( so > 0 ? SET : CLR );

		// �R���p���[�^�R�̔�r�J�E���^�I���FCOUNTER3(�ėp�E�΍�)
		renv4.UNID.BIT.C3C0 = 0;
		renv4.UNID.BIT.C3C1 = sts;

		// �R���p���[�^�R�̔�r���@�I���FRCMP3����r�J�E���^
		renv4.UNID.BIT.C3S0 = sts;
		renv4.UNID.BIT.C3S1 = 0;
		renv4.UNID.BIT.C3S2 = 0;
		renv4.UNID.BIT.C3S3 = 0;

		// �R���p���[�^�R�����������̏����I���F����~
		renv4.UNID.BIT.C3D0 = sts;
		renv4.UNID.BIT.C3D1 = 0;
		
		// 9Fh
		if (!SetMotorRegisterInt(dev_no, WRG_RENV4, renv4.UNID.data) )
		{
			result = E_MNT_ACCESS + WRG_RENV4;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RMD �ݒ�( BCD-4020FUA , BCD-4020FU )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRmdFu( UB dev_no, UB mod )
{
	ER 			result = E_SUCCESS;
	RMD			rmd;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// ���̓��j�b�g
	{
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &rmd, CLR, sizeof( RMD ) );
		
		if (!GetMotorRegister(dev_no, RRG_RMD, &rmd.UNID.data))
		{
			return(( E_MNT_ACCESS + RRG_RMD ));
		}
		rmd.UNID.BIT.MOD = mod;
		rmd.UNID.BIT.MINT = 1;

		if (!SetMotorRegisterInt(dev_no, WRG_RMD, rmd.UNID.data))
		{
			return(( E_MNT_ACCESS + WRG_RMD ));
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RMV �ݒ�
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRmv( UB dev_no, INT pos )
{
	ER 			result = E_SUCCESS;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )
	{
		// ���̓��j�b�g
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		if (!SetMotorRegisterInt(dev_no, WRG_RMV, pos))
		{
			return(( E_MNT_ACCESS + WRG_RMV ));
		}
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	RMV �ݒ�
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRmvPCD4611( UB dev_no, INT pos )
{
	WriteMemoryIntPCD4611( pos );
	WriteMemoryPCD4611( WRG_RMV );

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [�T�v]	RENV1 �ݒ�( PCD4611 )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv1PCD4611( UB dev_no )
{

	return( E_SUCCESS );
}

/******************************************************************************
* 
* [�T�v]	RENV2 �ݒ�( PCD4611 )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv2PCD4611()
{
	WriteMemoryIntPCD4611( 0x00000005 );// EA/EB: A/B��1���{, P1�`P0: �ėp�o��
	WriteMemoryPCD4611( WPCD_RENV2 );	// 0x9D

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [�T�v]	RENV3 �ݒ�( PCD4611 )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRenv3PCD4611( UB dev_no, UB enc )
{
	return( E_SUCCESS );
}
/******************************************************************************
* 
* [�T�v]	RMD �ݒ�( PCD4611 )
* [����]	�|
* [�ߒl]	�|
* 
******************************************************************************/
static ER SetRmdPCD4611( UB dev_no, UB mod )
{
	WriteMemoryIntPCD4611( mod );
	WriteMemoryPCD4611( WPCD_PRMD );

	return( E_SUCCESS );
}

#ifdef DEBUG_INFO
static 	INT dbgread;

/******************************************************************************
* 
* [�T�v]	SBSY��Ԏ擾
* [����]	�|
* [�ߒl]	1:ON�A0:OFF
* 
******************************************************************************/
static void DebugMnetInfo( UB mm )
{

	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMV, &dbgread);           //  RRG_PRMV    = 0x00C0,   PRMV ���W�X�^ �̓ǂݏo��

	dbgread = 0;
//	GetMotorRegister(mm, RRG_PRFL, &dbgread);           //  RRG_PRFL    = 0x00C1,   PRFL ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_PRFH, &dbgread);           //  RRG_PRFH    = 0x00C2,   PRFH ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_PRUR, &dbgread);           //  RRG_PRUR    = 0x00C3,   PRUR ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_PRDR, &dbgread);           //  RRG_PRDR    = 0x00C4,   PRDR ���W�X�^ �̓ǂݏo��

	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMG, &dbgread);           //  RRG_PRMG    = 0x00C5,   PRMG ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRDP, &dbgread);           //  RRG_PRDP    = 0x00C6,   PRDP ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMD, &dbgread);           //  RRG_PRMD    = 0x00C7,   PRMD ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRUS, &dbgread);           //  RRG_PRUS    = 0x00C9,   PRUS ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRDS, &dbgread);           //  RRG_PRDS    = 0x00CA,   PRDS ���W�X�^ �̓ǂݏo��
	
	dbgread = 0;
	GetMotorRegister(mm, RRG_RMV, &dbgread);			//  RRG_RMV     = 0x00D0,   RMV ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RFL, &dbgread);            //  RRG_RFL     = 0x00D1,   RFL ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RFH, &dbgread);            //  RRG_RFH     = 0x00D2,   RFH ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RUR, &dbgread);            //  RRG_RUR     = 0x00D3,   RUR ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RDR, &dbgread);            //  RRG_RDR     = 0x00D4,   RDR ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RMG, &dbgread);            //  RRG_RMG     = 0x00D5,   RMG ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RDP, &dbgread);            //  RRG_RDP     = 0x00D6,   RDP ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RMD, &dbgread);            //  RRG_RMD     = 0x00D7,   RMD ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RUS, &dbgread);            //  RRG_RUS     = 0x00D9,   RUS ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RDS, &dbgread);            //  RRG_RDS     = 0x00DA,   RDS ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RFA, &dbgread);            //  RRG_RFA     = 0x00DB,   RFA ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV1, &dbgread);          //  RRG_RENV1   = 0x00DC,   RENV1 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV2, &dbgread);          //  RRG_RENV2   = 0x00DD,   RENV2 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV3, &dbgread);          //  RRG_RENV3   = 0x00DE,   RENV3 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV4, &dbgread);          //  RRG_RENV4   = 0x00DF,   RENV4 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV5, &dbgread);          //  RRG_RENV5   = 0x00E0,   RENV5 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV6, &dbgread);          //  RRG_RENV6   = 0x00E1,   RENV6 ���W�X�^ �̓ǂݏo��
//	GetMotorRegister(mm, RRG_RENV7, &dbgread);          //  RRG_RENV7   = 0x00E2,   RENV7 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN1, &dbgread);          //  RRG_RCUN1   = 0x00E3,   RCUN1 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN2, &dbgread);          //  RRG_RCUN2   = 0x00E4,   RCUN2 ���W�X�^ �̓ǂݏo��
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN3, &dbgread);          //  RRG_RCUN3   = 0x00E5,   RCUN3 ���W�X�^ �̓ǂݏo��

}
#endif

//	struct
//	{
//		UH	BBSY	: 1;	// RENV0(8)= "1" �Ńu���[�N�ʐM�R�}���h�i0610h�j�𔭍s�����Ƃ��A
//								// �u���[�N�ʐM����������܂� "1" �ƂȂ�܂��B
//		UH	DBSY	: 1;	// �V�X�e���ʐM���A�܂��̓f�[�^�ʐM����1 �ɂȂ�܂��B
//		UH	RBSY	: 1;	// ���Z�b�g��������1 �ɂȂ�܂��B
//		UH	SBSY	: 1;	// �T�C�N���b�N�ʐM�X�^�[�g����1 �ɂȂ�܂��B
//		UH	reserve2: 1;	// ���0 �ɂȂ�܂��B
//		UH	RDBB	: 1;	// �f�[�^��M�pFIFO �Ɏ�M�f�[�^�����鎞��1 �ɂȂ�܂��B
//		UH	TDBB	: 1;	// �f�[�^���M�pFIFO �ɑ��M�f�[�^�����鎞��1 �ɂȂ�܂��B
//		UH	REF		: 1;	// �����M�̏o�̓|�[�g�f�[�^�����鎞1 �ɂȂ�܂��B
//		UH	reserve1: 1;	// ���0 �ɂȂ�܂��B
//		UH	CAER	: 1;	// CPU �̃A�N�Z�X�G���[�ł��B
//		UH	ERAE	: 1;	// ۰�ّ���M�����װ��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
//		UH	EDTE	: 1;	// �f�[�^�ʐM�G���[��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
//		UH	EIOE	: 1;	// �T�C�N���b�N�ʐM�װ��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
//		UH	IOPC	: 1;	// �u���͕ω����荞�ݐݒ�v��1 �ɃZ�b�g�������̓|�[�g�̏�Ԃ��ω���������1 �ɂȂ�A
//								// ���荞�ݐM��(INT)���o�͂��܂��B
//		UH	BRKF	: 1;	// �u���[�N�t���[����M����1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
//		UH	CEND	: 1;	// �f�[�^���M�pFIFO �������݉\����1 �ɂȂ�܂��B
//	}BIT;

