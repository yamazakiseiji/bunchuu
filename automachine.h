/******************************************************************************
* ���u�ŗL���                                                                *
*                                                                             *
* File name : machien.h                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _MACHINE_H_
#define _MACHINE_H_


#define DEBUG_SIM_MODE			0		// �� 2 = �ʐM�^�C���A�E�g,1 = DEBUG�p, 0 = ���i�p
#define DEBUG_SIM_PRESS			0		// �� 1 = ���̓Z���T��V�~�����[�g, 0 = ���̓Z���T��ڑ�
#define DEBUG_DROPC_OPEN		0		// �� 1 = DROPC OPEN�̂܂܁A0 = �ʏ�g�p
#define DEBUG_LOG_SUCK0			0		// �� 1 = C-SUCK0�g���[�X�o��
#define DEBUG_LOG_MIX00			0		// �� 1 = C-MIX00�g���[�X�o��
#define DEBUG_TEMP_DEV			0		// �� 1 = ������V�~�����[�g�A0 = ���@
//*******************************
// ���[�^��
//*******************************

#define DVMIN					1		// ���[�^�ԍ��ŏ��l
#define DVMAX					7		// ���[�^�ԍ��ő�l

//*******************************
// �`���l����
//*******************************
#define MIN_CH					1		// �`���l���ŏ���
#define MAX_CH					63		// �`���l���ő吔
#define	INMIN					1		// 
#define	INMAX					10		// 
#define OUMIN					0		// 
#define	OUMAX					10		// 
#define NZLNUM					1		// �m�Y�����ڐ�( 0:���ڐ�1,1:���ڐ�2 ) 
#define	NONE					0xff
#define	PROTMAX					20


//*******************************
// ���[�^�f�o�C�X�^�C�v
//*******************************
#define	BCD4020FU				0x00	// MNET-BCD4020FU
#define	BCD4020FUA				0x01	// MNET_BCD4020FUA
#define	D420					0x02	// MNET-D420(����16ch)
#define	D411					0x03	// MNET-D411(����8ch,�o��8ch)
#define	D402					0x04	// MNET-D402(�o��16ch)
#define	D340					0x05	// MNET-D340(����32ch)
#define	D322					0x06	// MNET-D322(����16ch,�o��16ch)
#define	D304					0x07	// MNET-D304(�o��32ch)
#define	M204DUM					0x08	// MNET-M204DUM
#define	PCD4611					0x09	// PCD4611�iNICHIYO MOTOR DRIVER�j
#define	NCPRSUNIT				0x0A	// �j�`�����[���̓��j�b�g


//*******************************
// ���[�^�f�o�C�X�h�c
// FFh:������
//*******************************
#define MT_Y0				1		// Y0���i�����w�b�h�j
#define MT_Z0				2		// Z���i�����w�b�h�j
#define MT_P0				3		// �v�����W�����i�����w�b�h�j
#define MT_F0				4		// �t�H�g�Z���T�ғ���
#define MT_Y1				5		// Y1���i�g���C�ғ��j
#define MT_U0				6		// NICIRYO���͌��m���j�b�g


//*******************************
// �o�͂b�g
//*******************************

	enum OUT_CH
	{
		O_DAIAPMP = 1 ,	// �_�C�A�t�����|���v
		O_LIGHLED = 2 ,	// �Ɩ�LED
		O_UVLAMP1 = 3 ,	// UV�����v
		O_DORLOCK = 4 ,	// �h�A���b�N
		O_POWERDW = 5,	// �d��OFF
		O_DCFAN01 = 6 ,	// DC�t�@��
		O_PATLRED = 7 ,	// �p�g���C�g1
		O_PATLGRN = 8 ,	// �p�g���C�g2
		O_BUZZER1 = 9 ,	// �u�U�[1
		O_RESRV10 = 10,	// 
		O_RESRV11 = 11,	// 
		O_RESRV12 = 12,	// 
		O_RESRV13 = 13,	// 
		O_RESRV14 = 14,	// 
		O_RESRV15 = 15,	// 
		O_RESRV16 = 16,	// 
		O_RESRV17 = 17,	// ��
		O_RESRV18 = 18,	// ��
		O_RESRV19 = 19,	// ��
		O_RESRV20 = 20,	// ��
		O_RESRV21 = 21,	// ��
		O_RESRV22 = 22,	// ��
		O_RESRV23 = 23,	// ��
		O_RESRV24 = 24,	// ��
		O_RESRV25 = 25,	// 
		O_RESRV26 = 26,	// 
		O_RESRV27 = 27,	// 
		O_RESRV28 = 28,	// 
		O_RESRV29 = 29,	// 
		O_RESRV30 = 30,	// 
		O_RESRV31 = 31,	// 
		O_RESRV32 = 32,	// 
		O_RESRV33 = 33,	// 
		O_RESRV34 = 34,	// 
		O_RESRV35 = 35,	// 
		O_RESRV36 = 36,	// 
		O_RESRV37 = 37,	// 
		O_RESRV38 = 38,	// 
		O_RESRV39 = 39,	// 
		O_RESRV40 = 40,	// 
		O_RESRV41 = 41,	// 
		O_RESRV42 = 42,	// 
		O_RESRV43 = 43,	// 
		O_RESRV44 = 44,	// 
		O_RESRV45 = 45,	// 
		O_RESRV46 = 46,	// 
		O_RESRV47 = 47,	// 
		O_RESRV48 = 48,	// 
		O_RESRV49 = 49,	// 
		O_RESRV50 = 50,	// 
		O_RESRV51 = 51,	// 
		O_RESRV52 = 52,	// 
		O_RESRV53 = 53,	// 
		O_RESRV54 = 54,	// 
		O_RESRV55 = 55,	// 
		O_RESRV56 = 56,	// 
		O_RESRV57 = 57,	// 
		O_RESRV58 = 58,	// 
		O_RESRV59 = 59,	// 
		O_RESRV60 = 60,	// 
		O_RESRV61 = 61,	// 
		O_RESRV62 = 62,	// 
		O_RESRV63 = 63,	// 
	};

//*******************************
// ���͂b�g
//*******************************

	// �b�g�C���f�b�N�X
	enum IN_CH
	{
		I_DOORSW1 = 1 ,	// �h�A�X�C�b�`1
		I_DOORSW2 = 2 ,	// �h�A�X�C�b�`2
		I_RESRV03 = 3,	// �\��
		I_RESRV04 = 4,	// �\��
		I_RESRV05 = 5,	// �\��
		I_RESRV06 = 6,	// �\��
		I_RESRV07 = 7,	// �\��
		I_RESRV08 = 8,	// �\��
		I_SPTUB01 = 9 ,	// �T���v���`���[�u1�L���Z���T
		I_SPTUB02 = 10,	// �T���v���`���[�u2�L���Z���T
		I_SPTUB03 = 11,	// �T���v���`���[�u3�L���Z���T
		I_SPTUB04 = 12,	// �T���v���`���[�u4�L���Z���T
		I_SPTUB05 = 13,	// �T���v���`���[�u5�L���Z���T
		I_SPTUB06 = 14,	// �T���v���`���[�u6�L���Z���T
		I_SPTUB07 = 15,	// �T���v���`���[�u7�L���Z���T
		I_SPTUB08 = 16,	// �T���v���`���[�u8�L���Z���T
		I_TRAYSW1 = 17,	// �g���CSW
		I_COLTUB1 = 18,	// ����`���[�u�L���Z���T
		I_TMPALM1 = 19,	// ����ALM1
		I_TMPALM2 = 20,	// ����ALM2
		I_SPTUB09 = 21,	// �T���v���`���[�u9�L���Z���T
		I_SPTUB10 = 22,	// �T���v���`���[�u10�L���Z���T
		I_SPTUB11 = 23,	// �T���v���`���[�u11�L���Z���T
		I_SPTUB12 = 24,	// �T���v���`���[�u12�L���Z���T
		I_RESRV25 = 25,	// 
		I_RESRV26 = 26,	// 
		I_RESRV27 = 27,	// 
		I_RESRV28 = 28,	// 
		I_RESRV29 = 29,	// 
		I_RESRV30 = 30,	// 
		I_RESRV31 = 31,	// 
		I_RESRV32 = 32,	// 
		I_RESRV33 = 33,	// 
		I_RESRV34 = 34,	// 
		I_RESRV35 = 35,	// 
		I_RESRV36 = 36,	// 
		I_RESRV37 = 37,	// 
		I_RESRV38 = 38,	// 
		I_RESRV39 = 39,	// 
		I_RESRV40 = 40,	// 
		I_RESRV41 = 41,	// 
		I_RESRV42 = 42,	// 
		I_RESRV43 = 43,	// 
		I_RESRV44 = 44,	// 
		I_RESRV45 = 45,	// 
		I_RESRV46 = 46,	// 
		I_RESRV47 = 47,	// 
		I_RESRV48 = 48,	// 
		I_RESRV49 = 49,	// 
		I_RESRV50 = 50,	// 
		I_RESRV51 = 51,	// 
		I_RESRV52 = 52,	// 
		I_RESRV53 = 53,	// 
		I_RESRV54 = 54,	// 
		I_RESRV55 = 55,	// 
		I_RESRV56 = 56,	// 
		I_RESRV57 = 57,	// 
		I_RESRV58 = 58,	// 
		I_RESRV59 = 59,	// 
		I_RESRV60 = 60,	// 
		I_RESRV61 = 61,	// 
		I_RESRV62 = 62,	// 
		I_RESRV63 = 63,	// 
	};
	
//*******************************
// ���̓Z���T�p�^�[��
//*******************************

// �t�ʌ��o
enum LIQSNS_PTN
{
	LIQSNS_OFF = 0,		// 
	LIQSNS_ON = 1		// ���o�ς�
};

// �l�܂茟�o
enum TUMARI_PTN
{
	TUMARI_OFF = 0,		// 
	TUMARI_ON = 1		// �l�܂���
};

// ��z�����o
enum KARASU_PTN
{
	KARASU_OFF = 0,		// 
	KARASU_ON = 1		// ��z�����
};

// �`�b�v�Z���T
enum TIPSNS_PTN
{
	TIPSNS_NON = 0,		// �`�b�v��
	TIPSNS_EXT = 1		// �`�b�v�L
};

// �W���~���O�Z���T
enum DOOR_SW_PTN
{
	DOOR_SW_OFF = 0,	// 
	DOOR_SW_ON = 1		// SW ON
};

// PAUSE�{�^��
enum PAUSESW_PTN
{
	PAUSESW_OFF = 0,	// 
	PAUSESW_ON = 1		// SW ON
};

// STOP�{�^��
enum STOP_SW_PTN
{
	STOP_SW_OFF = 0,	// 
	STOP_SW_ON = 1		// SW ON
};

// �J��ΏۃL���b�v
enum LED_PTN
{
	LED_OFF = 0,	// ����
	LED_ON  = 1		// �_��
};


// �|�W�V�����ԍ�
	enum POS_CH
	{
		POS_Y0_OFFSE = 1 ,   // [Y0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��P
		POS_Z0_OFFSE = 2 ,   // [Z0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��Q
		POS_P0_OFFSE = 3 ,   // [P0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��R
		POS_F0_OFFSE = 4 ,   // [F0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��S
		POS_Y1_OFFSE = 5 ,   // [Y1] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��T
		POS_Y0_TAIHI = 6 ,   // [Y0] �ޔ��ʒu                           �|�W�V�����ԍ��U
		POS_Z0_TAIHI = 7 ,   // [Z0] �ޔ��ʒu                           �|�W�V�����ԍ��V
		POS_P0_TAIHI = 8 ,   // [P0] �ޔ��ʒu                           �|�W�V�����ԍ��W
		POS_F0_TAIHI = 9 ,   // [F0] �ޔ��ʒu                           �|�W�V�����ԍ��X
		POS_Y1_TAIHI = 10,   // [Y1] �ޔ��ʒu                           �|�W�V�����ԍ��P�O
		POS_STRIPTP1 = 11,   // [Y0] ����X�g���b�v-�V���[�g�`�b�v�P    �|�W�V�����ԍ��P�P
		POS_STRIPTP2 = 12,   // [Y0] ����X�g���b�v-�V���[�g�`�b�v�Q    �|�W�V�����ԍ��P�Q
		POS_STRIPTPL = 13,   // [Y0] ����X�g���b�v-�����O�`�b�v        �|�W�V�����ԍ��P�R
		POS_STRIPTUB = 14,   // [Y0] ����X�g���b�v-�T���v���`���[�u    �|�W�V�����ԍ��P�S
		POS_STRIPSM6 = 15,   // [Y0] ����X�g���b�v-���o�p����6         �|�W�V�����ԍ��P�T
		POS_STRIPSM5 = 16,   // [Y0] ����X�g���b�v-���o�p����5         �|�W�V�����ԍ��P�U
		POS_STRIPSM4 = 17,   // [Y0] ����X�g���b�v-���o�p����4         �|�W�V�����ԍ��P�V
		POS_STRIPSM3 = 18,   // [Y0] ����X�g���b�v-���o�p����3         �|�W�V�����ԍ��P�W
		POS_STRIPSM2 = 19,   // [Y0] ����X�g���b�v-���o�p����2         �|�W�V�����ԍ��P�X
		POS_STRIPSM1 = 20,   // [Y0] ����X�g���b�v-���o�p����1         �|�W�V�����ԍ��Q�O
		POS_STRIPCAT = 21,   // [Y0] ����X�g���b�v-�J�[�g���b�W        �|�W�V�����ԍ��Q�P
		POS_STRIPCOL = 22,   // [Y0] ����X�g���b�v-�j�_����t          �|�W�V�����ԍ��Q�Q
		POS_Z0_STOPE = 23,   // [Z0] ����X�g���b�v-�V�[���J���ŏI�ʒu	�|�W�V�����ԍ��Q�R
		POS_Z0_STOP1 = 24,   // [Z0] ����X�g���b�v-�V�[���J���P�i�ʒu	�|�W�V�����ԍ��Q�S
		POS_Z0_STRIP = 25,   // [Z0] ����X�g���b�v-�����ʒu	        �|�W�V�����ԍ��Q�T
		POS_Z0_TPGTE = 26,   // [Z0] �`�b�v�擾�ŏI�ʒu                 �|�W�V�����ԍ��Q�U
		POS_Z0_TPGT1 = 27,   // [Z0] �`�b�v�擾�P�i�ʒu                 �|�W�V�����ԍ��Q�V
		POS_Z0_TPDLE = 28,   // [Z0] �`�b�v�p���ŏI�ʒu                 �|�W�V�����ԍ��Q�W
		POS_Z0_TPDL1 = 29,   // [Z0] �`�b�v�p���P�i�ʒu                 �|�W�V�����ԍ��Q�X
		POS_Z0_LIQPU = 30,   // [Z0] �p�t�f�o�ʒu                       �|�W�V�����ԍ��R�O
		POS_Z0_COPOS = 31,   // [Z0] �R���N�V���� ����`���[�u�ʒu      �|�W�V�����ԍ��R�P
		POS_Z0_COLDS = 32,   // [Z0] �p�t�`���[�u�ʒu                   �|�W�V�����ԍ��R�Q
		//-------------------------------------------------------------------------------
		POS_Z0_SUCS1 = 33,   // [Z0] �z���J�n����1                      �|�W�V�����ԍ��R�R
		POS_Z0_SUCS2 = 34,   // [Z0] �z���J�n����2                      �|�W�V�����ԍ��R�S
		POS_Z0_SUCS3 = 35,   // [Z0] �z���J�n����3                      �|�W�V�����ԍ��R�T
		POS_Z0_SUCS4 = 36,   // [Z0] �z���J�n����4                      �|�W�V�����ԍ��R�U
		POS_Z0_SUCS5 = 37,   // [Z0] �z���J�n����5                      �|�W�V�����ԍ��R�V
		POS_Z0_SUCE1 = 38,   // [Z0] �z���I������1                      �|�W�V�����ԍ��R�W
		POS_Z0_SUCE2 = 39,   // [Z0] �z���I������2                      �|�W�V�����ԍ��R�X
		POS_Z0_SUCE3 = 40,   // [Z0] �z���I������3                      �|�W�V�����ԍ��S�O
		POS_Z0_SUCE4 = 41,   // [Z0] �z���I������4                      �|�W�V�����ԍ��S�P
		POS_Z0_SUCE5 = 42,   // [Z0] �z���I������5                      �|�W�V�����ԍ��S�Q
		POS_Z0_PUTS1 = 43,   // [Z0] �f�o�J�n����1                      �|�W�V�����ԍ��S�R
		POS_Z0_PUTS2 = 44,   // [Z0] �f�o�J�n����2                      �|�W�V�����ԍ��S�S
		POS_Z0_PUTS3 = 45,   // [Z0] �f�o�J�n����3                      �|�W�V�����ԍ��S�T
		POS_Z0_PUTS4 = 46,   // [Z0] �f�o�J�n����4                      �|�W�V�����ԍ��S�U
		POS_Z0_PUTS5 = 47,   // [Z0] �f�o�J�n����5                      �|�W�V�����ԍ��S�V
		POS_Z0_PUTE1 = 48,   // [Z0] �f�o�I������1                      �|�W�V�����ԍ��S�W
		POS_Z0_PUTE2 = 49,   // [Z0] �f�o�I������2                      �|�W�V�����ԍ��S�X
		POS_Z0_PUTE3 = 50,   // [Z0] �f�o�I������3                      �|�W�V�����ԍ��T�O
		POS_Z0_PUTE4 = 51,   // [Z0] �f�o�I������4                      �|�W�V�����ԍ��T�P
		POS_Z0_PUTE5 = 52,   // [Z0] �f�o�I������5                      �|�W�V�����ԍ��T�Q
		POS_Z0_MIXS1 = 53,   // [Z0] �h�a�z���J�n����                   �|�W�V�����ԍ��T�R
		POS_Z0_MIXE1 = 54,   // [Z0] �h�a�z���I������                   �|�W�V�����ԍ��T�S
		POS_P0_TIPDL = 55,   // [P0] �p������		                    �|�W�V�����ԍ��T�T
		POS_XX_RSV56 = 56,   // 0�Œ�	 	                            �|�W�V�����ԍ��T�U
		POS_XX_RSV57 = 57,   // 0�Œ�	 	                            �|�W�V�����ԍ��T�V
		POS_XX_RSV58 = 58,   // 0�Œ�	 	                            �|�W�V�����ԍ��T�W
		POS_XX_RSV59 = 59,   // 0�Œ�	 	                            �|�W�V�����ԍ��T�X
		POS_XX_RSV60 = 60,   // 0�Œ�	 	                            �|�W�V�����ԍ��U�O
		POS_XX_RSV61 = 61,   // 0�Œ�	 	                            �|�W�V�����ԍ��U�P
		POS_XX_RSV62 = 62,   // 0�Œ�	 	                            �|�W�V�����ԍ��U�Q
		POS_XX_RSV63 = 63,   // 0�Œ�	 	                            �|�W�V�����ԍ��U�R
		POS_XX_RSV64 = 64,   // 0�Œ�	 	                            �|�W�V�����ԍ��U�S
		//-------------------------------------------------------------------------------
		POS_H1_C1VO1 = 65,   // �␳�p�^�[���P	�e�� 10%                �|�W�V�����ԍ��U�T
		POS_H1_C1VO2 = 66,   //                 �e�� 50%          		�|�W�V�����ԍ��U�U
		POS_H1_C1VO3 = 67,   //                 �e��100%           		�|�W�V�����ԍ��U�V
		POS_H1_C1OUT = 68,   //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��U�W
		POS_H1_C1AIR = 69,   //                 �G�A�L���b�v��          �|�W�V�����ԍ��U�X
		POS_H1_C1TRA = 70,   //                 �]��� 			        �|�W�V�����ԍ��V�O
		POS_H1_C2VO1 = 71,   // �␳�p�^�[���Q  �e�� 10%                �|�W�V�����ԍ��V�P
		POS_H1_C2VO2 = 72,   //                 �e�� 50%   	            �|�W�V�����ԍ��V�Q
		POS_H1_C2VO3 = 73,   //                 �e��100%           		�|�W�V�����ԍ��V�R
		POS_H1_C2OUT = 74,   //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��V�S
		POS_H1_C2AIR = 75,   //                 �G�A�L���b�v��          �|�W�V�����ԍ��V�T
		POS_H1_C2TRA = 76,   //                 �]��� 			        �|�W�V�����ԍ��V�U
		POS_H1_C3VO1 = 77,   // �␳�p�^�[���R  �e�� 10%                �|�W�V�����ԍ��V�V
		POS_H1_C3VO2 = 78,   //                 �e�� 50%          		�|�W�V�����ԍ��V�W
		POS_H1_C3VO3 = 79,   //                 �e��100%           		�|�W�V�����ԍ��V�X
		POS_H1_C3OUT = 80,   //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��W�O
		POS_H1_C3AIR = 81,   //                 �G�A�L���b�v��          �|�W�V�����ԍ��W�P
		POS_H1_C3TRA = 82,   //                 �]��� 			        �|�W�V�����ԍ��W�Q
		POS_XX_RSV83 = 83,   // 0�Œ�		                            �|�W�V�����ԍ��W�R
		POS_XX_RSV84 = 84,   // 0�Œ�		                            �|�W�V�����ԍ��W�S
		POS_XX_RSV85 = 85,   // 0�Œ�		                            �|�W�V�����ԍ��W�T
		POS_XX_RSV86 = 86,   // 0�Œ�		                            �|�W�V�����ԍ��W�U
		POS_XX_RSV87 = 87,   // 0�Œ�		                            �|�W�V�����ԍ��W�V
		POS_XX_RSV88 = 88,   // 0�Œ�		                            �|�W�V�����ԍ��W�W
		POS_XX_RSV89 = 89,   // 0�Œ�		                            �|�W�V�����ԍ��W�X
		POS_XX_RSV90 = 90,   // 0�Œ�		                            �|�W�V�����ԍ��X�O
		POS_XX_RSV91 = 91,   // 0�Œ�		                            �|�W�V�����ԍ��X�P
		POS_XX_RSV92 = 92,   // 0�Œ�		                            �|�W�V�����ԍ��X�Q
		POS_XX_RSV93 = 93,   // 0�Œ�		                            �|�W�V�����ԍ��X�R
		POS_XX_RSV94 = 94,   // 0�Œ�		                            �|�W�V�����ԍ��X�S
		POS_XX_RSV95 = 95,   // 0�Œ�		                            �|�W�V�����ԍ��X�T
		POS_XX_RSV96 = 96,   // 0�Œ�		                            �|�W�V�����ԍ��X�U
	};

// 
enum STOP_STATUS
{
	IDLE_STS = 0,	// 
	STOP_STS = 1,	// 
	PAUS_STS = 2	// 
};


//*******************************
// automachine.c �O���Q��
//*******************************


// *********************************
// �V�X�e�����
// *********************************

// �t�@�[���E�F�A����
extern const char FIRMWARE[];

// �o�[�W����
extern const char VER_INFO[];
extern const char VER_INFO_TO_TP[];

// �G���R�[�_�[�L�����  0:���A1:�L
extern const char ENCODER_EXIST[8];






// *********************************
// �f�o�C�X���蓖�ď��
// *********************************

extern const char MNET_DRV_TYPE[32];
extern volatile const unsigned char MNET_NO[32] ;
extern volatile const unsigned char MNET_M204DUM_ASGN[32];





// *********************************
// �h�^�n�|�[�g���蓖�ď��
// *********************************

// �|�[�g���̓A�T�C��
extern const char INPUTCH_INFTBL[(MAX_CH+1)][3];

// �|�[�g�o�̓A�T�C��
extern const char OUTPUTCH_INFTBL[(MAX_CH+1)][3];


#endif  // _MACHINE_H_

