





/******************************************************************************
* �R�}���h���s�^�X�N                                                          *
*                                                                             *
* File name : tskcmd.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                             h                                *
******************************************************************************/
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "iodefine.h"
#include "define.h"
#include "datastruct.h"
#include "automachine.h"
#include "tskcmd.h"
#include "sub.h"
#include "mnet.h"
#include "machine.h"
#include "tskmotwait.h"
#include "tblchr.h"
#include "macro.h"

scanf

//*************************************
// �萔��`
//*************************************
//#define	SRAM_START_ADDRESS			0x0C000000	// 0C00 0000 - 0C07 FFFF
#define	USER_CT_MAX					10
#define	USER_ID_MAX					8
#define	USER_PW_MAX					4
#define	PROT_CT_MAX					20

#define	PULSEMOTOR_ADDR			0x0C009000		// �p���X���[�^�R���g���[���A�h���X
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
#define	PULSEMOTOR_COUNT_ERROR_LIMIT	1		// �J�E���g�G���[���e�l	[cnt]


//*************************************
// �֐���`
//*************************************
static	void	CmdTskInit();											// �R�}���h�^�X�N�f�[�^������
static	void	CmdTskInitAct();										// �R�}���h�^�X�N����f�[�^������
static	void	CmdProc(T_CMD_MSG* mrcv);								// �^�X�N�ʉ�͏���
static	void	Rx0RcvMsg( T_CMD_MSG* mrcv );							// �o�b��M�i�q�w�O�j�^�X�N����̃��b�Z�[�W��M����
static	void	Tx0RcvMsg( T_CMD_MSG* mrcv );							// �o�b���M�i�s�w�O�j�^�X�N����̃��b�Z�[�W��M����
static	void	AlmRcvMsg( T_CMD_MSG* mrcv );							// �A���[���n���h�������M
static	void	EepRcvMsg( T_CMD_MSG* mrcv );							// �d�d�o�q�n�l�Ǘ��^�X�N����̃��b�Z�[�W��M����
static	void	IntRcvMsg( T_CMD_MSG* mrcv );							// �h�m�s�^�X�N�����M
static	void	MotWaitRcvMsg( T_CMD_MSG* mrcv );						// �E�F�C�g�^�X�N�����M
static	void	MotEndRcvMsg( T_CMD_MSG* mrcv );						// �I���Ď��^�X�N�����M
static	void	RxExRcvMsg( T_CMD_MSG* mrcv );							// ��M�i�q�w�d�w�j�^�X�N����̃��b�Z�[�W��M����
static	void	PunitRcvMsg( T_CMD_MSG* mrcv );
static	void	TmpCntRcvMsg( T_CMD_MSG* mrcv );

static	void	CmdAnalyze( T_CMD_MSG* mrcv );							// �R�}���h���
typedef	void	(*CMD_FUNCTBL)( T_CMD_MSG* mrcv, UB anum, const char** args );// ������s�t�@���N�V����
typedef	void	(*TP_FUNCTBL) ( T_CMD_MSG* mrcv, UB anum, const char** args );// ������s�t�@���N�V����
static	ER		ParamDataCheck( UB msg , UB anum ,  const char** args );// ��M�p�����[�^�`�F�b�N


