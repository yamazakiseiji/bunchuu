/******************************************************************************
* ���[�V�����l�b�g�Ǘ��^�X�N                                                  *
*                                                                             *
* File name : tskmnt.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _MNTTASK_H_
#define _MNTTASK_H_


//--------------------------------------
// �}�N���u���̒�`
//--------------------------------------
#define	MNET_BASE_ADDRESS			0x0C008000	// Motionnet�A�h���X
#define	PULSEMOTOR_ADDR				0x0C009000	// �p���X���[�^�R���g���[���A�h���X

#define	MOTIONNET_DEVICE_MAX		64			// Motionnet�̃f�o�C�X��
#define	MOTIONNET_PORT_MAX			4			// Motionnet�̃|�[�g��

#define	MOTIONNET_CMD_CYCLIC_START	0x3000		// �T�C�N���b�N�ʐM�̊J�n
#define	MOTIONNET_CMD_CYCLIC_STOP	0x3100		// �T�C�N���b�N�ʐM�̒�~
#define	MOTIONNET_CMD_DATA_START	0x4000		// �f�[�^�ʐM
#define	MOTIONNET_CMD_DATA_CANCEL	0x4100		// �f�[�^�ʐM�L�����Z��
#define	MOTIONNET_CMD_WRENV0		0x5500		// [W] ���ݒ�0


#define	ON							1
#define	OFF							0

// ���{�ݒ�
#define	EC_N						0			// ����
#define	EC_1						1			// �P���{
#define	EC_2						2			// �Q���{
#define	EC_3						3			// �S���{

// �J�E���^�ݒ�
#define	ENC_COUNTER					1			// �G���R�[�_
#define	PLS_COUNTER					2			// �o�̓p���X





//--------------------------------------
// �\���̂̒�`
//--------------------------------------
 ///<summary>
/// ���[�^�[�R�}���h
///</summary>
enum MOTOR_COMMAND
{
	CMD_NOTYPE	= 0,			// �����R�}���h
	CMD_STAFL	= 0x0050,		// �e�k�葬�X�^�[�g
	CMD_STAFH	= 0x0051,		// �e�g�葬�X�^�[�g
	CMD_STAUD	= 0x0053,		// �����X�^�[�g(������FH �葬������)
	CMD_CNTFL	= 0x0054,		// �c�ʂe�k�葬�X�^�[�g
	CMD_CNTFH	= 0x0055,		// �c�ʂe�g�葬�X�^�[�g
	CMD_CNTUD	= 0x0057,		// �c�ʍ����X�^�[�g
	CMD_CMSTA	= 0x0006,		// ���r�s�`�o��(�����X�^�[�g)
	CMD_SPSTA	= 0x002A,		// ���r�s�`���͑�s
	CMD_FCHGL	= 0x0040,		// �e�k�葬�֏u�����x�ύX
	CMD_FCHGH	= 0x0041,		// �e�g�葬�֏u�����x�ύX
	CMD_FSCHL	= 0x0042,		// �e�k���x�܂Ō���
	CMD_FSCHH	= 0x0043,		// �e�g���x�܂ŉ���
	CMD_STOP	= 0x0049,		// ����~
	CMD_SDSTP	= 0x004A,		// ������~
	CMD_CMSTP	= 0x0007,		// ���r�s�o�o��(������~)
	CMD_CMEMG	= 0x0005,		// ����~
	CMD_NOP		= 0x0000,		// �����R�}���h
	CMD_INTRS	= 0x0008,		// ���荞�ݏo��(Ҳݽð��_Bit0)���Z�b�g
	CMD_SRST	= 0x0004,		// �\�t�g�E�F�A���Z�b�g
	CMD_CUN1R	= 0x0020,		// COUNTER1(�w�߈ʒu)���Z�b�g
	CMD_CUN2R	= 0x0021,		// COUNTER2(�@�B�ʒu)���Z�b�g
	CMD_CUN3R	= 0x0022,		// COUNTER3(�ėp�E�΍�)���Z�b�g
	CMD_ERCOUT	= 0x0024,		// �d�q�b�M���̏o��
	CMD_ERCRST	= 0x0025,		// �d�q�b�M���̃��Z�b�g
	CMD_STAON	= 0x0028,		// �o�b�r���͑�s
	CMD_LTCH	= 0x0029,		// �k�s�b���͑�s
};

struct StMotionnet {
	// 0x000�`0x001
	B	commandStatus0;						// �R�}���h���X�e�[�^�X�i�r�b�g7�`0�j
	B	commandStatus1;						// �R�}���h���X�e�[�^�X�i�r�b�g15�`8�j
	
	// 0x002�`0x003
	B	intrStatus0;						// ���荞�݃X�e�[�^�X�i�r�b�g7�`0�j
	B	intrStatus1;						// ���荞�݃X�e�[�^�X�i�r�b�g15�`8�j
	
	// 0x004�`0x005
	B	inputOutputBuff0;					// ���o�̓o�b�t�@�i�r�b�g7�`0�j
	B	inputOutputBuff1;					// ���o�̓o�b�t�@�i�r�b�g15�`8�j
	
	// 0x006�`0x007
	B	fifo0;								// �f�[�^����M�pFIFO�i�r�b�g7�`0�j
	B	fifo1;								// �f�[�^����M�pFIFO�i�r�b�g15�`8�j
	
	// 0x008�`0x077
	B	fifo2;
	B	fifo3;
	B	fifo4;
	B	fifo5;
	B	fifo6;
	B	fifo7;
	B	fifo8;
	B	fifo9;
	B	fifo10;
	B	fifo11;
	B	fifo12;
	B	fifo13;
	B	rsv08[100];							// ����`
	
	// 0x078�`0x0B7
	B	deviceInformation[MOTIONNET_DEVICE_MAX];
												// �f�o�C�X���
	// 0x0B8�`0x0BF
	B	cyclicCommErrorFlg[MOTIONNET_DEVICE_MAX / 8];
												// �T�C�N���b�N�ʐM�G���[�t���O
	// 0x0C0�`0x0DF
	B	inputChangeIntrSettting[MOTIONNET_DEVICE_MAX / 2];
												// ���͕ω����荞�ݐݒ�
	// 0x0E0�`0x0FF
	B	inputChangeIntrFlg[MOTIONNET_DEVICE_MAX / 2];
												// ���͕ω����荞�݃t���O
	// 0x100�`0x1FF
	B	portData[MOTIONNET_DEVICE_MAX][MOTIONNET_PORT_MAX];
												// �|�[�g�f�[�^
};

///<summary>
/// RSTS���W�X�^�F�g���X�e�[�^�X���m�F�ł��܂��B�i�ǂݏo����p�j
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve1: 4;	// �i��ɂO�ɂȂ�܂��j
			INT	SPH4	: 1;	// �㎥�M���o�͂o�g�S���n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPH3	: 1;	// �㎥�M���o�͂o�g�R���n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPH2	: 1;	// �㎥�M���o�͂o�g�Q���n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPH1	: 1;	// �㎥�M���o�͂o�g�P���n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPLS	: 1;	// �p���X�o��(�})���n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SCP3	: 1;	// �b�l�o�R��r�����������ɂP�ɂȂ�܂��B
			INT	SCP2	: 1;	// �b�l�o�Q��r�����������ɂP�ɂȂ�܂��B
			INT	SCP1	: 1;	// �b�l�o�P��r�����������ɂP�ɂȂ�܂��B
			INT	SINP	: 1;	// �h�m�o���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SLTC	: 1;	// �k�s�b���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SCLR	: 1;	// �b�k�q���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SEZ		: 1;	// �d�y���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SERC	: 1;	// �d�q�b�o�͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPCS	: 1;	// �o�b�r���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SEMG	: 1;	// ���d�l�f���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SSTP	: 1;	// ���r�s�o���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SSTA	: 1;	// ���r�s�`���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SDIN	: 1;	// �r�c���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B�i�r�c���͒[�q�̏�ԁj
			INT	SSD		: 1;	// �r�c���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B�i�r�c���b�`�̏�ԁj
			INT	SORG	: 1;	// �n�q�f���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SMEL	: 1;	// �|�d�k���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SPEL	: 1;	// �{�d�k���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SALM	: 1;	// �`�k�l���͐M�����n�m��Ԃ̎��ɂP�ɂȂ�܂��B
			INT	SDIR	: 1;	// ��������i�O�F�{���� �P�F�|�����j
			INT	CND		: 4;	// �����Ԃ�\���܂��B
		}BIT;
	}UNID;
} RSTS;

///<summary> 
/// RSTS���W�X�^  PCD4611�F�g���X�e�[�^�X���m�F�ł��܂��B�i�ǂݏo����p�j
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve31	: 1;	// b31 
			INT	reserve30	: 1;	// b30 
			INT	reserve29	: 1;	// b29 
			INT	reserve28	: 1;	// b28 
			INT	reserve27	: 1;	// b27 
			INT	reserve26	: 1;	// b26 
			INT	reserve25	: 1;	// b25 
			INT	reserve24	: 1;	// b24 
			INT	reserve23	: 1;	// b23 
			INT	reserve22	: 1;	// b22 
			INT	reserve21	: 1;	// b21 
			INT	reserve20	: 1;	// b20 
			INT	reserve19	: 1;	// b19 
			INT	reserve18	: 1;	// b18 
			INT	reserve17	: 1;	// b17 
			INT	SDIR		: 1;	// b16 
			INT	SINP		: 1;	// b15 
			INT	SDIN		: 1;	// b14 
			INT	SLTC		: 1;	// b13 
			INT	SDRM		: 1;	// b12 
			INT	SDRP		: 1;	// b11 
			INT	SEZ			: 1;	// b10 
			INT	SERC		: 1;	// b9  
			INT	SPCS		: 1;	// b8  
			INT	SEMG		: 1;	// b7  
			INT	SSTP		: 1;	// b6  
			INT	SSTA		: 1;	// b5  
			INT	SCD			: 1;	// b4  
			INT	CND3		: 1;	// b3  
			INT	CND2		: 1;	// b2  
			INT	CND1		: 1;	// b1  
			INT	CND0		: 1;	// b0  
		}BIT;
	}UNID;
} RSTSPCD;

///<summary>
/// RENV1���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	CDWN      : 1;	// b31   - / FU-A
			INT	MREV      : 1;	// b30   - / FU-A
			INT	PDTC      : 1;	// b29  FU / FU-A
			INT	SEDR      : 1;	// b28  FU / FU-A
			INT	SEDM      : 1;	// b27  FU / FU-A
			INT	DTMF      : 1;	// b26  FU / FU-A
			INT	FLTR      : 1;	// b25  FU / FU-A
			INT	PCSL      : 1;	// b24  FU / FU-A
			INT	LTCL      : 1;	// b23  FU / FU-A
			INT	INPL      : 1;	// b22  FU / FU-A
			INT	CLR1      : 1;	// b21  FU / FU-A
			INT	CLR0      : 1;	// b20  FU / FU-A
			INT	STPM      : 1;	// b19  FU / FU-A
			INT	STAM      : 1;	// b18  FU / FU-A
			INT	ETW1      : 1;	// b17  FU / FU-A
			INT	ETW0      : 1;	// b16  FU / FU-A
			INT	ERCL      : 1;	// b15  FU / FU-A
			INT	EPW2      : 1;	// b14  FU / FU-A
			INT	EPW1      : 1;	// b13  FU / FU-A
			INT	EPW0      : 1;	// b12  FU / FU-A
			INT	EROR      : 1;	// b11  FU / FU-A
			INT	EROE      : 1;	// b10  FU / FU-A  EA/EB ���͎d�l��ݒ�(00:1���{,01:2���{,10:4���{)
			INT	ALML      : 1;	// b9   FU / FU-A   �V
			INT	ALMM      : 1;	// b8   FU / FU-A
			INT	ORGL      : 1;	// b7   FU / FU-A
			INT	SDL	      : 1;	// b6   FU / FU-A
			INT	SDLT	  : 1;	// b5   FU / FU-A
			INT	SDM	      : 1;	// b4   FU / FU-A
			INT	ELM	      : 1;	// b3   FU / FU-A
			INT	PMD2      : 1;	// b2   FU / FU-A
			INT	PMD1      : 1;	// b1   FU / FU-A
			INT	PMD0      : 1;	// b0   FU / FU-A
		}BIT;
	}UNID;
} RENV1;
///<summary>
/// RENV1���W�X�^  PCD4611
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	PMSK      : 1;	// b31  
			INT	PCSM      : 1;	// b30  
			INT	INTM      : 1;	// b29  
			INT	DTMF      : 1;	// b28  
			INT	DRF       : 1;	// b27  
			INT	FLTR      : 1;	// b26  
			INT	DRL       : 1;	// b25  
			INT	PCSL      : 1;	// b24  
			INT	LTCL      : 1;	// b23  
			INT	INPL      : 1;	// b22  
			INT	FTM1      : 1;	// b21  
			INT	FTM0      : 1;	// b20  
			INT	STPM      : 1;	// b19  
			INT	STAM      : 1;	// b18  
			INT	ETW1      : 1;	// b17  
			INT	ETW0      : 1;	// b16  
			INT	ERCL      : 1;	// b15  
			INT	EPW2      : 1;	// b14  
			INT	EPW1      : 1;	// b13  
			INT	EPW0      : 1;	// b12  
			INT	EROR      : 1;	// b11  
			INT	EROE      : 1;	// b10  
			INT	ALML      : 1;	// b9   
			INT	ALMM      : 1;	// b8   
			INT	ORGL      : 1;	// b7   
			INT	SDL	      : 1;	// b6   
			INT	SDLT	  : 1;	// b5   
			INT	SDM	      : 1;	// b4   
			INT	ELM	      : 1;	// b3   
			INT	PMD2      : 1;	// b2   
			INT	PMD1      : 1;	// b1   
			INT	PMD0      : 1;	// b0   
		}BIT;
	}UNID;
} RENV1PCD;
///<summary>
/// RENV2���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	PDTC      : 1;	// b31 
			INT	PCSM      : 1;	// b30 
			INT	INTM      : 1;	// b29 
			INT	DTMF      : 1;	// b28 
			INT	DRF       : 1;	// b27 
			INT	FLTR      : 1;	// b26 
			INT	DRL       : 1;	// b25 
			INT	PCSL      : 1;	// b24 
			INT	LTCL      : 1;	// b23 
			INT	INPL      : 1;	// b22 
			INT	CLR1      : 1;	// b21 
			INT	CLR0      : 1;	// b20 
			INT	STPM      : 1;	// b19 
			INT	STAM      : 1;	// b18 
			INT	ETW1      : 1;	// b17
			INT	ETW0      : 1;	// b16
			INT	ERCL      : 1;	// b15 
			INT	EPW2      : 1;	// b14
			INT	EPW1      : 1;	// b13
			INT	EPW0      : 1;	// b12
			INT	EROR      : 1;	// b11 
			INT	EROE      : 1;	// b10 EA/EB ���͎d�l��ݒ�(00:1���{,01:2���{,10:4���{)
			INT	ALML      : 1;	// b9  �V
			INT	ALMM      : 1;	// b8  
			INT	ORGL      : 1;	// b7  
			INT	SDL	      : 1;	// b6  
			INT	SDLT	  : 1;	// b5  
			INT	SDM	      : 1;	// b4  
			INT	ELM	      : 1;	// b3  
			INT	PMD2      : 1;	// b2
			INT	PMD1      : 1;	// b1
			INT	PMD0      : 1;	// b0
		}BIT;
	}UNID;
} RENV1DUM;

///<summary>
/// RENV2���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve31 : 1;	// b31�i0�Œ�j
			INT	reserve30 : 1;	// b30�i0�Œ�j
			INT	reserve29 : 1;	// b29�i0�Œ�j
			INT	IMSK      : 1;	// b28�iG9003B:0�Œ�j 
			INT	EXER      : 1;	// b27�iG9003B:0�Œ�j 
			INT	GN2	      : 1;	// b26�iG9003B:0�Œ�j 
			INT	GN1	      : 1;	// b25�iG9003B:0�Œ�j 
			INT	GN0	      : 1;	// b24�iG9003B:0�Œ�j 
			INT	SIFM      : 1;	// b23�iG9003B:0�Œ�j
			INT	IDL2      : 1;	// b22 
			INT	IDL1      : 1;	// b21 
			INT	IDL0      : 1;	// b20 
			INT	ROMB      : 1;	// b19�iG9003B:0�Œ�j
			INT	P0FF      : 1;	// b18 
			INT	E0FF      : 1;	// b17 
			INT	PDIR      : 1;	// b16 
			INT	PIM1      : 1;	// b15 
			INT	PIM0      : 1;	// b14 
			INT	PINF      : 1;	// b13 
			INT	EZL	      : 1;	// b12 
			INT	EDIR      : 1;	// b11 
			INT	EIM1      : 1;	// b10 EA/EB ���͎d�l��ݒ�(00:1���{,01:2���{,10:4���{)
			INT	EIM0      : 1;	// b9  �V
			INT	EINF      : 1;	// b8  
			INT	P7M	      : 1;	// b7  
			INT	P6M	      : 1;	// b6  
			INT	P5M	      : 1;	// b5  
			INT	P4M	      : 1;	// b4  
			INT	P3M	      : 1;	// b3  
			INT	P2M	      : 1;	// b2  
			INT	P1M	      : 1;	// b1  
			INT	P0M	      : 1;	// b0  
		}BIT;
	}UNID;
} RENV2;

///<summary>
/// RENV2���W�X�^  PCD4611
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve31 : 1;	// b31
			INT	IEND      : 1;	// b30
			INT	ORM       : 1;	// b29
			INT	EZL       : 1;	// b28
			INT	EZD3      : 1;	// b27
			INT	EZD2      : 1;	// b26
			INT	EZD1      : 1;	// b25
			INT	EZD0      : 1;	// b24
			INT	PDIR      : 1;	// b23
			INT	PINF      : 1;	// b22
			INT	PIM1      : 1;	// b21
			INT	PIM0      : 1;	// b20
			INT	EDIR      : 1;	// b19
			INT	EINF      : 1;	// b18
			INT	EIM1      : 1;	// b17
			INT	EIM0      : 1;	// b16
			INT	P0EF      : 1;	// b15
			INT	E0FF      : 1;	// b14
			INT	CSP0      : 1;	// b13
			INT	P7M       : 1;	// b12
			INT	P6M       : 1;	// b11
			INT	P5M       : 1;	// b10
			INT	P4M1      : 1;	// b9 
			INT	P4M0      : 1;	// b8 
			INT	P3M1      : 1;	// b7 
			INT	P3M0      : 1;	// b6 
			INT	P2M1      : 1;	// b5 
			INT	P2M0      : 1;	// b4 
			INT	P1M1      : 1;	// b3 
			INT	P1M0      : 1;	// b2 
			INT	P0M1      : 1;	// b1 
			INT	P0M0      : 1;	// b0 
		}BIT;
	}UNID;
} RENV2PCD;

///<summary>
/// RENV3���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{                       //           ���Q�l.RENV2
			INT	POFF      : 1;	// b31       reserve31
			INT	EOFF      : 1;	// b30       reserve30
			INT	SMAX      : 1;	// b29       reserve29
			INT	PMSK      : 1;	// b28       IMSK
			INT	IEND      : 1;	// b27       EXER
			INT	PDIR      : 1;	// b26       GN2
			INT	PIM1      : 1;	// b25       GN1
			INT	PIM0      : 1;	// b24       GN0
			INT	EZL       : 1;	// b23       SIFM
			INT	EDIR      : 1;	// b22       IDL2
			INT	EIM1      : 1;	// b21       IDL1
			INT	EIM0      : 1;	// b20       IDL0
			INT	PINF      : 1;	// b19       ROMB
			INT	EINF      : 1;	// b18       P0FF
			INT	P1L       : 1;	// b17       E0FF
			INT	P0L       : 1;	// b16       PDIR
			INT	P7M1      : 1;	// b15       PIM1
			INT	P7M0      : 1;	// b14       PIM0
			INT	P6M1      : 1;	// b13       PINF
			INT	P6M0      : 1;	// b12       EZL
			INT	P5M1      : 1;	// b11       EDIR
			INT	P5M0      : 1;	// b10       EIM1
			INT	P4M1      : 1;	// b9        EIM0
			INT	P4M0      : 1;	// b8        EINF
			INT	P3M1      : 1;	// b7        P7M
			INT	P3M0      : 1;	// b6        P6M
			INT	P2M1      : 1;	// b5        P5M
			INT	P2M0      : 1;	// b4        P4M
			INT	P1M1      : 1;	// b3        P3M
			INT	P1M0      : 1;	// b2        P2M
			INT	P0M1      : 1;	// b1        P1M
			INT	P0M0      : 1;	// b0        P0M
		}BIT;
	}UNID;
} RENV2DUM;

///<summary>
/// RENV3���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve6: 1;	//�i��ɂO�ɂȂ�܂��j
			INT	CU3H	: 1;	// b30 
			INT	CU2H	: 1;	// b29 
			INT	CU1H	: 1;	// b28 
			INT	reserve5: 1;	// b27 
			INT	CU3B	: 1;	// b26 
			INT	CU2B	: 1;	// b25 
			INT	CU1B	: 1;	// b24 
			INT	reserve4: 1;	// b23 
			INT	CU3R	: 1;	// b22 
			INT	CU2R	: 1;	// b21 
			INT	CU1R	: 1;	// b20 
			INT	reserve3: 1;	// b19 
			INT	CU3C	: 1;	// b18 
			INT	CU2C	: 1;	// b17 
			INT	CU1C	: 1;	// b16 
			INT	reserve2: 1;	// b15 
			INT	reserve1: 1;	// b14 
			INT	BSYC	: 1;	// b13 
			INT	CI32	: 1;	// b12 �b�n�t�m�s�d�q�R�i�ėp�E�΍��j�̃J�E���g���͂�ݒ�
			INT	CI31	: 1;	// b11 �V
			INT	CI30	: 1;	// b10 �V
			INT	CI21	: 1;	// b9  �b�n�t�m�s�d�q�Q�i�@�B�ʒu�j�̃J�E���g���͂�ݒ�
			INT	CI20	: 1;	// b8  �V
			INT	EZD3	: 1;	// b7  
			INT	EZD2	: 1;	// b6  
			INT	EZD1	: 1;	// b5  
			INT	EZD0	: 1;	// b4  
			INT	ORM3	: 1;	// b3  
			INT	ORM2	: 1;	// b2  
			INT	ORM1	: 1;	// b1  
			INT	ORM0	: 1;	// b0  
		}BIT;
	}UNID;
} RENV3;

///<summary>
/// RENV3���W�X�^  PCD4611
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve31: 1;	// b31 
			INT	reserve30: 1;	// b30 
			INT	reserve29: 1;	// b29 
			INT	reserve28: 1;	// b28 
			INT	reserve27: 1;	// b27 
			INT	reserve26: 1;	// b26 
			INT	reserve25: 1;	// b25 
			INT	reserve24: 1;	// b24 
			INT	reserve23: 1;	// b23 
			INT	reserve22: 1;	// b22 
			INT	SYI1	 : 1;	// b21 
			INT	SYI0	 : 1;	// b20 
			INT	SY03     : 1;	// b19 
			INT	SY02	 : 1;	// b18 
			INT	SY01	 : 1;	// b17 
			INT	SY00	 : 1;	// b16 
			INT	C2S1     : 1;	// b15 
			INT	C2S0     : 1;	// b14 
			INT	C1S1	 : 1;	// b13 
			INT	C1S0	 : 1;	// b12 
			INT	C2RM	 : 1;	// b11 
			INT	CU2R	 : 1;	// b10 
			INT	LOF2	 : 1;	// b9  
			INT	CU2L	 : 1;	// b8  
			INT	C1RM	 : 1;	// b7  
			INT	CU1R	 : 1;	// b6  
			INT	LOF1	 : 1;	// b5  
			INT	CU1L	 : 1;	// b4  
			INT	CU2H	 : 1;	// b3  
			INT	CU1H	 : 1;	// b2  
			INT	CIS2	 : 1;	// b1  
			INT	CIS1	 : 1;	// b0  
		}BIT;
	}UNID;
} RENV3PCD;

///<summary>
/// RENV4���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	CU4H	: 1;	// 
			INT	CU3H	: 1;	// 
			INT	CU2H	: 1;	// 
			INT	reserve2: 1;	// 
			INT	CU4B	: 1;	// 
			INT	CU3B	: 1;	// 
			INT	CU2B	: 1;	// 
			INT	CU1B	: 1;	// 
			INT	CU4R	: 1;	// 
			INT	CU3R	: 1;	// 
			INT	CU2R	: 1;	// 
			INT	CU1R	: 1;	// 
			INT	CU4C	: 1;	// 
			INT	CU3C	: 1;	// 
			INT	CU2C	: 1;	// 
			INT	CU1C	: 1;	// 
			INT	reserve1: 1;	// 
			INT	BSYC	: 1;	// 
			INT	CI41	: 1;	// 
			INT	CI40	: 1;	// 
			INT	CI31	: 1;	// 
			INT	CI30	: 1;	// 
			INT	CI21	: 1;	// 
			INT	CI20	: 1;	// 
			INT	EZD3	: 1;	// 
			INT	EZD2	: 1;	// 
			INT	EZD1	: 1;	// 
			INT	EZD0	: 1;	// 
			INT	ORM3	: 1;	// 
			INT	ORM2	: 1;	// 
			INT	ORM1	: 1;	// 
			INT	ORM0	: 1;	// 
		}BIT;
	}UNID;
} RENV3DUM;

///<summary>
/// RENV4���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve3: 4;	//�i��ɂO�ɂȂ�܂��j
			INT	LT0F	: 1;	// b27 
			INT	LTFD	: 1;	// b26 
			INT	LTM1	: 1;	// b25 
			INT	LTM0	: 1;	// b24 
			INT	C3D1	: 1;	// b23 
			INT	C3D0	: 1;	// b22 
			INT	C3S3	: 1;	// b21 
			INT	C3S2	: 1;	// b20 
			INT	C3S1	: 1;	// b19 
			INT	C3S0	: 1;	// b18
			INT	C3C1	: 1;	// b17 
			INT	C3C0	: 1;	// b16 
			INT	reserve2: 1;	// b15 
			INT	C2D1	: 1;	// b14 
			INT	C2D0	: 1;	// b13 
			INT	C2S2	: 1;	// b12 
			INT	C2S1	: 1;	// b11 
			INT	C2S0	: 1;	// b10 
			INT	C2C1	: 1;	// b9  
			INT	C2C0	: 1;	// b8  
			INT	reserve1: 1;	// b7  
			INT	C1D1	: 1;	// b6  
			INT	C1D0	: 1;	// b5  
			INT	C1S2	: 1;	// b4  
			INT	C1S1	: 1;	// b3  
			INT	C1S0	: 1;	// b2  
			INT	C1C1	: 1;	// b1  
			INT	C1C0	: 1;	// b0  
		}BIT;
	}UNID;
} RENV4;

///<summary>
/// �R�}���h�\����
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	C4D1		: 1;	// 
			INT	C4D0		: 1;	// 
			INT	C4S3		: 1;	// 
			INT	C4S2		: 1;	// 
			INT	C4S1		: 1;	// 
			INT	C4S0		: 1;	// 
			INT	C4C1		: 1;	// 
			INT	C4C0		: 1;	// 
			INT	IDMX		: 1;	// 
			INT	C3D1		: 1;	// 
			INT	C3D0		: 1;	// 
			INT	C3S2		: 1;	// 
			INT	C3S1		: 1;	// 
			INT	C3S0		: 1;	// 
			INT	C3C1		: 1;	// 
			INT	C3C0		: 1;	// 
			INT	C2RM		: 1;	// 
			INT	C2D1		: 1;	// 
			INT	C2D0		: 1;	// 
			INT	C2S2		: 1;	// 
			INT	C2S1		: 1;	// 
			INT	C2S0		: 1;	// 
			INT	C2C1		: 1;	// 
			INT	C2C0		: 1;	// 
			INT	C1RM		: 1;	// 
			INT	C1D1		: 1;	// 
			INT	C1D0		: 1;	// 
			INT	C1S2		: 1;	// 
			INT	C1S1		: 1;	// 
			INT	C1S0		: 1;	// 
			INT	C1C1		: 1;	// 
			INT	C1C0		: 1;	// 
		}BIT;
	}UNID;
} RENV4DUM;

///<summary>
/// (M204DUM)RENV5���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve7: 1;	// b31
			INT	reserve6: 1;	// b30 
			INT	reserve5: 1;	// b29 
			INT	reserve4: 1;	// B28
			INT	CU4L	: 1;	// b27 
			INT	CU3L	: 1;	// b26 
			INT	CU2L	: 1;	// b25 
			INT	CU1L	: 1;	// b24 
			INT	reserve3: 1;	// b23 
			INT	reserve2: 1;	// b22 
			INT	SYI1	: 1;	// b21 
			INT	SYI0	: 1;	// b20 
			INT	SYO3	: 1;	// b19 
			INT	SYO2	: 1;	// b18 
			INT	SYO1	: 1;	// b17 
			INT	SYO0	: 1;	// b16 
			INT	LTOF	: 1;	// b15 
			INT	LTFD	: 1;	// b14 
			INT	LTM1	: 1;	// b13 
			INT	LTM0	: 1;	// b12 
			INT	reserve1: 1;	// b11 
			INT	IDL2	: 1;	// b10 
			INT	IDL1	: 1;	// b9  
			INT	IDL0	: 1;	// b8  
			INT	C5D1	: 1;	// b7  
			INT	C5D0	: 1;	// b6  
			INT	C5S2	: 1;	// b5  
			INT	C5S1	: 1;	// b4  
			INT	C5S0	: 1;	// b3  
			INT	C5C2	: 1;	// b2  
			INT	C5C1	: 1;	// b1  
			INT	C5C0	: 1;	// b0  
		}BIT;
	}UNID;
} RENV5DUM;

///<summary>
/// (M204DUM)RENV6���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	PMG4	: 1;	// b31
			INT	PMG3	: 1;	// b30 
			INT	PMG2	: 1;	// b29 
			INT	PMG1	: 1;	// B28
			INT	PMG0	: 1;	// b27 
			INT	PD10	: 1;	// b26 
			INT	PD9		: 1;	// b25 
			INT	PD8		: 1;	// b24 
			INT	PD7		: 1;	// b23 
			INT	PD6		: 1;	// b22 
			INT	PD5		: 1;	// b21 
			INT	PD4		: 1;	// b20 
			INT	PD3		: 1;	// b19 
			INT	PD2		: 1;	// b18 
			INT	PD1		: 1;	// b17 
			INT	PD0		: 1;	// b16 
			INT	PSTP	: 1;	// b15 
			INT	reserve1: 1;	// b14 
			INT	ADJ1	: 1;	// b13 
			INT	ADJ0	: 1;	// b12 
			INT	BR11	: 1;	// b11 
			INT	BR10	: 1;	// b10 
			INT	BR9		: 1;	// b9  
			INT	BR8		: 1;	// b8  
			INT	BR7		: 1;	// b7  
			INT	BR6		: 1;	// b6  
			INT	BR5		: 1;	// b5  
			INT	BR4		: 1;	// b4  
			INT	BR3		: 1;	// b3  
			INT	BR2		: 1;	// b2  
			INT	BR1		: 1;	// b1  
			INT	BR0		: 1;	// b0  
		}BIT;
	}UNID;
} RENV6DUM;

///<summary>
/// (M204DUM)RENV7���W�X�^
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	FT15: 1;	// b31
			INT	FT14: 1;	// b30 
			INT	FT13: 1;	// b29 
			INT	FT12: 1;	// B28
			INT	FT11: 1;	// b27 
			INT	FT10: 1;	// b26 
			INT	FT9 : 1;	// b25 
			INT	FT8 : 1;	// b24 
			INT	FT7 : 1;	// b23 
			INT	FT6 : 1;	// b22 
			INT	FT5 : 1;	// b21 
			INT	FT4 : 1;	// b20 
			INT	FT3 : 1;	// b19 
			INT	FT2 : 1;	// b18 
			INT	FT1 : 1;	// b17 
			INT	FT0 : 1;	// b16 
			INT	RT15: 1;	// b15 
			INT	RT14: 1;	// b14 
			INT	RT13: 1;	// b13 
			INT	RT12: 1;	// b12 
			INT	RT11: 1;	// b11 
			INT	RT10: 1;	// b10 
			INT	RT9 : 1;	// b9  
			INT	RT8 : 1;	// b8  
			INT	RT7 : 1;	// b7  
			INT	RT6 : 1;	// b6  
			INT	RT5 : 1;	// b5  
			INT	RT4 : 1;	// b4  
			INT	RT3 : 1;	// b3  
			INT	RT2 : 1;	// b2  
			INT	RT1 : 1;	// b1  
			INT	RT0 : 1;	// b0  
		}BIT;
	}UNID;
} RENV7DUM;

///<summary>
/// �R�}���h�\����
///</summary>
typedef struct
{
	UH		code;			// �R�}���h�R�[�h
//1004	UH		combi_pm;		// �A�gPM
	UH		status;			// ���[�^�X�e�[�^�X
//1004	BOOL	slave;

} ST_COMMAND;

// <summary>
// ���[�^�[�X�e�[�^�X�\����
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	reserve2: 7;	// ���0�ɂȂ�܂��B
			UH	SBSY	: 1;	// �p���X�o�͊J�n�łP�ɂȂ�܂��B�����~�łO�ɂȂ�܂��B�i���a�r�x�j
			UH	reserve1: 4;	// ���0�ɂȂ�܂��B
			UH	SEVT	: 1;	// �C�x���g���荞�ݔ����ɂ��P�ɂȂ�A�q�h�r�s�ǂݏo���łO�ɂȂ�܂��B
			UH	SERR	: 1;	// �G���[���荞�ݔ����ɂ��P�ɂȂ�A�q�d�r�s�ǂݏo���łO�ɂȂ�܂��B
			UH	SEND	: 1;	// �����~�ɂ�芄�荞�݂𔭐����A�P�ɂȂ�܂��B���荞�݃��Z�b�g�R�}���h(0008h)�łO�ɂȂ�܂��B
			UH	SINT	: 1;	// ���荞�ݔ����ɂ��P�ɂȂ�܂��B(�r�b�g�P,�Q,�R�̂����ꂩ���P)
		}BIT;
	}UNID;
} MotorStatus;

typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	SPDF	: 1;	// BIT15
			UH	SPRF	: 1;	// BIT14
			UH	SEOR	: 1;	// BIT15
			UH	SCP5	: 1;	// BIT16
			UH	SCP4	: 1;	// BIT11
			UH	SCP3	: 1;	// BIT10
			UH	SCP2	: 1;	// BIT9
			UH	SCP1	: 1;	// BIT8
			UH	SSC1	: 1;	// BIT7
			UH	SSC0	: 1;	// BIT6
			UH	SINT	: 1;	// BIT5
			UH	SERR	: 1;	// BIT4
			UH	SEND	: 1;	// BIT3
			UH	SEN1	: 1;	// BIT2
			UH	SRUN	: 1;	// BIT1
			UH	SSCM	: 1;	// BIT0
		}BIT;
	}UNID;
} MotorStatusDum;

// <summary>
// �i�S���j�T�u�X�e�[�^�X�\����
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	SSD : 1;	// BIT15 :SSD SD����ON����1�ɂȂ�܂��B(SD���͂̃��b�`�M��)
			UH	SORG: 1;	// BIT14 :SORG ORG����ON����1�ɂȂ�܂��B
			UH	SMEL: 1;	// BIT13 :SMEL -EL����ON����1�ɂȂ�܂��B
			UH	SPEL: 1;	// BIT12 :SPEL +EL����ON����1�ɂȂ�܂��B
			UH	SALM: 1;	// BIT11 :SALM ALM����ON����1�ɂȂ�܂��B
			UH	SFC : 1;	// BIT10 :SFC �葬���쒆��1�ɂȂ�܂��B
			UH	SFD : 1;	// BIT9  :SFD ��������1�ɂȂ�܂��B
			UH	SFU : 1;	// BIT8  :SFU ��������1�ɂȂ�܂��B
			UH	IOP : 8;	// BIT7-0:IOP7�`0 P7�`P0�[�q�̏�Ԃ�ǂݏo���܂��B(0:L���x���A1:H���x��)
		}BIT;
	}UNID;
} MNETSubStatus;

///<summary>
/// ���[�^�p�����[�^�\����
///</summary>
typedef struct 
{
	INT			factor;		// ���[�^����{��
	UH	accel_type;			// ����������
	INT			FL;			// �������쑬�x(pps)
	INT			FH;			// �葬���쑬�x(pps)
	INT			utime;		// ��������(ms)
	INT			dtime;		// ��������(ms)
	INT			us;			// �������r���͈�
	INT			ds;			// �������r���͈�
} ST_MOTOR_PARAM;


///<summary>
/// ���[�^���\����
///</summary>
typedef struct
{
	INT			factor;		// ���[�^����{��
	ST_MOTOR_PARAM	param;	// ���[�^�p�����[�^
	INT			reg[10];		// ���[�^�R���g���[�����W�X�^
	UH			dir;		// ���[�^�ړ�����
//1004	BOOL		stopCW;		// CW��������֎~�t���O
//1004	BOOL		stopCCW;	// CCW��������֎~�t���O
	INT			pos;		// ���[�^��Έʒu
	UH			power;		// �㎥�ݒ�
	BOOL		paused;		// �ꎞ��~�t���O
//1004	BOOL		active;		// 
//1004	BOOL		enableDelay;// �x���L���t���O
//1004	INT			delay;		// �x������
//1004	INT			dcounter;	// �x���J�E���^
//1004	BOOL		interrupt;
//1004	BOOL		notified;
//1004	BOOL		not_notify;
	ST_COMMAND		command;	// �R�}���h
	UH	startParam;	// ����J�n�R�}���h�p�����[�^

} ST_PM_INFO;


// <summary>
// �Z���^�[�f�o�C�X�X�e�[�^�X�\����
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	BBSY	: 1;	// RENV0(8)= "1" �Ńu���[�N�ʐM�R�}���h�i0610h�j�𔭍s�����Ƃ��A
									// �u���[�N�ʐM����������܂� "1" �ƂȂ�܂��B
			UH	DBSY	: 1;	// �V�X�e���ʐM���A�܂��̓f�[�^�ʐM����1 �ɂȂ�܂��B
			UH	RBSY	: 1;	// ���Z�b�g��������1 �ɂȂ�܂��B
			UH	SBSY	: 1;	// �T�C�N���b�N�ʐM�X�^�[�g����1 �ɂȂ�܂��B
			UH	reserve2: 1;	// ���0 �ɂȂ�܂��B
			UH	RDBB	: 1;	// �f�[�^��M�pFIFO �Ɏ�M�f�[�^�����鎞��1 �ɂȂ�܂��B
			UH	TDBB	: 1;	// �f�[�^���M�pFIFO �ɑ��M�f�[�^�����鎞��1 �ɂȂ�܂��B
			UH	REF		: 1;	// �����M�̏o�̓|�[�g�f�[�^�����鎞1 �ɂȂ�܂��B
			UH	reserve1: 1;	// ���0 �ɂȂ�܂��B
			UH	CAER	: 1;	// CPU �̃A�N�Z�X�G���[�ł��B
			UH	ERAE	: 1;	// ۰�ّ���M�����װ��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
			UH	EDTE	: 1;	// �f�[�^�ʐM�G���[��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
			UH	EIOE	: 1;	// �T�C�N���b�N�ʐM�װ��������1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
			UH	IOPC	: 1;	// �u���͕ω����荞�ݐݒ�v��1 �ɃZ�b�g�������̓|�[�g�̏�Ԃ��ω���������1 �ɂȂ�A
									// ���荞�ݐM��(INT)���o�͂��܂��B
			UH	BRKF	: 1;	// �u���[�N�t���[����M����1 �ɂȂ�A���荞�ݐM��(INT)���o�͂��܂��B
			UH	CEND	: 1;	// �f�[�^���M�pFIFO �������݉\����1 �ɂȂ�܂��B
		}BIT;
	}UNID;
} MNETStatus;
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	reserve4: 1;	// 
			UH	SPRF	: 1;	// ����p�v�����W�X�^�����t = 1
			UH	SEOR	: 1;	// �ڕW�ʒu�I�[�o���C�h�����s�o���Ȃ� = 1
			UH	reserve3: 1;	// 
			UH	reserve2: 1;	// 
			UH	reserve1: 1;	// 
			UH	SCP2	: 1;	// 
			UH	SCP1	: 1;	// 
			UH	SSC1	: 1;	// 
			UH	SSC0	: 1;	// 
			UH	SINT	: 1;	// �C�x���g�����ݔ��� = 1�ARIST�Ǎ��� 0�ƂȂ�
			UH	SERR	: 1;	// �G���[�����ݔ��� = 1�AREST�Ǎ��� 0�ƂȂ�
			UH	SEND	: 1;	// �X�^�[�g�R�}���h�������ݎ� = 0�A�����~��1�ƂȂ�
			UH	SENI	: 1;	// 
			UH	SRUN	: 1;	// �p���X�o�͊J�n�� = 1�A�����~��0�ƂȂ�
			UH	SSCM	: 1;	// �X�^�[�g�R�}���h�������ݎ� = 1�A�����~��0�ƂȂ�
		}BIT;
	}UNID;
} MNETStatusPCD4611;

///<summary>
/// ���[�^���W�X�^�\����
///</summary>
typedef struct
{
	INT			reg[10];
} PM_REG;

///<summary>
/// RMD���W�X�^�F���샂�[�h��ݒ肷�郌�W�X�^�ł��B
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve2: 7;	// 
			INT	MIOR	: 1;	// 
			INT	MFH		: 1;	// 
			INT	MUB		: 1;	// 
			INT	MMPH	: 1;	// 
			INT	MPH		: 1;	// 
			INT	MINT	: 1;	// 
			INT	MMSK	: 1;	// 
			INT	MADJ	: 1;	// 
			INT	MSPO	: 1;	// 
			INT	MSPE	: 1;	// 
			INT	MSY		: 1;	// 
			INT	MPCS	: 1;	// 
			INT	MSDP	: 1;	// 
			INT	METM	: 1;	// 
			INT	MSMD	: 1;	// 
			INT	MINP	: 1;	// 
			INT	MSDE	: 1;	// 
			INT	reserve1: 1;	// 
			INT	MOD		: 7;	// 
		}BIT;
	}UNID;
} RMD;

///<summary>
/// RMD���W�X�^�F���샂�[�h��ݒ肷�郌�W�X�^�ł��B
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve31	: 1;	//  b31
			INT	reserve30	: 1;	//  b30
			INT	MCD0		: 1;	//  b29
			INT	MCDE		: 1;	//  B28
			INT	reserve27	: 1;	//  b27
			INT	MADJ		: 1;	//  b26
			INT	MSP0		: 1;	//  b25
			INT	MSPE		: 1;	//  b24
			INT	MAX3		: 1;	//  b23
			INT	MAX2		: 1;	//  b22
			INT	MAX1		: 1;	//  b21
			INT	MAX0		: 1;	//  b20
			INT	MSY1		: 1;	//  b19
			INT	MSY0		: 1;	//  b18
			INT	MAN1		: 1;	//  b17
			INT	MAN0		: 1;	//  b16
			INT	reserve15	: 1;	//  b15
			INT	MPCS		: 1;	//  b14
			INT	MSDP		: 1;	//  b13
			INT	METM		: 1;	//  b12
			INT	MCCE		: 1;	//  b11
			INT	MSMD		: 1;	//  b10
			INT	MINP		: 1;	//  b09
			INT	MSDE		: 1;	//  b08
			INT	reserve07	: 1;	//  b07
			INT	MOD			: 6;	//  b00-b06
		}BIT;
	}UNID;
} RMDPCD;

///<summary>
/// ���[�^�X�e�[�^�X�\����
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve	: 4;	// �i��ɂO�ɐݒ肵�ĉ������B�j
			INT	MPIE	: 1;	// 
			INT	MADJ	: 1;	// �e�g�␳�@�\�̐ݒ�B(�O�F�n�m �P�F�n�e�e�j
			INT	MSPO	: 1;	// �P�F�ُ��~���Ɂ��r�s�o�i������~�j�M�����o�͂��܂��B
			INT	MSPE	: 1;	// �P�F���r�s�o���͂ɂ�茸����~�܂��͑���~�����܂��B�ق��̎��ُ̈��~�ɂ�達�r�s�o�������A
			INT	MAX		: 4;	// 
			INT	MSY		: 2;	// 
			INT	MSN		: 2;	// 
			INT	MIPF	: 1;	// 
			INT	MPCS	: 1;	// �o�b�r���̗͂L���^�����ݒ�B(�P�F�ʒu���ߓ��쎞�ɁA�o�b�r���͂n�m����p���X���Ǘ����s���܂��B)�m�ڕW�ʒu�̃I�[�o�[���C�h�Q�n
			INT	MSDP	: 1;	// �������쎞�̃X���[�_�E���|�C���g��ݒ�B�ʒu���ߓ��쎞�ɗL���B�O�F�����ݒ� �P�F�}�j���A���ݒ�

			INT	METM	: 1;	// ���슮���^�C�~���O��ݒ�B(�O�F�������� �P�F�p���X����)
			INT	MCCE	: 1;	// 
			INT	MSMD	: 1;	// �������쎞�̉�����������ݒ�B(�O�F���������� �P�F�r��������)
			INT	MINP	: 1;	// �O�F�h�m�o���͂ɂ��x���͖����ɂȂ�܂��B(RSTS �ł̊m�F�͉�) 
									//�P�F�h�m�o���͂n�m�œ��슮���ɂȂ�܂��B
			INT	MSDE	: 1;	// �O�F�r�c���͖͂����ɂȂ�܂��B�i�T�u�X�e�[�^�X�ARSTS �ł̊m�F�͉j
									// �P�F�r�c���͂n�m�ɂ�茸���i������~�j���܂��B
			INT	MENI	: 1;	// 
			INT	MOD		: 7;	// ���샂�[�h��ݒ肵�܂��B
		}BIT;
	}UNID;
} RMDDUM;

///<summary>
/// ���[�^�X�e�[�^�X�\����
///</summary>
typedef struct
{
	INT			pos;				// �p���X�o�͒l
	INT			enc;				// �G���R�[�_�l
	UINT		dev;				// �p���X�o�͒l�ƃG���R�[�_�l�̕΍�
	BOOL		busy;
	BOOL		cw_limit;
	BOOL		ccw_limit;

} ST_MOTOR_STATUS;

	///<summary>
	/// ����������
	///</summary>
	enum ACCEL_TYPE 
	{
		ACCEL_LINEAR = 0,			// ����������
		ACCEL_STYPE  = 1,			// S��������
	};

	///<summary>
	/// �p���X���[�^No
	///</summary>
	enum PM
	{
		NO_PM	= -1,
		PM00	= 0,
		PM01	= 1,
		PM02	= 2,
		PM03	= 3,
		PM04 	= 4,
		PM05 	= 5,
		PM06 	= 6,
		PM07 	= 7,
		PM08 	= 8,
		PM09 	= 9,
		PM10 	= 10,
	};
	///<summary>
	/// ���W�X�^
	///</summary>
	typedef enum							
	{
		R0 = 0,		// �v���Z�b�g�ʐݒ�^�c�p���X�m�F	R/W  0�`0xFFFFFF
		R1 = 1,		// FL���x�ݒ�						W(R) 1�`0x1FFFF
		R2 = 2,		// FH���x�ݒ�						W(R) 1�`0x1FFFF
		R3 = 3,		// �����x���[�g�ݒ�					W(R) 2�`0x3FF
		R4 = 4,		// �{���ݒ�							W(R) 2�`0x3FF
		R5 = 5,		// �X���[�_�E���|�C���g�ݒ�			W(R) 0�`0xFFFF
		R6 = 6,		// �A�C�h�����O�p���X�ݒ�			W(R) 0�`7
		R7 = 7,		// ���f�[�^�ݒ�(PCD4541�̂�)		W(R) 0, 1
		R8 = 8,		// �����r���͈͐ݒ�					W(R) 1�`0xC350
		R9 = 9,		// �����r���͈͐ݒ�					W(R) 1�`0xC350

	} REGISTER;


	///<summary>
	/// �������ݗp���W�X�^
	///</summary>
	enum WRITE_REGISTER
	{
		WRG_RMVOR	= 0x0080,	// RMV ���W�X�^ �ւ̃I�[�o�[���C�h��������
		WRG_WPRCP5  = 0x008B,	// RCMP5���W�X�^ �ւ̃I�[�o�[���C�h��������
		WRG_RMV		= 0x0090,	// RMV ���W�X�^ �ւ̏�������
		WRG_RFL		= 0x0091,	// RFL ���W�X�^ �ւ̏�������
		WRG_RFH		= 0x0092,	// RFH ���W�X�^ �ւ̏�������
		WRG_RDP		= 0x0096,	// RDP ���W�X�^ �ւ̏�������
		WRG_RMD		= 0x0097,	// RMD ���W�X�^ �ւ̏�������
		WRG_RFA		= 0x009B,	// RFA ���W�X�^ �ւ̏�������
		WRG_RENV1	= 0x009C,	// RENV1 ���W�X�^ �ւ̏�������
		WRG_RENV2	= 0x009D,	// RENV2 ���W�X�^ �ւ̏�������
		WRG_RENV3	= 0x009E,	// RENV3 ���W�X�^ �ւ̏�������
		WRG_RENV4	= 0x009F,	// RENV4 ���W�X�^ �ւ̏�������
		WRG_RENV5	= 0x00A0,	// RENV5 ���W�X�^ �ւ̏�������
		WRG_RENV6	= 0x00A1,	// RENV6 ���W�X�^ �ւ̏�������
		WRG_RENV7	= 0x00A2,	// RENV7 ���W�X�^ �ւ̏�������
		WRG_RCUN1	= 0x00A3,	// RCUN1 ���W�X�^ �ւ̏�������
		WRG_RCUN2	= 0x00A4,	// RCUN2 ���W�X�^ �ւ̏�������
		WRG_RCMP1	= 0x00A7,	// RCMP1 ���W�X�^ �ւ̏�������
		WRG_RCMP2	= 0x00A8,	// RCMP2 ���W�X�^ �ւ̏�������
		WRG_RCMP3	= 0x00A9,	// RCMP3 ���W�X�^ �ւ̏�������
		WRG_RCMP5	= 0x00AB,	// RCMP5 ���W�X�^ �ւ̏�������
		WRG_RIRQ	= 0x00AC,	// RIRQ ���W�X�^ �ւ̏�������
	};

	///<summary>
	/// �������ݗp���W�X�^
	///</summary>
	enum WRITE_REGISTER16
	{
		WRG_RUR		= 0x0093,	// RUR ���W�X�^ �ւ̏�������
		WRG_RDR		= 0x0094,	// RDR ���W�X�^ �ւ̏�������
		WRG_RMG		= 0x0095,	// RMG ���W�X�^ �ւ̏�������
		WRG_RUS		= 0x0099,	// RUS ���W�X�^ �ւ̏�������
		WRG_RDS		= 0x009A,	// RDS ���W�X�^ �ւ̏�������
		WRG_RCUN3	= 0x00A5,	// RCUN3 ���W�X�^ �ւ̏�������
	};

	///<summary>
	/// �������ݗp���W�X�^
	///</summary>
	enum WRITE_PREREGISTER
	{
		WRG_PRMV	= 0x0080,	// RMV ���W�X�^ �ւ̏�������
		WRG_PRFL	= 0x0081,	// RFL ���W�X�^ �ւ̏�������
		WRG_PRFH	= 0x0082,	// RFH ���W�X�^ �ւ̏�������
		WRG_PRUR	= 0x0083,	// RUS ���W�X�^ �ւ̏�������
		WRG_PRDR	= 0x0084,	// RDR ���W�X�^ �ւ̏�������
		WRG_PRMG	= 0x0085,	// RMG ���W�X�^ �ւ̏�������
		WRG_PRDP	= 0x0086,	// RDP ���W�X�^ �ւ̏�������
		WRG_PRMD	= 0x0087,	// RMD ���W�X�^ �ւ̏�������
		WRG_PRIP	= 0x008B,	// RIP ���W�X�^ �ւ̏�������
		WRG_PRUS	= 0x0089,	// RUS ���W�X�^ �ւ̏�������
		WRG_PRDS	= 0x008A,	// RDS ���W�X�^ �ւ̏�������
	};

	///<summary>
	/// �ǂݍ��ݗp���W�X�^
	///</summary>
	enum READ_REGISTER
	{
		RRG_RMV		= 0x00D0,	//  RMV ���W�X�^ �̓ǂݏo��
		RRG_RFL		= 0x00D1,	//  RFL ���W�X�^ �̓ǂݏo��
		RRG_RFH		= 0x00D2,	//  RFH ���W�X�^ �̓ǂݏo��
		RRG_RUR		= 0x00D3,	//  RUR ���W�X�^ �̓ǂݏo��
		RRG_RDR		= 0x00D4,	//  RDR ���W�X�^ �̓ǂݏo��
		RRG_RMG		= 0x00D5,	//  RMG ���W�X�^ �̓ǂݏo��
		RRG_RDP		= 0x00D6,	//  RDP ���W�X�^ �̓ǂݏo��
		RRG_RMD		= 0x00D7,	//  RMD ���W�X�^ �̓ǂݏo��
		RRG_RUS		= 0x00D9,	//  RUS ���W�X�^ �̓ǂݏo��
		RRG_RDS		= 0x00DA,	//  RDS ���W�X�^ �̓ǂݏo��
		RRG_RFA		= 0x00DB,	//  RFA ���W�X�^ �̓ǂݏo��
		RRG_RENV1	= 0x00DC,	//  RENV1 ���W�X�^ �̓ǂݏo��
		RRG_RENV2	= 0x00DD,	//  RENV2 ���W�X�^ �̓ǂݏo��
		RRG_RENV3	= 0x00DE,	//  RENV3 ���W�X�^ �̓ǂݏo��
		RRG_RENV4	= 0x00DF,	//  RENV4 ���W�X�^ �̓ǂݏo��
		RRG_RENV5	= 0x00E0,	//  RENV5 ���W�X�^ �̓ǂݏo��
		RRG_RENV6	= 0x00E1,	//  RENV6 ���W�X�^ �̓ǂݏo��
		RRG_RCUN1	= 0x00E3,	//  RCUN1 ���W�X�^ �̓ǂݏo��
		RRG_RCUN2	= 0x00E4,	//  RCUN2 ���W�X�^ �̓ǂݏo��
		RRG_RCUN3	= 0x00E5,	//  RCUN3 ���W�X�^ �̓ǂݏo��
		RRG_RCMP1	= 0x00E7,	//  RCMP1 ���W�X�^ �̓ǂݏo��
		RRG_RCMP2	= 0x00E8,	//  RCMP2 ���W�X�^ �̓ǂݏo��
		RRG_RCMP3	= 0x00E9,	//  RCMP3 ���W�X�^ �̓ǂݏo��
		RRG_RIRQ	= 0x00EC,	//  RIRQ ���W�X�^ �̓ǂݏo��
		RRG_RLTC1	= 0x00ED,	//  RLTC1 ���W�X�^ �̓ǂݏo��
		RRG_RLTC2	= 0x00EE,	//  RLTC2 ���W�X�^ �̓ǂݏo��
		RRG_RLTC3	= 0x00EF,	//  RDTC3 ���W�X�^ �̓ǂݏo��
		RRG_RSTS	= 0x00F1,	//  RSTS ���W�X�^ �̓ǂݏo��
		RRG_REST	= 0x00F2,	//  REST ���W�X�^ �̓ǂݏo��
		RRG_RIST	= 0x00F3,	//  RIST ���W�X�^ �̓ǂݏo��
		RRG_RPLS	= 0x00F4,	//  RPLS ���W�X�^ �̓ǂݏo��
		RRG_RSPD	= 0x00F5,	//  RSPD ���W�X�^ �̓ǂݏo��
		RRG_RSDC	= 0x00F6,	//  RSDC ���W�X�^ �̓ǂݏo��
	};
	enum READ_PREREGISTER
	{
		RRG_PRMV	= 0x00C0,	//  PRMV ���W�X�^ �̓ǂݏo��
		RRG_PRFL	= 0x00C1,	//  PRFL ���W�X�^ �̓ǂݏo��
		RRG_PRFH	= 0x00C2,	//  PRFH ���W�X�^ �̓ǂݏo��
		RRG_PRUR	= 0x00C3,	//  PRUR ���W�X�^ �̓ǂݏo��
		RRG_PRDR	= 0x00C4,	//  PRDR ���W�X�^ �̓ǂݏo��
		RRG_PRMG	= 0x00C5,	//  PRMG ���W�X�^ �̓ǂݏo��
		RRG_PRDP	= 0x00C6,	//  PRDP ���W�X�^ �̓ǂݏo��
		RRG_PRMD	= 0x00C7,	//  PRMD ���W�X�^ �̓ǂݏo��
		RRG_PRUS	= 0x00C9,	//  PRUS ���W�X�^ �̓ǂݏo��
		RRG_PRDS	= 0x00CA,	//  PRDS ���W�X�^ �̓ǂݏo��
	};

	// PCD ---
	enum WRITE_REGISTER_PCD
	{
		WPCD_RMVOR	= 0x80,	// RMV ���W�X�^ �ւ̃I�[�o�[���C�h��������
		WPCD_WPRCP5 = 0x8B,	// RCMP5���W�X�^ �ւ̃I�[�o�[���C�h��������
		WPCD_RMV	= 0x90,	// RMV ���W�X�^ �ւ̏�������
		WPCD_RFL	= 0x91,	// RFL ���W�X�^ �ւ̏�������
		WPCD_RFH	= 0x92,	// RFH ���W�X�^ �ւ̏�������
		WPCD_RUR	= 0x93,	// RUR ���W�X�^ �ւ̏�������
		WPCD_RDR	= 0x94,	// RDR ���W�X�^ �ւ̏�������
		WPCD_RMG	= 0x95,	// RMG ���W�X�^ �ւ̏�������
		WPCD_RUS	= 0x99,	// RUS ���W�X�^ �ւ̏�������
		WPCD_RDS	= 0x9A,	// RDS ���W�X�^ �ւ̏�������
		WPCD_RFA	= 0x9B,	// RFA ���W�X�^ �ւ̏�������
		WPCD_RENV1	= 0x9C,	// RENV1 ���W�X�^ �ւ̏�������
		WPCD_RENV2	= 0x9D,	// RENV2 ���W�X�^ �ւ̏�������
		WPCD_RENV3	= 0x9E,	// RENV3 ���W�X�^ �ւ̏�������
		WPCD_RENV4	= 0x9F,	// RENV4 ���W�X�^ �ւ̏�������
		WPCD_RENV5	= 0xA0,	// RENV5 ���W�X�^ �ւ̏�������
		WPCD_RENV6	= 0xA1,	// RENV6 ���W�X�^ �ւ̏�������
		WPCD_RENV7	= 0xA2,	// RENV7 ���W�X�^ �ւ̏�������
		WPCD_RCUN1	= 0xA3,	// RCUN1 ���W�X�^ �ւ̏�������
		WPCD_RCUN2	= 0xA4,	// RCUN2 ���W�X�^ �ւ̏�������
		WPCD_RCUN3	= 0xA5,	// RCUN3 ���W�X�^ �ւ̏�������
		WPCD_RCMP1	= 0xA7,	// RCMP1 ���W�X�^ �ւ̏�������
		WPCD_RCMP2	= 0xA8,	// RCMP2 ���W�X�^ �ւ̏�������
		WPCD_RCMP3	= 0xA9,	// RCMP3 ���W�X�^ �ւ̏�������
		WPCD_RCMP5	= 0xAB,	// RCMP5 ���W�X�^ �ւ̏�������
		WPCD_RIRQ	= 0xAC,	// RIRQ ���W�X�^ �ւ̏�������
	};

	///<summary>
	/// �������ݗp�v�����W�X�^
	///</summary>
	enum WRITE_PREREGISTER_PCD
	{
		WPCD_PRMV	= 0x80,	// RMV ���W�X�^ �ւ̏�������
		WPCD_PRFL	= 0x81,	// RFL ���W�X�^ �ւ̏�������
		WPCD_PRFH	= 0x82,	// RFH ���W�X�^ �ւ̏�������
		WPCD_PRUR	= 0x83,	// RUS ���W�X�^ �ւ̏�������
		WPCD_PRDR	= 0x84,	// RDR ���W�X�^ �ւ̏�������
		WPCD_PRMG	= 0x85,	// RMG ���W�X�^ �ւ̏�������
		WPCD_PRDP	= 0x86,	// RDP ���W�X�^ �ւ̏�������
		WPCD_PRMD	= 0x87,	// RMD ���W�X�^ �ւ̏�������
		WPCD_PRIP	= 0x8B,	// RIP ���W�X�^ �ւ̏�������
		WPCD_PRUS	= 0x89,	// RUS ���W�X�^ �ւ̏�������
		WPCD_PRDS	= 0x8A,	// RDS ���W�X�^ �ւ̏�������
	};

	///<summary>
	/// �ǂݍ��ݗp���W�X�^
	///</summary>
	enum READ_REGISTER_PCD
	{
		RPCD_RMV	= 0xD0,	//  RMV ���W�X�^ �̓ǂݏo��
		RPCD_RFL	= 0xD1,	//  RFL ���W�X�^ �̓ǂݏo��
		RPCD_RFH	= 0xD2,	//  RFH ���W�X�^ �̓ǂݏo��
		RPCD_RUR	= 0xD3,	//  RUR ���W�X�^ �̓ǂݏo��
		RPCD_RDR	= 0xD4,	//  RDR ���W�X�^ �̓ǂݏo��
		RPCD_RMG	= 0xD5,	//  RMG ���W�X�^ �̓ǂݏo��
		RPCD_RDP	= 0xD6,	//  RDP ���W�X�^ �̓ǂݏo��
		RPCD_RMD	= 0xD7,	//  RMD ���W�X�^ �̓ǂݏo��
		RPCD_RUS	= 0xD9,	//  RUS ���W�X�^ �̓ǂݏo��
		RPCD_RDS	= 0xDA,	//  RDS ���W�X�^ �̓ǂݏo��
		RPCD_RFA	= 0xDB,	//  RFA ���W�X�^ �̓ǂݏo��
		RPCD_RENV1	= 0xDC,	//  RENV1 ���W�X�^ �̓ǂݏo��
		RPCD_RENV2	= 0xDD,	//  RENV2 ���W�X�^ �̓ǂݏo��
		RPCD_RENV3	= 0xDE,	//  RENV3 ���W�X�^ �̓ǂݏo��
		RPCD_RENV4	= 0xDF,	//  RENV4 ���W�X�^ �̓ǂݏo��
		RPCD_RENV5	= 0xE0,	//  RENV5 ���W�X�^ �̓ǂݏo��
		RPCD_RENV6	= 0xE1,	//  RENV6 ���W�X�^ �̓ǂݏo��
		RPCD_RCUN1	= 0xE3,	//  RCUN1 ���W�X�^ �̓ǂݏo��
		RPCD_RCUN2	= 0xE4,	//  RCUN2 ���W�X�^ �̓ǂݏo��
		RPCD_RCUN3	= 0xE5,	//  RCUN3 ���W�X�^ �̓ǂݏo��
		RPCD_RCMP1	= 0xE7,	//  RCMP1 ���W�X�^ �̓ǂݏo��
		RPCD_RCMP2	= 0xE8,	//  RCMP2 ���W�X�^ �̓ǂݏo��
		RPCD_RCMP3	= 0xE9,	//  RCMP3 ���W�X�^ �̓ǂݏo��
		RPCD_RIRQ	= 0xEC,	//  RIRQ ���W�X�^ �̓ǂݏo��
		RPCD_RLTC1	= 0xED,	//  RLTC1 ���W�X�^ �̓ǂݏo��
		RPCD_RLTC2	= 0xEE,	//  RLTC2 ���W�X�^ �̓ǂݏo��
		RPCD_RLTC3	= 0xEF,	//  RDTC3 ���W�X�^ �̓ǂݏo��
		RPCD_RSTS	= 0xF1,	//  RSTS ���W�X�^ �̓ǂݏo��
		RPCD_REST	= 0xF2,	//  REST ���W�X�^ �̓ǂݏo��
		RPCD_RIST	= 0xF3,	//  RIST ���W�X�^ �̓ǂݏo��
		RPCD_RPLS	= 0xF4,	//  RPLS ���W�X�^ �̓ǂݏo��
		RPCD_RSPD	= 0xF5,	//  RSPD ���W�X�^ �̓ǂݏo��
		RPCD_RSDC	= 0xF6,	//  RSDC ���W�X�^ �̓ǂݏo��
	};
	enum READ_PREREGISTER_PCD
	{
		RPCD_PRMV	= 0xC0,	//  PRMV ���W�X�^ �̓ǂݏo��
		RPCD_PRFL	= 0xC1,	//  PRFL ���W�X�^ �̓ǂݏo��
		RPCD_PRFH	= 0xC2,	//  PRFH ���W�X�^ �̓ǂݏo��
		RPCD_PRUR	= 0xC3,	//  PRUR ���W�X�^ �̓ǂݏo��
		RPCD_PRDR	= 0xC4,	//  PRDR ���W�X�^ �̓ǂݏo��
		RPCD_PRMG	= 0xC5,	//  PRMG ���W�X�^ �̓ǂݏo��
		RPCD_PRDP	= 0xC6,	//  PRDP ���W�X�^ �̓ǂݏo��
		RPCD_PRMD	= 0xC7,	//  PRMD ���W�X�^ �̓ǂݏo��
		RPCD_PRUS	= 0xC9,	//  PRUS ���W�X�^ �̓ǂݏo��
		RPCD_PRDS	= 0xCA,	//  PRDS ���W�X�^ �̓ǂݏo��
	};
	// PCD ---
	
	
	///<summary>
	/// ���샂�[�h
	///</summary>
	enum MODE
	{
		MOD00	= 0x00,	// �R�}���h����ɂ�� (�{)�����A������
		MOD08	= 0x08,	// �R�}���h����ɂ�� (�|)�����A������
		MOD01	= 0x01,	// �p���T(PA/PB)���͂ɂ��A������
		MOD10	= 0x10,	// (�{)���� ���_���A����                       o
		MOD18	= 0x18,	// (�|)���� ���_���A����                       o
		MOD12	= 0x12,	// (�{)���� ���_�����o������                   o
		MOD1A	= 0x1A,	// (�|)���� ���_�����o������                   o
		MOD15	= 0x15,	// (�{)���� ���_�T�[�`����
		MOD1D	= 0x1D,	// (�|)���� ���_�T�[�`����
		MOD20	= 0x20,	// �{�d�k�܂��́{�r�k�ʒu�܂œ���
		MOD28	= 0x28,	// �|�d�k�܂��́|�r�k�ʒu�܂œ���
		MOD22	= 0x22,	// �|�d�k�܂��́|�r�k�����o������
		MOD2A	= 0x2A,	// �{�d�k�܂��́{�r�k�����o������
		MOD24	= 0x24,	// (�{)�����ɂd�y�J�E���g����������
		MOD2C	= 0x2C,	// (�|)�����ɂd�y�J�E���g����������
		MOD41	= 0x41,	// �ʒu���ߓ���(�ڕW���Έʒu�w��)              o
		MOD42	= 0x42,	// �ʒu���ߓ��� (COUNTER1 ��Έʒu�w��)        o
		MOD43	= 0x43,	// �ʒu���ߓ��� (COUNTER2 ��Έʒu�w��)
		MOD44	= 0x44,	// �w�߈ʒu(COUNTER1)�O�_���A����
		MOD45	= 0x45,	// �@�B�ʒu(COUNTER2)�O�_���A����
		MOD46	= 0x46,	// (�{)���� �P�p���X����
		MOD4E	= 0x4E,	// (�|)���� �P�p���X����
		MOD47	= 0x47,	// �^�C�}����
		MOD51	= 0x51,	// �p���T(PA/PB)���͂ɂ��ʒu���ߓ���
		MOD52	= 0x52,	// PA/PB ������ �ʒu���ߓ��� (COUNTER1 ��Έʒu�w��)
		MOD53	= 0x53,	// PA/PB ������ �ʒu���ߓ��� (COUNTER2 ��Έʒu�w��)
		MOD54	= 0x54,	// �p���T(PA/PB)���͂ɂ��w�߈ʒu�O�_���A����
		MOD55	= 0x55,	// �p���T(PA/PB)���͂ɂ��@�B�ʒu�O�_���A����
	};

	// <summary>
    // �A�h���X�}�b�v�萔
    // </summary>
    // <remarks></remarks>
    enum AddressMap
	{
        MNET_COMMAND					= 0,		// �R�}���h
        MNET_STATUS						= 0,		// �X�e�[�^�X
        MNET_INTERRUPT_STATUS			= 0x2,		// �����݃X�e�[�^�X
        MNET_IO_BUFFER					= 0x4,		// ���o�̓o�b�t�@
        MNET_FIFO						= 0x6,		// FIFO
        MNET_DEVICE_INFO_BASE			= 0x78,		// �f�o�C�X���J�n�A�h���X
        MNET_ERROR_FLAG_BASE			= 0xB8,		// �ʐM�G���[�t���O�J�n�A�h���X
        MNET_INTERRUPT_SETTING_BASE		= 0xC0,		// �����ݐݒ�J�n�A�h���X
        MNET_INTERRUPT_FLAG_BASE		= 0xE0,		// �����݃t���O�J�n�A�h���X
        MNET_PORT_DATA_BASE				= 0x100,	// �|�[�g�f�[�^�J�n�A�h���X
	};

	// <summary>
    // �ʐM���x�萔
    // </summary>
    // <remarks></remarks>
    enum CommSpeed
	{
        MNET_COMMSPEED_2_5M				= 0,
        MNET_COMMSPEED_5M				= 1,
        MNET_COMMSPEED_10M				= 2,
        MNET_COMMSPEED_20M				= 3,
	};

	// <summary>
    // �Z���^�[�R�}���h
    // </summary>
    // <remarks></remarks>
	enum CenterCommand
	{
		MNET_CENTER_RESET				= 0x100,    // ���Z�b�g
		MNET_CMD_CLEAR_SEND_FIFO		= 0x200,    // ���M�pFIFO���Z�b�g
		MNET_CMD_CLEAR_RECV_FIFO		= 0x300,    // ��M�pFIFO���Z�b�g
		MNET_CENTER_CLR_STS				= 0x400,    // �X�e�[�^�X�N���A(�X�e�[�^�X���[�h�ɂ��r�b�g�N���A�����ݒ莞(RENV0-Bit9:1)�ɗL��,
		MNET_CENTER_CLR_ERCNT			= 0x600,	// �G���[�J�E���^�N���A
		MNET_CENTER_BRKCOM				= 0x610,    // �u���[�N�ʐM����(�u���C�N�@�\OFF��(RENV0-Bit8:1)�ɗL��)
		MNET_CENTER_SYSCOM				= 0x1000,	// �V�X�e���ʐM
		MNET_CMD_GET_ATTRIBUTE			= 0x1300,   // �f�o�C�X�̑������̎擾
		MNET_CENTER_STA_IOCOM			= 0x3000,   // I/O�ʐM�X�^�[�g
		MNET_CENTER_STP_IOCOM			= 0x3100,   // I/O�ʐM�X�g�b�v
		MNET_DATA_COMMUNICATION_BASE	= 0x4000,	// �f�[�^�ʐM
		MNET_DATA_COMMUNICATION_CANCEL	= 0x4100,	// �f�[�^�ʐM�L�����Z��

		// ���o�̓o�b�t�@�g�p�R�}���h
		MNET_CMD_DEVICE_INFO_BASE		= 0x5000,	// �f�o�C�X��񏑂�����
	};

	// <summary>
    // ���C�����W�X�^�[
    // </summary>
    // <remarks></remarks>
	enum MainRegister
	{
		MNET_CENTER_WRENV0				= 0x5500,   // RENV0��������
		MNET_CENTER_RRENV0				= 0x6500,   // RENV0�ǂݏo��
		MNET_CENTER_RRERCNT				= 0x6501,   // �G���[�J�E���^�ǂݏo��
		MNET_CENTER_RRCYCLE				= 0x6502,   // �T�C�N���b�N�����ǂݏo��
		MNET_CENTER_RRDADR				= 0x6503,   // �ŏI�f�[�^�ʐM�f�o�C�X�A�h���X�ǂݏo��
		MNET_CENTER_RRVER				= 0x6504,   // �o�[�W�����ǂݏo��
	};

	// <summary>
    // INT�`�X�e�[�^�X�̃N���A�r�b�g(MNET_CENTER_CLR_STS�R�}���h�ɉ��Z)
    // </summary>
    // <remarks></remarks>
	enum IntClearBit
	{
		CAER		= 0x40,
		ERAE		= 0x20,
		EDTE		= 0x10,
		BRKF		= 0x02,
		CEND		= 0x01,
	};

	// <summary>
    // ���C���������[�A�N�Z�X�^�C�v�萔
    // </summary>
    // <remarks></remarks>
    enum AccessType
	{
        MNET_LOCAL_PORT0				= 0,		// �|�[�g0
        MNET_LOCAL_PORT1				= 1,		// �|�[�g1
        MNET_LOCAL_PORT2				= 2,		// �|�[�g2
        MNET_LOCAL_PORT3				= 3,		// �|�[�g3
        MNET_LOCAL_ERRFLG				= 4,		// �G���[�t���O
        MNET_LOCAL_INTSET				= 5,		// ���͕ω������ݒ�
        MNET_LOCAL_INTFLG				= 6,		// ���͕ω������t���O
        MNET_LOCAL_DEVINFO				= 7,		// �f�o�C�X���
        MNET_LOCAL_PORTL				= 10,		// ���ʃ|�[�g(PORT1-0)
        MNET_LOCAL_PORTH				= 11,		// ��ʃ|�[�g(PORT3-2)
	};
	enum ERA_STATUS		// ERA value
	{
        IOPortMissmatch				= 0x1,
        IsNotDataDevice				= 0x2,
        DataDeviceBufferFull		= 0x3,
	};

    enum CAE_STATUS		// CAE value
	{
        DeviceNotFount				= 0x1,
        DeviceDataNotFount			= 0x2,
        LineIsBusy					= 0x3,
        DeviceNotRegisterd			= 0x4,
	};


	// <summary>
	// �f�o�C�X�^�C�v�萔
	// </summary>
	// <remarks></remarks>
	enum Type	// DeviceType value
	{
		IO		= 0x01,		// IO�f�o�C�X
		DATA	= 0x81,		// �f�[�^�f�o�C�X
	};

	// <summary>
	// �f�o�C�X���萔
	// </summary>
	// <remarks></remarks>
	enum Name	// DeviceName value
	{
		G9003	= 0x000,		// G9003
		G9004A	= 0x001,		// G9004A
		G9103A	= 0x002,		// G9103A
	};

	enum TIP
	{
		TIP_IDL	= 0,
		TIP_REQ	= 1,
	};

	enum PM_STATUS
	{
		PSTAT_OK	= 0,
		PSTAT_NG	= 1,
		PSTAT_READY = 2,
		PSTAT_BUSY	= 3,
		PSTAT_LIMIT	= 4,
		PSTAT_STOP	= 5,
		PSTAT_ABORT	= 6,
		PSTAT_INT	= 7,
	};
	enum COMMAND
	{
		CMD_NOCOMMAND	= 0,
		CMD_ABORT		= 1,
 		CMD_STOP_		= 2,
 		CMD_INT			= 3,
 		CMD_RDINT		= 4,
 		CMD_SET			= 5,
 		CMD_PSET		= 6,
 		CMD_PDELAY		= 7,
 		CMD_STATUS		= 8,
 		CMD_RELMV		= 9,
		CMD_ABSMV		= 10,
 		CMD_MAP			= 11,
 		CMD_LIMIT		= 12,
 		CMD_SCAN		= 13,
 		CMD_POWER		= 14,
 		CMD_ALLIN		= 15,
 		CMD_IN			= 16,
 		CMD_BIN			= 17,
 		CMD_OUT			= 18,
 		CMD_BOUT		= 19,
 		CMD_WOUT		= 22,
 		CMD_MEMORY		= 23,
 		CMD_WRITE		= 24,
 		CMD_READ		= 25,
 		CMD_VER			= 26,
		CMD_FACTOR		= 27,
		CMD_SHOT		= 28,
		CMD_TXD			= 29,
		CMD_PAUSE		= 30,
		CMD_IS_PAUSED	= 31,
		CMD_ENABLE_INT	= 32,
		CMD_LED			= 33,
		CMD_LED_7SEG	= 34,
		CMD_TIP_COUNT	= 35,
		CMD_UPDATE		= 36,

		CMD_INTRPT		= 99,
	};

	///<summary>
	/// ���[�^�ړ�����
	///</summary>
	enum MOTOR_DIR
	{
		NO_DIR		= 0,
		CW			= 1,
		CCW			= 2,
		BOTH		= 3,
	};
	enum EVENT_SOURCE
	{
		NO_TYPE		= 0,
		MOTION_NET	= 1,
		TOUCH_PAD	= 2,
	};

	enum EVENT_TYPE
	{
		NO_TYPE2		= 0,
		RESULT_NOTIFY	= 1,
		INTERRUPT		= 2,
		TOUCH_CODE		= 3,
	};

	// 
	enum ER_4700_DETAIL
	{
		MNET_COMM		= 0,
	};
		
		
		
	///<summary>
	/// �A�h���X�}�b�v
	///</summary>
	enum ADRRESS_MAP
	{                           // [�ǂݍ��ݏ���]          / [�������ݏ���]
		ADR_RW_CRTCMD = 0x0000, // ���C���X�e�[�^�XB07-B00 / ����R�}���h��������   
		ADR_RW_JIKREQ = 0x0001, // ���C���X�e�[�^�XB15-B08 / ���w��                 
		ADR_RW_IOPORT = 0x0002, // �ėp���o�̓|�[�g        / �ėp���o�̓|�[�g       
		ADR_RW_SUBSTS = 0x0003, // �T�u�X�e�[�^�X          / ����                   
		ADR_RW_BUFF01 = 0x0004, // �o�b�t�@��������B07-B00 / �o�b�t�@��������B07-B00
		ADR_RW_BUFF02 = 0x0005, // �o�b�t�@��������B15-B08 / �o�b�t�@��������B15-B08
		ADR_RW_BUFF03 = 0x0006, // �o�b�t�@��������B23-B16 / �o�b�t�@��������B23-B16
		ADR_RW_BUFF04 = 0x0007  // �o�b�t�@��������B31-B24 / �o�b�t�@��������B31-B24
	};

	///<summary>
	/// �������e
	///</summary>
	enum PROCESS_CONT
	{
		ADRFIX = 0x0000,		// �A�h���X�Œ�
		ADRINC = 0x0080,		// �A�h���X���Z
		ADRDEC = 0x00C0			// �A�h���X���Z
	};

	///<summary>
	/// �������e�ݒ�
	///</summary>
	enum PROCESS_SET
	{
		NPSET = 0x0000,			// NOP
		WRSET = 0x0900,			// �������ݏ���
		RDSET = 0x0A00,			// �ǂݍ��ݏ���
		DTSET = 0x0300			// �f�[�^��M����
	};
		
//#define E_SUCCESS           0x0000          // ����
//#define E_SYS_ERROR         0x0001          // �V�X�e���R�[���G���[
//#define E_PMSTPHND_ERROR    0x0002          // �p���X���[�^��~�n���h���łُ̈�
//#define E_EMC_EMPTY         0x0003          // EMC���[�h�F�e�X�g�f�[�^�Ȃ�
//#define E_CMD_UNKOWN        0x0101          // ��`����Ă��Ȃ��R�}���h
//#define E_CMD_PARAM         0x0102          // �p�����[�^�ُ�
//#define E_PIO_CHANNEL       0x0200          // �`���l���ԍ��ُ�
//#define E_MTR_PARAM         E_CMD_PARAM     // �p�����[�^�ُ�
//#define E_MTR_INT           0x0301          // INT�R�}���h�ɂ��ꎞ��~
//#define E_MTR_SOUT          0x0302          // �E��
//#define E_DEV_NORES         0x0411          // �R���g���[��������
//#define E_DEV_RESET         0x0412          // �R���g���[�����Z�b�g
//#define E_DAC_BUSY          0x0500          // DAC�r�W�[
//#define E_DAC_NO            E_MTR_NO        // ���[�^�ԍ��ُ�
//#define E_ROM_ADDRESS       E_CMD_PARAM     // �A�h���X�ُ�
//#define E_ROM_BUSY          E_DAC_BUSY      // BUSY
//#define E_ADC_CHANNEL       E_PIO_CHANNEL
//#define E_ADC_PARAM         E_CMD_PARAM
//#define E_MTR_BUSY          E_DAC_BUSY      // ���[�^BUSY
//#define E_MTR_LIMIT         0x0600          // ���~�b�g�G���[
//#define E_SCI_OVFW          0x0700          // �o�b�t�@�I�[�o�[�t���[
//#define E_SCI_ORN           0x0701          // �I�[�o�[�����G���[
//#define E_SCI_FER           0x0702          // �t���[�~���O�G���[
//#define E_SCI_PER           0x0703          // �p���e�B�[�G���[

TASK TskMnt( void );
ER MntInit( void );
void IRQ4_ISR();
BOOL IsExist(UB dev_no);

#endif  // _MNTTASK_H_