// �R�}���h����
static void CmdDummy  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   �_�~�[���[�`��                                    */
static void CmdHELLO  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   HELLO   �N���ʒm                                  */
static void CmdVER    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   VER     �o�[�W�����m�F                            */
static void CmdALLIN  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ALLIN   �SI/O����                                 */
static void CmdBIN    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   BIN     I/O����                                   */
static void CmdBOUT   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   BOUT    I/O�o��                                   */
static void CmdCINIT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   CINIT   ���̓J�E���^�̐ݒ�                        */
static void CmdCOUNT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   COUNT   ���̓J�E���^�̓ǂݏo��                    */
static void CmdINT    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   INT     ���荞�ݐݒ�                              */
static void CmdABSMV  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ABSMV   �p���X���[�^�ړ�(��Έʒu)                */
static void CmdABORT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ABORT   �p���X���[�^����~                        */
static void CmdENCODER( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ENCODER �G���R�[�_�̐ݒ�                          */
static void CmdLIMIT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   LIMIT   �p���X���[�^���_���A                      */
static void CmdMAP    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   MAP     �Q���̃p���X���[�^�ړ�                    */
static void CmdPDELAY ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�      */
static void CmdPOWER  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   POWER   �p���X���[�^�̗㎥����                    */
static void CmdPSET   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   PSET    �p���X���[�^�̃J�E���^���Z�b�g            */
static void CmdRELMV  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   RELMV   �p���X���[�^�ړ�(���Έʒu)                */
static void CmdSET    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   SET     �p���X���[�^���x�ݒ�                      */
static void CmdSTATUS ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   STATUS  �p���X���[�^��ԓǂݏo��                  */
static void CmdSTOP   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   STOP    �p���X���[�^������~                      */
static void CmdBZCNT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   BZCNT                                             */
static void CmdEEPREAD( T_CMD_MSG* mrcv, UB anum, const char** args );	/*   EEPREAD   */
static void CmdEEPWRIT( T_CMD_MSG* mrcv, UB anum, const char** args );	/*   EEPWRIT   */
static void CmdTmpSet ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdTmpGet ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdVini   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdPini   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdPren   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdPtan   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdPcan   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdVout   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdPsns   ( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdImpPdat( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdImpSped( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdImpPosi( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdImpOptn( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdExpRunl( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdExpComl( T_CMD_MSG* mrcv, UB anum, const char** args );
static void CmdExpClog( T_CMD_MSG* mrcv, UB anum, const char** args );

static void SetDevice( UB DevNo );										// �f�o�C�X���쒆�Z�b�g
static BOOL GetDeviceStatus( UB DevNo );								// �f�o�C�X�����Ԏ擾
static void ClrDevice( UB DevNo );										// �f�o�C�X���쒆�N���A
static ER GetDeviceActionChk ( UB msg, UB bcid, UB mm );				// �a�b����\�^�s����
static ER LimitAction( UB prid1, UB bcid, UB mm, UB dr, UB md, INT sp );	// ���~�b�g����
static ER RelmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp );				// ���Έʒu�ړ�����
static ER AbsmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp );				// ��Έʒu�ړ�����
static ER MapAction( UB prid1, UB bcid, UB mm1, UB mm2, INT pppp1, INT pppp2 );	// MAP�ړ�����
static ER WaitAction( UB prid1, UB bcid, INT waittim );							// �v�`�h�s����
static void GetBCDevice( UB msg, UB mm, INT *data );					// �a�b�ŗ��p�����f�o�C�X���̎擾

static ER QgProcList_INITA( T_CMD_MSG* mrcv );
static void TpRspFWVER();
static void TpRspLGIN1();
static void TpRspOPTIF();
static void TpRspINITA( ER result );
static void TpRspPRCTL();
static void TpRspFWPOL();
void TplRpMakeSend( UB parno, INT pardat );
void TplRpDataSend();


//*************************************
// �^�b�`�p�l���֐���`
//*************************************
       void TpAnalyze( T_CMD_MSG* mrcv );
static void TplDummy( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 00 �_�~�[                                             */
static void TplFwver( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 01 FW�o�[�W�������                                   */
static void TplOptif( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 02 �I�v�V�������                                     */
static void TplLgin1( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 03 ���O�C���F��1                                      */
static void TplLgin2( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 04 ���O�C���F��2                                      */
static void TplInita( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 05 �������w��                                         */
static void TplTitle( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 06 �v���g�R���^�C�g���v��                             */
static void TplPrExt( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 07 �v���g�R���L��                                     */
static void TplUsral( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 08 ���[�U�[���S�擾                                 */
static void TplInf01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 11 ���[�U�[1���                                      */
static void TplInf02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 12 ���[�U�[2���                                      */
static void TplInf03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 13 ���[�U�[3���                                      */
static void TplInf04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 14 ���[�U�[4���                                      */
static void TplInf05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 15 ���[�U�[5���                                      */
static void TplInf06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 16 ���[�U�[6���                                      */
static void TplInf07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 17 ���[�U�[7���                                      */
static void TplInf08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 18 ���[�U�[8���                                      */
static void TplInf09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 19 ���[�U�[9���                                      */
static void TplInf10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 1A ���[�U�[10���                                     */
static void TplRec01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 21 ���[�U�[1�o�^                                      */
static void TplRec02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 22 ���[�U�[2�o�^                                      */
static void TplRec03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 23 ���[�U�[3�o�^                                      */
static void TplRec04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 24 ���[�U�[4�o�^                                      */
static void TplRec05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 25 ���[�U�[5�o�^                                      */
static void TplRec06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 26 ���[�U�[6�o�^                                      */
static void TplRec07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 27 ���[�U�[7�o�^                                      */
static void TplRec08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 28 ���[�U�[8�o�^                                      */
static void TplRec09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 29 ���[�U�[9�o�^                                      */
static void TplRec10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 2A ���[�U�[10�o�^                                     */
static void TplChg01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 31 ���[�U�[1�ύX                                      */
static void TplChg02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 32 ���[�U�[2�ύX                                      */
static void TplChg03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 33 ���[�U�[3�ύX                                      */
static void TplChg04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 34 ���[�U�[4�ύX                                      */
static void TplChg05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 35 ���[�U�[5�ύX                                      */
static void TplChg06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 36 ���[�U�[6�ύX                                      */
static void TplChg07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 37 ���[�U�[7�ύX                                      */
static void TplChg08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 38 ���[�U�[8�ύX                                      */
static void TplChg09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 39 ���[�U�[9�ύX                                      */
static void TplChg10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 3A ���[�U�[10�ύX                                     */
static void TplDel01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 41 ���[�U�[1�폜                                      */
static void TplDel02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 42 ���[�U�[2�폜                                      */
static void TplDel03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 43 ���[�U�[3�폜                                      */
static void TplDel04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 44 ���[�U�[4�폜                                      */
static void TplDel05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 45 ���[�U�[5�폜                                      */
static void TplDel06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 46 ���[�U�[6�폜                                      */
static void TplDel07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 47 ���[�U�[7�폜                                      */
static void TplDel08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 48 ���[�U�[8�폜                                      */
static void TplDel09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 49 ���[�U�[9�폜                                      */
static void TplDel10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 4A ���[�U�[10�폜                                     */
static void TplFwpol( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 50 FW��ԁi�|�[�����O�j                               */
static void TplPrtcl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 51 �v���g�R�����s                                     */
static void TplResta( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 52 ���X�^�[�g���s                                     */
static void TplPause( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 53 �ꎞ��~                                           */
static void TplResum( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 54 �ĊJ                                               */
static void TplAbort( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 55 ���f                                               */
static void TplOptcd( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 58 �I�v�V�����o�[�R�[�h���s                           */
static void TplSmbar( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 59 �T���v���`���[�uQR�R�[�h                           */
static void TplSmsgl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5A �T���v���`���[�u�L���m�F�i�P�j                     */
static void TplSmall( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5B �T���v���`���[�u�L���m�F�i�S�j                     */
static void TplCllne( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5C ����`���[�u���[��QR�R�[�h                         */
static void TplClqrc( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5D ����`���[�uQR�R�[�h                               */
static void TplClsgl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5E ����`���[�u�L���i�P�j                             */
static void TplClall( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5F ����`���[�u�L���i�S�j                             */
static void TplLdlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 60 LED                                                */
static void TplUvlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 61 UV�_��                                             */
static void TplPtlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 62 �p�g�����v                                         */
static void TplPowof( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 63 �d��OFF                                            */
static void TplExRun( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 6B �^�]����                                           */
static void TplCommDataSend( UB *tpdat, INT tpsiz );					/* */
static void TplCommDataSendImp( UB *tpdat, INT tpsiz, UB msg );
static void TplRidDataSend( UB *txdat, UB id );
static ER   TpCmdCheck( T_CMD_MSG* mrcv );								/* �^�b�`�p�l���R�}���h�m�F                              */
static void InitQgProcList( UB procid );
static void StaQgProcList( UB bcid, UB msgid, UB devno, UB actstp, UB prid1, UB inf, UB inf2);
static UB GetQgProcList( UB procid );
static BOOL IsQgProcList( UB procid );
static ER QgProcList( T_CMD_MSG* mrcv );
static ER QgProcList_INITA( T_CMD_MSG* mrcv );
static ER QgProcList_PRTCL( T_CMD_MSG* mrcv );
static void SetUserInfoSorce( ST_USERINFO *user );
static void ClrUserInfoSorce();
static UB   ChkUserInfo();
void	check_main_pulsemotor_initialize( void );


static ER  CmdActionStatus( T_CMD_MSG* mrcv );	//��Ԏ擾
static ER  CmdActionTipGet( T_CMD_MSG* mrcv );	//�`�b�v�擾
static ER  CmdActionTipDsp( T_CMD_MSG* mrcv );	//�`�b�v�p��
static ER  CmdActionStrOpn( T_CMD_MSG* mrcv );	//������
static ER  CmdActionPress ( T_CMD_MSG* mrcv );	//����
static ER  CmdActionWait  ( T_CMD_MSG* mrcv );	//�E�F�C�g
static ER  CmdActionMix   ( T_CMD_MSG* mrcv );	//���a
static ER  CmdActionSuc   ( T_CMD_MSG* mrcv );	//�z��
static ER  CmdActionPut   ( T_CMD_MSG* mrcv );	//�f�o
static ER  CmdActionPrep1 ( T_CMD_MSG* mrcv );	//�����P
static ER  CmdActionPrep2 ( T_CMD_MSG* mrcv );	//�����Q
static ER  CmdActionIncub ( T_CMD_MSG* mrcv );	//�C���L���x�[�V����
static ER  CmdActionMove  ( T_CMD_MSG* mrcv );	//�ړ�

static ER  PressReq( UB *par );
static INT PressStatus( UB *par );
static INT GetPositionValue( UB index );
static UB  GetMoveDevice( UB posi );
static UB  GetTipPatern( UB commdat );
static void  SetPauseStop( UB act );
static UB  GetPauseStop();
static UB  InitPauseStop();
static INT  BlowoutAirValue( ST_HOSEI_PTN *adr );
static INT  AirGapValue( ST_HOSEI_PTN *adr );
static INT  SurplusValue( ST_HOSEI_PTN *adr );
static INT  ValueCal( ST_HOSEI_PTN *adr, INT value );
static INT GetPosiData( UB posino );
static unsigned short  NzlTargetGet();
static ER PressureChk();
static ER RePressureChk();
static BOOL TempChk();
static void IncuvDataInit( unsigned char tmp, unsigned int tim );

void SetRcvData( unsigned char *buf, unsigned int len );
void TplExRunSend();
void TplRpExpReqSend();

//*************************************
// �e�[�u��
//*************************************
// �R�[���t�@���N�V����
static CMD_FUNCTBL	CmdFuncTbl[ID_MSG_BASE_CMD_MAX] = 
{
	/*===========================================================================================================================*/
	/*  ��{�R�}���h                                                       No. Command ���e                                      */
	/*===========================================================================================================================*/
	&CmdVER    ,														/* 0   HELLO   �N���ʒm                                  */
	&CmdDummy  ,														/* 1   dummy                                             */
	&CmdVER    ,														/* 2   VER     �o�[�W�����m�F                            */
	&CmdALLIN  ,														/* 3   ALLIN   �SI/O����                                 */
	&CmdBIN    ,														/* 4   BIN     I/O����                                   */
	&CmdBOUT   ,														/* 5   BOUT    I/O�o��                                   */
	&CmdCINIT  ,														/* 6   CINIT   ���̓J�E���^�̐ݒ�                        */
	&CmdCOUNT  ,														/* 7   COUNT   ���̓J�E���^�̓ǂݏo��                    */
	&CmdINT    ,														/* 8   INT     ���荞�ݐݒ�                              */
	&CmdABSMV  ,														/* 9   ABSMV   �p���X���[�^�ړ�(��Έʒu)                */
	&CmdABORT  ,														/* 10  ABORT   �p���X���[�^����~                        */
	&CmdENCODER,														/* 11  ENCODER �G���R�[�_�̐ݒ�                          */
	&CmdLIMIT  ,														/* 12  LIMIT   �p���X���[�^���_���A                      */
	&CmdMAP    ,														/* 13  MAP     �Q���̃p���X���[�^�ړ�                    */
	&CmdPDELAY ,														/* 14  PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�      */
	&CmdPOWER  ,														/* 15  POWER   �p���X���[�^�̗㎥����                    */
	&CmdPSET   ,														/* 16  PSET    �p���X���[�^�̃J�E���^���Z�b�g            */
	&CmdRELMV  ,														/* 17  RELMV   �p���X���[�^�ړ�(���Έʒu)                */
	&CmdSET    ,														/* 18  SET     �p���X���[�^���x�ݒ�                      */
	&CmdSTATUS ,														/* 19  STATUS  �p���X���[�^��ԓǂݏo��                  */
	&CmdSTOP   ,														/* 20  STOP    �p���X���[�^������~                      */
	&CmdBZCNT,															/* 21                                                    */
	&CmdEEPREAD,														/* 22                                                    */
	&CmdEEPWRIT,														/* 23                                                    */
	&CmdDummy  ,														/* 24                                                    */
	&CmdDummy  ,														/* 25                                                    */
	&CmdTmpSet ,														/* 26                                                    */
	&CmdTmpGet ,														/* 27                                                    */
	&CmdDummy  ,														/* 28                                                    */
	&CmdVini   ,														/* 29                                                    */
	&CmdPini   ,														/* 30                                                    */
	&CmdPren   ,														/* 31                                                    */
	&CmdPtan   ,														/* 32                                                    */
	&CmdPcan   ,														/* 33                                                    */
	&CmdVout   ,														/* 34                                                    */
	&CmdPsns   , 														/* 35                                                    */
	&CmdImpPdat,														/* 36                                                    */
	&CmdImpSped,														/* 37                                                    */
	&CmdImpPosi,														/* 38                                                    */
	&CmdImpOptn,														/* 39                                                    */
	&CmdExpRunl,														/* 40                                                    */
	&CmdExpComl,														/* 41                                                    */
	&CmdDummy  ,														/* 42                                                    */
	&CmdDummy  ,														/* 43                                                    */
	&CmdDummy  															/* 44                                                    */
};


//-----------------------------
//        �^�b�`�p�l���ʐM
//-----------------------------
// �R�[���t�@���N�V����
TP_FUNCTBL	TpFuncTbl[TP_ID_PROC_MAX] = 
{
	/*===========================================================================================================================*/
	/*  ��{�R�}���h                                                       No. Command ���e                                      */
	/*===========================================================================================================================*/
	&TplDummy ,                                                         /* 00 �_�~�[                                             */
	&TplFwver ,                                                         /* 01 FW�o�[�W�������                                   */
	&TplOptif ,                                                         /* 02 �I�v�V�������                                     */
	&TplLgin1 ,                                                         /* 03 ���O�C���F��1                                      */
	&TplLgin2 ,                                                         /* 04 ���O�C���F��2                                      */
	&TplInita ,                                                         /* 05 �������w��                                         */
	&TplDummy ,                                                         /* 06 �_�~�[                                             */
	&TplDummy ,                                                         /* 07 �_�~�[                                             */
	&TplUsral ,                                                         /* 08 ���[�U�[���S�擾                                 */
	&TplDummy ,                                                         /* 09 �_�~�[                                             */
	&TplDummy ,                                                         /* 0A �_�~�[                                             */
	&TplDummy ,                                                         /* 0B �_�~�[                                             */
	&TplDummy ,                                                         /* 0C �_�~�[                                             */
	&TplDummy ,                                                         /* 0D �_�~�[                                             */
	&TplDummy ,                                                         /* 0E �_�~�[                                             */
	&TplDummy ,                                                         /* 0F �_�~�[                                             */
	&TplDummy ,                                                         /* 10 �_�~�[                                             */
	&TplInf01 ,                                                         /* 11 ���[�U�[1���                                      */
	&TplInf02 ,                                                         /* 12 ���[�U�[2���                                      */
	&TplInf03 ,                                                         /* 13 ���[�U�[3���                                      */
	&TplInf04 ,                                                         /* 14 ���[�U�[4���                                      */
	&TplInf05 ,                                                         /* 15 ���[�U�[5���                                      */
	&TplInf06 ,                                                         /* 16 ���[�U�[6���                                      */
	&TplInf07 ,                                                         /* 17 ���[�U�[7���                                      */
	&TplInf08 ,                                                         /* 18 ���[�U�[8���                                      */
	&TplInf09 ,                                                         /* 19 ���[�U�[9���                                      */
	&TplInf10 ,                                                         /* 1A ���[�U�[10���                                     */
	&TplDummy ,                                                         /* 1B �_�~�[                                             */
	&TplDummy ,                                                         /* 1C �_�~�[                                             */
	&TplDummy ,                                                         /* 1D �_�~�[                                             */
	&TplDummy ,                                                         /* 1E �_�~�[                                             */
	&TplDummy ,                                                         /* 1F �_�~�[                                             */
	&TplDummy ,                                                         /* 20 �_�~�[                                             */
	&TplRec01 ,                                                         /* 21 ���[�U�[1�o�^                                      */
	&TplRec02 ,                                                         /* 22 ���[�U�[2�o�^                                      */
	&TplRec03 ,                                                         /* 23 ���[�U�[3�o�^                                      */
	&TplRec04 ,                                                         /* 24 ���[�U�[4�o�^                                      */
	&TplRec05 ,                                                         /* 25 ���[�U�[5�o�^                                      */
	&TplRec06 ,                                                         /* 26 ���[�U�[6�o�^                                      */
	&TplRec07 ,                                                         /* 27 ���[�U�[7�o�^                                      */
	&TplRec08 ,                                                         /* 28 ���[�U�[8�o�^                                      */
	&TplRec09 ,                                                         /* 29 ���[�U�[9�o�^                                      */
	&TplRec10 ,                                                         /* 2A ���[�U�[10�o�^                                     */
	&TplDummy ,                                                         /* 2B �_�~�[                                             */
	&TplDummy ,                                                         /* 2C �_�~�[                                             */
	&TplDummy ,                                                         /* 2D �_�~�[                                             */
	&TplDummy ,                                                         /* 2E �_�~�[                                             */
	&TplDummy ,                                                         /* 2F �_�~�[                                             */
	&TplDummy ,                                                         /* 30 �_�~�[                                             */
	&TplChg01 ,                                                         /* 31 ���[�U�[1�ύX                                      */
	&TplChg02 ,                                                         /* 32 ���[�U�[2�ύX                                      */
	&TplChg03 ,                                                         /* 33 ���[�U�[3�ύX                                      */
	&TplChg04 ,                                                         /* 34 ���[�U�[4�ύX                                      */
	&TplChg05 ,                                                         /* 35 ���[�U�[5�ύX                                      */
	&TplChg06 ,                                                         /* 36 ���[�U�[6�ύX                                      */
	&TplChg07 ,                                                         /* 37 ���[�U�[7�ύX                                      */
	&TplChg08 ,                                                         /* 38 ���[�U�[8�ύX                                      */
	&TplChg09 ,                                                         /* 39 ���[�U�[9�ύX                                      */
	&TplChg10 ,                                                         /* 3A ���[�U�[10�ύX                                     */
	&TplDummy ,                                                         /* 3B �_�~�[                                             */
	&TplDummy ,                                                         /* 3C �_�~�[                                             */
	&TplDummy ,                                                         /* 3D �_�~�[                                             */
	&TplDummy ,                                                         /* 3E �_�~�[                                             */
	&TplDummy ,                                                         /* 3F �_�~�[                                             */
	&TplDummy ,                                                         /* 40 �_�~�[                                             */
	&TplDel01 ,                                                         /* 41 ���[�U�[1�폜                                      */
	&TplDel02 ,                                                         /* 42 ���[�U�[2�폜                                      */
	&TplDel03 ,                                                         /* 43 ���[�U�[3�폜                                      */
	&TplDel04 ,                                                         /* 44 ���[�U�[4�폜                                      */
	&TplDel05 ,                                                         /* 45 ���[�U�[5�폜                                      */
	&TplDel06 ,                                                         /* 46 ���[�U�[6�폜                                      */
	&TplDel07 ,                                                         /* 47 ���[�U�[7�폜                                      */
	&TplDel08 ,                                                         /* 48 ���[�U�[8�폜                                      */
	&TplDel09 ,                                                         /* 49 ���[�U�[9�폜                                      */
	&TplDel10 ,                                                         /* 4A ���[�U�[10�폜                                     */
	&TplDummy ,                                                         /* 4B �_�~�[                                             */
	&TplDummy ,                                                         /* 4C �_�~�[                                             */
	&TplDummy ,                                                         /* 4D �_�~�[                                             */
	&TplDummy ,                                                         /* 4E �_�~�[                                             */
	&TplDummy ,                                                         /* 4F �_�~�[                                             */
	&TplFwpol ,                                                         /* 50 FW��ԁi�|�[�����O�j                               */
	&TplPrtcl ,                                                         /* 51 �v���g�R�����s                                     */
	&TplResta ,                                                         /* 52 ���X�^�[�g���s                                     */
	&TplPause ,                                                         /* 53 �ꎞ��~                                           */
	&TplResum ,                                                         /* 54 �ĊJ                                               */
	&TplAbort ,                                                         /* 55 ���f                                               */
	&TplDummy ,                                                         /* 56 �_�~�[                                             */
	&TplDummy ,                                                         /* 57 �_�~�[                                             */
	&TplOptcd ,                                                         /* 58 �I�v�V�����o�[�R�[�h���s                           */
	&TplSmbar ,                                                         /* 59 �T���v���`���[�uQR�R�[�h                           */
	&TplSmsgl ,                                                         /* 5A �T���v���`���[�u�L���m�F�i�P�j                     */
	&TplSmall ,                                                         /* 5B �T���v���`���[�u�L���m�F�i�S�j                     */
	&TplCllne ,                                                         /* 5C ����`���[�u���[��QR�R�[�h                         */
	&TplClqrc ,                                                         /* 5D ����`���[�uQR�R�[�h                               */
	&TplClsgl ,                                                         /* 5E ����`���[�u�L���i�P�j                             */
	&TplClall ,                                                         /* 5F ����`���[�u�L���i�S�j                             */
	&TplLdlmp ,                                                         /* 60 LED                                                */
	&TplUvlmp ,                                                         /* 61 UV�_��                                             */
	&TplPtlmp ,                                                         /* 62 �p�g�����v                                         */
	&TplPowof ,                                                         /* 63 �d��OFF                                            */
	&TplDummy ,                                                         /* 64 �_�~�[                                             */
	&TplDummy ,                                                         /* 65 �_�~�[                                             */
	&TplDummy ,                                                         /* 66 �_�~�[                                             */
	&TplDummy ,                                                         /* 67 �_�~�[                                             */
	&TplDummy ,                                                         /* 68 �_�~�[                                             */
	&TplDummy ,                                                         /* 69 �_�~�[                                             */
	&TplDummy ,                                                         /* 6A �_�~�[                                             */
	&TplExRun ,                                                         /* 6B �^�]����                                           */
	&TplDummy ,                                                         /* 6C �_�~�[                                             */
	&TplDummy ,                                                         /* 6D �_�~�[                                             */
	&TplDummy ,                                                         /* 6E �_�~�[                                             */
	&TplDummy                                                           /* 6F �_�~�[                                             */
};

// �f�[�^�`�F�b�N�e�[�u��
// num:�p�����[�^���iCMD�����܂ށj
const ST_DATACHK DATCHK_TBL[44] = {
/*======================================================================================================================================================================================================================================================================*/
/*  ��{�R�}���h                                                               ��{�R�}���h                                                                                                                                                                             */
/*======================================================================================================================================================================================================================================================================*/
				//num   max1   max2   max3   max4   max5   max6   max7   max8  
/* 0  HELLO   */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 1  LOG     */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 2  VER     */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 3  ALLIN   */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 4  BIN     */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 5  BOUT    */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 6  CINIT   */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 7  COUNT   */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 8  INT     */	5  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 9  ABSMV   */	4  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 10 ABORT   */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 11 ENCODER */	6  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 12 LIMIT   */	5  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 13 MAP     */	7  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 14 PDELAY  */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 15 POWER   */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 16 PSET    */	5  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 17 RELMV   */	4  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 18 SET     */	8  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 19 STATUS  */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 20 STOP    */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 21 BCNT    */	4  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 22 EEPREAD */	0  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 23 EEPWRIT */	0  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 24 reserve */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 25 reserve */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 26 TEMPSET */	0  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 27 TEMPGET */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 28 reserve */	0  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 29 VINI    */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 30 PINI    */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 31 PREN    */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 32 PTAN    */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 33 PCAN    */	1  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 34 VOUT    */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 35 PSNS    */	2  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 36 IMPPDAT */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 37 IMPSPED */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 38 IMPPOSI */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 39 IMPOPTN */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 40 EXPRUNL */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 41 EXPCOML */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 42 reserve */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 43 reserve */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , /*  */
/* 44 reserve */	3  ,0    , 0    , 0    , 0    , 0    , 0    , 0    , 0      /*  */
};

static	const TPN_REQ_DAT TP_RID_SEND_FORM   = { 13, STX, 'W', 'N', ' ', 'R', 'I', 'D', ' ', '&', 'H', '*', '*', ETX, 0x30, 0x30, 0x0d };
static	const char TP_RP1_SEND_FORM01[13] = { STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '0', ETX,  0x30, 0x30, 0x0d };
static	const char TP_RP1_SEND_FORM02[14] = { STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '0', '0', ETX,  0x30, 0x30, 0x0d};
static	const char TP_RP1_SEND_FORM04[16] = { STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '0', '0',  '0',  '0',  ETX,  0x30, 0x30, 0x0d };
static	const char TP_RP3_SEND_FORM[44]   = { STX,'W','N',' ','R','P','3',' ','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*', ETX, 0x30, 0x30, 0x0d };



//*************************************
// �f�[�^��`
//*************************************
static	ST_CNTLDATA			stCtl;										// ���s�^�X�N����p�ް�

static	char				RecvData[CMD_BUFSIZ];						// 
static	const	char*		RecvArgs[MAX_ARGS];							// 
static	UB 					bufwk[CMD_BUFSIZ];							// 
static	ST_EEPBUF			eepb;
static	ST_USERINFO			RcvUserInfo;
static	INT 				DeviceStatus[2];							// ���[�^���쒆���  BIT=1:���쒆
static	ER 					MnetStatus;
static	ST_POWER			power[(DVMAX+1)];
static	ST_PROTCOL			*pProcAdr;
static	ST_PROTCOL_DATA		*pProcDat;
static	ST_BZCNT			bzcnt;
static	UB					PauseStopFlg;
static	unsigned int 		RcvDataSize;
static	unsigned char 		ExpRunlSeqno;
static	unsigned char 		ExpRunlRetry;
static	ST_CNTLDATA			tp;											// ���s�^�X�N����p�ް�
static	char				TpRecvData[CMD_BUFSIZ];						// 
static	const	char*		TpRecvArgs[MAX_ARGS];						// 
static	ST_EEPBUF			eepb;
static	ST_BCCTRL			bcctrl[BCCTRL_MAX];	
static ST_PAGE1				BkP1;
static ST_SPEED 			Sp[DVMAX+1][5];
static ST_SPEED_PTN			SpPtn;
static ST_PAGE7   			BkP7;
static ST_PAGE8				BkP8;
static ST_PAGE9				BkP9;
static ST_PAGE10			BkP10;
static ST_PROTCOL_DATA		BkPrData[20]; 
static ST_PROTCOL			BkPr; 
static ST_INCUV				incuv;
static ST_VOUT				vout;
static ST_PSNS				psns;
static ST_PROTCOL_SEL		psel;
static unsigned short		nzlsts;
static	char				rspbuf2[128];
static	char				psnsdat[128];
static	char				rspbuf[CMD_BUFSIZ];
static	char				ImpExpRcvData[3400];
static	INT					peekdat;
		UB					MentePcSendbuf[3400];

// debug�p
static unsigned debug1_flg;


/***********************************************************************
* 
* [�T�v]�R�}���h�Ǘ��^�X�N
* 
* 
* 
***********************************************************************/
TASK TskCmd( void )
{
    T_CMD_MSG*  	msg;
	ER          	err;

	// ������
	// �X���[�u��胁�C����̃��[�V�����l�b�g���A��ɗ����オ�����ꍇ�A�X���[�u���ڑ��Ɣ��f����邽��
	// �X���[�u�������オ�鎞�ԕ���҂��Ă���N��
	dly_tsk(1000);

	// �R�}���h�^�X�N������
	CmdTskInit();

	// ���[�V�����l�b�g������
	MnetStatus = MntInit();

	// �R�}���h�^�X�N��������
	if( MnetStatus != E_SUCCESS )
	{
		CmdTskInitAct();
	}
	
	for( ;; )
    {
        // �R�}���h�҂�
        err = rcv_mbx( ID_MBX_CMD, (T_MSG**)&msg);

    	// ���b�Z�[�W����
    	CmdProc(msg);

    	RelMpf( ID_MPF_CMD, (VP)msg );// ���������
    }
}
/***********************************************************************
* 
* [�T�v]	���s�^�X�N������
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdTskInit()
{
	INT Loop;

	// �f�[�^�N���A
	memset( &stCtl,       CLR,  sizeof( stCtl ) );
	memset( &RecvData,    CLR,  sizeof( RecvData ) );
	memset( &RecvArgs,    CLR,  sizeof( RecvArgs ) );
	memset( &TpRecvData,  CLR,  sizeof( TpRecvData ) );
	memset( &TpRecvArgs,  CLR,  sizeof( TpRecvArgs ) );
	memset( &bufwk[0],    CLR,  CMD_BUFSIZ );
	memset( &psel,        CLR,  sizeof( psel ) );
	memset( &incuv,       CLR,  sizeof( incuv ) );

	for( Loop = 0; Loop < BCCTRL_MAX; Loop++ )
	{
		bcctrl[Loop].actstp = 0;
		bcctrl[Loop].msgid  = NO_DATA;
		bcctrl[Loop].devno  = NO_DATA;
		bcctrl[Loop].prid1 = NO_DATA;
	}

	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		memset( &power[Loop], CLR, sizeof( ST_POWER ) );
	}

	// 7SEG�ҋ@�\��
	*( (UB *)LED_ADDR ) = CLR;

}
/***********************************************************************
* 
* [�T�v]	�R�}���h�^�X�N��������
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdTskInitAct()
{
	INT Loop;
    ST_MOTOR_PARAM	mp;

	// �������x�ݒ�
	mp.accel_type = 1;												// �쓮�^�C�v(�O�F��`�A�P�F�r��)
	mp.factor     = GetFactor(3);									// ���[�^�{��
	mp.FL         = 300;											// ����[pls](0�`100,000)
	mp.FH         = 1000;											// �ō���[pls](0�`100,000)
	mp.utime      = 500;											// ��������[ms](�P�`65535)
	mp.dtime      = 500;											// ��������[ms](�P�`65535)
	mp.us         = 500;											// �������r���͈�
	mp.ds         = 500;											// �������r���͈�
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		motor_SET( Loop, mp );
	}

}
/***********************************************************************
* 
* [�T�v]	�^�X�N�ʉ�͏���
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_RX0:	// TSK_RX0->
		Rx0RcvMsg(mrcv);
		break;
	case ID_TSK_TX0:	// TSK_TX0->
		Tx0RcvMsg(mrcv);
		break;
	case ID_TSK_CMD:
//		AlmRcvMsg(mrcv);
//		break;
	case ID_TSK_RXEX:	// TSK_RX2->
		RxExRcvMsg(mrcv);
		break;
	case ID_TSK_TXEX:
		break;
	case ID_TSK_EEP:	// TSK_EEP->
		EepRcvMsg(mrcv);
		break;
	case ID_TSK_MOTWAIT:
		MotWaitRcvMsg(mrcv);
		break;
	case ID_TSK_MOTEND:
		MotEndRcvMsg(mrcv);
		break;
	case ID_TSK_INT:
		IntRcvMsg(mrcv);
		break;
	case ID_TSK_PUNIT:
		PunitRcvMsg(mrcv);
		break;
	case ID_TSK_TMPCNT:
		TmpCntRcvMsg(mrcv);
		break;
		
	default:
		break;
	}
}
/***********************************************************************
* 
* [�T�v]	�o�b��M�i�q�w�O�j�^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void Rx0RcvMsg( T_CMD_MSG* mrcv )
{
	UB			msgid;

	// ���b�Z�[�W�h�c�擾
	msgid = mrcv->msgdata.msgid;

	switch( msgid )
	{
	case ID_MSG_ACK:
		// ����f�[�^������
		memset( &stCtl,  CLR,  sizeof( stCtl ) );
		break;

	case ID_MSG_NAK:
		if( stCtl.rsp.rty > RTY_OVER ) 
		{
			// ���g���C�I�[�o�[
			// ����f�[�^������
			memset( &stCtl,  CLR,  sizeof( stCtl ) );
		}
		else
		{
			// �đ�
			CmdSendResponse( &stCtl );
		}
		break;

	default:
		// ��{�R�}���h
		if( msgid < ID_MSG_BASE_CMD_MAX )
		{
			CmdAnalyze( mrcv );
		}
		else
		{
			// ���肦�Ȃ�
		}
		break;
	}
}

/***********************************************************************
* 
* [�T�v]	�o�b���M�i�s�w�O�j�^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void Tx0RcvMsg( T_CMD_MSG* mrcv )
{
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void AlmRcvMsg( T_CMD_MSG* mrcv )
{
}

/***********************************************************************
* 
* [�T�v]	�d�d�o�q�n�l�Ǘ��^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void EepRcvMsg( T_CMD_MSG* mrcv )
{
	UB	msgid;
	UB	inf;
	UB	inf2;
	UB	prid1;
	UB	bcid;
	UB	result = SET;
	int i;
	UB	*sramadr;
	int index;
	
	prid1 = mrcv->msgdata.prid1;										// 
	bcid  = mrcv->msgdata.bcid;											// 
	msgid = mrcv->msgdata.msgid;
	inf   = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	
	if( msgid == ID_MSG_EEPREAD )
	{
		switch( prid1 )
		{
		case ID_TPL_FWVER:
			if( (inf2 & EEPROM_SPED_PTRN) )	// ���x���iPage2�`Page4�j
			{
				eep_GetSpeedData( &Sp[MT_Y0][0].ac );
				eep_GetSpeedPattern( &SpPtn.Y0_SpdIniPtn );
			}
			if( (inf2 & EEPROM_POSI_INFO) )	// �ʒu���iPage7�`Page9�j
			{
				eep_GetPositionInfo( &BkP7.Y0_Offset );
			}
			TpRspFWVER();
			break;
		
		case ID_TPL_LGIN1:
		case ID_TPL_LGIN2:
			TpRspLGIN1();
			break;

		case ID_TPL_OPTIF:
			eep_GetOptionInfo  ( &BkP10.Login );
			TpRspOPTIF();
			break;

		case ID_TPL_PRTCL:
			if(( inf2 > 0 )&&( inf2 <= PROT_CT_MAX ))
			{
				index = inf2 - 1;
				
				// BkPrData
				// UB Cnt; �g�[�^���X�e�b�v���i 1 �` 80 �j
				// UB Prc; �H���L��
				// UB Stp; �����X�e�b�v
				pProcDat = (ST_PROTCOL_DATA *)&BkPrData[index].Cnt;
				eep_GetProtocolData( pProcDat,  index );
				
				pProcAdr = (ST_PROTCOL *)( SRAM_START_ADDRESS + sizeof(ST_PROTCOL) * index);
				eep_GetProtocol( pProcAdr );

				StaQgProcList( bcid,          // bcid
							   msgid,         // msgid
					           NO_DATA,       // devno
					           1,             // actstp
					           ID_TPL_PRTCL,  // prid1
				               inf,           // inf
				               inf2           // inf2(�v���g�R���ԍ�)
				);
				
				QgProcList( mrcv );
			}
			break;
		}
	}

	if(
		( msgid == ID_MSG_IMPPDAT )||
		( msgid == ID_MSG_IMPSPED )||
		( msgid == ID_MSG_IMPPOSI )
	)
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,                                         // 
							PARAM2, 										// �p�����[�^��
							STRTBL_MSG[msgid], 								// �R�}���h������
							E_SUCCESS,										// �G���[�R�[�h

							0, 												// 
							0, 												// 
							0,												// 
							0, 												// 
							0, 												// 

							0, 												// 
							0, 												// 
							0,												// 
							0, 												// 
							0 												// 
						);
		CmdSendResponse( &stCtl );											// �������M
	}

}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void IntRcvMsg( T_CMD_MSG* mrcv )
{
	ER result = E_SUCCESS;
	UB Loopd;
	UB intbuf[8];
	INT TargetDevice[2];
	INT CompareDevice[2];
	
	// �����~�������f�o�C�X���֘A����BLKCMD������ꍇ��BLKCMD�Ǘ��e�[�u���̓o�^����O��
	memcpy( &intbuf[0], &mrcv->msgdata.data[0], 8 );
	
	// ���
	TargetDevice[1] =	intbuf[7] * 0x1000000 +
						intbuf[6] * 0x10000 +
						intbuf[5] * 0x100 +
						intbuf[4];

	// 
	TargetDevice[0] =	intbuf[3] * 0x1000000 +
						intbuf[2] * 0x10000 +
						intbuf[1] * 0x100 +
						intbuf[0];

}
/***********************************************************************
* 
* [�T�v]	�^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotWaitRcvMsg( T_CMD_MSG* mrcv )
{
	ER result = E_SUCCESS;
	
	if( mrcv->msgdata.bcid != NO_DATA )
	{
//		result = BlkCmdProc( mrcv );
	}
	if( mrcv->msgdata.prid1 != NO_DATA )
	{
		result = QgProcList( mrcv );
	}
}
	
/***********************************************************************
* 
* [�T�v]	�^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void MotEndRcvMsg( T_CMD_MSG* mrcv )
{
	ER result  = E_SUCCESS;
	ER result1 = E_SUCCESS;
	ER result2 = E_SUCCESS;
	ST_MOTOR_STATUS		sts;										// 
	ST_MOTOR_STATUS		sts2;										// 
	UB prid1;
	UB bcid;
	UB msg;
	UB mm1;
	UB mm2;
	UB Loopd;
	INT TargetDevice[2];
	INT CompareDevice[2];

	prid1 = mrcv->msgdata.prid1;
	bcid = mrcv->msgdata.bcid;
	msg  = mrcv->msgdata.msgid;
	mm1 = mrcv->msgdata.inf;
	mm2 = mrcv->msgdata.inf2;
	stCtl.rsp.siz = mrcv->msgdata.siz;
	
	ClrDevice( mm1 );	

	if( bcid == NO_DATA )
	{
		switch( msg )
		{
		case ID_MSG_MAP:
			if( GetDeviceStatus( mm2 ) == TRUE )	return;

			result1 = motor_STATUS( mm1, &sts  );
			result2 = motor_STATUS( mm2, &sts2 );

			if( result2 != E_SUCCESS )		result = result2;
			if( result1 != E_SUCCESS )		result = result1;
			
			// �������b�Z�[�W�쐬
			CmdMakeResponse(	&stCtl,
								PARAM12, 								// �p�����[�^��
								STRTBL_MSG[msg], 						// �R�}���h������
								result, 								// �G���[�R�[�h

								mm1,									// ���[�^�P�ԍ�
								sts.pos, 								// �p���X�J�E���g�P
								sts.enc,								// �G���R�[�_�P
								sts.cw_limit, 							// CW���~�b�g�Z���T(0:OFF,1:ON)
								sts.ccw_limit, 							// CCW���~�b�g�Z���T(0:OFF,1:ON)

								mm2, 									// ���[�^�Q�ԍ�
								sts2.pos, 								// �p���X�J�E���g�Q
								sts2.enc,								// �G���R�[�_�Q
								sts2.cw_limit, 							// CW���~�b�g�Z���T(0:OFF,1:ON)
								sts2.ccw_limit							// CCW���~�b�g�Z���T(0:OFF,1:ON)
							);
			break;


		case ID_MSG_ABORT:
		case ID_MSG_STOP:
			// �����~�������f�o�C�X���֘A����a�b������ꍇ�ɂa�b�Ǘ��e�[�u���̓o�^����O��

			// �Ώۃf�o�C�X������
			memset( &TargetDevice[0], CLR, sizeof( TargetDevice ) );

			// �Ώۃf�o�C�X�擾
			BitSet_INT( mm1, &TargetDevice[0] );

			// ������  ���ɑ���  ��������������������������������������������

		default:
			// ���X�|���X�f�[�^�擾
			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], stCtl.rsp.siz );
			break;
		}
		
		// �������M
		CmdSendResponse( &stCtl );
	}
	else
	{
		ClrDevice( mm1 );	
		if( msg == ID_MSG_MAP )
		{
			if( GetDeviceStatus( mm2 ) == TRUE )	return;
		}
		
		QgProcList( mrcv );
	}
}

/***********************************************************************
* 
* [�T�v]	�g���V���A����M�^�X�N����̃��b�Z�[�W��M����
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void RxExRcvMsg( T_CMD_MSG* mrcv )
{
	if( mrcv->msgdata.prid1 < TP_ID_PROC_MAX )
	{
		TpAnalyze( mrcv );
	}
	else
	{
		//Error
	}
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void PunitRcvMsg( T_CMD_MSG* mrcv )
{
	UB msg;
	UB nzl;
	UB smp;
	unsigned char  result_h = CLR;
	unsigned char  result_l = CLR;
	ER result   = E_SUCCESS;
	
	msg  = mrcv->msgdata.msgid;
	
	switch( msg )
	{
		case ID_MSG_PRESSTAN_HS :
		case ID_MSG_PRESSREN_HS :
			memcpy( &psns.now[0], &mrcv->msgdata.data[0], sizeof(ST_PSNS) );

			// �������b�Z�[�W�쐬
			CmdMakeResponsePsns( msg,                                           // message
								STRTBL_MSG[msg], 								// �R�}���h������
								result,											// 
								psns.now[0], 									// 
								psns.now[1], 									// 
								psns.now[2],									// 
								psns.now[3], 									// 
								psns.now[4], 									// 
								psns.now[5], 									// 
								psns.now[6], 									// 
								psns.now[7],									// 
								psns.now[8], 									// 
								psns.now[9], 									// 
								psns.now[10], 									// 
								psns.now[11], 									// 
								psns.now[12]); 									// 
			break;
//
// �{���b�Z�[�W�́A�d�l���M�Ȃ��̂ŃR�����g
//		case ID_MSG_PRESSREN:
//		case ID_MSG_PRESSTAN:
// 
//			memcpy( &psns.now[0], &mrcv->msgdata.data[0], sizeof(ST_PSNS) );
// 
//			for( nzl = 0; nzl < NOZZLE_MAX ; nzl++ )
//			{
//				for( smp = 0; smp < (PRESSMP_MAX-1) ; smp++ )
//				{
//					psns.dat[nzl][smp+1] = psns.dat[nzl][smp];
//				}
//				psns.dat[nzl][0] = psns.now[nzl];
//			}
//			QgProcList( mrcv );
// 
//			break;
//

//		case ID_MSG_VALVEOUT_HS :
//			memcpy( &vout.target, &mrcv->msgdata.data[0],      sizeof(ST_VOUT) );
//			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], sizeof(ST_VOUT) );
//			CmdSendResponse( &stCtl );
//			break;


		case ID_MSG_VALVEOUT_HS :
		case ID_MSG_VALVEINI_HS:
		case ID_MSG_PRESSINI_HS:
		case ID_MSG_PRESSCAN_HS:
//			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], mrcv->msgdata.siz );
			result_h = mrcv->msgdata.data[0];
			result_l = mrcv->msgdata.data[1];
			result = result_h * 0x0100 + result_l;
		
			// �������b�Z�[�W�쐬
			CmdMakeResponse( 	&stCtl,                                         // 
								PARAM2, 										// �p�����[�^��
								STRTBL_MSG[msg], 								// �R�}���h������
								result,											// �G���[�R�[�h

								0, 												// 
								0, 												// 
								0,												// 
								0, 												// 
								0, 												// 

								0, 												// 
								0, 												// 
								0,												// 
								0, 												// 
								0 												// 
							);
		// �������b�Z�[�W�쐬
//		CmdMakeResponse( 	&msgdat,
//							PARAM7, 									// �p�����[�^��
//							STRTBL_MSG[msg], 							// �R�}���h������
//							result, 									// �G���[�R�[�h
//
//							(INT)mm,									// ���[�^�P�ԍ�
//							sts.pos, 									// �p���X�J�E���g�P
//							sts.enc,									// �G���R�[�_�P
//							sts.cw_limit, 								// CW���~�b�g�Z���T(0:OFF,1:ON)
//							sts.ccw_limit, 								// CCW���~�b�g�Z���T(0:OFF,1:ON)
//
//							0, 											// ���[�^�Q�ԍ�
//							0, 											// �p���X�J�E���g�Q
//							0,											// �G���R�[�_�Q
//							0, 											// CW���~�b�g�Z���T(0:OFF,1:ON)
//							0			 								// CCW���~�b�g�Z���T(0:OFF,1:ON)
//						);

		
			CmdSendResponse( &stCtl );
			break;


		case ID_MSG_VALVEINI:
		case ID_MSG_PRESSINI:
		case ID_MSG_PRESSCAN:
		case ID_MSG_VALVEOUT:
			QgProcList( mrcv );
			break;

		default:
			break;
	}
}
/**********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TmpCntRcvMsg( T_CMD_MSG* mrcv )
{
	UB msg;

	msg  = mrcv->msgdata.msgid;
	
	switch( msg )
	{
		case ID_MSG_TMPSET :
		case ID_MSG_TMPGET :
		case ID_MSG_TMPGETR :
			QgProcList( mrcv );
			break;

		case ID_MSG_TMPSET_HS :
		case ID_MSG_TMPGET_HS :
//			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], stCtl.rsp.siz );	// ���X�|���X�f�[�^�擾
			stCtl.rsp.siz = mrcv->msgdata.siz;
			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], mrcv->msgdata.siz );	// ���X�|���X�f�[�^�擾
			CmdSendResponse( &stCtl );											// �������b�Z�[�W���M
			break;


		default:
			break;
	}
}
/***********************************************************************
* 
* [�T�v]	�R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void AlmComR0Ack( VP_INT exinf )
{
   int save_i;
	exinf = 0;

#if 0// 2022.03.02
	stp_alm( ID_ALM_R0ACK );												// 
#endif
    // �����݋֎~����
    save_i = get_imask();
 	set_imask(0x0f);

	if( stCtl.rsp.rty > RTY_OVER ) 
	{
		// ���g���C�I�[�o�[
		
		
		// ����f�[�^������
		memset( &stCtl,  CLR,  sizeof( stCtl ) );
	}
	else
	{
		// �đ�
		CmdSendResponse( &stCtl );
	}
	set_imask(save_i);
}
/***********************************************************************
* 
* [�T�v]	�o�[�R�[�h���X�|���X�^�C���A�E�g
*           ��莞�Ԍo�߂��Ă��o�[�R�[�h���烌�X�|���X���Ȃ��ꍇ�̃^�C��
*           �A�E�g����
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void AlmCom2RspTmo( VP_INT exinf )
{
//    RelMpf( ID_MPF_CMD, (VP)msg );// ���������
}

/***********************************************************************
* 
* [�T�v]	�R�}���h�̉��
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdAnalyze( T_CMD_MSG* mrcv )
{
	UB			msgid;
	UB			anum;

	// ���b�Z�[�W�h�c�擾
	msgid = mrcv->msgdata.msgid;
	memset( &RecvData,    CLR,  sizeof( RecvData ) );
	memset( &RecvArgs,    CLR,  sizeof( RecvArgs ) );
	
	if(
		( msgid == ID_MSG_IMPPDAT )||
		( msgid == ID_MSG_IMPSPED )||
		( msgid == ID_MSG_IMPPOSI )||
		( msgid == ID_MSG_IMPOPTN )||
		( msgid == ID_MSG_EXPRUNL )||
		( msgid == ID_MSG_EXPCOML )
	)
	{
		// ImpExpRcvData�́A��M�����Ŋ��ɑ啶�����Ă���̂ŕs�v�B
		anum = SplitOnly( &ImpExpRcvData[0], RecvArgs );
	}
	else
	{
		memcpy( &RecvData[0],     &mrcv->msgdata.data[0],CMD_BUFSIZ );		// �R�}���h�d���R�s�[
		ToUpper( (char*)&RecvData );										// �啶��
		anum = Split( &RecvData[0], RecvArgs );								// ���� �� �p�����[�^���擾
	}
	
	// �R�[���t�@���N�V����
	(*CmdFuncTbl[msgid])( mrcv, anum, RecvArgs  );
}

/***********************************************************************
* 
* [�T�v]	����I���ʒm
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void CmdEndResponse( ID txkid, UB prid1, UB bcid, UB msgid, UB inf1, UB inf2, ST_CNTLDATA* cntl )
{
	mbx_send	(
					txkid,												// ���ID
					ID_MPF_CMD,											// �߰�ID
					ID_MBX_CMD,											// MBOXID
					prid1,												// PROCID
					bcid,												// BCID
					msgid,												// ү����ID
					inf1,												// ���1
					inf2,												// ���2
					&cntl->rsp.buf[0],									// DATA
					cntl->rsp.siz										// DATA�T�C�Y
				);
}
/***********************************************************************
* 
* [�T�v]	�R�}���h�����̑��M
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void CmdSendResponse( ST_CNTLDATA* cntl )
{
	// ���X�|���X���M
	SendResponse( cntl );
	cntl->rsp.rty++;
}
/***********************************************************************
* 
* [�T�v]	���X�|���X���M
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void SendResponse( ST_CNTLDATA* cntl )
{
	//( ���ID, ����߰�ID, ү����BOXID,ү����ID, ���, DATA, DATA�T�C�Y)
	mbx_send	(
					ID_TSK_CMD,               // ID tskid,    ���ID
					ID_MPF_TX0,               // ID mpfid,    �߰�ID
					ID_MBX_TX0,               // ID mbxid,    MBOXID
					NO_DATA,                  // UB prid1,    PROCID
					NO_DATA,                  // UB bcid,     BCID
					ID_MSG_CMD,               // UB msgid,    ү����ID
					0,                        // UB sts,      ���1
					0,                        // UB sts2,     ���2
					&cntl->rsp.buf[0],        // UB* data,    DATA
					cntl->rsp.siz             // INT siz      DATA�T�C�Y
				);
}
/***********************************************************************
* 
* [�T�v]	�R�}���h�����̑��M
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void CmdSendResponseEtc( ST_CNTLDATA* cntl, UB msgid )
{
	//( ���ID, ����߰�ID, ү����BOXID,ү����ID, ���, DATA, DATA�T�C�Y)
	mbx_send	(
					ID_TSK_CMD,               // ID tskid,    ���ID
					ID_MPF_TX0,               // ID mpfid,    �߰�ID
					ID_MBX_TX0,               // ID mbxid,    MBOXID
					NO_DATA,                  // UB prid1,    PROCID
					NO_DATA,                  // UB bcid,     BCID
					msgid,                    // UB msgid,    ү����ID
					0,                        // UB sts,      ���1
					0,                        // UB sts2,     ���2
					&cntl->rsp.buf[0],        // UB* data,    DATA
					cntl->rsp.siz             // INT siz      DATA�T�C�Y
				);
	
	cntl->rsp.rty++;
}

/***********************************************************************
* 
* [�T�v]	�R�}���h�����̑��M
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void CmdSendResponse2( ST_CNTLDATA* cntl )
{
	// ���X�|���X���M
	SendResponse2( cntl );
	cntl->rsp.rty++;
}
/***********************************************************************
* 
* [�T�v]	���X�|���X���M
* [����]	�|
* [�ߒl]	�|
* 
***********************************************************************/
void SendResponse2( ST_CNTLDATA* cntl )
{
	//( ���ID, ����߰�ID, ү����BOXID,ү����ID, ���, DATA, DATA�T�C�Y)
	mbx_send	(
					ID_TSK_CMD,
					ID_MPF_TXEX,
					ID_MBX_TXEX,
					NO_DATA,
					NO_DATA,
					ID_MSG_CMD,
					0,
					0,
					&cntl->rsp.buf[0],
					cntl->rsp.siz
				);
}

/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponsePsns( UB msgid,
						const char* cmdchr,
						INT result,
						unsigned short d1,
						unsigned short d2,
						unsigned short d3,
						unsigned short d4,
						unsigned short d5,
						unsigned short d6,
						unsigned short d7,
						unsigned short d8,
						unsigned short d9,
						unsigned short d10,
						unsigned short d11,
						unsigned short d12,
						unsigned short d13 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;
	UB dmyadr ;

	memset( &rspbuf2,  CLR,  sizeof( rspbuf2 ) );
	
				//    1   2  D01 D02 D03 D04 D05 D06 D07 D08 D09 D10 D11 D12 D13
	sprintf(rspbuf2, "%s,%04X,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d" ,cmdchr,result,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13);

	len = strlen(&rspbuf2);
	psnsdat[0] = STX;							// STX
	memcpy( &psnsdat[1], &rspbuf2, len );		// �R�}���h�`�f�[�^�܂ŃR�s�[
	psnsdat[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( (UB*)&psnsdat[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	psnsdat[(len+2)] = bcch;					// BCC1
	psnsdat[(len+3)] = bccl;					// BCC2
	
	
	mbx_send	(
				ID_TSK_CMD,               // ID tskid,    ���ID
				ID_MPF_TX0,               // ID mpfid,    �߰�ID
				ID_MBX_TX0,               // ID mbxid,    MBOXID
				NO_DATA,                  // UB prid1,    PROCID
				NO_DATA,                  // UB bcid,     BCID
				msgid,               // UB msgid,    ү����ID
				0,                        // UB sts,      ���1
				0,                        // UB sts2,     ���2
				&dmyadr,                  // UB* data,    DATA
				1                         // INT siz      DATA�T�C�Y
			);
}
/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponse(	ST_CNTLDATA* cntl,
						UB anum,
						const char* p1,
						INT p2,
						INT p3,
						INT p4,
						INT p5,
						INT p6,
						INT p7,
						INT p8,
						INT p9,
						INT p10,
						INT p11,
						INT p12 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );

	if(( anum < PARAM1 )||( anum > PARAM12 ))	return;
											 //  1   2   3   4   5   6   7   8   9   10  11  12
 	if( anum == PARAM12)		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d" , p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
	if( anum == PARAM11)		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d"     , p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
	if( anum == PARAM10)		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d"         , p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
	if( anum == PARAM9 )		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d,%0d,%0d"             , p1, p2, p3, p4, p5, p6, p7, p8, p9);
	if( anum == PARAM8 )		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d,%0d"                 , p1, p2, p3, p4, p5, p6, p7, p8);
	if( anum == PARAM7 )		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d,%0d"                     , p1, p2, p3, p4, p5, p6, p7);
	if( anum == PARAM6 )		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d,%0d"                         , p1, p2, p3, p4, p5, p6);
	if( anum == PARAM5 )		sprintf(rspbuf, "%s,%04X,%0d,%0d,%0d"                             , p1, p2, p3, p4, p5);
	if( anum == PARAM4 )		sprintf(rspbuf, "%s,%04X,%0d,%0d"                                 , p1, p2, p3, p4);
	if( anum == PARAM3 )		sprintf(rspbuf, "%s,%04X,%0d"                                     , p1, p2, p3);
	if( anum == PARAM2 )		sprintf(rspbuf, "%s,%04X"                                         , p1, p2);
	if( anum == PARAM1 )		sprintf(rspbuf, "%s"                                              , p1);
	
	
	len = strlen(&rspbuf);
	cntl->rsp.buf[0] = STX;							// STX
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );			// �R�}���h�`�f�[�^�܂ŃR�s�[
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2��4byte���������O�X�ɉ��Z
}
/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponseX( ST_CNTLDATA* cntl, UB anum, const char* cmd, INT err, INT p2,INT p3, INT p4, INT p5, INT p6, INT p7, INT p8 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;
//	char rspbuf[CMD_BUFSIZ];

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );
	
	if(( anum < PARAM1 )||( anum > PARAM9 ))	return;

	if( anum == PARAM9 )		sprintf(rspbuf, "%s,%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X", cmd, err, p2, p3, p4, p5, p6, p7, p8);
	if( anum == PARAM8 )		sprintf(rspbuf, "%s,%04X,%04X,%04X,%04X,%04X,%04X,%04X", cmd, err, p2, p3, p4, p5, p6, p7);
	if( anum == PARAM7 )		sprintf(rspbuf, "%s,%04X,%04X,%04X,%04X,%04X,%04X", cmd, err, p2, p3, p4, p5, p6);
	if( anum == PARAM6 )		sprintf(rspbuf, "%s,%04X,%04X,%04X,%04X,%04X", cmd, err, p2, p3, p4, p5);
	if( anum == PARAM5 )		sprintf(rspbuf, "%s,%04X,%04X,%04X,%04X", cmd, err, p2, p3, p4);
	if( anum == PARAM4 )		sprintf(rspbuf, "%s,%04X,%04X,%04X", cmd, err, p2, p3);
	if( anum == PARAM3 )		sprintf(rspbuf, "%s,%04X,%04X", cmd, err, p2);
	if( anum == PARAM2 )		sprintf(rspbuf, "%s,%04X", cmd, err);
	if( anum == PARAM1 )		sprintf(rspbuf, "%s", cmd);
	
	
	len = strlen(&rspbuf);
	cntl->rsp.buf[0] = STX;							// STX
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// �R�}���h�`�f�[�^�܂ŃR�s�[
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2��4byte���������O�X�ɉ��Z
}
/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponseInt( ST_CNTLDATA* cntl, UB anum, const char* p1, INT p2, INT p3 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );

	if(( anum < PARAM1 )||( anum > PARAM3 ))	return;

	if( anum == PARAM3 )		sprintf(rspbuf, "%s,%0d,%0d" , p1, p2, p3);
	if( anum == PARAM2 )		sprintf(rspbuf, "%s,%0d"     , p1, p2);
	if( anum == PARAM1 )		sprintf(rspbuf, "%s"         , p1);
	
	len = strlen(&rspbuf);
	cntl->rsp.buf[0] = STX;							// STX
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// �R�}���h�`�f�[�^�܂ŃR�s�[
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2��4byte���������O�X�ɉ��Z
}
/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬(���̓J�E���^�ǂݏo���p)
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponseCount( ST_CNTLDATA* cntl, UB anum, const char* p1, INT p2, INT p3, UINT p4 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );
	sprintf(rspbuf, "%s,%04X,%0d,%0u" , p1, p2, p3, p4);
	
	len = strlen(&rspbuf);
	cntl->rsp.buf[0] = STX;							// STX
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// �R�}���h�`�f�[�^�܂ŃR�s�[
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2��4byte���������O�X�ɉ��Z
}
/***********************************************************************
* 
* [�T�v]	�����f�[�^�쐬
* [����]	
* [�ߒl]	�|
* 
***********************************************************************/
void CmdMakeResponseStr( ST_CNTLDATA* cntl, UB anum, const char* cmd, INT err, char* str2, char* str3 )
{
	INT  len;
	UB bcc;
	UB bcch;
	UB bccl;

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );

	if(( anum < PARAM1 )||( anum > PARAM4 ))	return;

	if( anum == PARAM4 )		sprintf(rspbuf, "%s,%04X,%s,%s", cmd, err, str2, str3);
	if( anum == PARAM3 )		sprintf(rspbuf, "%s,%04X,%s",    cmd, err, str2);
	if( anum == PARAM2 )		sprintf(rspbuf, "%s,%04X",       cmd, err);
	if( anum == PARAM1 )		sprintf(rspbuf, "%s",            cmd);

	len = strlen(&rspbuf);
	cntl->rsp.buf[0] = STX;							// STX
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// �R�}���h�`�f�[�^�܂ŃR�s�[
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC�v�Z
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10�i����16�i����ASC�ϊ�
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2��4byte���������O�X�ɉ��Z
}
/***********************************************************************
* 
* [�T�v]	��M�p�����[�^�`�F�b�N
* [����]	msg:�R�}���h�h�c�Aanum:�p�����[�^���Aargs:�p�����[�^�f�[�^�A�h���X
* [�ߒl]	���
* 
***********************************************************************/
static ER ParamDataCheck( UB msg , UB anum ,  const char** args )
{
	INT ParDat;
	INT Loop;
	INT argnum;

	// �p�����[�^�ߕs���`�F�b�N
	if( anum < DATCHK_TBL[msg].argnum )		return( E_PAR_SHORTG );		// �p�����[�^���ߕs��

	// �p�����[�^�e�[�u����`���̐��l�`�F�b�N
	argnum = DATCHK_TBL[msg].argnum;

	// �R�}���h�ȊO�̃p�����[�^�`�F�b�N
	if( argnum > 1 )
	{
		// anum�́A�R�}���h�����܂܂ꂽ�p�����[�^���Ȃ̂ŁA�R�}���h�ȊO�̃p�����[�^�̏㉺���ɂ��ă`�F�b�N
		for( Loop = PARAM1; Loop < argnum; Loop++ )
		{
			if( 
				( DATCHK_TBL[msg].range[Loop].min != CLR )||
				( DATCHK_TBL[msg].range[Loop].max != CLR )
			)
			{
				// �p�����[�^�͈̓`�F�b�N
				ParDat = Atoi( args[Loop], N_DEC, sizeof(INT), N_SIG );
				if( 
					( ParDat < DATCHK_TBL[msg].range[Loop].min )||
					( ParDat > DATCHK_TBL[msg].range[Loop].max )
				)
				{
					return( ( E_PAR_OUTRNG + Loop ));					// �p�����[�^�ُ͈͈�
				}
			}
		}
	}

	return( E_SUCCESS );
}



//
//
//
//
//
//
//
//
/*=============================================================================================================================================================================================================================*/
/*  ��{�R�}���h   ��������                                                                                                                                                                                     ��{�R�}���h   */
/*=============================================================================================================================================================================================================================*/

/***********************************************************************
* 
* [�T�v]	�_�~�[���[�`��
* [����]	cmd:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
static void CmdDummy( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// �K�v�ł���΃G���[�o��
}


static	UB temp1;
static	UB temp2;

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdTmpSet  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	temp1 = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
	
	mbx_send	(
					ID_TSK_CMD,       // tskid
					ID_MPF_TMPCNT,       // mpfid
					ID_MBX_TMPCNT,       // mbxid
					NO_DATA,             // prid1
					NO_DATA,             // bcid
					ID_MSG_TMPSET_HS,    // msgid
					NO_DATA,             // sts
					NO_DATA,             // sts2/inf2
					(UB *)&temp1,        // data
					sizeof( temp1 )      // siz
				);
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdTmpGet  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	UB dummy = 0;
	
	mbx_send	(
					ID_TSK_CMD,       // tskid
					ID_MPF_TMPCNT,       // mpfid
					ID_MBX_TMPCNT,       // mbxid
					NO_DATA,             // prid1
					NO_DATA,             // bcid
					ID_MSG_TMPGET_HS,    // msgid
					NO_DATA,             // sts/inf
					NO_DATA,             // sts2/inf2
					(UB *)&dummy,        // data
					sizeof( dummy )      // siz
				);
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdVini   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_VALVEINI_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&dumy,         // data
						sizeof( dumy )       // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPini   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_PRESSINI_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&dumy,         // data
						sizeof( dumy )       // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPren   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_PRESSREN_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_PRESSREN_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&dumy,         // data
						sizeof( dumy )       // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPtan   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_PRESSTAN_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_PRESSTAN_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&dumy,         // data
						sizeof( dumy )       // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPcan   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_PRESSCAN_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_PRESSCAN_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&dumy,         // data
						sizeof( dumy )       // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdVout  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		memset( &vout, CLR, sizeof( ST_VOUT ) );
		vout.target = Atoi( args[PARAM1], N_HEX, sizeof(unsigned short), N_UNSIG );
		vout.valout = Atoi( args[PARAM2], N_HEX, sizeof(unsigned short), N_UNSIG );
		
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_VALVEOUT_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						(UB *)&vout.target,  // data
						sizeof( ST_VOUT )    // siz
					);
	}
}
/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPsns  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	dummy  = CLR;
	
	result = ParamDataCheck( ID_MSG_PRESSURE_HS, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		mbx_send	(
						ID_TSK_CMD,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						NO_DATA,             // prid1
						NO_DATA,             // bcid
						ID_MSG_PRESSURE_HS,  // msgid
						NO_DATA,             // sts
						NO_DATA,             // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
	}
}

/***********************************************************************
*
* [�T�v]  HELLO   �N���ʒm
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdHELLO  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
 	// �������b�Z�[�W�쐬
	CmdMakeResponseStr(		&stCtl,
							PARAM4,
							STRTBL_MSG[ID_MSG_HELLO],
							MnetStatus,
							(char*)&FIRMWARE,
							(char*)&VER_INFO
						);

	// ���X�|���X�f�[�^���M
	SendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  VER     �o�[�W�����m�F
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdVER    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// �������b�Z�[�W�쐬
	CmdMakeResponseStr(		&stCtl,
							PARAM4, 
							args[0], 
							MnetStatus,
							(char*)&FIRMWARE, 
							(char*)&VER_INFO
						);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  ALLIN   �SI/O����
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdALLIN  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	allsts[INDAT_CNTRL_MAX];
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  ������
	memset( &allsts, CLR,  sizeof( allsts ) );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �S���̓|�[�g��Ԏ擾
		pio_ALLIN( &allsts[0] );
	}
	// �������b�Z�[�W�쐬
	CmdMakeResponseX(	&stCtl,
						PARAM6,
						args[0],
						result,

						(allsts[7]*0x100 + allsts[6]),
						(allsts[5]*0x100 + allsts[4]),
						(allsts[3]*0x100 + allsts[2]),
						(allsts[1]*0x100 + allsts[0]),
						0,
						0,
						0
					);

	// �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  BIN     I/O����
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdBIN    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	sts  = CLR;
	INT	par  = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		par = Atoi( args[PARAM1], N_DEC, sizeof(INT), N_SIG );
	
		// ���͏�Ԏ擾
	    sts = pio_BIN( par );	
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM4, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						par, 											//3  ���͂���
						sts, 											//4  ���͏�Ԏ擾
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);


    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  BOUT    I/O�o��
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdBOUT   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	INT	ch  = CLR;
	INT	act = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		ch  = Atoi( args[PARAM1], N_DEC, sizeof(INT), N_SIG );
		act = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );

		// I/O�o��
		pio_BOUT( ch, act );
	}
	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM6, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						ch, 											//3  ���͂���
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

	
    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  CINIT   ���̓J�E���^�̐ݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdCINIT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	nn  = CLR;
	UB	f   = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		nn  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		f   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );

		// �J�E���g�J�n
		if( f )
		{
			mbx_send( ID_TSK_CMD, 
				      ID_MPF_IO, 
				      ID_MBX_IO, 
				      NO_DATA, 
				      NO_DATA, 
				      ID_MSG_CNT_START, 
				      (UB)nn, 
				      0, 
				      &bufwk[0], 
				      0 );
		}
		// �J�E���g��~
		else
		{
			mbx_send( ID_TSK_CMD, 
				      ID_MPF_IO, 
				      ID_MBX_IO, 
				      NO_DATA, 
				      NO_DATA, 
				      ID_MSG_CNT_STOP, 
				      (UB)nn, 
				      0, 
				      &bufwk[0], 
				      0 );
		}
	}
	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						nn, 											//3  ���͂���
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}
/***********************************************************************
*
* [�T�v]  COUNT   ���̓J�E���^�̓ǂݏo��
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdCOUNT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	nn  = CLR;
	UINT	cccc = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		nn  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		
		// �J�E���g�擾
		cccc = GetCount(nn);
	}
	// �������b�Z�[�W�쐬
	CmdMakeResponseCount( 	&stCtl,
							PARAM4, 									//0  �p�����[�^��
							args[0], 									//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							nn, 										//3  ���͂���
							cccc 										//4  �J�E���g�l
						);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  INT     ���荞�ݐݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdINT    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	ST_INTINF			Intinf;							// 
	UB  msg;

	msg = mrcv->msgdata.msgid;

	// ������
	memset( &bufwk,  CLR, sizeof( bufwk ) );
	memset( &Intinf, CLR, sizeof( ST_INTINF ) );

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );
	
	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		Intinf.nn   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		Intinf.trg  = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		
		// �ꎞ��~���郂�[�^�w��
		SetByteArray( args[PARAM3],&Intinf.mmm[0]);
		
		// �o�̓`���l���֏o�͂���l
		SetByteArray( args[PARAM4],&Intinf.dat[0]);

		// ���荞�ݔ������ɏo�͂���`���l���w��
		SetByteArray( args[PARAM5],&Intinf.msk[0]);

		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
		memcpy( &bufwk[0], (B*)&Intinf, sizeof( ST_INTINF ) );

		// �҂��^�X�N�֒ʒm
		result = mbx_send( ID_TSK_CMD,  
			               ID_MPF_INT, 
			               ID_MBX_INT, 
			               NO_DATA, 
			               NO_DATA, 
			               msg, 
			               Intinf.nn, 
			               DEFDAT, 
			               &bufwk[0], 
			               CMD_BUFSIZ );
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						Intinf.nn,										//3  ���͂���
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  ABSMV   �p���X���[�^�ړ�(��Έʒu)
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdABSMV  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_ABSMV absmv;
	UB  msg;

	msg = mrcv->msgdata.msgid;
	
	// ������
	memset( &bufwk, 0, sizeof( bufwk ) );
	memset( &absmv,  0,  sizeof( ST_ABSMV ) );

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		absmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		absmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		absmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, absmv.mm );

		if( result == E_SUCCESS )
		{
			result = AbsmvAction( NO_DATA, NO_DATA, absmv.mm, absmv.st, absmv.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  �p�����[�^��
							args[0], 									//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							(INT)absmv.mm,								//3  ���[�^�P�ԍ�
							0, 											//4  �p���X�J�E���g�P
							0,											//5  �G���R�[�_�P
							0, 											//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							0, 											//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  ABORT   �p���X���[�^����~
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdABORT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_STOP abort;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		abort.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
		memcpy( &bufwk[0], (B*)&abort, sizeof( ST_STOP ) );

		// ���ɗ㎥ON�̏ꍇ�́A���s�^�X�N��
		if( power[abort.mm].sw == SET)
		{
			result = mbx_send	(
									ID_TSK_CMD,
									ID_MPF_MOTACT,
									ID_MBX_MOTACT,
									NO_DATA,
									NO_DATA,
									ID_MSG_ABORT,
									abort.mm,
									DEFDAT,
									&bufwk[0],
									CMD_BUFSIZ
								);
		}
		else
		{
			result = mbx_send	(
									ID_TSK_CMD,
									ID_MPF_MOTWAIT,
									ID_MBX_MOTWAIT,
									NO_DATA,
									NO_DATA,
									ID_MSG_ABORT,
									abort.mm,
									DEFDAT,
									&bufwk[0],
									CMD_BUFSIZ
								);
		}
		ClrDevice( abort.mm );
	}
	else
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 										//0  �p�����[�^��
							args[0], 										//1  �R�}���h������
							result,											//2  �G���[�R�[�h

							0,												//3  ���[�^�P�ԍ�
							0, 												//4  �p���X�J�E���g�P
							0,												//5  �G���R�[�_�P
							0, 												//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							0, 												//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 												//8  
							0, 												//9  
							0,												//10 
							0, 												//11 
							0 												//12 
						);

		// �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  ENCODER �G���R�[�_�̐ݒ�
* [����]  ���b�Z�[�W�A�����̐��A����ޓd��
* [�ߒl]  �G���[
* 
***********************************************************************/
static void CmdENCODER( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	mm   = CLR;
	UB	ec   = CLR;
	UB	dtyp = CLR;
	UB	dv   = CLR;
	INT	so   = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		ec   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		dtyp = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );
		dv   = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );
		so   = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );
		
		result = motor_ENCODER( mm, ec, dtyp, dv, so );
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						mm, 											//3  ���[�^�ԍ�
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  LIMIT   �p���X���[�^���_���A
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdLIMIT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER       result = E_SUCCESS;
	ST_LIMIT limit;
	UB       msg;

	msg = mrcv->msgdata.msgid;

	// ������
	memset( &bufwk, 0, sizeof( bufwk ) );
	memset( &limit,  0,  sizeof( ST_LIMIT ) );

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		limit.mm  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		limit.dr = GetCwInfo( args[PARAM2 ] );						// �ړ�����		CW : ���]����, CCW : �t�]����
		limit.sp = Atoi( args[PARAM3], N_DEC, sizeof(INT), N_SIG );
		limit.md = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, limit.mm );

		if( result == E_SUCCESS )
		{
			// �f�o�C�X���쒆�ɃZ�b�g
			result = LimitAction(	
							NO_DATA,
							NO_DATA,
							limit.mm,
							limit.dr, 
							GENTEN_POS,
							limit.sp
						);												// ���~�b�g�ʒu����
		}
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM3, 									//0  �p�����[�^��
							args[0], 									//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							(INT)limit.mm,								//3  ���[�^�P�ԍ�
							0, 											//4  �p���X�J�E���g�P
							0,											//5  �G���R�[�_�P
							0, 											//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							0, 											//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}
/***********************************************************************
*
* [�T�v]  MAP     �Q���̃p���X���[�^�ړ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdMAP    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result  = E_SUCCESS;
	UB  msg;
	ST_ABSMV absmv;
	ST_ABSMV absmv2;
	ST_MOTOR_STATUS		sts1;										// 
	ST_MOTOR_STATUS		sts2;										// 

	msg = mrcv->msgdata.msgid;
	
	// ������
	memset( &bufwk,  CLR, sizeof( bufwk ) );
	memset( &sts1,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &sts2,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &absmv,  CLR, sizeof( ST_ABSMV ) );
	memset( &absmv2, CLR, sizeof( ST_ABSMV ) );
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		absmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		absmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		absmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// �p�����[�^�擾
		absmv2.mm   = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );
		absmv2.pppp = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );
		absmv2.st   = Atoi( args[PARAM6], N_DEC, sizeof(UB), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, absmv.mm );

		if( result == E_SUCCESS )
		{
			result = MapAction( NO_DATA, NO_DATA, absmv.mm, absmv2.mm, absmv.pppp, absmv2.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM12, 									//0  �p�����[�^��
							args[0], 									//1  �R�}���h������
							result, 									//2  �G���[�R�[�h

							absmv.mm,									//3  ���[�^�P�ԍ�
							sts1.pos, 									//4  �p���X�J�E���g
							sts1.enc,									//5  �G���R�[�_
							sts1.cw_limit, 								//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							sts1.ccw_limit, 							//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							absmv2.mm,									//8  ���[�^�Q�ԍ�
							sts2.pos, 									//9  �p���X�J�E���g
							sts2.enc,									//10 �G���R�[�_
							sts2.cw_limit, 								//11 CW���~�b�g�Z���T(0:OFF,1:ON)
							sts2.ccw_limit 								//12 CCW���~�b�g�Z���T(0:OFF,1:ON)
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPDELAY ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_PDELAY	pdelay;
	UB msg;

	msg = mrcv->msgdata.msgid;

	// ������
	memset( &bufwk, 0, sizeof( bufwk ) );
	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		pdelay.mm    = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		pdelay.tttt  = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, pdelay.mm );

		if( result == E_SUCCESS )
		{
			// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
			memcpy( &bufwk[0], (B*)&pdelay, sizeof( ST_PDELAY ) );

			// �҂��^�X�N�֒ʒm
			result = mbx_send( ID_TSK_CMD,  
				               ID_MPF_MOTWAIT, 
				               ID_MBX_MOTWAIT, 
				               NO_DATA, 
				               NO_DATA, 
				               msg, 
				               pdelay.mm, 
				               DEFDAT, 
				               &bufwk[0], 
				               CMD_BUFSIZ);

			result = mbx_send( ID_TSK_CMD,  
				               ID_MPF_MOTEND,  
				               ID_MBX_MOTEND,  
				               NO_DATA, 
				               NO_DATA, 
				               msg, 
				               pdelay.mm, 
				               DEFDAT, 
				               &bufwk[0], 
				               CMD_BUFSIZ);
			
		}
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						(INT)pdelay.mm,									//3  ���[�^�P�ԍ�
						0, 												//4  �p���X�J�E���g�P
						0,												//5  �G���R�[�_�P
						0, 												//6  CW���~�b�g�Z���T(0:OFF,1:ON)
						0, 												//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  POWER   �p���X���[�^�̗㎥����
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPOWER  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	UB mm     = CLR;
	UB sw;
	UB msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		sw   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// �㎥
			result = motor_POWER( mm, sw );

			// POWER��ԓo�^
			power[mm].mm = mm;
			power[mm].sw = sw;
		}
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						(INT)mm, 										//3  ���[�^�P�ԍ�
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [�T�v]  PSET    �p���X���[�^�̃J�E���^���Z�b�g
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdPSET   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB		mm     = CLR;		// 
	INT		cnt;				// 
	INT		enc    = CLR;		// 
	INT		def    = CLR;		// 
	UB  msg;

	msg = mrcv->msgdata.msgid;

	
	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		cnt  = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		enc  = Atoi( args[PARAM3], N_DEC, sizeof(INT), N_SIG );
		def  = Atoi( args[PARAM4], N_DEC, sizeof(INT), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// �p���X���[�^�̃J�E���^���Z�b�g
			result = motor_PSET( mm, cnt, enc, def );
		}
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM4, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						(INT)mm, 										//3  ���[�^�P�ԍ�
						0,  											//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);
	
    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}
/***********************************************************************
*
* [�T�v]  RELMV   �p���X���[�^�ړ�(���Έʒu)
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdRELMV  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ST_RELMV relmv;
	ER result = E_SUCCESS;
	UB msg;

	msg = mrcv->msgdata.msgid;
	
	// ������
	memset( &bufwk,CLR, sizeof( bufwk ) );
	memset( &relmv,CLR, sizeof( ST_RELMV ) );

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		relmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		relmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		relmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, relmv.mm );

		if( result == E_SUCCESS )
		{
			result = RelmvAction( NO_DATA, NO_DATA, relmv.mm, relmv.st, relmv.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  �p�����[�^��
							args[0], 									//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							(INT)relmv.mm,								//3  ���[�^�P�ԍ�
							0, 											//4  �p���X�J�E���g
							0,											//5  �G���R�[�_
							0, 											//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							0, 											//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  SET     �p���X���[�^���x�ݒ�
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdSET    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB		mm     = CLR;		// ���[�^�ԍ�
    ST_MOTOR_PARAM		mp;		// 
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾   sample: SET,1,1,3,500,7500,500,500
		mm            = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		mp.accel_type = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );	// �쓮�^�C�v(�O�F��`�A�P�F�r��)
		// �f�o�C�X:PCD4611
		if( MNET_DRV_TYPE[mm] == PCD4611 )
		{
			mp.factor     = GetFactorPCD4611( Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG ) );    // ���[�^�{��
		}
		else
		{
			mp.factor     = GetFactor( Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG ) );    // ���[�^�{��
		}
		mp.FL         = Atoi( args[PARAM4], N_DEC, sizeof(INT), N_SIG );// ����[pls](0�`100,000)
		mp.FH         = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );// �ō���[pls](0�`100,000)
		mp.utime      = Atoi( args[PARAM6], N_DEC, sizeof(INT), N_SIG );// ��������[ms](�P�`65535)
		mp.dtime      = mp.utime;                                       // ��������[ms](�P�`65535)
		mp.us         = Atoi( args[PARAM7], N_DEC, sizeof(INT), N_SIG );// �������r���͈�
		mp.ds         = mp.us;                                          // �������r���͈�

		// �f�o�C�X�����Ԋm�F
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// �ݒ�
			motor_SET( mm, mp );
		}
	}

	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						mm, 											//3  ���[�^�P�ԍ�
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );

}

/***********************************************************************
*
* [�T�v]  STATUS  �p���X���[�^��ԓǂݏo��
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdSTATUS ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;	// �G���[���
	UB		mm     = CLR;		// ���[�^�ԍ�
	ST_MOTOR_STATUS sts;		// ���[�^���
	UB  msg;

	msg = mrcv->msgdata.msgid;

	// ������
	memset( &sts,  CLR,  sizeof( ST_MOTOR_STATUS ) );


	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{

		// �p�����[�^�擾
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// ��Ԏ擾
	    result  = motor_STATUS( mm, &sts );
	}

	// �������b�Z�[�W�쐬
	CmdMakeResponse( 	&stCtl,
						PARAM9, 										//0  �p�����[�^��
						args[0], 										//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						mm,	 											//3  ���[�^�P�ԍ�
						(INT)sts.busy, 									//4  ������
						sts.pos,										//5  �p���X�J�E���^�l
						sts.enc, 										//6  �G���R�[�_�l
						sts.dev, 										//7  �΍��J�E���^�l

						sts.cw_limit, 									//8  CW���~�b�g�Z���T�l(0:OFF,1:ON)
						sts.ccw_limit, 									//9  CCW���~�b�g�Z���T�l(0:OFF,1:ON)
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );
}


/***********************************************************************
*
* [�T�v]  STOP    �p���X���[�^������~
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdSTOP   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_MOTOR_STATUS sts;
	ST_STOP stop;
	UB msg;

	msg = mrcv->msgdata.msgid;

	// ������
	memset( &sts,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &bufwk, CLR, sizeof( bufwk ) );
	memset( &stop,  CLR, sizeof( ST_STOP ) );

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		stop.mm  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
		memcpy( &bufwk[0], (B*)&stop, sizeof( ST_STOP ) );

		// ���ɗ㎥ON�̏ꍇ�́A���s�^�X�N��
		if( power[stop.mm].sw == SET)
		{
			result = mbx_send	(
									ID_TSK_CMD,
									ID_MPF_MOTACT,
									ID_MBX_MOTACT,
									NO_DATA,
									NO_DATA,
									ID_MSG_STOP,
									stop.mm,
									DEFDAT,
									&bufwk[0],
									CMD_BUFSIZ
								);
		}
		else
		{
			result = mbx_send	(
									ID_TSK_CMD,
									ID_MPF_MOTWAIT,
									ID_MBX_MOTWAIT,
									NO_DATA,
									NO_DATA,
									ID_MSG_STOP,
									stop.mm,
									DEFDAT,
									&bufwk[0],
									CMD_BUFSIZ
								);
		}
	}

	if( result != E_SUCCESS )
	{
	    motor_STATUS( stop.mm, &sts );

		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_STOP], 					//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							(INT)stop.mm,								//3  ���[�^�P�ԍ�
							sts.pos, 									//4  �p���X�J�E���g�P
							sts.pos,									//5  �G���R�[�_�P
							sts.cw_limit, 								//6  CW���~�b�g�Z���T(0:OFF,1:ON)
							sts.ccw_limit, 								//7  CCW���~�b�g�Z���T(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );

		ClrDevice( stop.mm );
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdBZCNT( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER  result = E_SUCCESS;

	ST_BZCNT bzcnt;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  �f�[�^�`�F�b�N
	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		bzcnt.Act    = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		bzcnt.Colptn = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		bzcnt.Buzptn = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
		memcpy( &bufwk[0], (B*)&bzcnt, sizeof( ST_BZCNT ) );

		// �^�X�N�ʒm
		mbx_send(
						ID_TSK_CMD,										// ���M�����ID
						ID_MPF_LED,										// ����߰�ID
						ID_MBX_LED,										// Ұ��ޯ��ID
						ID_TPL_PTLMP,									// PROCID
						mrcv->msgdata.bcid,								// BCID
						ID_MSG_CBZCNT,									// message
						NO_DATA,										// ���1(�ėp)
						NO_DATA,										// ���2(�ėp)
						&bufwk[0],										// �ް�
						sizeof( bufwk )									// �ް�����
					);
		
	}

	CmdMakeResponse( 	&stCtl,
						PARAM2, 										//0  �p�����[�^��
						STRTBL_MSG[ID_MSG_CBZCNT], 						//1  �R�}���h������
						result,											//2  �G���[�R�[�h

						0, 												//3  
						0, 												//4  
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // �������b�Z�[�W���M
	CmdSendResponse( &stCtl );

}

/***********************************************************************
*
* [�T�v]  EEPROM �w��y�[�W�̂P�y�[�W(128byte)�f�[�^��ǂݍ���
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdEEPREAD( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB  msg;
	msg = mrcv->msgdata.msgid;

	// ������
	memset( &eepb,  0xff, sizeof( ST_EEPBUF ) );

	//  �f�[�^�`�F�b�N
//	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
//	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		eepb.no = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		eepb.siz = EEP_RWSIZE;
		
		// �����݃f�[�^�z��
		memcpy( &bufwk[0], (B*)&eepb, sizeof( ST_EEPBUF ) );

		result = mbx_send	(
								ID_TSK_CMD,// tskid      ---->   msg->msgdata.tskid
								ID_MPF_EEP,// memory     ---->   -
								ID_MBX_EEP,// mailbox    ---->   -
								NO_DATA,   // procid     ---->   msg->msgdata.procid
								NO_DATA,   // bcid       ---->   msg->msgdata.bcid
								msg,       // msg        ---->   msg->msgdata.msgid
								eepb.no,   // sts        ---->   msg->msgdata.inf
								DEFDAT,    // sts2       ---->   msg->msgdata.inf2
								&bufwk[0], // data       ---->   msg->msgdata.data[]
								CMD_BUFSIZ // data siz   ---->   msg->msgdata.siz
							);
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_EEPREAD], 				//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							0,											//3  
							0, 											//4  
							0,											//5  
							0, 											//6  
							0, 											//7  

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  EEPROM �w��y�[�W�ɂP�y�[�W(128byte)�f�[�^����������
* [����]  ����ޓd�������ް�
* [�ߒl]  -
* 
***********************************************************************/
static void CmdEEPWRIT( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB  msg;
	msg = mrcv->msgdata.msgid;

	// ������
	memset( &eepb,  0xff, sizeof( ST_EEPBUF ) );

	//  �f�[�^�`�F�b�N
//	result = ParamDataCheck( msg, anum, args );

	//  �f�[�^���킩
//	if( result == E_SUCCESS )
	{
		// �p�����[�^�擾
		eepb.no = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		eepb.siz = EEP_RWSIZE;
		
		// �����݃f�[�^�z��
		SetByteArray( args[PARAM2],&eepb.buf[0]);
		memcpy( &bufwk[0], (B*)&eepb, sizeof( ST_EEPBUF ) );

		result = mbx_send	(
								ID_TSK_CMD,// tskid
								ID_MPF_EEP,// memory
								ID_MBX_EEP,// mailbox
								NO_DATA,   // procid
								NO_DATA,   // bcid
								msg,       // msg
								eepb.no,   // sts
								DEFDAT,    // sts2
								&bufwk[0], // data
								CMD_BUFSIZ // data siz
							);
	}

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_EEPWRIT], 				//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							0,											//3  
							0, 											//4  
							0,											//5  
							0, 											//6  
							0, 											//7  

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdImpPdat( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER  result = E_SUCCESS;
	UB  msg;
	UB  pno;
	UB bcc;
	UB bcch;
	UB bccl;
	INT  len;
	unsigned char *pnt;

	msg = mrcv->msgdata.msgid;

	// �^�b�`�p�l���ւ��̂܂܃f�[�^�ʒm
	TplRpDataSend();
	TplRidDataSend ( &tp.rsp.buf[0] , ID_TPL_PRINF );

	// EEP�֏����ݎw��
	pno = Get_pno();
	
	if(( pno > 0 )&&( pno <= PROTMAX ))
	{
		result = mbx_send	(
								ID_TSK_CMD,// tskid
								ID_MPF_EEP,// memory
								ID_MBX_EEP,// mailbox
								NO_DATA,   // procid
								NO_DATA,   // bcid
								msg,       // msg
								DEFDAT,    // sts
								pno,       // sts2
								&bufwk[0], // data
								CMD_BUFSIZ // data siz
							);
	}
	else
	{
		result = E_PAR_OUTRNG;
	}
	
	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_IMPPDAT], 				//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							0,											//3  
							0, 											//4  
							0,											//5  
							0, 											//6  
							0, 											//7  

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdImpSped( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER  result = E_SUCCESS;
	UB  dummy;
	UB  msg;
	
	msg = mrcv->msgdata.msgid;
	
	result = mbx_send	(
							ID_TSK_CMD,// tskid
							ID_MPF_EEP,// memory
							ID_MBX_EEP,// mailbox
							NO_DATA,   // procid
							NO_DATA,   // bcid
							msg,       // msg
							DEFDAT,    // sts
							DEFDAT,    // sts2
							&dummy,    // data
							sizeof(dummy) // data siz
						);

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_IMPSPED], 				//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							0,											//3  
							0, 											//4  
							0,											//5  
							0, 											//6  
							0, 											//7  

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}


/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdImpPosi( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER  result = E_SUCCESS;
	UB  dummy;
	UB  msg;
	
	msg = mrcv->msgdata.msgid;
	
	result = mbx_send	(
							ID_TSK_CMD,// tskid
							ID_MPF_EEP,// memory
							ID_MBX_EEP,// mailbox
							NO_DATA,   // procid
							NO_DATA,   // bcid
							msg,       // msg
							DEFDAT,    // sts
							DEFDAT,    // sts2
							&dummy,    // data
							sizeof(dummy) // data siz
						);

	if( result != E_SUCCESS )
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  �p�����[�^��
							STRTBL_MSG[ID_MSG_IMPPOSI], 				//1  �R�}���h������
							result,										//2  �G���[�R�[�h

							0,											//3  
							0, 											//4  
							0,											//5  
							0, 											//6  
							0, 											//7  

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // �������b�Z�[�W���M
		CmdSendResponse( &stCtl );
	}
}


/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdImpOptn( T_CMD_MSG* mrcv, UB anum, const char** args )
{
}


/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdExpRunl( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ExpRunlSeqno = 1;
	ExpRunlRetry = 0;
	TplExRunSend();
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_EXRUN );
}


/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdExpComl( T_CMD_MSG* mrcv, UB anum, const char** args )
{
}


/***********************************************************************
*
* [�T�v]  
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void CmdExpClog( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// �^�b�`�p�l���ւ��̂܂܃f�[�^�ʒm
	TplRpDataSend();
	TplRidDataSend ( &tp.rsp.buf[0] , ID_TPL_PRINF );
	
	
	
	
	
}






/*=============================================================================================================================================================================================================================*/
/*  ��{�R�}���h   �����܂�                                                                                                                                                                                     ��{�R�}���h   */
/*=============================================================================================================================================================================================================================*/
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
/***********************************************************************
*
* [�T�v]  �f�o�C�X���쒆�Z�b�g
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void SetDevice( UB DevNo )
{
	// ���o�^�E�K��O
	if( DevNo == NO_DATA )	return;
	if( DevNo > DVMAX )		return;

	
	return;//debug
	
	BitSet_INT( DevNo, &DeviceStatus[0] );
}

/***********************************************************************
*
* [�T�v]  �f�o�C�X�����Ԏ擾
* [����]  
* [�ߒl]  TRUE:���쒆�AFALSE:�ҋ@��
* 
***********************************************************************/
static BOOL GetDeviceStatus( UB DevNo )
{
	// ���o�^��K��O�͓��쒆�����Ƃ���
	if( DevNo == NO_DATA )	return( TRUE );
	if( DevNo > DVMAX )		return( TRUE );

	return(( BitGet_INT( DevNo, &DeviceStatus[0] ) > 0 ? TRUE : FALSE ));
}

/***********************************************************************
*
* [�T�v]  �f�o�C�X���쒆�N���A
* [����]  
* [�ߒl]  -
* 
***********************************************************************/
static void ClrDevice( UB DevNo )
{
	if( DevNo == NO_DATA )	return;
	if( DevNo > DVMAX )		return;

	BitClr_INT( DevNo, &DeviceStatus[0] );
}

/******************************************************************************
* 
* [�T�v]	�p���X���[�^��㎥
* [����]	���[�^�[�ԍ��A�㎥����w��
* [�ߒl]	TRUE:�����AFALSE:���s
* 
* 
***********************************************************************/
static ER GetDeviceActionChk ( UB msg, UB bcid, UB mm )
{
	ER result = E_SUCCESS;
#if 0	// ��
	INT AsnDevice[2];
	INT CmpDevice[2];
	INT TargetDevice;
	UB devnowk;

	// **************************************************************
	// ���݁A���쒆�̃f�o�C�X�Ƃ��Ԃ�Ȃ����m�F
	// ����{�R�}���h��bcctrl�ɓo�^����Ă��Ȃ��̂œ��쒆�t���O�Ŋm�F
	// **************************************************************
	memset( &CmpDevice,  CLR,  sizeof( CmpDevice ) );
	GetBCDevice( msg,    mm,     &CmpDevice[0] );
	
	TargetDevice = ( DeviceStatus[0] & CmpDevice[0] );
	if( TargetDevice )
	{
		devnowk = GetFirstCh( &TargetDevice );
		return( (E_BUSY + devnowk) );
	}
	
	TargetDevice = ( DeviceStatus[1] & CmpDevice[1] );
	if( TargetDevice )
	{
		devnowk = GetFirstCh( &TargetDevice );
		return( (E_BUSY + devnowk + 32) );
	}
#endif
	return( result );
}

/******************************************************************************
* 
* [�T�v]	�p���X���[�^��㎥
* [����]	���[�^�[�ԍ��A�㎥����w��
* [�ߒl]	TRUE:�����AFALSE:���s
* 
******************************************************************************/
BOOL GetPowerSts(UB pm)
{
	BOOL result;// port��� H = TRUE�AL = FALSE

	if( power[pm].mm == pm )
	{
		if( power[pm].sw )
		{
			result = TRUE;
		}
		else
		{
			result = FALSE;
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
* [�T�v]	���~�b�g����
* [����]	id   : ��ۯ������ID
*           mm   : ���޲�NO
*           dr   : ����
*           md   : ����Ӱ��
*           sp   : ���x
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
static ER LimitAction( UB prid1, UB bcid, UB mm, UB dr, UB md, INT sp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_LIMIT lmt;

	// LIMIT�f�[�^�R�s�[
	lmt.mm = mm;
	lmt.dr = dr;
	lmt.md = md;
	lmt.sp = sp;
	
	memcpy( &bufwk[0], &lmt, sizeof( ST_LIMIT )); 
	
	// �㎥�n�m���ł���Ύ��s�^�X�N�֒ʒm
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// �㎥�n�e�e�ł���΃E�F�C�g�^�X�N�֒ʒm
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// ���b�Z�[�W�ʒm
	result = mbx_send(
						ID_TSK_CMD,
						mpfid,
						mbxid,
						prid1,   
						bcid,
						ID_MSG_LIMIT,
						mm,
						NO_DATA,
						&bufwk[0],
						sizeof( ST_LIMIT )
					);

	if( result == E_SUCCESS )
	{
		SetDevice( mm );
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	���Έʒu�ړ�����
* [����]	id   : ��ۯ������ID
*           mm   : ���޲�NO
*           pppp : mm�ړ���
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
static ER RelmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_RELMV rel;

	// ��{�R�}���h�̓f�o�C�X��Ԃ��m�F
	if( bcid == NO_DATA )
	{
		if( GetDeviceStatus(mm) ==TRUE )
		{
			return( (E_BUSY + mm) );
		}
	}

	// RELMV�f�[�^�R�s�[
	rel.mm = mm;
	rel.st = 0;
	rel.pppp = pppp;
	
	memcpy( &bufwk[0], &rel, sizeof( ST_RELMV )); 
	
	// �㎥�n�m���ł���Ύ��s�^�X�N�֒ʒm
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// �㎥�n�e�e�ł���΃E�F�C�g�^�X�N�֒ʒm
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// ���b�Z�[�W�ʒm
	result = mbx_send(
						ID_TSK_CMD,                                 // tskid
						mpfid,                                      // mpfid
						mbxid,                                      // mbxid
						prid1,                                      // prid1
						bcid,                                       // bcid
						ID_MSG_RELMV,                               // msgid
						mm,                                         // sts
						NO_DATA,                                    // sts2
						&bufwk[0],                                  // data
						sizeof( ST_RELMV )                          // siz
					);

	if( result == E_SUCCESS )
	{
		SetDevice( mm );
	}

	return( result );
}
/******************************************************************************
* 
* [�T�v]	��Έʒu�ړ�����
* [����]	id   : ��ۯ������ID
*           mm   : ���޲�NO
*           pppp : mm�ړ���
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
static ER AbsmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_ABSMV abs;

	// ��{�R�}���h�̓f�o�C�X��Ԃ��m�F
	if( bcid == NO_DATA )
	{
		if( GetDeviceStatus(mm) ==TRUE )
		{
			return( (E_BUSY + mm) );
		}
	}

	// ABSMV�f�[�^�R�s�[
	abs.mm = mm;
	abs.st = st;
	abs.pppp = pppp;
	
	memcpy( &bufwk[0], &abs, sizeof( ST_ABSMV )); 
	
	// �㎥�n�m���ł���Ύ��s�^�X�N�֒ʒm
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// �㎥�n�e�e�ł���΃E�F�C�g�^�X�N�֒ʒm
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// ���b�Z�[�W�ʒm
	result = mbx_send(												
						ID_TSK_CMD,                                 // tskid
						mpfid,                                      // mpfid
						mbxid,                                      // mbxid
						prid1,                                      // prid1
						bcid,                                       // bcid
						ID_MSG_ABSMV,                               // msgid
						mm,                                         // sts
						NO_DATA,                                    // sts2
						&bufwk[0],                                  // data
						sizeof( ST_ABSMV )                          // siz
					);

	if( result == E_SUCCESS )
	{
		SetDevice( mm );
	}
	return( result );
}
/******************************************************************************
* 
* [�T�v]	MAP�ړ�����
* [����]	id   : ��ۯ������ID
*           mm1  : ���޲�NO
*           mm2  : ���޲�NO
*           pppp1: mm1�ړ���
*           pppp2: mm2�ړ���
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
static ER MapAction( UB prid1, UB bcid, UB mm1, UB mm2, INT pppp1, INT pppp2 )
{
	ID mpfid;
	ID mbxid;
	ER result  = E_SUCCESS;
	ER result1 = E_SUCCESS;
	ER result2 = E_SUCCESS;
	ST_ABSMV absmv;
	ST_ABSMV absmv2;
	
	// ��{�R�}���h�̓f�o�C�X��Ԃ��m�F
	if( bcid == NO_DATA )
	{
		if( GetDeviceStatus(mm1) ==TRUE )
		{
			return( (E_BUSY + mm1) );
		}
		if( GetDeviceStatus(mm2) ==TRUE )
		{
			return( (E_BUSY + mm2) );
		}
	}

	// ������
	memset( &bufwk,  CLR, sizeof( bufwk ) );

    absmv.mm   = mm1; 
    absmv.st   = 0;
    absmv.pppp = pppp1;

	absmv2.mm   = mm2; 
    absmv2.st   = 0;
    absmv2.pppp = pppp2;

	// �㎥�n�m���ł���Ύ��s�^�X�N�֒ʒm
	if(
		(power[absmv.mm].sw == SET)&&
		(power[absmv2.mm].sw == SET)
	)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// �㎥�n�e�e�ł���΃E�F�C�g�^�X�N�֒ʒm
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// -------------------
	// �P�ڂ̃f�o�C�X
	// -------------------
	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &bufwk[0], (B*)&absmv, sizeof( ST_ABSMV ) );
	
	// ���b�Z�[�W�ʒm
	result1 = mbx_send(
						ID_TSK_CMD,                      // tskid
						mpfid,                           // mpfid
						mbxid,                           // mbxid
						prid1,                           // prid1
						bcid,                            // bcid
						ID_MSG_MAP,                      // msgid
						mm1,                             // sts
						mm2,                             // sts2
						&bufwk[0],                       // data
						sizeof( ST_ABSMV )               // siz
					);
	
	
	// -------------------
	// �Q�ڂ̃f�o�C�X
	// -------------------
	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &bufwk[0], (B*)&absmv2, sizeof( ST_ABSMV ) );

	// ���b�Z�[�W�ʒm
	result2 = mbx_send(
						ID_TSK_CMD,                      // tskid
						mpfid,                           // mpfid
						mbxid,                           // mbxid
						prid1,                           // prid1
						bcid,                            // bcid
						ID_MSG_MAP,                      // msgid
						mm2,                             // sts
						mm1,                             // sts2
						&bufwk[0],                       // data
						sizeof( ST_ABSMV )               // siz
					);
	
	// �G���[�`�F�b�N(�G���[���́A�u1�v���D��Ń��O�ɏo��)
	if( result2 != E_SUCCESS )		result = result2;
	if( result1 != E_SUCCESS )		result = result1;

	if( result == E_SUCCESS )
	{
		SetDevice( mm1 );
		SetDevice( mm2 );
	}
	return( result );
}

/******************************************************************************
* 
* [�T�v]	�v�`�h�s����
* [����]	id   : ��ۯ������ID
*           mm   : ���޲�NO
*           pppp : mm�ړ���
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
static ER WaitAction( UB prid1, UB bcid, INT waittim )
{
	ER result = E_SUCCESS;

	memcpy( &bufwk[0], &waittim, sizeof( INT )); 

	// ���b�Z�[�W�ʒm
	result = mbx_send(
						ID_TSK_CMD,				// tskid
						ID_MPF_MOTWAIT,			// mpfid
						ID_MBX_MOTWAIT,			// mbxid
						prid1,                  // prid1
						bcid,					// bcid
						ID_MSG_TIMWAIT,			// msgid
						NO_DATA,				// sts
						NO_DATA,				// sts2
						&bufwk[0],				// data
						sizeof( INT )		    // siz
					);

	return( result );
}
/***********************************************************************
*
* [�T�v]  �a�b�ŗ��p�����f�o�C�X���̎擾
* [����]  msg:���b�Z�[�W�h�c�Cmm:�f�o�C�X�ԍ��C*data:�i�[��
* [�ߒl]  �|
* 
***********************************************************************/
static void GetBCDevice( UB msg, UB mm, INT *data )
{
	if( mm != NO_DATA )
	{
		BitSet_INT( mm, &data[0] );
	}
}


//-------------------------------------------------------------------------------------------------------------------------
//        �^�b�`�p�l���ʐM
//-------------------------------------------------------------------------------------------------------------------------

/***********************************************************************
* 
* [�T�v]	�R�}���h�̉��
* [����]	data:�R�}���h���N�G�X�g�d��
* [�ߒl]	�|
* 
***********************************************************************/
void TpAnalyze( T_CMD_MSG* mrcv )
{
	UB bcid;
	UB anum;
	UB wkbcid;
	UB procid;

	wkbcid = 0;
	procid = mrcv->msgdata.prid1;
	
	if( IsQgProcList( procid ) == FALSE )				// �d���Ȃ�
	{
		mrcv->msgdata.bcid = GetQgProcList( procid ) ;	// �R�}���h���X�g��index�擾
	}
	else
	{
		mrcv->msgdata.bcid = NO_DATA;					// �d������
	}
	
	if( procid == ID_TPL_EXRUN )						// �^�]������M
	{
		
		
		
	}
	else
	{
		memset( &TpRecvData,    CLR,  sizeof( TpRecvData ) );
		memset( &TpRecvArgs,    CLR,  sizeof( TpRecvArgs ) );

		// �R�}���h�d���R�s�[
		memcpy( &TpRecvData[0],     &mrcv->msgdata.data[0],CMD_BUFSIZ );

		// �啶��
//	ToUpper( (char*)&TpRecvData );

		// ���� �� �p�����[�^���擾
		anum = SplitSpace( &TpRecvData[0], TpRecvArgs );
	}

	// �R�[���t�@���N�V����
	(*TpFuncTbl[procid])( mrcv, anum, TpRecvArgs  );
}


/***********************************************************************
* 	
* [�T�v]	
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDummy( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	








}

/***********************************************************************
* 	
* [�T�v]	01 FW�o�[�W�������
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplFwver( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );

	mrcv->msgdata.inf2 = (EEPROM_SPED_PTRN+EEPROM_POSI_INFO);

	// EEPROM �f�[�^�擾�w��
	mbx_send( ID_TSK_CMD,          // tskid
               ID_MPF_EEP,         // mpfid
               ID_MBX_EEP,         // mbxid
               mrcv->msgdata.prid1,// prid1
               mrcv->msgdata.bcid, // bcid
               ID_MSG_EEPREAD,     // msgid
               mrcv->msgdata.inf,  // sts
               mrcv->msgdata.inf2, // sts2
               &bufwk[0],          // data
               CMD_BUFSIZ          // siz
             );

}

/***********************************************************************
* 	
* [�T�v]	02 �I�v�V�������
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	

	// �Œ�l
	tp.rsp.buf[8] = '0';
***********************************************************************/
static void TplOptif( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );

	mrcv->msgdata.inf2 = CLR;

	// EEPROM �f�[�^�擾�w��
	mbx_send( ID_TSK_CMD,          // tskid
               ID_MPF_EEP,         // mpfid
               ID_MBX_EEP,         // mbxid
               mrcv->msgdata.prid1,// prid1
               mrcv->msgdata.bcid, // bcid
               ID_MSG_EEPREAD,     // msgid
               mrcv->msgdata.inf,  // sts
               mrcv->msgdata.inf2, // sts2
               &bufwk[0],          // data
               CMD_BUFSIZ          // siz
             );
}

/***********************************************************************
* 	
* [�T�v]	03 ���O�C���F��1
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplLgin1( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	ST_USERINFO Userwk;

	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );
	memset( &Userwk.Id[0], CLR,  sizeof(ST_USERINFO) );

	Strcpy( (char*)&Userwk.Id[0] , TpRecvArgs[1]  );
	Strcpy( (char*)&Userwk.Pw[0] , TpRecvArgs[2]  );

	memcpy( &bufwk[0], &Userwk.Id[0], sizeof(ST_USERINFO) );
	SetUserInfoSorce( (ST_USERINFO *) &Userwk.Id[0] );

	mrcv->msgdata.inf2 = CLR;

	// EEPROM �f�[�^�擾�w��
	mbx_send( ID_TSK_CMD,          // tskid
               ID_MPF_EEP,         // mpfid
               ID_MBX_EEP,         // mbxid
               mrcv->msgdata.prid1,// prid1
               mrcv->msgdata.bcid, // bcid
               ID_MSG_EEPREAD,     // msgid
               mrcv->msgdata.inf,  // sts
               mrcv->msgdata.inf2, // sts2
               &Userwk.Id[0],      // data
               sizeof(ST_USERINFO) // siz
             );
}

/***********************************************************************
* 	
* [�T�v]	04 ���O�C���F��2
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplLgin2( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	ST_USERINFO Userwk;

	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );
	memset( &Userwk.Id[0], CLR,  sizeof(ST_USERINFO) );

	Strcpy( (char*)&Userwk.Id[0] , TpRecvArgs[1]  );
	Strcpy( (char*)&Userwk.Pw[0] , TpRecvArgs[2]  );

	memcpy( &bufwk[0], &Userwk.Id[0], sizeof(ST_USERINFO) );

	mrcv->msgdata.inf2 = CLR;

	mbx_send( ID_TSK_CMD,          // tskid
               ID_MPF_EEP,         // mpfid
               ID_MBX_EEP,         // mbxid
               mrcv->msgdata.prid1,// prid1
               mrcv->msgdata.bcid, // bcid
               ID_MSG_EEPREAD,     // msgid
               mrcv->msgdata.inf,  // sts
               mrcv->msgdata.inf2, // sts2
               &bufwk[0],          // data
               CMD_BUFSIZ          // siz
             );
}

/***********************************************************************
* 	
* [�T�v]	05 �������w��
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInita( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	bcid;
	UB	inf;
	UB	axsis;

	memset( &bufwk[0], CLR,  CMD_BUFSIZ );
	memset( &axsis   , CLR,  sizeof(axsis) );

	Strcpy( (char*)&axsis , TpRecvArgs[1]  );
	axsis = AsctoHex(axsis);
	bcctrl[bcid].inf = axsis;
	
	bcid = mrcv->msgdata.bcid;
	if( bcid == NO_DATA )
	{
		TpRspINITA( E_BUSY );
		return;
	}

	
	if( axsis == 0 )
	{
		TpRspINITA( E_PAR_OUTRNG );
		return;
	}
	
//axsis =0x1f;// ���f�o�b�N
	
	
	if( (axsis & BIT1) )		bcctrl[bcid].devno = MT_Z0;
	else if( (axsis & BIT2) )	bcctrl[bcid].devno = MT_P0;
	else if( (axsis & BIT0) )	bcctrl[bcid].devno = MT_Y0;
	else if( (axsis & BIT4) )	bcctrl[bcid].devno = MT_Y1;
	else if( (axsis & BIT3) )	bcctrl[bcid].devno = MT_F0;

	StaQgProcList( bcid,               // bcid
				   ID_MSG_LIMIT,       // msgid
		           bcctrl[bcid].devno, // devno
		           1,                  // actstp
		           ID_TPL_INITA,       // prid1
				   axsis,              // inf
				   mrcv->msgdata.inf2  // inf2
	);
	
	QgProcList_INITA( mrcv );
}
/***********************************************************************
* 	
* [�T�v]	08 ���[�U�[���S�擾
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplUsral( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	

/***********************************************************************
* 	
* [�T�v]	11 ���[�U�[1���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	12 ���[�U�[2���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	13 ���[�U�[3���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	 14 ���[�U�[4���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	15 ���[�U�[5���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	16 ���[�U�[6���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	17 ���[�U�[7���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	18 ���[�U�[8���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	19 ���[�U�[9���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	1A ���[�U�[10���
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplInf10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	21 ���[�U�[1�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	22 ���[�U�[2�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	23 ���[�U�[3�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	24 ���[�U�[4�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	25 ���[�U�[5�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	26 ���[�U�[6�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	27 ���[�U�[7�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	28 ���[�U�[8�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	29 ���[�U�[9�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	2A ���[�U�[10�o�^
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRec10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	31 ���[�U�[1�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	32 ���[�U�[2�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	33 ���[�U�[3�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	34 ���[�U�[4�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	35 ���[�U�[5�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	36 ���[�U�[6�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	37 ���[�U�[7�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	38 ���[�U�[8�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	39 ���[�U�[9�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	3A ���[�U�[10�ύX
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplChg10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	41 ���[�U�[1�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	42 ���[�U�[2�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	43 ���[�U�[3�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	44 ���[�U�[4�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	45 ���[�U�[5�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	46 ���[�U�[6�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	47 ���[�U�[7�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	48 ���[�U�[8�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	49 ���[�U�[9�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	4A ���[�U�[10�폜
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplDel10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	50 FW��ԁi�|�[�����O�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplFwpol( T_CMD_MSG* mrcv, UB anum, const char** args )	
{
	TpRspFWPOL();
}

/***********************************************************************
* 	
* [�T�v]	51 �v���g�R�����s
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* [���l]
*         
*         tskid     ID_TSK_CMD
*         mpfid     ID_MPF_EEP
*         mbxid     ID_MBX_EEP
*         prid1
*         bcid
*         msgid     ID_MSG_EEPREAD
*         inf 
*         inf2
*         data
*         siz
* 
***********************************************************************/
static void TplPrtcl( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	procno;

	memset( &bufwk[0], CLR,  CMD_BUFSIZ );

	psel.pno = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
	psel.smp = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
	psel.val = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );
	
	mrcv->msgdata.prid1 = ID_TPL_PRTCL;
	mrcv->msgdata.inf2   = psel.pno;
	
	if( DeviceStatusGet() == E_IDLING )
	{
		// EEPROM �f�[�^�擾�w��
		mbx_send( ID_TSK_CMD,          // tskid
	               ID_MPF_EEP,         // mpfid
	               ID_MBX_EEP,         // mbxid
	               mrcv->msgdata.prid1,// prid1  �^�b�`�p�l���w��
	               mrcv->msgdata.bcid, // bcid
	               ID_MSG_EEPREAD,     // msgid
	               mrcv->msgdata.inf,  // sts
	               mrcv->msgdata.inf2, // sts2   �v���g�R���ԍ�
	               &bufwk[0],          // data   null
	               CMD_BUFSIZ          // siz
	             );

		// �v���g�R������
		TpRspPRCTL( E_SUCCESS );
	}
}

/***********************************************************************
* 	
* [�T�v]	52 ���X�^�[�g���s
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplResta( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	53 �ꎞ��~
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplPause( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	SetPauseStop( PAUS_STS );
	TplRpMakeSend( PARAM1, OK );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_PAUSE );
}	
/***********************************************************************
* 	
* [�T�v]	54 �ĊJ
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplResum( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	SetPauseStop( IDLE_STS );
	TplRpMakeSend( PARAM1, OK );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_RESUM );
}	
/***********************************************************************
* 	
* [�T�v]	55 ���f
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplAbort( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	SetPauseStop( STOP_STS );
	TplRpMakeSend( PARAM1, OK );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_ABORT );
}
/***********************************************************************
* 	
* [�T�v]	 58 �I�v�V�����o�[�R�[�h���s
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplOptcd( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	59 �T���v���`���[�uQR�R�[�h
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplSmbar( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	5A �T���v���`���[�u�L���m�F�i�P�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplSmsgl( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	result = NG;
	UB	param1;

	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		param1 = AsctoHex(param1);

	if(( param1 >= 1 )&&( param1 <= 12 ))
	{
		result = OK;//����
	}
	
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_SMSGL );
}

/***********************************************************************
* 	
* [�T�v]	5B �T���v���`���[�u�L���m�F�i�S�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplSmall( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	result = NG;
	UB	allsts[INDAT_CNTRL_MAX];

	// �S���̓|�[�g��Ԏ擾
	pio_ALLIN( &allsts[0] );

	if(( allsts[0] == 0xff )&&( allsts[1] == 0xc0 ))
	{
		result = OK;//����
	}
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_SMALL );
}

/***********************************************************************
* 	
* [�T�v]	5C ����`���[�u���[��QR�R�[�h
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplCllne( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	5D ����`���[�uQR�R�[�h
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplClqrc( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	5E ����`���[�u�L���i�P�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplClsgl( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	5F ����`���[�u�L���i�S�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplClall( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [�T�v]	60 LED
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplLdlmp( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	param1;
	UB	result = NG;

	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		param1 = AsctoHex(param1);

	if(( param1 >= LED_OFF )&&( param1 <= LED_ON ))
	{
		pio_BOUT( O_LIGHLED, param1 );
		result = OK;
	}
	
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_LDLMP );
}

/***********************************************************************
* 	
* [�T�v]	61 UV�_��
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplUvlmp( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	param1;
	UB	result = NG;

	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		param1 = AsctoHex(param1);

	if(( param1 >= LED_OFF )&&( param1 <= LED_ON ))
	{
		pio_BOUT( O_UVLAMP1, param1 );
		result = OK;
	}
	
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_UVLMP );
	
}

/***********************************************************************
* 	
* [�T�v]	62 �p�g�����v
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplPtlmp( T_CMD_MSG* mrcv, UB anum, const char** args )	
{
	UB param1;
	UB param2;
	UB param3;
	UB result;

	// �f�[�^�擾
	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		bzcnt.Act    = AsctoHex(param1);
	Strcpy( (char*)&param2 , TpRecvArgs[2]  );		bzcnt.Colptn = AsctoHex(param2);
	Strcpy( (char*)&param3 , TpRecvArgs[3]  );		bzcnt.Buzptn = AsctoHex(param3);

	// �f�[�^�`�F�b�N
	if(
		(( bzcnt.Act    >= PATOF   )&&( bzcnt.Act  <= PATBL1))&&
		(( bzcnt.Colptn >= C_GRN )&&( bzcnt.Colptn <= C_RED ))&&
		(( bzcnt.Buzptn >= LEDOF )&&( bzcnt.Buzptn <= LEDBL ))
	)
	{
		result = OK;
	}
	else
	{
		result = NG;
	}

	// �^�b�`�p�l���֕ԐM
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_PTLMP );

	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &bufwk[0], (B*)&bzcnt, sizeof( ST_BZCNT ) );

	// �^�X�N�ʒm
	mbx_send(
					ID_TSK_CMD,										// ���M�����ID
					ID_MPF_LED,										// ����߰�ID
					ID_MBX_LED,										// Ұ��ޯ��ID
					ID_TPL_PTLMP,									// PROCID
					mrcv->msgdata.bcid,								// BCID
					ID_MSG_CLEDON,									// message
					NO_DATA,										// ���1(�ėp)
					NO_DATA,										// ���2(�ėp)
					&bufwk[0],										// �ް�
					sizeof( bufwk )									// �ް�����
				);
}
/***********************************************************************
* 	
* [�T�v]	63 �d��OFF
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplPowof( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	UB result = OK;

	pio_BOUT( O_POWERDW, ON );

	// �^�b�`�p�l���֕ԐM
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_POWOF );

}
/***********************************************************************
* 	
* [�T�v]	6B �^�]�����i�^�b�`�p�l�����^�]������M�j
* [����]	�d���A�p�����[�^���A�A�h���X
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplExRun( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER err;
	INT siz;
	
	siz = sizeof( err );

	if( mrcv->msgdata.siz == siz )// error
	{
		ExpRunlRetry++;
		if( ExpRunlRetry > 3 )
		{
			// �G���[�������b�Z�[�W�쐬
			CmdMakeResponse( 	&stCtl,                                         // 
								PARAM2, 										// �p�����[�^��
								STRTBL_MSG[ID_MSG_EXPRUNL], 					// �R�}���h������
								E_RECV_FAIL,									// �G���[�R�[�h

								0, 												// 
								0, 												// 
								0,												// 
								0, 												// 
								0, 												// 

								0, 												// 
								0, 												// 
								0,												// 
								0, 												// 
								0 												// 
							);
			CmdSendResponseEtc( &stCtl, ID_MSG_RUNLOGERR );
		}
		else
		{
			// �^�b�`�p�l���փ��g���C
			memcpy( &err, &mrcv->msgdata.data[0], siz );

			TplExRunSend();
			TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_EXRUN );
		}
	}
	else
	{
		// �������b�Z�[�W�쐬
		CmdMakeResponse( 	&stCtl,                                         // 
							PARAM2, 										// �p�����[�^��
							STRTBL_MSG[ID_MSG_EXPRUNL], 					// �R�}���h������
							E_SUCCESS,										// �G���[�R�[�h

							0, 												// 
							0, 												// 
							0,												// 
							0, 												// 
							0, 												// 

							0, 												// 
							0, 												// 
							0,												// 
							0, 												// 
							0 												// 
						);
	    // �������b�Z�[�W���M
		CmdSendResponseEtc( &stCtl, ID_MSG_EXPRUNL );
		
		
		// �^�b�`�p�l���֎����R�[�h���˗�
		ExpRunlSeqno++;
		TplExRunSend();
		TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_EXRUN );
	}
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TpRspFWVER()
{
	UB	bcccal;
	UB	datsiz;
	UB	calsiz;
	UB	comsiz;
	char *pnter;

	pnter = &TP_RP1_SEND_FORM04[0];
	
	// �p�����[�^�t�H�[��
	datsiz = sizeof(TP_RP1_SEND_FORM04) - 3;
	calsiz = sizeof(TP_RP1_SEND_FORM04) - 5;
	comsiz = sizeof(TP_RP1_SEND_FORM04);

	memcpy( &tp.rsp.buf[0],&TP_RP1_SEND_FORM04[0],comsiz );
	memcpy( &tp.rsp.buf[8],&VER_INFO_TO_TP[0]  , 4 );

	bcccal = CalcBCC( &tp.rsp.buf[1], calsiz );
	tp.rsp.buf[(datsiz+0)] = HextoAsc(  ((bcccal >> 4) & 0x0f)  );
	tp.rsp.buf[(datsiz+1)] = HextoAsc(  ( bcccal       & 0x0f)  );

	TplCommDataSend( &tp.rsp.buf[0] , comsiz );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_FWVER );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TpRspLGIN1()
{
	UB	result;
	UB	bcccal;
	UB	datsiz;
	UB	calsiz;
	UB	comsiz;
	char *pnter;

	pnter = &TP_RP1_SEND_FORM02[0];
	
	datsiz = sizeof(TP_RP1_SEND_FORM02) - 3;
	calsiz = sizeof(TP_RP1_SEND_FORM02) - 5;
	comsiz = sizeof(TP_RP1_SEND_FORM02);

	memcpy( &tp.rsp.buf[0],pnter,comsiz );
	result = ChkUserInfo();

	tp.rsp.buf[8] = DectoAscH(result);
	tp.rsp.buf[9] = DectoAscL(result);
	
	bcccal = CalcBCC( &tp.rsp.buf[1], calsiz );
	tp.rsp.buf[(datsiz+0)] = HextoAsc(  ((bcccal >> 4) & 0x0f)  );
	tp.rsp.buf[(datsiz+1)] = HextoAsc(  ( bcccal       & 0x0f)  );

	TplCommDataSend( &tp.rsp.buf[0] , comsiz );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_LGIN1 );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TpRspLGIN2()
{
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TpRspOPTIF()
{
	UB	bcccal;
	UB	datsiz;
	UB	calsiz;
	UB	comsiz;
	char *pnter;

	pnter = &TP_RP1_SEND_FORM01[0];
	
	datsiz = sizeof(TP_RP1_SEND_FORM01) - 3;
	calsiz = sizeof(TP_RP1_SEND_FORM01) - 5;
	comsiz = sizeof(TP_RP1_SEND_FORM01);

	memcpy( &tp.rsp.buf[0],pnter,comsiz );
	tp.rsp.buf[8] = HextoAsc( BkP10.Login );
	
	bcccal = CalcBCC( &tp.rsp.buf[1], calsiz );
	tp.rsp.buf[(datsiz+0)] = HextoAsc(  ((bcccal >> 4) & 0x0f)  );
	tp.rsp.buf[(datsiz+1)] = HextoAsc(  ( bcccal       & 0x0f)  );

	TplCommDataSend( &tp.rsp.buf[0] , comsiz );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_OPTIF );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* [���l]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspINITA( ER result )
{
	INT data;

	if( result == E_SUCCESS )		data = 0;	// ����
	else							data = 1;	// ���s
	
	TplRpMakeSend( 1, data );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_INITA );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* [���l]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspPRCTL( ER result )
{
	INT data;

	if( result == E_SUCCESS )		data = 0;	// ����
	else							data = 1;	// ���s

	TplRpMakeSend( 1, data );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_PRTCL );
}
/***********************************************************************
* 
* [�T�v]	�|�[�����O
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* [���l]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspFWPOL()
{
	TplRpMakeSend( 1, DeviceProtcolNoGet() );		// �p�����[�^�P�d���쐬
	TplRpMakeSend( 2, DeviceStatusGet() );			// �p�����[�^�Q�d���쐬
	TplRpMakeSend( 3, (INT)DeviceStepGet() );		// �p�����[�^�R�d���쐬
	
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_FWPOL );
}
/***********************************************************************
* 
* [�T�v]	LED�_���^�����w��
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
static void TpRspLDLMP()
{
}

/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l���p�����[�^�d���쐬�����M
* [����]	�p�����[�^�ԍ��A�p�����[�^�f�[�^
* [�ߒl]	�|
* 	
***********************************************************************/
void TplRpMakeSend( UB parno, INT pardat )
{
	UB bcc;
	UB bcch;
	UB bccl;
	INT  len;

	memset( &rspbuf,  CLR,  sizeof( rspbuf ) );
	
	sprintf(rspbuf, "WN RP%0d &H%08X", parno, pardat );
	bcc = bcch = bccl = CLR;
	
	len = strlen(&rspbuf);
	memcpy( &tp.rsp.buf[1], &rspbuf, (len+1) );
	tp.rsp.buf[0] = STX;
	tp.rsp.buf[(len+1)] = ETX;

	bcc = CalcBCC( &tp.rsp.buf[1], len );
	DectoHexASC( bcc, &bcch, &bccl );

	tp.rsp.buf[(len+2)] = bcch;
	tp.rsp.buf[(len+3)] = bccl;
	tp.rsp.buf[(len+4)] = CRCHR;
	
	TplCommDataSend( &tp.rsp.buf[0] , (len+5) );
}
/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l���։^�]����v���d���쐬�����M
* [����]	
* [�ߒl]	�|
* [���l]	2WN RP1 XX3XXD
* 	        12345678901234
***********************************************************************/
void TplExRunSend()
{
	UB bcc  = CLR;
	UB bcch = CLR;
	UB bccl = CLR;
	INT len = 9;
	
	sprintf( ( char * )&MentePcSendbuf[1], "WN RP1 %02d", &ExpRunlSeqno );

	MentePcSendbuf[0] = STX;
	MentePcSendbuf[(len+1)] = ETX;

	bcc = CalcBCC( &MentePcSendbuf[1], len );
	DectoHexASC( bcc, &bcch, &bccl );

	MentePcSendbuf[(len+2)] = bcch;
	MentePcSendbuf[(len+3)] = bccl;
	MentePcSendbuf[(len+4)] = CRCHR;

	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,
				NO_DATA,
				ID_MSG_EXPRUNL,
				0,
				0,
				&MentePcSendbuf[0],
				(len+5)
			);
}
static	INT  lendbg;
/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l���p�����[�^�d���쐬�����M
* [����]	�p�����[�^�ԍ��A�p�����[�^�f�[�^
* [�ߒl]	�|
* 	
***********************************************************************/
void TplRpDataSend()
{
	UB bcc;
	UB bcch;
	UB bccl;
	INT  len;

	memset( &MentePcSendbuf[0], CLR, sizeof( MentePcSendbuf ) );
	sprintf( ( char * )&MentePcSendbuf[1], "WN RP100 %s", &ImpExpRcvData[0]);
	
	bcc = bcch = bccl = CLR;

	len = strlen(&MentePcSendbuf[1]);
	lendbg = len;
	
	MentePcSendbuf[0] = STX;
	MentePcSendbuf[(len+1)] = ETX;

	bcc = CalcBCC( &MentePcSendbuf[1], len );
	DectoHexASC( bcc, &bcch, &bccl );

	MentePcSendbuf[(len+2)] = bcch;
	MentePcSendbuf[(len+3)] = bccl;
	MentePcSendbuf[(len+4)] = CRCHR;

	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,            // procid
				NO_DATA,
				ID_MSG_IMPPDAT,
				0,
				0,
				&MentePcSendbuf[0],
				(len+5)
			);
}
/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l���փG�N�X�|�[�g�v���쐬�����M
* [����]	
* [�ߒl]	�|
* 	
***********************************************************************/
void TplRpExpReqSend()
{
	UB bcc;
	UB bcch;
	UB bccl;
	INT  len;

	sprintf( ( char * )&MentePcSendbuf[1], "WN RP1 %s", &ImpExpRcvData[0] );
	bcc = bcch = bccl = CLR;

	len = strlen(&ImpExpRcvData);
	MentePcSendbuf[0] = STX;
	MentePcSendbuf[(len+1)] = ETX;

	bcc = CalcBCC( &MentePcSendbuf[1], len );
	DectoHexASC( bcc, &bcch, &bccl );

	MentePcSendbuf[(len+2)] = bcch;
	MentePcSendbuf[(len+3)] = bccl;
	MentePcSendbuf[(len+4)] = CRCHR;

	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,            // procid
				NO_DATA,
				ID_MSG_IMPPDAT,
				0,
				0,
				&MentePcSendbuf[0],
				(len+5)
			);
}

/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l������ID�d���쐬
* [����]	�d���A����ID
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplRidDataSend( UB *txdat, UB id )
{
// ����01 �R�}���h                                        0    1    2    3    4    5    6    7    8    9   10
// static	const TPN_REQ_DAT TP_RID_SEND_FORM   = { 11, STX, 'W', 'N', ' ', 'R', 'I', 'D', ' ', '*', '*', ETX, 0x30, 0x30, 0x0d };
	UB	bcccal;
	UB	bcccal_h;
	UB	bcccal_l;
	UB	datsiz;
	UB	calsiz;
	UB	comsiz;

	datsiz = TP_RID_SEND_FORM.siz;
	calsiz = TP_RID_SEND_FORM.siz - 2;
	comsiz = TP_RID_SEND_FORM.siz + 3;
	memcpy( &txdat[0],&TP_RID_SEND_FORM.req[0], comsiz);
	txdat[10] = HextoAsc(  ((id >> 4) & 0x0f)  );
	txdat[11] = HextoAsc(  ( id       & 0x0f)  );
	
	
	bcccal = CalcBCC( &txdat[1], calsiz );
	txdat[(datsiz+0)] = HextoAsc(  ((bcccal >> 4) & 0x0f)  );
	txdat[(datsiz+1)] = HextoAsc(  ( bcccal       & 0x0f)  );

	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,            // procid
				NO_DATA,
				ID_MSG_CMD,
				0,
				0,
				txdat,
				comsiz
			);
}
/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l�����M�f�[�^�𑗐M�^�X�N�֒ʒm
* [����]	�d���A����ID
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplCommDataSend( UB *tpdat, INT tpsiz )
{
	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,            // procid
				NO_DATA,
				ID_MSG_CMD,
				0,
				0,
				tpdat,
				tpsiz
			);
}
/***********************************************************************
* 	
* [�T�v]	�^�b�`�p�l�����M�f�[�^�𑗐M�^�X�N�֒ʒm
* [����]	�d���A����ID
* [�ߒl]	�|
* 	
***********************************************************************/
static void TplCommDataSendImp( UB *tpdat, INT tpsiz, UB msg )
{
	mbx_send	(
				ID_TSK_CMD,
				ID_MPF_TXEX,
				ID_MBX_TXEX,
                NO_DATA,            // procid
				NO_DATA,
				msg,
				0,
				0,
				tpdat,
				tpsiz
			);
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	0xff:�d���ُ�
* 
***********************************************************************/
static ER TpCmdCheck( T_CMD_MSG* mrcv )
{
	ER	err = E_COM_CMDDEF;
	UB	id;
 	UB	cmd1wk = 'A';
 	UB	cmd2wk = '$';

	
	// TP�R�}���h�`�F�b�N
	if( 
		( mrcv->msgdata.data[0]  == cmd1wk  )&&
		( mrcv->msgdata.data[1]  == cmd2wk  ) 
	)
	{
		id    = ((AsctoHex( mrcv->msgdata.data[2] ) << 4) & 0xf0);
		id   |= ((AsctoHex( mrcv->msgdata.data[3] )     ) & 0x0f);

		// ����ID�`�F�b�N
		if(
			(( id >= ID_TPL_FWVER  )&&( id <= ID_TPL_INITA  ))||
			(( id >= ID_TPL_INF01  )&&( id <= ID_TPL_INF10  ))||
			(( id >= ID_TPL_REC01  )&&( id <= ID_TPL_REC10  ))||
			(( id >= ID_TPL_CHG01  )&&( id <= ID_TPL_CHG10  ))||
			(( id >= ID_TPL_DEL01  )&&( id <= ID_TPL_DEL10  ))||
			(( id >= ID_TPL_FWPOL  )&&( id <= ID_TPL_ABORT  ))||
			(( id >= ID_TPL_OPTCD  )&&( id <= ID_TPL_CLALL  ))||
			(( id >= ID_TPL_LDLMP  )&&( id <= ID_TPL_PTLMP  ))
		)
		{
			// ����
			err = E_SUCCESS;
		}
	}

	return( err );
}
/***********************************************************************
* 
* [�T�v]	QG12S�������X�g
* [����]	
* [�ߒl]	NO_DATA�F�󂫖���
* 
***********************************************************************/
static ER QgProcList( T_CMD_MSG* mrcv )
{
	ER  result = E_SUCCESS;
	UB  prid1;
	
	prid1 = mrcv->msgdata.prid1;

	switch( prid1 )
	{
		case ID_TPL_FWVER:        /* FW�o�[�W�������                */
			break;
		case ID_TPL_OPTIF:        /* �I�v�V�������                  */
			break;
		case ID_TPL_LGIN1:        /* ���O�C���F��1                   */
		case ID_TPL_LGIN2:        /* ���O�C���F��2                   */
			break;
		case ID_TPL_INITA:        /* �������w��                      */
			result = QgProcList_INITA( mrcv );
			break;
		case ID_TPL_USRAL:        /* ���[�U�[���S�擾              */
			break;
		case ID_TPL_INF01:        /* ���[�U�[1���                   */
		case ID_TPL_INF02:        /* ���[�U�[2���                   */
		case ID_TPL_INF03:        /* ���[�U�[3���                   */
		case ID_TPL_INF04:        /* ���[�U�[4���                   */
		case ID_TPL_INF05:        /* ���[�U�[5���                   */
		case ID_TPL_INF06:        /* ���[�U�[6���                   */
		case ID_TPL_INF07:        /* ���[�U�[7���                   */
		case ID_TPL_INF08:        /* ���[�U�[8���                   */
		case ID_TPL_INF09:        /* ���[�U�[9���                   */
		case ID_TPL_INF10:        /* ���[�U�[10���                  */
			break;
		case ID_TPL_REC01:        /* ���[�U�[1�o�^                   */
		case ID_TPL_REC02:        /* ���[�U�[2�o�^                   */
		case ID_TPL_REC03:        /* ���[�U�[3�o�^                   */
		case ID_TPL_REC04:        /* ���[�U�[4�o�^                   */
		case ID_TPL_REC05:        /* ���[�U�[5�o�^                   */
		case ID_TPL_REC06:        /* ���[�U�[6�o�^                   */
		case ID_TPL_REC07:        /* ���[�U�[7�o�^                   */
		case ID_TPL_REC08:        /* ���[�U�[8�o�^                   */
		case ID_TPL_REC09:        /* ���[�U�[9�o�^                   */
		case ID_TPL_REC10:        /* ���[�U�[10�o�^                  */
			break;
		case ID_TPL_CHG01:        /* ���[�U�[1�ύX                   */
		case ID_TPL_CHG02:        /* ���[�U�[2�ύX                   */
		case ID_TPL_CHG03:        /* ���[�U�[3�ύX                   */
		case ID_TPL_CHG04:        /* ���[�U�[4�ύX                   */
		case ID_TPL_CHG05:        /* ���[�U�[5�ύX                   */
		case ID_TPL_CHG06:        /* ���[�U�[6�ύX                   */
		case ID_TPL_CHG07:        /* ���[�U�[7�ύX                   */
		case ID_TPL_CHG08:        /* ���[�U�[8�ύX                   */
		case ID_TPL_CHG09:        /* ���[�U�[9�ύX                   */
		case ID_TPL_CHG10:        /* ���[�U�[10�ύX                  */
			break;
		case ID_TPL_DEL01:        /* ���[�U�[1�폜                   */
		case ID_TPL_DEL02:        /* ���[�U�[2�폜                   */
		case ID_TPL_DEL03:        /* ���[�U�[3�폜                   */
		case ID_TPL_DEL04:        /* ���[�U�[4�폜                   */
		case ID_TPL_DEL05:        /* ���[�U�[5�폜                   */
		case ID_TPL_DEL06:        /* ���[�U�[6�폜                   */
		case ID_TPL_DEL07:        /* ���[�U�[7�폜                   */
		case ID_TPL_DEL08:        /* ���[�U�[8�폜                   */
		case ID_TPL_DEL09:        /* ���[�U�[9�폜                   */
		case ID_TPL_DEL10:        /* ���[�U�[10�폜                  */
			break;
		case ID_TPL_FWPOL:        /* FW��ԁi�|�[�����O�j            */
			break;
		case ID_TPL_PRTCL:        /* �v���g�R�����s                  */
			result = QgProcList_PRTCL( mrcv );
			break;
		case ID_TPL_RESTA:        /* ���X�^�[�g���s                  */
			break;
		case ID_TPL_PAUSE:        /* �ꎞ��~                        */
			break;
		case ID_TPL_RESUM:        /* �ĊJ                            */
			break;
		case ID_TPL_ABORT:        /* ���f                            */
			break;
		case ID_TPL_OPTCD:        /* �I�v�V�����o�[�R�[�h���s        */
			break;
		case ID_TPL_SMBAR:        /* �T���v���`���[�uQR�R�[�h        */
			break;
		case ID_TPL_SMSGL:        /* �T���v���`���[�u�L���m�F�i�P�j  */
		case ID_TPL_SMALL:        /* �T���v���`���[�u�L���m�F�i�S�j  */
			break;
		case ID_TPL_CLLNE:        /* ����`���[�u���[��QR�R�[�h      */
			break;
		case ID_TPL_CLQRC:        /* ����`���[�uQR�R�[�h            */
			break;
		case ID_TPL_CLSGL:        /* ����`���[�u�L���i�P�j          */
		case ID_TPL_CLALL:        /* ����`���[�u�L���i�S�j          */
			break;
		case ID_TPL_LDLMP:        /* LED                             */
			break;
		case ID_TPL_UVLMP:        /* UV�_��                          */
			break;
		case ID_TPL_PTLMP:        /* �p�g�����v                      */
//			result = QgProcList_PTLMP( mrcv );
			break;
	}
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�������w��
* [����]	
* [�ߒl]	NO_DATA�F�󂫖���
* [���l]
*
* typedef struct {
* 	UB	msgid;		ID_MSG_LIMIT
* 	UB	devno;		��
* 	UB	actstp;		����X�e�b�v
* 	UB	prid1;		�v���Z�X�ԍ��iID_TPL_INITA�j
* 	UB	inf;		����������
* 	UB	inf2;		���g�p
* 	UB	inf3;		���g�p
* 	UB	inf4;		���g�p
* }ST_BCCTRL;
*
***********************************************************************/
static ER QgProcList_INITA( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB  prid1;
	UB  bcid;
	UB  inf;
	UB  spdptn;
	UB  bcidwk;
	UB  mm;
	UB  *spdadr;
	INT  offs;
	INT  *posadr;

	prid1  = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	bcidwk  = mrcv->msgdata.bcid;
	mm   = bcctrl[bcid].devno;
	
	
	spdadr = &SpPtn.Y0_SpdIniPtn;
	spdadr += (mm - 1);
	spdptn = *spdadr;
	
	MAC_SPEED_SET( mm, 
	               Sp[mm][spdptn].ac, 
	               Sp[mm][spdptn].fl, 
	               Sp[mm][spdptn].fh, 
	               Sp[mm][spdptn].gl );
	
	if(( spdptn < 0 )||( spdptn > 6 ))
	{
		TpRspINITA( E_CON_ERR);
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// �f�[�^������
		InitQgProcList(bcid);
		return( E_CON_ERR );
	}
	
	switch( bcctrl[bcid].actstp )
	{
	case 0:// �A�C�h�����O
		// �|�|�|
		// �|�|�|  �A�C�h�����O���i����w���Ȃ��j  �|�|�|
		// �|�|�|
		break;
	
	case 1:// ���_�Z���T�𓥂ނ܂ňړ�(����)
		 result = LimitAction(
							prid1, 
							bcid, 
							mm,   
							CCW,  
							GENTEN_POS,
//							NUKEDA_POS,
						 	Sp[mm][spdptn].fl );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;
	
	case 2:// ���_�Z���T�𔲂���܂ňړ�
//		motor_PSET( mm, 0, 0, 0 );
//		result = LimitAction(	
//							prid1, 
//							bcid, 
//							mm,
//							CW, 
//							NUKEDA_POS,
//						 	Sp[mm][spdptn].fl );
//		
//		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
//		WaitAction( prid1, bcid, 10 );
//		break;

	case 3:// �����J�E���^�[���Z�b�g
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 4:// ���z���_�ʒu�܂ňړ�
		posadr = &BkP7.Y0_Offset + mm - 1;
		offs = *posadr;
		
		result = RelmvAction( prid1, bcid, mm, REQ_STAUD, offs );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;

	case 5:// ���_�Z���T�𓥂ނ܂ňړ�(�������)
		 result = LimitAction(	
							prid1, 
							bcid, 
							mm,
							CCW, 
							GENTEN_POS,
						 	Sp[mm][spdptn].fl );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;
	
	case 6:// �����J�E���^�[���Z�b�g
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ������  ���ɑ���  ��������������������������������������������
	
	case 7:// ���z���_�ʒu�܂ňړ�
		posadr = &BkP7.Y0_Offset + mm - 1;
		offs = *posadr;
		
		result = RelmvAction( prid1, bcid, mm, REQ_STAUD, offs );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;
	
	case 8:// �����J�E���^�[���Z�b�g
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ������  ���ɑ���  ��������������������������������������������
	
	case 9:// �I������
//		bcctrl[bcid].inf &= 0xC0;
		BitClr( bcctrl[bcid].devno, &bcctrl[bcid].inf );

		if( bcctrl[bcid].inf == CLR )
		{
			result = E_SUCCESS;
		}
		else
		{
			if( (bcctrl[bcid].inf & BIT1) )			bcctrl[bcid].devno = MT_Z0;
			else if( (bcctrl[bcid].inf & BIT2) )	bcctrl[bcid].devno = MT_P0;
			else if( (bcctrl[bcid].inf & BIT0) )	bcctrl[bcid].devno = MT_Y0;
			else if( (bcctrl[bcid].inf & BIT4) )	bcctrl[bcid].devno = MT_Y1;
			else if( (bcctrl[bcid].inf & BIT3) )	bcctrl[bcid].devno = MT_F0;

			bcctrl[bcid].actstp = 1 ;
			WaitAction( prid1, bcid, 25 );
		}
		break;
	
	default:// �I������
		result = E_SUCCESS;
		break;
	}

	if( result != E_CONTINUE )
	{
		TpRspINITA( result );
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// �f�[�^������
	}

	return( result );	
}
/***********************************************************************
* 
* [�T�v]	�v���g�R������
* [����]	
* [�ߒl]	NO_DATA�F�󂫖���
* [���l]
*
* typedef struct {
* 	UB	msgid;		ID_MSG_LIMIT
* 	UB	devno;		��
* 	UB	actstp;		����X�e�b�v
* 	UB	prid1;		�v���Z�X�ԍ��iID_TPL_INITA�j
* 	UB	inf;		���g�p
* 	UB	inf2;		���g�p
* 	UB	inf3;		���g�p
* 	UB	inf4;		���g�p
* }ST_BCCTRL;
*
***********************************************************************/
static ER QgProcList_PRTCL( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB  prid1;
	UB  bcid;
	UB  pno;
	UB  Cmd;
	UB  Sts;
	
	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	pno   = mrcv->msgdata.inf2;// proc no
	
	if( bcctrl[bcid].actstp == CLR )
	{
		// �|�|�|
		// �|�|�|  �A�C�h�����O���i����w���Ȃ��j  �|�|�|
		// �|�|�|
		return( result );	
	}

	// ���f�m�F
	if( GetPauseStop() == STOP_STS )			return( E_PROTCOLSTOP );

	// �ꎞ��~�m�F
	if( GetPauseStop() == PAUS_STS )			return( E_PROTCOLPAUSE );

//
#if 0
	//  ������ �f�o�b�N ������ ����
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd = P_PRS;  // ���
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par02 = 1;      // �����p�^�[��  �|���v���x
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par03 = 1;      // �|�W�V����
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par04 = 123;    // PEEK����             kPa
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par05 = 4;      // PEEK�����^�C���A�E�g sec
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par06 = 56;     // �����i00H�`FFH�j     kPa
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par07 = 7;      // �����^�C���A�E�g     sec
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par08 = 89;     // �����ω�             kPa


	// ������ �f�o�b�N ������ ���a
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd = P_MIX;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par02 = 0x01;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par03 = 0x13;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par04 = 0x11;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par05 = 0x11;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par06 = 53;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par07 = 54;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par13 = 0x1f4;
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par13 = 0x1f4;
	pProcAdr->step[(bcctrl[bcid].actstp )].StCmd = P_NON;
#endif
//������ �f�o�b�N ������
	
	// --- �������瓮��J�n ----------------------------------------------------------------
	Cmd = ( pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd & P_MSK );
	Sts =   pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd;
	
	switch( Cmd )
	{
		case P_TGT://�`�b�v�擾
			result = CmdActionTipGet( mrcv );
			break;
		case P_TDS://�`�b�v�p��
			result = CmdActionTipDsp( mrcv );
			break;
		case P_OPN://������
			result = CmdActionStrOpn( mrcv );
			break;
		case P_PRS://����
			result = CmdActionPress( mrcv );
			break;
		case P_WAI://�E�F�C�g
			result = CmdActionWait( mrcv );
			break;
		case P_MIX://���a
			result = CmdActionMix( mrcv );
			break;
		case P_SUC://�z��
			result = CmdActionSuc( mrcv );
			break;
		case P_PUT://�f�o
			result = CmdActionPut( mrcv );
			break;
		case P_PR1://�����P
			result = CmdActionPrep1( mrcv );
			break;
		case P_PR2://�����Q
			result = CmdActionPrep2( mrcv );
			break;
		case P_INC://�C���L���x�[�V����
			result = CmdActionIncub( mrcv );
			break;
		case P_MOV://�ړ�
			result = CmdActionMove( mrcv );
			break;
	}
		
	switch( result )
	{
		case E_CONTINUE:
			break;

		case E_SUCCESS:
			pProcDat->Stp = CLR;
			bcctrl[bcid].actstp++;

			if( bcctrl[bcid].actstp > pProcDat->Cnt )		// �S�s���I��
			{
				memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );
			}
			else
			{
				Cmd = ( pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd & P_MSK );
				if( Cmd == P_NON )
				{
					memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );
				}
				else
				{
					WaitAction( prid1, bcid, 10 );
					result = E_CONTINUE;
				}
			}
			break;

		case E_PRES_FAIL:
			memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );
			memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );
			break;
	}
	
	if( result != E_CONTINUE )
	{
		DeviceInfoClr();
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// �f�[�^������
	}
	else
	{
		DeviceInfoSet( result, bcctrl[bcid].inf2, bcctrl[bcid].actstp, Sts );
	}
	
	return( result );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* [���l]	
* 
***********************************************************************/
static void InitQgProcList( UB bcid )
{
	if( ( bcid != NO_DATA )&&( bcid < BCCTRL_MAX ))
	{
		//	UB	msgid;
		//	UB	devno;
		//	UB	actstp;
		//	UB	prid1;
		//	UB	inf;
		//	UB	inf2;
		//	UB	inf3;
		//	UB	inf4;
		bcctrl[bcid].actstp = 0;
		bcctrl[bcid].msgid  = NO_DATA;
		bcctrl[bcid].devno  = NO_DATA;
		bcctrl[bcid].prid1  = NO_DATA;
		bcctrl[bcid].inf    = NO_DATA;
		bcctrl[bcid].inf2   = NO_DATA;
	}
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* [���l]	
* 
***********************************************************************/
static void StaQgProcList( UB bcid, UB msgid, UB devno, UB actstp, UB prid1, UB inf, UB inf2)
{
	if( ( bcid != NO_DATA )&&( bcid < BCCTRL_MAX ))
	{
		bcctrl[bcid].msgid  = msgid;
		bcctrl[bcid].devno  = devno;
		bcctrl[bcid].prid1  = prid1;
		bcctrl[bcid].actstp = actstp;
		bcctrl[bcid].inf    = inf;
		bcctrl[bcid].inf2   = inf2;// �v���g�R�����s���́A�v���g�R���ԍ�
	}
}

/***********************************************************************
* 
* [�T�v]	QG12S�������X�g�󂫊m�F
* [����]	
* [�ߒl]	NO_DATA�F�󂫖���
* 
***********************************************************************/
static UB GetQgProcList( UB procid )
{
	UB   result = NO_DATA;
	UB index;

	if(( procid != NO_DATA )&&( procid != CLR ))
	{
		for( index = 0; index < BCCTRL_MAX; index++ )
		{
			if(( bcctrl[index].prid1 == NO_DATA )&&( result == NO_DATA ))
			{
				return( index );//�ŏ��ɂ݂����󂫈ʒu
			}
		}
	}
	else
	{
		result = NO_DATA;
	}
	
	return( result );
}

/***********************************************************************
* 
* [�T�v]	QG12S�������X�g�d���m�F
* [����]	
* [�ߒl]	TRUE�F�d����������ID�K��O
* 
***********************************************************************/
static BOOL IsQgProcList( UB procid )
{
	BOOL  result = FALSE;
	UB    index;

//	if(( procid != NO_DATA )&&( procid != CLR )&&( procid < BCCTRL_MAX ))
	if(( procid != NO_DATA )&&( procid != CLR ))
	{
		for( index = 0; index < BCCTRL_MAX; index++ )
		{
			if( bcctrl[index].prid1 == procid )
			{
				result =  TRUE;
				break;
			}
		}
	}
	else
	{
		result =  TRUE;
	}
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static void SetUserInfoSorce( ST_USERINFO *user )
{
	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &RcvUserInfo.Id[0], user, sizeof(ST_USERINFO) );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static void ClrUserInfoSorce()
{
	memset( &RcvUserInfo.Id[0], CLR, sizeof( ST_USERINFO ) );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB ChkUserInfo()
{
	UB i = 0;
	
	eep_GetUserInfo  ( &BkP1.User[0].Id[0] );

	for( i = 0; i < USER_CT_MAX; i++ )
	{
		if(
			( Strncmp( &RcvUserInfo.Id[0], &BkP1.User[i].Id[0], USER_ID_MAX ) == 0 )&&
			( Strncmp( &RcvUserInfo.Pw[0], &BkP1.User[i].Pw[0], USER_PW_MAX ) == 0 )
		)
		{
			i++;
			break;
		}
	}
	
	return(i);
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB GetProtocolStatus( UB dev, UB ptn )
{
	UB result = E_OK;
	
	// �X�e�b�v�����ԓǍ���
	
	
	
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	��Ԏ擾
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionStatus( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�`�b�v�擾
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionTipGet( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	UB spdptn;
	UB movptn;
	UB devno;
	INT movval;

	UB spdptnwk;
	bcid  = mrcv->msgdata.bcid;
	actstep = bcctrl[bcid].actstp -1 ;
	
	switch( pProcDat->Stp )
	{
	case 0:	// �`�b�v�������m�F�i�`�b�v�����ς݂̏ꍇ�A�`�b�v�����G���[�ʒm���I���j
		// ���`�b�v�L���m�F���������A�Z���T�����̂��߁A�X���[
		
		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������


	case 1:	// Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdIniPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 2:	// Y0�����x�ݒ�E�`�b�v�����ʒu�ړ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		movptn = GetTipPatern( pProcAdr->step[actstep].par03 );
		movval = GetPositionValue(movptn);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 3:	// Z���`�b�v�擾���x�i1�i�ځj�ݒ�E1�i�ڈʒu�ړ�
		spdptn = (( pProcAdr->step[actstep].par04 >> 4 ) & 0x0f );
		
		if( spdptn > 0 )
		{
			devno  = MT_Z0;
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			movval = GetPositionValue(POS_Z0_TPGT1);
			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}


	case 4:	// Z���`�b�v�擾���x�i�ŏI�j�ݒ�E�ŏI�ʒu�ړ�
		spdptn = ( pProcAdr->step[actstep].par04 & 0x0f );

		if( spdptn > 0 )
		{
			devno  = MT_Z0;
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			movval = GetPositionValue(POS_Z0_TPGTE);

			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}

	case 5:	// Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6:	// �`�b�v�����m�F�i�������s���A�`�b�v�������G���[�ʒm���I���j
		// ���`�b�v�L���m�F

		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	�`�b�v�p��
* [����]	
* [�ߒl]	
* [���l]	inf3�F�v�����W�����g���C�J�E���^�Ɏg�p
* 
***********************************************************************/
static ER  CmdActionTipDsp( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	UB spdptn;
	UB movptn;
	UB devno;
	INT movval;
	UB rtycnt;

	bcid  = mrcv->msgdata.bcid;
	actstep = bcctrl[bcid].actstp -1 ;
	
	switch( pProcDat->Stp )
	{
	case 0:	// �`�b�v�������m�F�i�`�b�v�����ς݂̏ꍇ�A�`�b�v�����G���[�ʒm���I���j
		// ���`�b�v�L���m�F���������A�Z���T�����̂��߁A�X���[
		
		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������


	case 1:	// Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdIniPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 2:	// Y0�����x�ݒ�E�`�b�v�p���ʒu�ړ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		movptn = GetTipPatern( pProcAdr->step[actstep].par03 );
		movval = GetPositionValue(movptn);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3:	// Z���`�b�v�p�����x�i1�i�ځj�ݒ�E1�i�ڈʒu�ړ�
//		spdptn = ( pProcAdr->step[actstep].par04 & 0x0f );
//		
//		if( spdptn >= 0 )
//		{
//			devno  = MT_Z0;
//			MAC_SPEED_SET( devno, 
//			               Sp[devno][spdptn].ac, 
//			               Sp[devno][spdptn].fl, 
//			               Sp[devno][spdptn].fh, 
//			               Sp[devno][spdptn].gl );
//			movval = GetPositionValue(POS_Z0_TPDL1);
//			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
//			if( result == E_SUCCESS ) result = E_CONTINUE;
//			pProcDat->Stp++;
//			break;
//		}
//		else
//		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
//		}


	case 4:	// Z���`�b�v�p�����x�i�ŏI�j�ݒ�E�ŏI�ʒu�ړ�
		spdptn = ( pProcAdr->step[actstep].par04 & 0x0f );

		if( spdptn >= 0 )
		{
			devno  = MT_Z0;
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			movval = GetPositionValue(POS_Z0_TPDLE);

			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}
		
		
	case 5:	// �p������ �v�����W�������֓���
		spdptn = (( pProcAdr->step[actstep].par04 >> 4 ) & 0x0f );

		if( spdptn > 0 )
		{
			devno  = MT_P0;
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			movval = GetPositionValue(POS_P0_TIPDL);
			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}


	case 6:	// �p������ �v�����W������֓���
		spdptn = (( pProcAdr->step[actstep].par04 >> 4 ) & 0x0f );

		if( spdptn > 0 )
		{
			devno  = MT_P0;
			movval = GetPositionValue(POS_P0_TIPDL)*(-1);

			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;

			rtycnt = ( ( pProcAdr->step[actstep].par03 >> 4 ) & 0x03 );

			if( bcctrl[bcid].inf3 >= rtycnt )
			{
				pProcDat->Stp++;
			}
			else
			{
				pProcDat->Stp = 5;
				bcctrl[bcid].inf3++;
			}
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}


	case 7:	// Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 8:	// �`�b�v�����m�F�i�������s���A�`�b�v�������G���[�ʒm���I���j
		// ���`�b�v�L���m�F

		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	������
* [����]	
* [�ߒl]	
* [���l]	
* 		pProcAdr->step[actstep].par03 == STRIP_S1 )	// �V���[�g�P
*
***********************************************************************/
static ER  CmdActionStrOpn( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	UB spdptn;
	UB movptn;
	UB devno;
	INT movval;

	bcid  = mrcv->msgdata.bcid;
	actstep = bcctrl[bcid].actstp - 1 ;

	switch( pProcDat->Stp )
	{
	case 0:	// ����X�g���b�o�L���m�F
		
		// ������X�g���b�o�L���m�F
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������


	case 1: // Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		movval = GetPositionValue(POS_Z0_TAIHI);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 2:	// Y0�����x�ݒ��A����X�g���b�p�u���[�N�ʒu�Ɉړ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		movptn = pProcAdr->step[actstep].par05;
		movval = GetPositionValue(movptn);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3:	// Z���V�[���u���[�N1�i�ʒu�i���݈ʒu����1�i�ڂ܂Łj
		spdptn = (( pProcAdr->step[actstep].par04 >> 4 ) & 0x0f );
		
		if( spdptn > 0 )
		{
			devno  = MT_Z0;
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			movval = GetPositionValue(POS_Z0_TPGT1);
			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}
		
		
	case 4:	// Z���V�[���u���[�N�ŏI�ʒu�i1�i�ڂ���ŏI�ʒu�܂Łj
		spdptn = ( pProcAdr->step[actstep].par04 & 0x0f );
		
		if( spdptn )
		{
			devno  = MT_Z0;
			MAC_SPEED_SET( MT_Z0, 
			               Sp[MT_Z0][spdptn].ac, 
			               Sp[MT_Z0][spdptn].fl, 
			               Sp[MT_Z0][spdptn].fh, 
			               Sp[MT_Z0][spdptn].gl );
			movval = GetPositionValue(POS_Z0_STOPE);
			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD,  movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
			break;
		}
		else
		{
			pProcDat->Stp++;
			// ������  ���ɑ���  ��������������������������������������������
		}


	case 5:	// Z�����x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6:	// 
		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	����
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionPress( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB nzl;
	UB prid1;
	UB bcid;
	UB inf1;
	UB inf2;
	UB actstep;
	UB dummy;
	
	unsigned short sftbit = 0x0001;
	unsigned short wkA = 0x0000;
	unsigned short wkB = 0x0000;
	unsigned short wkC = 0x0000;
	
	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	inf1  = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	actstep = bcctrl[bcid].actstp - 1 ;
	
	switch( pProcDat->Stp )
	{
	case 0:	// �|���v���x�ݒ�A�����O���͎擾�w��
		psns.suc  = 0;
		psns.idl  = IdleBitSet(psel.smp);
		psns.tcnt = 0;

		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSTAN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		WaitAction( prid1, bcid, PRESSMP_WAIT );
		pProcDat->Stp++;
		break;

	case 1:	// ����m�Y���̂ݓd���ق𗬘H���Ɏw��
		// typedef struct {
		// 	unsigned short	idl;
		// 	unsigned short	suc;
		// 	unsigned short	err;
		// 	unsigned short	tcnt;
		// 	unsigned short	pre[NOZZLE_MAX+1];//�����O
		// 	unsigned short	now[NOZZLE_MAX+1];//����
		// 	unsigned short	pek[NOZZLE_MAX+1];//Peek
		// 	unsigned short	dat[NOZZLE_MAX+1][PRESSMP_MAX];//����
		// } ST_PSNS;

		PressDataGet( &psns.pre[0] );

		vout.target = psns.idl | 0x01fff;
		vout.valout = ( ( psns.err ^ 0x1fff ) & psns.idl ) | 0x01fff;
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT,     // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,        // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;

	case 2:	// �|���v�쓮�i�����J�n�j
		pio_BOUT( O_DAIAPMP, 5 );// �����x�p�^�[�������
		WaitAction( prid1, bcid, PRESSMP_WAIT );
		pProcDat->Stp++;
		break;

	case 3:	// ���͒l�擾
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSREN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		WaitAction( prid1, bcid, PRESSMP_WAIT );
		
		pProcDat->Stp++;
		psns.tcnt++;
		break;

	case 4:	// ��������
		result = PressureChk();

		if( result == E_CONTINUE )
		{
			pProcDat->Stp = 3;
			WaitAction( prid1, bcid, PRESSMP_WAIT );
		}
		else if( result == E_SUCCESS )
		{
			memcpy( &psns.pek[0], &psns.now[0], (NOZZLE_MAX*2) );
			pio_BOUT( O_DAIAPMP, 0 );
			WaitAction( prid1, bcid, POMPAFT_WAIT );
			pProcDat->Stp++;
			psns.tcnt = 0;
			result = E_CONTINUE;
		}
		break;

	case 5:	// ���͒l�擾 ����
//		vout.target = ( 0x0fff ^ vout.valout);
//		vout.valout = vout.target;
		vout.target = 0x1fff;
		vout.valout = 0x0000;

		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT,     // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,        // data
						sizeof( ST_VOUT )    // siz
					);

		pProcDat->Stp++;
		break;

	case 6:	// ��������
		result = RePressureChk();

		psns.tcnt++;
		if( result == E_CONTINUE )
		{
			pProcDat->Stp = 5;
			WaitAction( prid1, bcid, PRESSMP_WAIT );
		}
		else if( result == E_SUCCESS )
		{
			pio_BOUT( O_DAIAPMP, 0 );
			WaitAction( prid1, bcid, POMPAFT_WAIT );
			pProcDat->Stp++;
			psns.tcnt = 0;
			result = E_CONTINUE;
		}
		else
		{
		}
		break;
		
		
	case 7:	// 
		if( PressStatus(&pProcAdr->step[actstep].par03) != E_SUCCESS )//��
		{
			result = E_CONTINUE;
			WaitAction( prid1, bcid, 25 );
			break;
		}
		else
		{
			pProcDat->Stp++;
			result = E_SUCCESS;
		}
		break;
	}
	return( result );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  PressureChk()
{
	ER result = E_CONTINUE;
	UB nzl;
	UB actstep;
	UB spdptn;
	unsigned short sftbit = 0x0001;
	unsigned short wkA = 0x0000;
	unsigned short wkB = 0x0000;
	unsigned short wkC = 0x0000;

	if( psns.tcnt >= ( 5 * pProcAdr->step[actstep].par05 ))//  Timeout����(msec) : Tsec * 1000�AInterval:200msec�A�^�C���A�E�g�J�E���g�F1000T/200 = 5T
	{
		// �^�C���A�E�g�����ꍇ�́A��Ώ�ch�E����ch�ȊO�G���[
		wkA = ( psns.idl | psns.suc );
		wkB = ( wkA      ^ 0x0fff   );
		wkC = ( wkB      ^ psns.err );
		psns.err = wkC;
		
		if( ( wkC & 0x0fff ) == 0x0fff )	// �S�ăG���[�͏I��
		{
			result = E_PRES_FAIL;
debug1_flg=0;
		}
		else
		{
			result = E_SUCCESS;

debug1_flg=1;
		}
	}
	else
	{
		PressDataGet( &psns.now[0] );

		for( nzl = 0; nzl < NOZZLE_MAX ; nzl++ )
		{
			if( (psns.idl & sftbit) )
			{
				wkA = (psns.suc | psns.err);

				if(( wkA & sftbit))
				{
					// �G���[�������͐����ς݂̏ꍇ�́A����s�v
				}
				else
				{
					peekdat = psns.now[nzl];
					if( peekdat >= pProcAdr->step[actstep].par04 )	// �����s�[�N�z��
					{
						psns.suc |= sftbit;
						psns.pek[nzl] = peekdat;// �����s�[�N�o�^
					}
				}
			}
			sftbit = ( sftbit << 1 );
		}
		
		wkA = (psns.idl | psns.suc | psns.err);
		
		if( ( wkA & 0x0fff ) == 0x0fff )	// �S�ďI��
		{
			result = E_SUCCESS;
debug1_flg=1;
		}
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER RePressureChk()
{
	ER result = E_CONTINUE;
	UB nzl;
	UB actstep;
	UB spdptn;
	unsigned short sftbit = 0x0001;
	unsigned short wkA = 0x0000;
	unsigned short wkB = 0x0000;
	unsigned short wkC = 0x0000;

	if( psns.tcnt >= ( 5 * pProcAdr->step[actstep].par07 ))//  Timeout����(msec) : Tsec * 1000�AInterval:200msec�A�^�C���A�E�g�J�E���g�F1000T/200 = 5T
	{
		// �^�C���A�E�g�����ꍇ�́A��Ώ�ch�E����ch�ȊO�G���[
		wkA = ( psns.idl | psns.suc );
		wkB = ( wkA      ^ 0x0fff   );
		wkC = ( wkB      ^ psns.err );
		psns.err = wkC;
		
		if( ( wkC & 0x0fff ) == 0x0fff )	// �S�ăG���[�͏I��
		{
			result = E_PRES_FAIL;
		}
		else
		{
			result = E_SUCCESS;
		}
	}
	else
	{
		PressDataGet( &psns.now[0] );

		for( nzl = 0; nzl < NOZZLE_MAX ; nzl++ )
		{
			if( (psns.idl & sftbit) )
			{
				wkA = (psns.suc | psns.err);

				if(!( wkA & sftbit))
				{
					if( psns.now[nzl] <= pProcAdr->step[actstep].par06 )	// ����������
					{
						psns.suc |= sftbit;
					}
					else if( (psns.pek[nzl]-psns.now[nzl]) >= pProcAdr->step[actstep].par08 )	// �}���Ɍ���������
					{
						psns.err |= sftbit;
					}
				}
				sftbit = ( sftbit << 1 );
			}
		}

		if( ( psns.err & 0x0fff ) == 0x0fff )	// �S�ăG���[�͏I��
		{
			result = E_SUCCESS;
		}
		
		wkA = (psns.idl | psns.suc | psns.err);
		if( ( wkA & 0x0fff ) == 0x0fff )		// �S�ďI��
		{
			result = E_SUCCESS;
		}
	}
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�E�F�C�g
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionWait( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	INT waittim;

	switch( pProcDat->Stp )
	{
	case 0:	// 1. �E�F�C�g���Ԑݒ��A�^�C�}�X�^�[�g
		bcid    = mrcv->msgdata.bcid;
		prid1   = mrcv->msgdata.prid1;
		actstep = bcctrl[bcid].actstp -1 ;
		waittim = ( pProcAdr->step[actstep].par03 * 60 * 1000 ) / DLY_10M ;// 10msec����

		WaitAction( prid1, bcid, waittim );
		pProcDat->Stp++;
		break;

	case 1:	// 2. �E�F�C�g�I���҂�
		result = E_SUCCESS;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	���a
* [����]	
* [�ߒl]	
* 


// 3�D����Z����h�a�z��������ʒu�ړ�
// 4�D�h�a�e�ʁ{�]��ʕ��i����̂݁j���z���Ɠ����ɕ���Z�������~������B
// 5�D�z����E�F�C�g
// 6�D���݂̕���Z���ʒu����h�a�e�ʂ�f�o�Ɠ����ɕ���Z���㏸������B
// 7�D�h�a�e�ʂ�f�o�@�����a�ŏI��̓f�o���̂ݗ]��ʕ����܂ߓf�o
// 8�D�f�o��E�F�C�g
// 9�D�4�`8���ݒ�񐔌J��Ԃ�
// 10�D����Z���u���[�A�E�g�����ʒu�ړ�
// 11�D�|���v���Z�b�g
// 12�D����Z���`�b�v�^�b�`�ʒu�ړ�  (�`�b�v�^�b�`�L��)
// 13�D����Z���z�[���|�W�V�����ړ�
//***********************************************************************/

static ER  CmdActionMix( T_CMD_MSG* mrcv )
{
	ST_HOSEI_PTN *adr;
	ER result = E_CONTINUE;
	INT tskid;
	UB prid1;
	UB bcid;
	UB msgid;
	UB inf1;
	UB inf2;
	UB actstep;
	UB spdptn;
	UB devno;
	UB movptn;
	UB dummy;
	unsigned short cntwk;
	INT movval;
	INT value;

	UB  z_sucdn;// par4  bit7�`bit4�F[Z]�z�����~���x�p�^�[��
	UB  p_sucsp;// par4  bit3�`bit0�F[P]�z�����x�p�^�[��
	UB  z_sucup;// par5  bit7�`bit4�F[Z]�f�o�㏸���x�p�^�[��
	UB  p_putsp;// par5  bit3�`bit0�F[P]�f�o���x�p�^�[��
	UB  staposi;// par6  [Z]���a�J�n�ʒu�|�W�V�����ԍ�
	UB  endposi;// par7  [Z]���a�I���ʒu�|�W�V�����ԍ�
	INT mixcnt; // par1  bit5�`bit0�F���a��
	INT zancnt;
	INT stahei;// par6   [Z]���a�J�n�ʒu
	INT endhei;// par7   [Z]���a�I���ʒu
	INT sucval;// par13  [P}�z����
	INT putval;// par14  [P}�f�o��
	
	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	inf1  = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	actstep = bcctrl[bcid].actstp - 1 ;
	
	switch( pProcDat->Stp )
	{
	case 0:	// [���a����] 
		z_sucdn = ((pProcAdr->step[actstep].par05 >> 4) & 0x0f);
		p_sucsp = ((pProcAdr->step[actstep].par05 >> 0) & 0x0f);
		z_sucup = ((pProcAdr->step[actstep].par05 >> 4) & 0x0f);
		p_putsp = ((pProcAdr->step[actstep].par05 >> 0) & 0x0f);
		stahei  = GetPosiData( pProcAdr->step[actstep].par07 );
		endhei  = GetPosiData( pProcAdr->step[actstep].par08 );
		sucval  = pProcAdr->step[actstep].par13;
		putval  = pProcAdr->step[actstep].par14;
		
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������
		
		
	case 1:// [���a����] 1�D�u���[�A�E�g�G�A�����i�ޔ������j�ړ�
		MAC_SPEED_SET( MT_Z0, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].ac, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fl, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fh, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 2:// [���a����] 2�D���͊����F���w��̃m�Y���`�����l���̓d���ق�ON�ɂ���
		psns.suc  = 0;
		psns.idl  = IdleBitSet(psel.smp);
		psns.tcnt = 0;

//		memcpy( &psns.pre[0], &psns.now[0], (NOZZLE_MAX*2) );
		vout.target = 0x0fff;
		vout.valout = ( (NzlTargetGet()) ^ 0x0fff );
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT, // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,  // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;
		
	case 3:// [���a����] 3�DP���u���[�A�E�g�G�A�z�����x�ݒ�
		// �u���[�A�E�g�G�A�z��
		devno  = MT_P0;
		spdptn = SpPtn.P0_SpdMovAirS;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		value = BlowoutAirValue( (ST_HOSEI_PTN *)&pProcAdr->step[actstep] );
		result = RelmvAction( ID_TPL_PRTCL, bcid, MT_P0, REQ_STAUD, value );
		if( result == E_SUCCESS ) result = E_CONTINUE;

		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSTAN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		pProcDat->Stp++;
		break;
		
	case 4:	// [���a����]  4�D����Z����h�a�z��������ʒu�ړ�
		if( tskid == ID_TSK_PUNIT )
		{
			// �����͊Ď��L�q
			
			
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
		}
		else
		{
			devno  = MT_Z0;
			spdptn = (pProcAdr->step[actstep].par04 >> 4);
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );

			movptn = pProcAdr->step[actstep].par06;
			movval = GetPositionValue(movptn);
			
			result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
			if( result == E_SUCCESS ) result = E_CONTINUE;
			pProcDat->Stp++;
		}
		break;
		
	case 5 :	// [���a����]  5 �D�h�a�e�ʁ{�]��ʕ��i����̂݁j���z���Ɠ����ɕ���Z�������~������B& �z�������͊Ď�
		if( tskid == ID_TSK_PUNIT )
		{
			// �ǂݎ̂�
		}
		else
		{
			devno  = MT_Z0;
			spdptn = (pProcAdr->step[actstep].par05 >> 4);
			movptn = pProcAdr->step[actstep].par07;				// �z���I������
			movval = GetPositionValue(movptn);

			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );

			devno  = MT_P0;
			spdptn = (pProcAdr->step[actstep].par05 & 0x0f);
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
			
			MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );
			if( result == E_SUCCESS ) result = E_CONTINUE;

			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
			
			pProcDat->Stp++;
		}
		break;
		
	case 6 :	// [���a����]  6 �D�z����E�F�C�g
		if( tskid == ID_TSK_PUNIT )
		{
			// �����͊Ď��L�q
			
			
			
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
		}
		else
		{
			WaitAction( prid1, bcid, 10 );
			pProcDat->Stp++;
		}
		break;
		
	case 7 :	// [���a����]  7 �D���݂̕���Z���ʒu����h�a�e�ʂ�f�o�Ɠ����ɕ���Z���㏸������B
		if( tskid == ID_TSK_PUNIT )
		{
			// �ǂݎ̂�
		}
		else
		{
			devno  = MT_Z0;
			spdptn = (pProcAdr->step[actstep].par05 >> 4);// �f�o�㏸���x�p�^�[��
			movptn = pProcAdr->step[actstep].par06;		  // ���a�J�n����
			movval = GetPositionValue(movptn);

			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );

			devno  = MT_P0;
			spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// �f�o���x�p�^�[��
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
	
			// ���ŏI�̓f�o���i�]��ʁ{�f�o�ʂ�f���o���j
			if( ((pProcAdr->step[actstep].par03 & 0x0f)+1) >= psns.tcnt )
			{
				MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par14 );
			}
			else
			{
				MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par14 );
			}
			if( result == E_SUCCESS ) result = E_CONTINUE;

			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
			
			pProcDat->Stp++;
		}
		break;
		
	case 8 :	// [���a����]  8 �D�h�a�e�ʂ�f�o
		if( tskid == ID_TSK_PUNIT )
		{
			// �ǂݎ̂�
		}
		else
		{
			pProcDat->Stp++;
			WaitAction( prid1, bcid, 10 );
		}
		break;
		
	case 9 :	// [���a����]  9 �D�f�o��E�F�C�g
		if( tskid == ID_TSK_PUNIT )
		{
			// �ǂݎ̂�
		}
		else
		{
			psns.tcnt++;

			cntwk  = pProcAdr->step[actstep].par03 & 0x0f;

			if( psns.tcnt >= cntwk )
			{
				pProcDat->Stp++;
			}
			else
			{
				pProcDat->Stp = 5;// �5�`9���ݒ�񐔌J��Ԃ�
			}
			WaitAction( prid1, bcid, 2 );
		}
		break;
		
	case 10:	// [���a����]  10�D����Z���u���[�A�E�g�����ʒu�ړ�
		MAC_SPEED_SET( MT_Z0, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].ac, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fl, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fh, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 11:	// [���a����]  11�D�|���v���Z�b�g
		// �u���[�A�E�g�G�A�z��
		devno  = MT_P0;
		spdptn = SpPtn.P0_SpdMovAirS;
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		result = RelmvAction( ID_TPL_PRTCL, bcid, MT_P0, REQ_STAUD, 0 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 12:	// [���a����]  12�D���͊����F���w��̃m�Y���`�����l���̓d���ق�ON�ɂ���
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������


	case 13:	// [���a����]  13�D����Z���`�b�v�^�b�`�ʒu�ړ�  (�`�b�v�^�b�`�L��)
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������


	case 14:	// [���a����]  14�D����Z���ޔ��ʒu�ړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 15:	// [���a����] 
		result = E_SUCCESS;
		break;
	}
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	�z��
* [����]	
* [�ߒl]	
* [�⑫]	P1  P2   P3     P4        P5  P6       P7       P8        P9  P10  P11  P12  P13    P14
*           *8       ���-�  �z���ʒu  -   �z�����x �z�����J �z�����I  -   -    -    -    �z���� -
* 
***********************************************************************/
static ER  CmdActionSuc( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	UB spdptn;
	UB inf1;
	UB inf2;
	UB movptn;
	UB devno;
	INT movval;
	UB dummy;

	bcid  = mrcv->msgdata.bcid;
	prid1   = mrcv->msgdata.prid1;
	inf1  = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	actstep = bcctrl[bcid].actstp -1 ;

//	psel.pno = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
//	psel.smp = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
//	psel.val = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );


	switch( pProcDat->Stp )
	{
	case 0: // 0�D �ҋ@
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 1: // 1�D Z���ʏ푬�x�ݒ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 2: // 2�D Z���ޔ������ֈړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3�D Y0���ʏ푬�x�ݒ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;
		movptn = pProcAdr->step[actstep].par03;
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ������  ���ɑ���  ��������������������������������������������

	case 4: // 4�D Y0���w��ʒu�ړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5�D Z���z�������ʒu�ړ�
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6�D ���͊����F���w��d����ON�A�w��d����OFF
		vout.target = 0x01fff;
		vout.valout = ( (NzlTargetGet()) ^ 0x0fff );
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT, // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,  // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;

	case 7: // 7�D P���z�����x�ݒ�
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);
		movptn = pProcAdr->step[actstep].par06;				// �z���I������
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par05 & 0x0f);
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 8: // 8�D �z���J�n�iP���z�� & Z�����~�j
//      MapAction( UB prid1, UB bcid, UB mm1, UB mm2, INT pppp1, INT pppp2 );
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );

		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 9: // 9�D ���͊����F�z�����쒆�Ɏw�肵���m�Y���`�����l���̈��͂��Ď�
	        //     ���e�`�����l���̈��̓Z���T�l���ݒ�l���������ꍇ�A�G���[�Ƃ��d���ق�ON�ɂ���B
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSREN,     // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		pProcDat->Stp++;
		break;

	case 10:// 10�D�z������I��
		pProcDat->Stp++;


	case 11:// 11�D���͊����F���͊Ď��I���B�S�Ẵm�Y���`�����l���̓d���ق�OFF�ɂ���B
		vout.target = 0x0fff;
		vout.valout = 0x0000;
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT,     // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,        // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;

	case 12:// 12�D�w�莞�ԃE�F�C�g
		pProcDat->Stp++;
		WaitAction( prid1, bcid, 10 );
		break;

	case 13:// 13�DZ���ޔ������ֈړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 14:	// 
		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�f�o
* [����]	
* [�ߒl]	
* [�⑫]	P1  P2    P3    P4       P5   P6   P7   P8   P9   P10   P11   P12   P13   P14
*           *8       ���-�  �z���ʒu -    -    -    -
*
***********************************************************************/
static ER  CmdActionPut( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	INT tskid;
	UB prid1;
	UB bcid;
	UB msgid;
	UB inf1;
	UB inf2;
	UB actstep;
	UB spdptn;
	UB devno;
	UB movptn;
	UB dummy;
	INT movval;

	bcid    = mrcv->msgdata.bcid;
	prid1   = mrcv->msgdata.prid1;
	actstep = bcctrl[bcid].actstp - 1 ;
	tskid    = mrcv->msgdata.tskid;
	msgid    = mrcv->msgdata.msgid;
	
	switch( pProcDat->Stp )
	{
	case 0: // 0�D �ҋ@
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	//****************************************************
	//***                    �z��                      ***
	//****************************************************
	case 1: // 1�D Z���ʏ푬�x�ݒ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;                    // [Z]�ʏ푬�x
		movval = GetPositionValue(POS_Z0_TAIHI);        // [Z]�ޔ��ʒu

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 2: // 2�D Z���ޔ������ֈړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3�D Y0���ʏ푬�x�ݒ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;                    // [Y0]�ʏ푬�x
		movptn = pProcAdr->step[actstep].par03;         // [Y0]�z���ʒu
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ������  ���ɑ���  ��������������������������������������������

	case 4: // 4�D Y0���w��ʒu�ړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5�D Z���z�������ʒu�ړ�
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;         // [Z]�z���J�n����
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6�D ���͊����F���w��d����ON�A�w��d����OFF
		vout.target = 0x0fff;
		vout.valout = ( (NzlTargetGet()) ^ 0x0fff );
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT, // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,  // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;

	case 7: // 7�D P���z�����x�ݒ�
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]�z�����~���x�p�^�[��
		movptn = pProcAdr->step[actstep].par07;         // [Z]�z���I������
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// [P]�z�����x�p�^�[��
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 8: // 8�D �z���J�n�iP���z�� & Z�����~�j
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 9: // 9�D ���͊����F�z�����쒆�Ɏw�肵���m�Y���`�����l���̈��͂��Ď�
	        //     ���e�`�����l���̈��̓Z���T�l���ݒ�l���������ꍇ�A�G���[�Ƃ��d���ق�ON�ɂ���B
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSTAN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		pProcDat->Stp++;
		break;

	case 10:// 10�D�z������I��
		if( tskid == ID_TSK_MOTEND )
		{
			WaitAction( prid1, bcid, 10 );
			pProcDat->Stp++;
		}
		if( tskid == ID_TSK_PUNIT )
		{
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
		}
		break;

	case 11:  // 11�D�w�莞�ԃE�F�C�g�҂�
		if( msgid == ID_MSG_PRESSTAN )
		{
			//�ǂݎ̂�
			break;
		}
		if( msgid == ID_MSG_TIMWAIT )
		{
			pProcDat->Stp++;
		}
		// ������  ���ɑ���  ��������������������������������������������
		
	case 12:// 12�DZ���ޔ������ֈړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 13:  // 13�D�I���҂�
		result = E_SUCCESS;
		break;
	}

	return( result );
}
/***********************************************************************
* 
* [�T�v]	�����P
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionPrep1( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	INT tskid;
	UB prid1;
	UB bcid;
	UB msgid;
	UB inf1;
	UB inf2;
	UB actstep;
	UB spdptn;
	UB devno;
	UB movptn;
	UB dummy;
	INT movval;
	
	bcid  = mrcv->msgdata.bcid;
	prid1   = mrcv->msgdata.prid1;
	actstep = bcctrl[bcid].actstp -1 ;
	
	switch( pProcDat->Stp )
	{
	case 0: // 0�D �ҋ@
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	//****************************************************
	//***                    �z��                      ***
	//****************************************************
	case 1: // 1�D Z���ʏ푬�x�ݒ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;                    // [Z]�ʏ푬�x
		movval = GetPositionValue(POS_Z0_TAIHI);        // [Z]�ޔ��ʒu

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 2: // 2�D Z���ޔ������ֈړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3�D Y0���ʏ푬�x�ݒ�
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;                    // [Y0]�ʏ푬�x
		movptn = pProcAdr->step[actstep].par03;         // [Y0]�z���ʒu
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ������  ���ɑ���  ��������������������������������������������

	case 4: // 4�D Y0���w��ʒu�ړ�
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5�D Z���z�������ʒu�ړ�
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;         // [Z]�z���J�n����
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6�D ���͊����F���w��d����ON�A�w��d����OFF
		vout.target = 0x0fff;
		vout.valout = ( (NzlTargetGet()) ^ 0x0fff );
		
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_VALVEOUT, // msgid
						inf1,                // sts
						inf2,                // sts2
						(UB *)&vout.target,  // data
						sizeof( ST_VOUT )    // siz
					);
		pProcDat->Stp++;
		break;

	case 7: // 7�D P���z�����x�ݒ�
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]�z�����~���x�p�^�[��
		movptn = pProcAdr->step[actstep].par07;         // [Z]�z���I������
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// [P]�z�����x�p�^�[��
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 8: // 8�D �z���J�n�iP���z�� & Z�����~�j
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 9: // 9�D ���͊����F�z�����쒆�Ɏw�肵���m�Y���`�����l���̈��͂��Ď�
	        //     ���e�`�����l���̈��̓Z���T�l���ݒ�l���������ꍇ�A�G���[�Ƃ��d���ق�ON�ɂ���B
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSTAN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		pProcDat->Stp++;
		break;

	case 10:// 10�D�z������I��
		if( tskid == ID_TSK_MOTEND )
		{
			WaitAction( prid1, bcid, 10 );
			pProcDat->Stp++;
		}
		if( tskid == ID_TSK_PUNIT )
		{
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
		}
		break;

	case 11:  // 11�D�w�莞�ԃE�F�C�g�҂�
		if( msgid == ID_MSG_PRESSTAN )
		{
			//�ǂݎ̂�
			break;
		}
		if( msgid == ID_MSG_TIMWAIT )
		{
			pProcDat->Stp++;
		}
		// ������  ���ɑ���  ��������������������������������������������
		
	case 12:// 12�DZ���ޔ������ֈړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	//****************************************************
	//***                    �f�o                      ***
	//****************************************************
	case 13:   // 13�DY0���w��ʒu�ړ�
		movptn = pProcAdr->step[actstep].par04;
		movval = GetPositionValue(movptn);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 14:   // 14�DZ���f�o�J�n�����ֈړ�
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par09;					// �f�o�J�n����
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 15:   // 15�DP���f�o���x�ݒ�AZ���㏸���x�ݒ�
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par08 >> 4);
		movptn = pProcAdr->step[actstep].par10;					// �f�o�I���ʒu
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par08 & 0x0f);
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 16:   // 16�D�f�o�J�n�iP���f�o�EZ���f�o�㏸�����쓮�j�A�����Ɉ��͊�ֈ��͒l�擾�w��
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par14 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������

	case 17:   // 17�DP���EZ������I���܂ŁA���͏�ԊĎ����s���B臒l�ȉ������o�����ꍇ�́A�f�o���s�B
		mbx_send	(
						ID_TSK_PUNIT,        // tskid
						ID_MPF_PUNIT,        // mpfid
						ID_MBX_PUNIT,        // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_PRESSTAN, // msgid
						inf1,                // sts
						inf2,                // sts2
						&dummy,              // data
						sizeof( dummy )      // siz
					);
		pProcDat->Stp++;
		break;

	case 18:   // 18�D�iP���EZ������I����j���͊����F���͊Ď��I���B�S�Ẵm�Y���`�����l���̓d���ق�OFF�ɂ���B
		if( tskid == ID_TSK_MOTEND )
		{
			vout.target = 0x0fff;
			vout.valout = 0x0000;
			
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_VALVEOUT, // msgid
							inf1,                // sts
							inf2,                // sts2
							(UB *)&vout.target,  // data
							sizeof( ST_VOUT )    // siz
						);
			pProcDat->Stp++;
		}
		if( tskid == ID_TSK_PUNIT )
		{
			mbx_send	(
							ID_TSK_PUNIT,        // tskid
							ID_MPF_PUNIT,        // mpfid
							ID_MBX_PUNIT,        // mbxid
							prid1,               // prid1
							bcid,                // bcid
							ID_MSG_PRESSTAN, // msgid
							inf1,                // sts
							inf2,                // sts2
							&dummy,              // data
							sizeof( dummy )      // siz
						);
		}
		break;
		
	case 19:  // 19�D�w�莞�ԃE�F�C�g
		if( msgid == ID_MSG_PRESSTAN )
		{
			//�ǂݎ̂�
		}
		if( msgid == ID_MSG_VALVEOUT )
		{
			pProcDat->Stp++;
			WaitAction( prid1, bcid, 10 );
		}
		break;
		
		
	case 20:  // 20�DZ���ʏ푬�x�ݒ�E�ޔ��ʒu�ړ�
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
		
	case 21:  // 21�D�I���҂�
		result = E_SUCCESS;
		break;
	}
	
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�z���ʒu�擾
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static INT  GetPositionValue( UB index )
{
	INT *posadr;
	INT result = CLR;
	
	if(( index >= POS_Y0_OFFSE )&&( index <= POS_Z0_COLDS ))	// 1�`32
	{
		posadr = &BkP7.Y0_Offset + (index - POS_Y0_OFFSE) ;
		result = *posadr;
	}

	if(( index >= POS_Z0_SUCS1 )&&( index <= POS_XX_RSV64 ))	// 33�`
	{
		posadr = &BkP8.Z0_SucStaHei1 + (index - POS_Z0_SUCS1) ;
		result = *posadr;
	}

	if(( index >= POS_H1_C1VO1 )&&( index <= POS_XX_RSV96 ))	// 65�`
	{
		posadr = &BkP9.Hosei[0].HS_VL1 + (index - POS_H1_C1VO1) ;
		result = *posadr;
	}

	if( index == POS_P0_TIPDL )	// 
	{
		result = 100;
	}
	return( result );
}

/***********************************************************************
* 
* [�T�v]	�����Q
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionPrep2( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;

	return( result );
}
/***********************************************************************
* 
* [�T�v]	�C���L���x�[�V����
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionIncub( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB	msgid;
	UB actstep;
	UB spdptn;
	UB movptn;
	UB devno;
	INT movval;
	UB	inf;
	UB	inf2;
	UB	prid1;
	UB	bcid;
	UB	tempdat;

	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	msgid = mrcv->msgdata.msgid;
	inf   = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	actstep = bcctrl[bcid].actstp -1 ;

	switch( pProcDat->Stp )
	{
	case 0:// [Z]�m�Y�������ړ�
		IncuvDataInit(pProcAdr->step[actstep].par03 , pProcAdr->step[actstep].par04);
		
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]�z�����~���x�p�^�[��
		movptn = pProcAdr->step[actstep].par06;         // [Z]�z���I������
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 1:	// ������֎w�肵�����x�ɐݒ�ʒm���s���B
		// �w�艷�x�ʒm
		tempdat = pProcAdr->step[actstep].par04;

		mbx_send	(
					ID_TSK_TMPCNT,       // tskid
					ID_MPF_TMPCNT,       // mpfid
					ID_MBX_TMPCNT,       // mbxid
					prid1,               // prid1
					bcid,                // bcid
					ID_MSG_TMPSET,       // msgid
					inf,                 // sts
					inf2,                // sts2/inf2
					(UB *)&tempdat,      // data
					sizeof( tempdat )    // siz
				);
		pProcDat->Stp++;
		break;

	case 2:	// �w�艷�x���w�莞�ԂɂȂ�܂ŊĎ�
		if( TempChk() == FALSE  )
		{
			mbx_send	(
						ID_TSK_TMPCNT,       // tskid
						ID_MPF_TMPCNT,       // mpfid
						ID_MBX_TMPCNT,       // mbxid
						prid1,               // prid1
						bcid,                // bcid
						ID_MSG_TMPGETR,      // msgid
						inf,                 // sts
						inf2,                // sts2/inf2
						(UB *)&tempdat,      // data
						sizeof( tempdat )    // siz
					);
			WaitAction( prid1, bcid, TEMP_CHK_WAIT );
			break;
		}
		else
		{
			pProcDat->Stp++;
		// ������  ���ɑ���  ��������������������������������������������
		}

	case 3:	// �I���҂�
		result = E_SUCCESS;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	TRUE = �C���L���x�[�V�����I�� 
* 
***********************************************************************/
static BOOL TempChk()
{
	BOOL blnResult = FALSE;
	unsigned char tempdat;
	
	tempdat = TmpDataGet();
	
	incuv.smp[2] = incuv.smp[1];
	incuv.smp[1] = incuv.smp[0];
	incuv.smp[0] = tempdat;
	
	if( tempdat == incuv.tmp )
	{
		incuv.cnt += (( TEMP_CHK_WAIT * 10 ) / 1000) ;
	}
	else
	{
		incuv.cnt = 0;
	}
	
	if( incuv.cnt >= incuv.tim )
	{
		blnResult = TRUE;
	}
	else
	{
	}
	
	return( blnResult );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	TRUE = �C���L���x�[�V�����I�� 
* 
***********************************************************************/
static void IncuvDataInit( unsigned char tmp, unsigned int tim )
{
	incuv.tmp = tmp;
	incuv.smp[0] = 0;
	incuv.smp[1] = 0;
	incuv.smp[2] = 0;
	incuv.tim = ( tim * 60 ) ;
	incuv.cnt = 0;
	incuv.tmo = 0;
}

/***********************************************************************
* 
* [�T�v]	�ړ�
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  CmdActionMove( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	UB bcid;
	UB prid1;
	UB actstep;
	UB spdptn;
	UB movptn;
	UB devno;
	INT movval;

	bcid  = mrcv->msgdata.bcid;
	prid1   = mrcv->msgdata.prid1;
	actstep = bcctrl[bcid].actstp -1 ;

	switch( pProcDat->Stp )
	{
	case 0:	//
		movptn = pProcAdr->step[actstep].par03;
		devno = GetMoveDevice(movptn);
		movval = GetPositionValue(movptn);
		
		if( devno == MT_Y0 )		spdptn = SpPtn.Y0_SpdNmlPtn;
		if( devno == MT_Z0 )		spdptn = SpPtn.Z0_SpdNmlPtn;
		if( devno == MT_P0 )		spdptn = SpPtn.P0_SpdNmlPtn;
		if( devno == MT_F0 )		spdptn = SpPtn.F0_SpdNmlPtn;
		if( devno == MT_Y1 )		spdptn = SpPtn.Y1_SpdNmlPtn;
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		break;
	}
	return( result );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB  GetTipPatern( UB commdat )
{
	UB result = CLR;
	UB tipwk;
	
	tipwk = ( commdat >> 6);
	
	switch( tipwk )
	{
	case 1:
		result = POS_STRIPTP1;
		break;
	case 2:
		result = POS_STRIPTP2;
		break;
	case 3:
		result = POS_STRIPTPL;
		break;
	default:
		break;
	}

	if( result )
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB  GetMoveDevice( UB posi )
{
	UB devno = CLR;
	
	switch( posi )
	{
		case POS_P0_OFFSE :   // [P0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��R
		case POS_P0_TAIHI :   // [P0] �ޔ��ʒu                           �|�W�V�����ԍ��W
			devno = MT_P0;
			break;

		case POS_F0_OFFSE :   // [F0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��S
		case POS_F0_TAIHI :   // [F0] �ޔ��ʒu                           �|�W�V�����ԍ��X
			devno = MT_F0;
			break;
		
		case POS_Y1_OFFSE :   // [Y1] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��T
		case POS_Y1_TAIHI :   // [Y1] �ޔ��ʒu                           �|�W�V�����ԍ��P�O
			devno = MT_Y1;
			break;

		case POS_Y0_OFFSE :   // [Y0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��P
		case POS_Y0_TAIHI :   // [Y0] �ޔ��ʒu                           �|�W�V�����ԍ��U
		case POS_STRIPTP1 :   // [Y0] ����X�g���b�v-�V���[�g�`�b�v�P    �|�W�V�����ԍ��P�P
		case POS_STRIPTP2 :   // [Y0] ����X�g���b�v-�V���[�g�`�b�v�Q    �|�W�V�����ԍ��P�Q
		case POS_STRIPTPL :   // [Y0] ����X�g���b�v-�����O�`�b�v        �|�W�V�����ԍ��P�R
		case POS_STRIPTUB :   // [Y0] ����X�g���b�v-�T���v���`���[�u    �|�W�V�����ԍ��P�S
		case POS_STRIPSM6 :   // [Y0] ����X�g���b�v-���o�p����6         �|�W�V�����ԍ��P�T
		case POS_STRIPSM5 :   // [Y0] ����X�g���b�v-���o�p����5         �|�W�V�����ԍ��P�U
		case POS_STRIPSM4 :   // [Y0] ����X�g���b�v-���o�p����4         �|�W�V�����ԍ��P�V
		case POS_STRIPSM3 :   // [Y0] ����X�g���b�v-���o�p����3         �|�W�V�����ԍ��P�W
		case POS_STRIPSM2 :   // [Y0] ����X�g���b�v-���o�p����2         �|�W�V�����ԍ��P�X
		case POS_STRIPSM1 :   // [Y0] ����X�g���b�v-���o�p����1         �|�W�V�����ԍ��Q�O
		case POS_STRIPCAT :   // [Y0] ����X�g���b�v-�J�[�g���b�W        �|�W�V�����ԍ��Q�P
		case POS_STRIPCOL :   // [Y0] ����X�g���b�v-�j�_����t          �|�W�V�����ԍ��Q�Q
			devno = MT_Y0;
			break;

		case POS_Z0_OFFSE :   // [Z0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��Q
		case POS_Z0_TAIHI :   // [Z0] �ޔ��ʒu                           �|�W�V�����ԍ��V
		case POS_Z0_STOPE :   // [Z0] ����X�g���b�v-�V�[���J���ŏI�ʒu  �|�W�V�����ԍ��Q�R
		case POS_Z0_STOP1 :   // [Z0] ����X�g���b�v-�V�[���J���P�i�ʒu  �|�W�V�����ԍ��Q�S
		case POS_Z0_STRIP :   // [Z0] ����X�g���b�v-�����ʒu	         �|�W�V�����ԍ��Q�T
		case POS_Z0_TPGTE :   // [Z0] �`�b�v�擾�ŏI�ʒu                 �|�W�V�����ԍ��Q�U
		case POS_Z0_TPGT1 :   // [Z0] �`�b�v�擾�P�i�ʒu                 �|�W�V�����ԍ��Q�V
		case POS_Z0_TPDLE :   // [Z0] �`�b�v�p���ŏI�ʒu                 �|�W�V�����ԍ��Q�W
		case POS_Z0_TPDL1 :   // [Z0] �`�b�v�p���P�i�ʒu                 �|�W�V�����ԍ��Q�X
		case POS_Z0_LIQPU :   // [Z0] �p�t�f�o�ʒu                       �|�W�V�����ԍ��R�O
		case POS_Z0_COPOS :   // [Z0] �R���N�V���� ����`���[�u�ʒu      �|�W�V�����ԍ��R�P
		case POS_Z0_COLDS :   // [Z0] �p�t�`���[�u�ʒu                   �|�W�V�����ԍ��R�Q
		case POS_Z0_SUCS1 :   // [Z0] �z���J�n����1                      �|�W�V�����ԍ��R�R
		case POS_Z0_SUCS2 :   // [Z0] �z���J�n����2                      �|�W�V�����ԍ��R�S
		case POS_Z0_SUCS3 :   // [Z0] �z���J�n����3                      �|�W�V�����ԍ��R�T
		case POS_Z0_SUCS4 :   // [Z0] �z���J�n����4                      �|�W�V�����ԍ��R�U
		case POS_Z0_SUCS5 :   // [Z0] �z���J�n����5                      �|�W�V�����ԍ��R�V
		case POS_Z0_SUCE1 :   // [Z0] �z���I������1                      �|�W�V�����ԍ��R�W
		case POS_Z0_SUCE2 :   // [Z0] �z���I������2                      �|�W�V�����ԍ��R�X
		case POS_Z0_SUCE3 :   // [Z0] �z���I������3                      �|�W�V�����ԍ��S�O
		case POS_Z0_SUCE4 :   // [Z0] �z���I������4                      �|�W�V�����ԍ��S�P
		case POS_Z0_SUCE5 :   // [Z0] �z���I������5                      �|�W�V�����ԍ��S�Q
		case POS_Z0_PUTS1 :   // [Z0] �f�o�J�n����1                      �|�W�V�����ԍ��S�R
		case POS_Z0_PUTS2 :   // [Z0] �f�o�J�n����2                      �|�W�V�����ԍ��S�S
		case POS_Z0_PUTS3 :   // [Z0] �f�o�J�n����3                      �|�W�V�����ԍ��S�T
		case POS_Z0_PUTS4 :   // [Z0] �f�o�J�n����4                      �|�W�V�����ԍ��S�U
		case POS_Z0_PUTS5 :   // [Z0] �f�o�J�n����5                      �|�W�V�����ԍ��S�V
		case POS_Z0_PUTE1 :   // [Z0] �f�o�I������1                      �|�W�V�����ԍ��S�W
		case POS_Z0_PUTE2 :   // [Z0] �f�o�I������2                      �|�W�V�����ԍ��S�X
		case POS_Z0_PUTE3 :   // [Z0] �f�o�I������3                      �|�W�V�����ԍ��T�O
		case POS_Z0_PUTE4 :   // [Z0] �f�o�I������4                      �|�W�V�����ԍ��T�P
		case POS_Z0_PUTE5 :   // [Z0] �f�o�I������5                      �|�W�V�����ԍ��T�Q
		case POS_Z0_MIXS1 :   // [Z0] �h�a�z���J�n����                   �|�W�V�����ԍ��T�R
		case POS_Z0_MIXE1 :   // [Z0] �h�a�z���I������                   �|�W�V�����ԍ��T�S
			devno = MT_Z0;
			break;
		
		default:
			break;
	}
	return( devno );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static INT GetPosiData( UB posino )
{
	INT result = CLR;
	INT *adr;
	
	if(
		( posino >= POS_P0_OFFSE )&&
		( posino <= POS_XX_RSV96 )
	)
	{
		adr = &BkP7.Y0_Offset + ( posino - 1 ) ;
		result = *adr;
	}

	return( result );
}

/***********************************************************************
* 
* [�T�v]	���̓��j�b�g�։����w��
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static ER  PressReq( UB *par )
{
	ER result = E_CONTINUE;
	// ���������
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	���̓��j�b�g�։����w��
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static INT  PressStatus( UB *par )
{
	INT result = E_CONTINUE;
	
	// ���������
	result = E_SUCCESS;
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static void  SetPauseStop( UB act )
{
	if( act > PAUS_STS )	PauseStopFlg = PAUS_STS;
	else 					PauseStopFlg = act;
	
	// ��ԏ�������
	
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB  GetPauseStop()
{
	return( PauseStopFlg );
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static UB  InitPauseStop()
{
	PauseStopFlg = IDLE_STS;
}

/***********************************************************************
* 
* [�T�v]	�u���[�A�E�g�G�A��
* [����]	�␳�O�e��
* [�ߒl]	�␳��e��
* 
***********************************************************************/
static INT  BlowoutAirValue( ST_HOSEI_PTN *adr )
{
	INT result = 0;
	
	result = ValueCal(adr, adr->HS_Out);
	
	return( result);
}
/***********************************************************************
* 
* [�T�v]	�G�A�M���b�v��
* [����]	�␳�O�e��
* [�ߒl]	�␳��e��
* 
***********************************************************************/
static INT  AirGapValue( ST_HOSEI_PTN *adr )
{
	INT result = 0;
	
	result = ValueCal(adr, adr->HS_Air);
	
	return( result);
}
/***********************************************************************
* 
* [�T�v]	�]���
* [����]	�␳�O�e��
* [�ߒl]	�␳��e��
* 
***********************************************************************/
static INT  SurplusValue( ST_HOSEI_PTN *adr )
{
	INT result = 0;
	
	result = ValueCal(adr, adr->HS_Tra);
	
	return( result);
}

/***********************************************************************
* 
* [�T�v]	�e�ʕ␳�v�Z
* [����]	�␳�O�e��
* [�ߒl]	�␳��e��
* 
***********************************************************************/
static INT  ValueCal( ST_HOSEI_PTN *adr, INT value )
{
	INT result = 0;
	INT valcal = 0;
	INT factor = 0;
	INT vl0_base = POMPVALUE_BASE * (0.0*10) ;
	INT vl1_base = POMPVALUE_BASE * (0.1*10) ;
	INT vl2_base = POMPVALUE_BASE * (0.5*10) ;
	INT vl3_base = POMPVALUE_BASE * (1.0*10) ;
	
	valcal = value * 10;
	
	if( ( vl0_base >= 0 )&&( ( valcal < vl1_base ) ))			//  0%�ȏ� �` 10%����
	{
		factor = adr->HS_VL1;
	}
	if( ( valcal >= vl1_base )&&( ( valcal < vl2_base ) ))	// 10%�ȏ� �` 50%����
	{
		factor = adr->HS_VL2;
	}
	if( ( valcal >= vl2_base )&&( ( valcal < vl3_base ) ))	// 50%�ȏ� �` 100%����
	{
		factor = adr->HS_VL3;
	}
	if( valcal >= vl3_base )										// 100%�ȏ�
	{
		factor = adr->HS_VL3;
	}
	
	result = factor * valcal;
	result = result / 1000;

	return( result);
}

/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static unsigned short  GetNzlStsAll()
{
	return( nzlsts );
}
/***********************************************************************
* 
* [�T�v]	�w��m�Y���̏�Ԃ��擾����
* [����]	�m�Y���ԍ�
* [�ߒl]	�m�Y����ԁi0 ��  �G���[��ԁA0 = �����ԁj
* 
***********************************************************************/
static unsigned short  GetNzlSts(UB no)
{
	unsigned short sftbit = 0x0001;
	unsigned short result = 0x0000;
	
	if( no > 0 )
	{
		sftbit = ( sftbit << ( no - 1) );
		result = ( nzlsts & sftbit );
	}
	
	return( result );
}
/***********************************************************************
* 
* [�T�v]	�m�Y����ԃZ�b�g
* [����]	�m�Y���ԍ�
* [�ߒl]	�|
* [���l]	bit0:�m�Y��1�E�E�E�Ebit11:�m�Y��12  �m�Y����ԁi1 =  �G���[��ԁA0 = �����ԁj
* 
***********************************************************************/
static void  SetNzlSts( unsigned char  no )
{
	unsigned short sftbit = 0x0001;
	
	if( no > 0 )
	{
		sftbit  = ( sftbit << ( no - 1) );
		nzlsts |= sftbit;
	}
}
/***********************************************************************
* 
* [�T�v]	�m�Y����ԃN���A
* [����]	�m�Y���ԍ�
* [�ߒl]	�|
* 
***********************************************************************/
static void  ClrNzlSts( unsigned char  no )
{
	unsigned short sftbit = 0x0001;
	
	if( no > 0 )
	{
		sftbit  = ( sftbit << ( no - 1) );
		nzlsts &= ( sftbit ^ 0xffff );
	}
}
/***********************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
***********************************************************************/
static unsigned short  NzlTargetGet()
{
	unsigned short nzltarget;
	unsigned short nzlerror;
	unsigned short nzlmask;
	unsigned short nzlresult;
	
	nzltarget = IdleBitSet( psel.smp );	// �^�[�Q�b�g�擾
	nzlerror  = GetNzlStsAll() ;		// �G���[��Ԏ擾
	nzlmask   = ( nzlerror ^ 0xffff );	// �G���[�`���l���}�X�N�f�[�^�쐬
	nzlresult = ( nzltarget & nzlmask );// �G���[�ȊO�̃^�[�Q�b�g�擾
	
	return( nzlresult );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
INT GetPsnsData( unsigned char *pdat )
{
	INT i;
	INT len;

	len = strlen(&psnsdat[0]);

	for( i=0;i<len;i++)
	{
		*pdat=psnsdat[i];
		pdat++;
	}
	return( len );
}

/***********************************************************************
* 
* [�T�v]	
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
INT GetRunData( unsigned char *buf )
{
	memcpy( buf, &ImpExpRcvData[0], RcvDataSize );
	
	return( RcvDataSize );
}

/***********************************************************************
* 
* [�T�v]	��e�ʃf�[�^�����ꍇ�A�ꎞ�I��RcvData�Ɋi�[ ���������Ȃ��݌v�K�v
* [����]	���b�Z�[�W�f�[�^
* [�ߒl]	�|
* 
***********************************************************************/
void SetRcvData( unsigned char *buf, unsigned int len )
{
	memset( &ImpExpRcvData[0], CLR, sizeof( ImpExpRcvData ) );

	RcvDataSize = len;
	memcpy( &ImpExpRcvData[0], buf, len );
}



