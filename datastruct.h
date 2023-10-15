/******************************************************************************
* �\���̒�`                                                                  *
*                                                                             *
* File name : datastruct.h                                                    *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H

#include "automachine.h"

//*************************************
// �R�}���h���b�Z�[�W
//*************************************
typedef struct {
    INT     tskid;                     	// �^�X�N�h�c          4
    UB      prid1;                      // ���                1
    UB      prid2;                      // ���                1
    UB      bcid;                       // �u���b�N�R�}���h�h�c1
    UB      msgid;                      // ���b�Z�[�W          1
    UB      inf;                        // ���                1
    UB      inf2;                       // ���                1
    UB      inf3;                       // ���                1
    UB      inf4;                       // ���                1
    INT     siz;                        //                     4
    UB      data[CMD_BUFSIZ];           // �f�[�^(stx.....bcc) 60
}T_MSG_DAT;

// �R�}���h��R�}���h�����o�b�t�@
typedef struct {
    T_MSG       msghead;                // ���b�Z�[�W�w�b�_
	T_MSG_DAT   msgdata;
} T_CMD_MSG;

//*************************************
// �ʐM�f�[�^
//*************************************
										
typedef union {
    struct {
        B   type;                       // ��M�f�[�^�^�C�v
        B   data;                       // ��M�f�[�^
        B   portno;                     // �ʐM�ԍ�
        B   reserved1;                  // �\���P
    } byte;
    VP_INT  data;
} T_COM_CHAR;

//*************************************
// SIO�ʐM����
//*************************************
typedef struct {
    INT         rstatus;                // ��M���
    INT         sstatus;                // ���M���
    INT         type;                   // �R�}���h�^�C�v
    UB          rbuff[3600];      // ��M�f�[�^
    UB          bcc[2];                 // ��MBCC
    INT         wpnt;                   // ��M�f�[�^�T�C�Y
    INT         sizmax;                 // ��M�f�[�^�T�C�Y
} ST_SEQ;
typedef struct {
    UB          rstatus;                // ��M���
    UB          bcc[2];                 // ��MBCC
    UB          dummy;                  // �_�~�[
    INT         type;                   // �R�}���h�^�C�v
    INT         wpnt;                   // ��M�f�[�^�T�C�Y
} ST_SEQ2;

// *************************************
//  ����PC�ʐM�f�[�^
// *************************************
typedef struct {
	UB		msgid;						// �R�}���h�C���f�b�N�X
	INT		siz;                        // �f�[�^�T�C�Y
	INT		rty;                        // ���g���C�J�E���^�i�������̃��g���C�Ŏg�p�j
	UB      buf[CMD_BUFSIZ];            // �f�[�^�i�R�}���h�^�X�N�����M�^�X�N�j
	UB      **arg[16];
	UB      argnum;
} ST_COMMDAT;

typedef struct {
	UB				bcid;				// 
	ST_COMMDAT		req;			    //�R�}���h���N�G�X�g�i�����s�^�X�N�j
	ST_COMMDAT		rsp;			    //�R�}���h���X�|���X�i�����M�^�X�N�j
} ST_CNTLDATA;

//// *************************************
////  �o�[�R�[�h�ʐM�f�[�^
//// *************************************
//typedef struct {
//	UB		msgid;						// �R�}���h�C���f�b�N�X
//	INT		siz;                        // �f�[�^�T�C�Y
//	INT		rty;                        // ���g���C�J�E���^�i�������̃��g���C�Ŏg�p�j
//	UB      buf[BAR_BUFSIZ];            // �o�[�R�[�h�ǂݎ��f�[�^
//	UB      **arg[16];
//	UB      argnum;
//} ST_COMMDAT_BAR;
//typedef struct {
//	UB				bcid;				// BCID
//	ST_COMMDAT_BAR	req;			    // 
//	ST_COMMDAT_BAR	rsp;			    // �R�}���h���X�|���X�i�����M�^�X�N�j
//} ST_CNTLDATA_BAR;
//
// *************************************
//  ���[�^����p�f�[�^
// *************************************
typedef struct {
	UB	mm;
	UB	st;
	UB	dmy[2];
	INT	pppp;
} ST_ABSMV;

typedef struct {
	UB	mm;																// �f�o�C�X�ԍ��i���[�^�[�ԍ��j
	UB	dr;																// ���~�b�g����
	UB	md;																// ���~�b�g���
	UB	dmy;															// ���U�[�u
	INT	sp;																// �葬�x
} ST_LIMIT;

//typedef struct {
//	UB mm;			// ���[�^�ԍ�	0-15 : �Ώۂ̃��[�^�ԍ�
//	UB dr;			// �ړ�����		CW : ���]����, CCW : �t�]����
//	UB rsv[2];		// reserve
//	INT spd;		// �쓮���x		1 - : ���[�^�̋쓮���x					( �P�� : Hz    )
//	INT orgpls;		// ���_�����o���p���X��		0 - : 						�ypulse�z
//} ST_INIT0;

typedef struct {
	UB	mm1;
	UB	mm2;
	UB	st1;
	UB	st2;
	INT	pppp1;
	INT	pppp2;
} ST_MAP;

typedef struct {
	UB	mm;
	UB	dmy[3];
	INT	tttt;
} ST_PDELAY;

typedef struct {
	UB	mm;
	UB	st;
	UB	dmy[2];
	INT	pppp;
} ST_RELMV;

typedef struct {
	UB	mm;
	UB	dmy[3];
} ST_STOP;

typedef struct {
	UB			req;													// �v�����P�J�E���g�Ď��J�n
	UB			tskid;													// 
	UB			prid1;													// 
	UB			bcid;													// 
	UB			msgid;													// 
	UB			motpare;												// 
	UB			inf;													// 
	UB			inf2;													// 
	UB			msgdat[CMD_BUFSIZ];										// 
	INT			tim;													// �^�C�}�l
	INT			cnt;													// �c����
}ST_MOTWAIT;

typedef struct {
	UB		nn;															// INT�R�}���h     �Ď����̓`���l��
	UB		trg;														// INT�R�}���h     ���̓`���l���g���K�`���l��
	UB		dmy[2];														// INT�R�}���h     �_�~�[
	// [0]   bit7 �E�E�E�E�E�Ebit0
	// [1]   bit15�E�E�E�E�E�Ebit8
	//              �E
	//              �E
	//              �E
	// [7]   bit63�E�E�E�E�E�Ebit56
	UB		mmm[8];														// INT�R�}���h     �ꎞ��~���郂�[�^
	UB		dat[8];														// INT�R�}���h     �o�̓`���l���֏o�͂���l
	UB		msk[8];														// INT�R�}���h     ���荞�ݔ������ɏo�͂���`���l���w��
	INT		EndZ;														// (�\���@�\)�t�ʌ��m���y���E�ʒu
}ST_INTINF;

typedef struct {
	UB			tskid;													// 
	UB			prid1;													// 
	UB			bcid;													// 
	UB			msgid;													// 
	ST_INTINF	intinf;
}ST_INTCNTL;


// SET�R�}���h
typedef struct {
	UB		mm;															// ���[�^�ԍ�
	UB		dr;															// �쓮�^�C�v
	UB		fc;															// ���x�ԍ�
	UB		dmy;														// dummy
	INT		fl;															// ����[Hz](0�`32767) 
	INT		fh;															// �ō���[Hz](0�`32767)
	INT		gl;															// ����������[ms](�P�`5000)
	INT		sc;															// �r���ϑ��̈�[Hz](�P�`16383) 
} ST_SET;


// BOUT�R�}���h
typedef struct {
	UB		nn;		// 
	UB		trg;	// 
	UB		dmy[2];														// �_�~�[
} ST_BOUT;

// BIN
typedef struct {
	UB		nn;		// 
	UB		dmy[3];														// �_�~�[
} ST_BIN;

// CINIT
typedef struct {
	UB		nn;		// 
	UB		f;		// 
	UB		dmy[2];														// �_�~�[
} ST_CINIT;

// COUNT
typedef struct {
	UB	nn;
	UB	dmy[3];
} ST_COUNT;

// ABORT
typedef struct {
	UB		mm;		// 
	UB		dmy[3];														// �_�~�[
} ST_ABORT;

// ENCODER
typedef struct {
	UB		mm;		// 
	UB		ec;		// 
	UB		dtyp;	// 
	UB		dv;		// 
	INT		so;		// 
} ST_ENCODER;

// POWER
typedef struct {
	UB		mm;		// 
	UB		sw;		// 
} ST_POWER;

// PSET
typedef struct {
	UB		mm;		// 
	INT		cnt;	// 
	INT		enc;	// 
	INT		def;	// 
} ST_PSET;

// STATUS
typedef struct {
	UB		mm;		// 
	UB		dmy[3];														// �_�~�[
} ST_STATUS;

// COMOPN
typedef struct {
	UB		ch;		// 
	UB		dd;		// 
	UB		dmy[2];														// �_�~�[
} ST_COMOPN;

// COMSET
typedef struct {
	UB		ch;		// 
	UB		ll;		// 
	UB		ss;		// 
	UB		pp;		// 
	UB		tt;		// 
	UB		dmy[3];														// �_�~�[
	INT		bb;		// 
} ST_COMSET;

// WAIT
typedef struct {
	INT		tm;		// 
} ST_WAIT;

// SCAN
typedef struct {
	UB		mm;
	UB		dr;
	UB		rsv[2];
} ST_SCAN;

// -----------------------------------------------------------------------------------------------

// C-INIT0
typedef struct {
	UB mm;			// ���[�^�ԍ�	0-15 : �Ώۂ̃��[�^�ԍ�
	UB dr;			// �ړ�����		CW : ���]����, CCW : �t�]����
	UB rsv[2];		// reserve
	INT spd;		// �쓮���x		1 - : ���[�^�̋쓮���x					( �P�� : Hz    )
	INT orgpls;		// ���_�����o���p���X��		0 - : 						�ypulse�z
} ST_INIT0;


// C-HDMOV
typedef struct {
	INT Xpos;		// X���W												�ypulse�z
	INT Ypos;		// Y���W												�ypulse�z
	INT Xfl;		// X����
	INT Xfh;		// X�ō���
	INT Xgl;		// X����������
	INT Yfl;		// Y����
	INT Yfh;		// Y�ō���
	INT Ygl;		// Y����������
	INT Zfl;		// Z����
	INT Zfh;		// Z�ō���
	INT Zgl;		// Z����������
} ST_HDMOV;


// C-TPGET
typedef struct {
	INT Xpos1;		// X���W( �m�Y���擾�ʒu[ 1��ڈړ� ]  )				�ypulse�z
	INT Ypos1;		// Y���W( �m�Y���擾�ʒu[ 1��ڈړ� ]  )				�ypulse�z
	INT Z1pos1;		// Z1���W( �`�b�v�擾�O )								�ypulse�z
	INT Z1pos2;		// Z1���W( �`�b�v�擾 )									�ypulse�z
	INT Z2pos1;		// Z2���W( �`�b�v�擾�O )								�ypulse�z
	INT Z2pos2;		// Z2���W( �`�b�v�擾 )									�ypulse�z
	INT PTpos1;		// �s�b�`�ύ��W( �`�b�v�L���m�F )						�ypulse�z
	INT PTpos2;		// �s�b�`�ύ��W( �`�b�v�擾�ʒu )						�ypulse�z
	INT Xfl;		// X����
	INT Xfh;		// X�ō���
	INT Xgl;		// X����������
	INT Yfl;		// Y����
	INT Yfh;		// Y�ō���
	INT Ygl;		// Y����������
	INT Z1fl;		// Z����
	INT Z1fh;		// Z�ō���
	INT Z1gl;		// Z����������
	INT Z2fl;		// Z����( �`�b�v�擾 )
	INT Z2fh;		// Z�ō���( �`�b�v�擾 )
	INT Z2gl;		// Z����������( �`�b�v�擾 )
	INT PTfl;		// �s�b�`�Ϗ���
	INT PTfh;		// �s�b�`�ύō���
	INT PTgl;		// �s�b�`�ω���������
	INT Pfl;		// �|���v����
	INT Pfh;		// �|���v�ō���
	INT Pgl;		// �|���v����������
	INT nn;			// �m�Y��1/2�擾�w�� �m�Y���w����Q�Ɓy0:���Ȃ�, 1:����z
} ST_TPGET;


// C-TPRLS
typedef struct {
	INT Xpos1;		// X���W( �G�W�F�N�g�ʒu )								�ypulse�z
	INT Xpos2;		// X���W( �X���C�h�ʒu )								�ypulse�z
	INT Ypos;		// Y���W( �G�W�F�N�g�ʒu )								�ypulse�z
	INT Z1pos1;		// Z1���W( ���~�ʒu )									�ypulse�z
	INT Z1pos2;		// Z1���W( 1�i�㏸�ʒu )								�ypulse�z
	INT Z2pos1;		// Z2���W( ���~�ʒu )									�ypulse�z
	INT Z2pos2;		// Z2���W( 1�i�㏸�ʒu )								�ypulse�z
	INT PTpos1;		// �s�b�`�ύ��W( �`�b�v�L���m�F )						�ypulse�z
	INT PTpos2;		// �s�b�`�ύ��W( �`�b�v�p���ʒu )						�ypulse�z
	INT Xfl;		// X����
	INT Xfh;		// X�ō���
	INT Xgl;		// X����������
	INT Yfl;		// Y����
	INT Yfh;		// Y�ō���
	INT Ygl;		// Y����������
	INT Z1fl;		// Z���� 
	INT Z1fh;		// Z�ō���
	INT Z1gl;		// Z����������
	INT Z2fl;		// Z����   ( 1�i�㏸���x )	
	INT Z2fh;		// Z�ō���( 1�i�㏸���x )
	INT Z2gl;		// Z����������( 1�i�㏸���x )
	INT PTfl;		// �s�b�`�Ϗ���
	INT PTfh;		// �s�b�`�ύō���
	INT PTgl;		// �s�b�`�ω���������
	INT nn;			// �m�Y��1/2�p���w��       �m�Y���w����Q�Ɓy0:���Ȃ�, 1:����z
} ST_TPRLS;
typedef struct {
	INT n1;
	INT n2;
} ST_TPRLS_RET;


// C-DROPN
typedef struct {
	UB nn;			// �J�w��(0:����A1:�J��)
	UB rsv[3];		// reserve
} ST_DROPN;
typedef struct {
	UB req;
	UB rty;
} ST_DROPC_CTRL;

// C-SKSET
typedef struct {
	INT Xpos    ;	// X���W                          [Pulse]
	INT Ypos    ;	// Y���W                          [Pulse]
	INT Ptpos   ;	// �߯��ύ��W(�����ׯ���)       [Pulse]
	INT vol1    ;	// ɽ��1�z����                    [Pulse]
	INT rsv1    ;	// ɽ��1�]��t��                  [Pulse]
	INT pd1     ;	// ɽ��1�]��߂��t��              [Pulse]
	INT blow1   ;	// ɽ��1��۰��Ĵ���               [Pulse]
	INT trans1  ;	// ɽ��1��ݽ�߰Ĵ���              [Pulse]
	INT sufvol1 ;	// ɽ��1�t�ʌ��m���̋z������      [Pulse]
	INT StartZ1 ;	// ɽ��1�z���J�n����              [Pulse]
	INT Endz1   ;	// ɽ��1�z����ʒu����            [Pulse]
	INT liqinv1 ;	// ɽ��1�t�ʌ��m��������          [Pulse]
	INT prvofs1 ;	// ɽ��1�t�ʂ���̒E�o��          [Pulse]
	INT fwdofs1 ;	// ɽ��1�t�ʂ���̓˂����ݗ�      [Pulse]
	INT escZ1   ;	// ɽ��1�ޔ�����                  [Pulse]
	INT vol2    ;	// ɽ��2�z����                    [Pulse]
	INT rsv2    ;	// ɽ��2�]��t��                  [Pulse]
	INT pd2     ;	// ɽ��2�]��߂��t��              [Pulse]
	INT blow2   ;	// ɽ��2��۰��Ĵ���               [Pulse]
	INT trans2  ;	// ɽ��2��ݽ�߰Ĵ���              [Pulse]
	INT sufvol2 ;	// ɽ��2�t�ʌ��m���̋z������      [Pulse]
	INT StartZ2 ;	// ɽ��2�z���J�n����              [Pulse]
	INT Endz2   ;	// ɽ��2�z����ʒu����            [Pulse]
	INT liqinv2 ;	// ɽ��2�t�ʌ��m��������          [Pulse]
	INT prvofs2 ;	// ɽ��2�t�ʂ���̒E�o��          [Pulse]
	INT fwdofs2 ;	// ɽ��2�t�ʂ���̓˂����ݗ�      [Pulse]
	INT escZ2   ;	// ɽ��2�ޔ�����                  [Pulse]
	INT Xfl     ;	// X����                          [Hz]
	INT Xfh     ;	// X�ō���                        [Hz]
	INT Xgl     ;	// X����������                    [ms]
	INT Yfl     ;	// Y����                          [Hz]
	INT Yfh     ;	// Y�ō���                        [Hz]
	INT Ygl     ;	// Y����������                    [ms]
	INT P1fl    ;	// ����ߏ���(�z��)                [Hz]
	INT P1fh    ;	// ����ߍō���(�z��)              [Hz]
	INT P1gl    ;	// ����߉���������(�z��)          [ms]
	INT P2fl    ;	// ����ߏ���(�t�ʌ��o)            [Hz]
	INT P2fh    ;	// ����ߍō���(�t�ʌ��o)          [Hz]
	INT P2gl    ;	// ����߉���������(�t�ʌ��o)      [ms]
	INT P3fl    ;	// ����ߏ���(�f�o)                [Hz]
	INT P3fh    ;	// ����ߍō���(�f�o)              [Hz]
	INT P3gl    ;	// ����߉���������(�f�o)          [ms]
	INT Z1fl    ;	// Z����(���~/�㏸)               [Hz]
	INT Z1fh    ;	// Z�ō���(���~/�㏸)             [Hz]
	INT Z1gl    ;	// Z����������(���~/�㏸)         [ms]
	INT Z2fl    ;	// Z����(�z�����~/�f�o�㏸)       [Hz]
	INT Z2fh    ;	// Z�ō���(�z�����~/�f�o�㏸)     [Hz]
	INT Z2gl    ;	// Z����������(�z�����~/�f�o�㏸) [ms]
	INT Z3fl    ;	// Z����(�t�ʌ��o)                [Hz]
	INT Z3fh    ;	// Z�ō���(�t�ʌ��o)              [Hz]
	INT Z3gl    ;	// Z����������(�t�ʌ��o)          [ms]
	INT Z4fl    ;	// Z����(�t�z����㏸)            [Hz]
	INT Z4fh    ;	// Z�ō���(�t�z����㏸)          [Hz]
	INT Z4gl    ;	// Z����������(�t�z����㏸)      [ms]
	INT PTfl    ;	// �߯��Ϗ���                   [Hz]
	INT PTfh    ;	// �߯��ύō���                 [Hz]
	INT PTgl    ;	// �߯��ω����x                 [ms]
	INT suf     ;	// �t�ʌ��m�ݒ�                   [-]
	INT suckwt  ;	// �z���㳪�Ď���                 [ms]
	INT dotwt   ;	// �f�o�㳪�Ď���                 [ms]
	INT El      ;	// �z���װ���o�ݒ�                [-]
} ST_SKSET;


// C-SUCK0
typedef struct {
	UB nn;			// �m�Y��1/2�z���w�� �m�Y���w����Q�Ɓy0:���Ȃ�, 1:����z
	UB rsv[3];		// reserve
} ST_SUCK0;
typedef struct {
	ER n1;			// �m�Y��1����
	INT z1_liq;		// �m�Y��1�t�ʌ��m����
	INT z1_suc;		// �m�Y��1�z������
	ER n2;			// �m�Y��2����
	INT z2_liq;		// �m�Y��2�t�ʌ��m����
	INT z2_suc;		// �m�Y��2�z������
	UB inf;			// BIT0 = 1:�m�Y��1�t�ʌ��m�ς�
					// BIT1 = 1:�m�Y��2�t�ʌ��m�ς�
					// BIT2 = 1:
					// BIT3 = 1:
					// BIT4 = 1:
					// BIT5 = 1:
	UB rsv[3];		// reserve
} ST_SUCK0_RET;


// C-DTSET
typedef struct {
	INT Xpos    ;	// X���W                          [Pulse]
	INT Ypos    ;	// Y���W                          [Pulse]
	INT Ptpos   ;	// �߯��ύ��W(ð��ٌ��߯�)      [Pulse]
	INT vol1    ;	// ɽ��1�f�o��                    [Pulse]
	INT cut1    ;	// ɽ��1�t�؂�z������            [Pulse]
	INT Dotz1   ;	// ɽ��1�f�o����                  [Pulse]
	INT cutz1   ;	// ɽ��1�t�؂�z������            [Pulse]
	INT Touchz1 ;	// ɽ��1�����������               [Pulse]
	INT vol2    ;	// ɽ��2�f�o��                    [Pulse]
	INT cut2    ;	// ɽ��2�t�؂�z������            [Pulse]
	INT Dotz2   ;	// ɽ��2�f�o����                  [Pulse]
	INT cutz2   ;	// ɽ��2�t�؂�z������            [Pulse]
	INT Touchz2 ;	// ɽ��2�����������               [Pulse]
	INT Xfl     ;	// X����                          [Hz]
	INT Xfh     ;	// X�ō���                        [Hz]
	INT Xgl     ;	// X����������                    [ms]
	INT Yfl     ;	// Y����                          [Hz]
	INT Yfh     ;	// Y�ō���                        [Hz]
	INT Ygl     ;	// Y����������                    [ms]
	INT P1fl    ;	// ����ߏ���(�f�o)                [Hz]
	INT P1fh    ;	// ����ߍō���(�f�o)              [Hz]
	INT P1gl    ;	// ����߉���������(�f�o)          [ms]
	INT P2fl    ;	// ����ߏ���(�z��)                [Hz]
	INT P2fh    ;	// ����ߍō���(�z��)              [Hz]
	INT P2gl    ;	// ����߉���������(�z��)          [ms]
	INT Z1fl    ;	// Z����(���~/�㏸)               [Hz]
	INT Z1fh    ;	// Z�ō���(���~/�㏸)             [Hz]
	INT Z1gl    ;	// Z����������(���~/�㏸)         [ms]
	INT Z2fl    ;	// Z����(�f�o�㏸)                [Hz]
	INT Z2fh    ;	// Z�ō���(�f�o�㏸)              [Hz]
	INT Z2gl    ;	// Z����������(�f�o�㏸)          [ms]
	INT Z3fl    ;	// Z����(�������)                 [Hz]
	INT Z3fh    ;	// Z�ō���(�������)               [Hz]
	INT Z3gl    ;	// Z����������(�������)           [ms]
	INT PTfl    ;	// �߯��Ϗ���                   [Hz]
	INT PTfh    ;	// �߯��ύō���                 [Hz]
	INT PTgl    ;	// �߯��ω���������             [ms]
	INT dotwt   ;	// �f�o�㳪�Ď���                 [ms]
	INT suckwt  ;	// �z���㳪�Ď���                 [ms]
	INT Add1st  ;	// 1��ړf�o�␳��                [Pulse]
	INT El      ;	// �f�o�װ���o�ݒ�                [-]
} ST_DTSET;


// C-DOT00
typedef struct {
	UB nn;			// �m�Y��1/2�z���w�� �m�Y���w����Q�Ɓy0:���Ȃ�, 1:����z
	UB rsv[3];		// reserve
} ST_DOT00;
typedef struct {
	INT n1;
	INT n2;
	UB z1_liq;
	UB z2_liq;
	UB z1_dot;
	UB z2_dot;
} ST_DOT00_RET;


// C-MXSET
typedef struct {
	INT Xpos   ;	// X���W                      �yPulse�z
	INT Ypos   ;	// Y���W                      �yPulse�z
	INT Ptpos  ;	// �߯��ύ��W(ð��ٌ��߯�)  �yPulse�z
	INT Z1pos1 ;	// ɽ��1Z���W(�z������)       �yPulse�z
	INT Z1pos2 ;	// ɽ��1Z���W(�f�o����)       �yPulse�z
	INT Z1pos3 ;	// ɽ��1Z���W(��۰��č���)    �yPulse�z
	INT Z1pos4 ;	// ɽ��1Z���W(�z���J�n����)   �yPulse�z
	INT Z1pos5 ;	// ɽ��1Z���W(�z���I������)   �yPulse�z
	INT Touchz1;	// ɽ��1�����������           �yPulse�z
	INT Cutz1  ;	// ɽ��1�t�؂�z������        �yPulse�z
	INT vol1   ;	// ɽ��1�z���f�o��            �yPulse�z
	INT cut1   ;	// ɽ��1�t�؂�z������        �yPulse�z
	INT blow1  ;	// ɽ��1��۰��Ĵ���           �yPulse�z
	INT advol1 ;	// ɽ��1�h�a�]���            �yPulse�z
	INT Z2pos1 ;	// ɽ��2Z���W(�z������)       �yPulse�z
	INT Z2pos2 ;	// ɽ��2Z���W(�f�o����)       �yPulse�z
	INT Z2pos3 ;	// ɽ��2Z���W(��۰��č���)    �yPulse�z
	INT Z2pos4 ;	// ɽ��4Z���W(�z���J�n����)   �yPulse�z
	INT Z2pos5 ;	// ɽ��5Z���W(�z���I������)   �yPulse�z
	INT Touchz2;	// ɽ��2�����������           �yPulse�z
	INT Cutz2  ;	// ɽ��2�t�؂�z������        �yPulse�z
	INT vol2   ;	// ɽ��2�z���f�o��            �yPulse�z
	INT cut2   ;	// ɽ��2�t�؂�z������        �yPulse�z
	INT blow2  ;	// ɽ��2��۰��Ĵ���           �yPulse�z
	INT advol2 ;	// ɽ��2�h�a�]���            �yPulse�z
	INT Num    ;	// �h�a��                   �y��z
	INT Xfl    ;	// X����                      �yHz�z
	INT Xfh    ;	// X�ō���                    �yHz�z
	INT Xgl    ;	// X����������                �yms�z
	INT Yfl    ;	// Y����                      �yHz�z
	INT Yfh    ;	// Y�ō���                    �yHz�z
	INT Ygl    ;	// Y����������                �yms�z
	INT P1fl   ;	// ����ߏ���(�z��)            �yHz�z
	INT P1fh   ;	// ����ߍō���(�z��)          �yHz�z
	INT P1gl   ;	// ����߉���������(�z��)      �yms�z
	INT P2fl   ;	// ����ߏ���(�f�o)            �yHz�z
	INT P2fh   ;	// ����ߍō���(�f�o)          �yHz�z
	INT P2gl   ;	// ����߉���������(�f�o)      �yms�z
	INT Z1fl   ;	// Z����(���~/�㏸)           �yHz�z
	INT Z1fh   ;	// Z�ō���(���~/�㏸)         �yHz�z
	INT Z1gl   ;	// Z����������(���~/�㏸)     �yms�z
	INT Z2fl   ;	// Z����(�z�����~)            �yHz�z
	INT Z2fh   ;	// Z�ō���(�z�����~)          �yHz�z
	INT Z2gl   ;	// Z����������(�z�����~)      �yms�z
	INT Z3fl   ;	// Z����(�f�o�㏸)            �yHz�z
	INT Z3fh   ;	// Z�ō���(�f�o�㏸)          �yHz�z
	INT Z3gl   ;	// Z����������(�f�o�㏸)      �yms�z
	INT PTfl   ;	// �߯��Ϗ���               �yHz�z
	INT PTfh   ;	// �߯��ύō���             �yHz�z
	INT PTgl   ;	// �߯��ω���������         �yms�z
	INT suckwt ;	// �z���㳪�Ď���             �yms�z
	INT dotwt  ;	// �f�o�㳪�Ď���             �yms�z
	INT El     ;	// �z���װ���o�ݒ�            �y- �z
} ST_MXSET;


// C-MIX00
typedef struct {
	UB nn;			// �m�Y��1/2�z���w�� �m�Y���w����Q�Ɓy0:���Ȃ�, 1:����z
	UB rsv[3];		// reserve
} ST_MIX00;
typedef struct {
	ER n1;
	ER n2;
	UB cnt;			// �h�a��
	UB inf;			// BIT0 = 1:
					// BIT1 = 1:
					// BIT2 = 1:
					// BIT3 = 1:
} ST_MIX00_RET;


// C-THMOV
typedef struct {
	INT Act;	// ����w��
	INT Xpos;	// X���W( �s�b�N�A�b�v�^�����[�X�^�ʒu�ړ� )
	INT Ypos;	// Y���W( �s�b�N�A�b�v�^�����[�X�^�ʒu�ړ� )
	INT Zpos1;	// Z���W( �s�b�N�A�b�v�^�����[�X�^�ʒu�ړ� )
	INT Zpos2;	// Z���W( �ړ����� )
	INT Zpos3;	// Z���W( �����[�X�㍂�� )
	INT Rpos;	// R���W( �s�b�N�A�b�v�^�����[�X�^�ʒu�ړ� )
	INT Xfl;	// X����
	INT Xfh;	// X�ō���
	INT Xgl;	// X����������
	INT Yfl;	// Y����
	INT Yfh;	// Y�ō���
	INT Ygl;	// Y����������
	INT Z1fl;	// Z1����
	INT Z1fh;	// Z1�ō���
	INT Z1gl;	// Z1����������
	INT Z2fl;	// Z2����
	INT Z2fh;	// Z2�ō���
	INT Z2gl;	// Z2����������
	INT Rfl;	// R����
	INT Rfh;	// R�ō���
	INT Rgl;	// R����������
	INT Delay;	// �����n���h�����f�B���C����[ms]
	INT Rout;	// XY���H( 0:X->Y , 1:Y->X )
} ST_THMOV;


// C-CPOPN/C-CPCLS
typedef struct {
	INT Act    ; // �J�w��(0:�J���Ȃ�,1:CAP1,2:CAP2,3:BOTH)
	INT ZposS  ; // �J��Z�� �J���J�n����           [Pls]
	INT ZposE  ; // �J��Z�� �J���I������           [Pls]
	INT R1posS ; // �J��R1�� �J���J�n�ʒu          [Pls]
	INT R1posE ; // �J��R1�� �J���I���ʒu          [Pls]
	INT R2posS ; // �J��R1�� �J���J�n�ʒu          [Pls]
	INT R2posE ; // �J��R1�� �J���I���ʒu          [Pls]
	INT Zcap   ; // �J��Z�� �L���b�v�L����           [Pls]
	INT Tpend  ; // �˂�����_�I���ʒu                 [Pls]
	INT Z2posS ; // �J��Z�� �J����2�i����            [Pls]
	INT Patn   ; // 0:�_���c��,1=�c�����_,2=�_���c���� [Pls]
	INT Znmfl  ; // �J��Z�� �ʏ푬�x ����             [Hz]
	INT Znmfh  ; // �J��Z�� �ʏ푬�x �ō���           [Hz]
	INT Znmgl  ; // �J��Z�� �ʏ푬�x ����������       [ms]
	INT Zopfl  ; // �J��Z�� �J�������x ����         [Hz]
	INT Zopfh  ; // �J��Z�� �J�������x �ō���       [Hz]
	INT Zopgl  ; // �J��Z�� �J�������x ����������   [ms]
	INT R1fl   ; // �J��R1�� �J�������x ����        [Hz]
	INT R1fh   ; // �J��R1�� �J�������x �ō���      [Hz]
	INT R1gl   ; // �J��R1�� �J�������x ����������  [ms]
	INT R2fl   ; // �J��R2�� �J�������x ����        [Hz]
	INT R2fh   ; // �J��R2�� �J�������x �ō���      [Hz]
	INT R2gl   ; // �J��R2�� �J�������x ����������  [ms]
	INT Tpfl   ; // �˂�����_�� ���x ����              [Hz]
	INT Tpfh   ; // �˂�����_�� ���x �ō���            [Hz]
	INT Tpgl   ; // �˂�����_�� ���x ����������        [Hz]
	INT Z2fl   ; // �J��Z�� �J����2�i���x ����      [Hz]
	INT Z2fh   ; // �J��Z�� �J����2�i���x �ō���    [Hz]
	INT Z2gl   ; // �J��Z�� �J����2�i���x ����������[ms]
} ST_CPOPN;

typedef struct {
	UB nn;
	UB inf;			// BIT0 = 1: �J��R1
					// BIT1 = 1: �J��R2
					// BIT2 = 1: �J��Z
					// BIT3 = 1: �˂��o���_
	UB base;
	UB rsv[1];		// reserve
} ST_CAP_RET;


// C-BCDSP
typedef struct {
	INT Rno   ;	// ���b�N�ʒu(1�`4)
	INT Bpos  ;	// �o�[�R�[�h�� �ړ���              [Pls]
	INT Bfl   ;	// �o�[�R�[�h�� ���쑬�x ����       [Hz]
	INT Bfh   ;	// �o�[�R�[�h�� ���쑬�x �ō���     [Hz]
	INT Bgl   ;	// �o�[�R�[�h�� ���쑬�x ���������� [ms]
	INT Tim   ;	// �o�[�R�[�h�ǎ��^�C���A�E�g     [ms]
} ST_BCDSP;

// �o�[�R�[�h�ǂݎ��f�[�^
typedef struct {
	INT  Pos;				// �|�W�V����ID
	char Id[TUBE_LEN16];	// �`���[�uID
} ST_BCDTB;

typedef struct {
	char RackId[8];	
	ST_BCDTB Tub[12];
} ST_BCDRD;



// C-TURN1
typedef struct {
	INT Act   ; 	// ����w��
	INT Rsrv1 ; 	// �\��(0�Œ�)
	INT Tpos1 ; 	// �^�[���e�[�u���� �ړ���              [Pls]
	INT Tfl   ; 	// �^�[���e�[�u���� ���쑬�x ����       [Hz]
	INT Tfh   ; 	// �^�[���e�[�u���� ���쑬�x �ō���     [Hz]
	INT Tgl   ; 	// �^�[���e�[�u���� ���쑬�x ���������� [ms]
} ST_TURN1;


// C-BZCNT
typedef struct {
	UB Act;			// �p�g���C�g����   ( 0:����, 1:�_��,   2:�_�� )
	UB Colptn;      // �p�g���C�g�J���[ ( 0:��    1:��,     2:��   )
	UB Buzptn;      // �u�U�[           ( 0:OFF,  1:�u�U�[         )
	UB rsv[1];		// reserve
} ST_BZCNT;


// C-LEDON
typedef struct {
	UB nn;			// �o�̓|�[�g�ԍ�		1 -
	UB act;			// �o�̓��[�h			(0:����, 1:�_��, 2:�_��)
	UB rsv[2];		// reserve
} ST_LEDON;



//
// === cyc handler =====================================================
//
typedef struct {
	UB			devno;						// �f�o�C�X�ԍ�
	UB			portno;						// �|�[�g�ԍ�
}ST_CH;

typedef struct {
		UB	cnt;
		UB	dat;
}IOCH;


//
// === tskio ===========================================================
//
// �J�E���^�Ǘ�
typedef struct {
	UB			sts;			// 1:�J�E���g���A0:�ҋ@��
	UB			old;			// 
	UB			now;			// 
	UB			rsv;			// �\��
	UINT		cnt;			// 
}ST_IOCOUNT;


//
// === tskcmd ==========================================================
//
// �f�[�^�`�F�b�N�p�����W
typedef struct {
	INT	min;
	INT	max;
}ST_RANGE;

// �f�[�^�`�F�b�N�Ǘ�
typedef struct {
	UB			argnum;
	ST_RANGE	range[MAX_ARGS];
}ST_DATACHK;


//typedef struct {
//	INT Xpos;	// [X��]�ړ��ʒu�i�p���X�j
//	INT Ypos;	// [Y��]�ړ��ʒu�i�p���X�j
//	INT Zpos1;	// [Z1��]�ړ��ʒu�i�p���X�j
//	INT Zpos2;	// [Z2��]�ړ��ʒu�i�p���X�j
//}ST_HDMOV;

// ���쐧��f�[�^
typedef struct {
	UB	msgid;
	UB	devno;
	UB	actstp;
	UB	prid1;	
	UB	inf;		// �ėp���
	UB	inf2;		// �ėp���
	UB	inf3;		// �ėp���
	UB	inf4;		// �ėp���
}ST_BCCTRL;


//
// === tskmotact =======================================================
//
typedef struct {
	UB		cmdsts;		// �R�}���h�i�s��� 0xff:IDLE,0x01:ACTTION,0x02:END
	UB		mot;		// ���[�^�[�ԍ��iMAP�Ή��̂��߂Q���[�^���j
	UB		msg;
}ST_CMDLIST;


//
// === tskeep =======================================================
//

typedef struct {
	INT		no;			// 
	INT		siz;		// 

#if ( EEP_RWSIZE_TYPE == 0 )
	UB		buf[EEP_RWSIZE];
	UB		dmy2[7];
#else
	UB		buf[EEP_RWSIZE];
#endif

}ST_EEPBUF;


//
// === tskled =======================================================
//
typedef struct{
	UB sts;
	UB cnt;
	UB dmy[2];
}ST_LED_STS;



//
// === tp =======================================================
//

typedef struct {
	INT	siz;
	UB	req[44];
}TPN_REQ_DAT;

typedef struct {
    UB Id[8];
    UB Pw[4];
} ST_USERINFO;

typedef struct {
    unsigned short  ac;
    unsigned short  gl;
    INT    fl;
    INT    fh;
} ST_SPEED;

typedef struct {//25
	UB Y0_SpdIniPtn;
	UB Z0_SpdIniPtn;
	UB P0_SpdIniPtn;
	UB Y1_SpdIniPtn;
	UB F0_SpdIniPtn;
	UB Y0_SpdNmlPtn;
	UB Z0_SpdNmlPtn;
	UB P0_SpdNmlPtn;
	UB Y1_SpdNmlPtn;
	UB F0_SpdNmlPtn;
	UB P0_SpdMovAirS;
	UB P0_SpdMovAirP;
	UB Z0_SpdTipGetDownE;
	UB Z0_SpdTipGetDown1;
	UB Z0_SpdTipDelDownE;
	UB Z0_SpdTipDelDown1;
	UB Z0_SpdTipDownE;
	UB Z0_SpdTipDown1;
	UB Y0_SpdSucMov;
	UB Y0_SpdPutMov;
	UB Z0_SpdPutDown;
	UB Z0_SpdSucDown;
	UB P0_SpdPut;
	UB P0_SpdSuc;
	UB P0_SpdTipDel;
	UB Reseve[43];
} ST_SPEED_PTN;

typedef struct {
	INT  sts; // ���
	UB no;  // �v���g�R���ԍ�
	UB step;// �����X�e�b�v
	UB proc;// �H��
	UB resv;// �\��
} ST_PROTOCOL_STS;

// --- Page 0 ---
typedef struct {
	ST_PROTOCOL_STS prot;// 8
	UB Reseve[120];		 // 120
} ST_PAGE0;

// --- Page 1 ---
typedef struct {
	ST_USERINFO User[10];
	UB Reseve[8];
} ST_PAGE1;

// --- Page 2 ---
typedef struct {
    ST_SPEED	Y0[5];//60
    ST_SPEED	Z0[5];//60
	UB Reseve[8];
} ST_PAGE2;

// --- Page 3 ---
typedef struct {
    ST_SPEED	P0[5];//60
    ST_SPEED	F0[5];//60
	UB Reseve[8];
} ST_PAGE3;

// --- Page 4 ---
typedef struct {
    ST_SPEED	  Y1[5];//60
	ST_SPEED_PTN  SpPtn;//68
} ST_PAGE4;

// --- Page 5 ---
typedef struct {
	INT  Y0_Offset;        // [Y0] ���z���_�I�t�Z�b�g�l                  �|�W�V�����ԍ��P  
	INT  Z0_Offset;        // [Z0] ���z���_�I�t�Z�b�g�l                  �|�W�V�����ԍ��Q  
	INT  P0_Offset;        // [P0] ���z���_�I�t�Z�b�g�l                  �|�W�V�����ԍ��R  
	INT  F0_Offset;        // [F0] ���z���_�I�t�Z�b�g�l                  �|�W�V�����ԍ��S  
	INT  Y1_Offset;        // [Y1] ���z���_�I�t�Z�b�g�l                  �|�W�V�����ԍ��T  
	INT  Y0_Taihi;         // [Y0] �ޔ��ʒu                              �|�W�V�����ԍ��U  
	INT  Z0_Taihi;         // [Z0] �ޔ��ʒu                              �|�W�V�����ԍ��V  
	INT  P0_Taihi;         // [P0] �ޔ��ʒu                              �|�W�V�����ԍ��W  
	INT  F0_Taihi;         // [Y1] �ޔ��ʒu                              �|�W�V�����ԍ��X  
	INT  Y1_Taihi;         // [F0] �ޔ��ʒu                              �|�W�V�����ԍ��P�O
	INT  StripTp1;         // [Y0] ����X�g���b�v-�V���[�g�`�b�v�P       �|�W�V�����ԍ��P�P
	INT  StripTp2;         // [Y0] ����X�g���b�v-�V���[�g�`�b�v�Q       �|�W�V�����ԍ��P�Q
	INT  StripTpL;         // [Y0] ����X�g���b�v-�����O�`�b�v           �|�W�V�����ԍ��P�R
	INT  StripTub;         // [Y0] ����X�g���b�v-�T���v���`���[�u       �|�W�V�����ԍ��P�S
	INT  StripSm6;         // [Y0] ����X�g���b�v-���o�p����6            �|�W�V�����ԍ��P�T
	INT  StripSm5;         // [Y0] ����X�g���b�v-���o�p����5            �|�W�V�����ԍ��P�U
	INT  StripSm4;         // [Y0] ����X�g���b�v-���o�p����4            �|�W�V�����ԍ��P�V
	INT  StripSm3;         // [Y0] ����X�g���b�v-���o�p����3            �|�W�V�����ԍ��P�W
	INT  StripSm2;         // [Y0] ����X�g���b�v-���o�p����2            �|�W�V�����ԍ��P�X
	INT  StripSm1;         // [Y0] ����X�g���b�v-���o�p����1            �|�W�V�����ԍ��Q�O
	INT  StripCat;         // [Y0] ����X�g���b�v-�J�[�g���b�W           �|�W�V�����ԍ��Q�P
	INT  StripCol;         // [Y0] ����X�g���b�v-�j�_����t             �|�W�V�����ԍ��Q�Q
	INT  Z0_StripOpnE;     // [Z0] ����X�g���b�v-�V�[���J���ŏI�ʒu     �|�W�V�����ԍ��Q�R
	INT  Z0_StripOpn1;     // [Z0] ����X�g���b�v-�V�[���J���P�i�ʒu     �|�W�V�����ԍ��Q�S
	INT  Z0_StripPres;     // [Z0] ����X�g���b�v-�����ʒu               �|�W�V�����ԍ��Q�T
	INT  Z0_TipGetE;       // [Z0] �`�b�v�擾�ŏI�ʒu                    �|�W�V�����ԍ��Q�U
	INT  Z0_TipGet1;       // [Z0] �`�b�v�擾�P�i�ʒu                    �|�W�V�����ԍ��Q�V
	INT  Z0_TipDelE;       // [Z0] �`�b�v�p���ŏI�ʒu                    �|�W�V�����ԍ��Q�W
	INT  Z0_TipDel1;       // [Z0] �`�b�v�p���P�i�ʒu                    �|�W�V�����ԍ��Q�X
	INT  Z0_LiqPut;        // [Z0] �p�t�f�o�ʒu                          �|�W�V�����ԍ��R�O
	INT  Y1_ColTube;       // [Y1] �R���N�V��������`���[�u�ʒu          �|�W�V�����ԍ��R�P
	INT  Y1_ColDspTube;    // [Y1] �p�t�`���[�u�ʒu                      �|�W�V�����ԍ��R�Q
} ST_PAGE7;

// --- Page 6 ---
typedef struct {
	INT  Z0_SucStaHei1;    // [Z0] �z���J�n����1                         �|�W�V�����ԍ��R�R
	INT  Z0_SucStaHei2;    // [Z0] �z���J�n����2                         �|�W�V�����ԍ��R�S
	INT  Z0_SucStaHei3;    // [Z0] �z���J�n����3                         �|�W�V�����ԍ��R�T
	INT  Z0_SucStaHei4;    // [Z0] �z���J�n����4                         �|�W�V�����ԍ��R�U
	INT  Z0_SucStaHei5;    // [Z0] �z���J�n����5                         �|�W�V�����ԍ��R�V
	INT  Z0_SucEndHei1;    // [Z0] �z���I������1                         �|�W�V�����ԍ��R�W
	INT  Z0_SucEndHei2;    // [Z0] �z���I������2                         �|�W�V�����ԍ��R�X
	INT  Z0_SucEndHei3;    // [Z0] �z���I������3                         �|�W�V�����ԍ��S�O
	INT  Z0_SucEndHei4;    // [Z0] �z���I������4                         �|�W�V�����ԍ��S�P
	INT  Z0_SucEndHei5;    // [Z0] �z���I������5                         �|�W�V�����ԍ��S�Q
	INT  Z0_PutStaHei1;    // [Z0] �f�o�J�n����1                         �|�W�V�����ԍ��S�R
	INT  Z0_PutStaHei2;    // [Z0] �f�o�J�n����2                         �|�W�V�����ԍ��S�S
	INT  Z0_PutStaHei3;    // [Z0] �f�o�J�n����3                         �|�W�V�����ԍ��S�T
	INT  Z0_PutStaHei4;    // [Z0] �f�o�J�n����4                         �|�W�V�����ԍ��S�U
	INT  Z0_PutStaHei5;    // [Z0] �f�o�J�n����5                         �|�W�V�����ԍ��S�V
	INT  Z0_PutEndHei1;    // [Z0] �f�o�I������1                         �|�W�V�����ԍ��S�W
	INT  Z0_PutEndHei2;    // [Z0] �f�o�I������2                         �|�W�V�����ԍ��S�X
	INT  Z0_PutEndHei3;    // [Z0] �f�o�I������3                         �|�W�V�����ԍ��T�O
	INT  Z0_PutEndHei4;    // [Z0] �f�o�I������4                         �|�W�V�����ԍ��T�P
	INT  Z0_PutEndHei5;    // [Z0] �f�o�I������5                         �|�W�V�����ԍ��T�Q
	INT  Z0_MixStaSuc;     // [Z0] ���a�z���J�n����                      �|�W�V�����ԍ��T�R
	INT  Z0_MixEndSuc;     // [Z0] ���a�z���I������                      �|�W�V�����ԍ��T�S
	INT  P0_TpDispose;     // [P0]�p������                               �|�W�V�����ԍ��T�T
	INT  XX_Reserve56;     // 0�Œ�                                      �|�W�V�����ԍ��T�U
	INT  XX_Reserve57;     // 0�Œ�                                      �|�W�V�����ԍ��T�V
	INT  XX_Reserve58;     // 0�Œ�                                      �|�W�V�����ԍ��T�W
	INT  XX_Reserve59;     // 0�Œ�                                      �|�W�V�����ԍ��T�X
	INT  XX_Reserve60;     // 0�Œ�                                      �|�W�V�����ԍ��U�O
	INT  XX_Reserve61;     // 0�Œ�                                      �|�W�V�����ԍ��U�P
	INT  XX_Reserve62;     // 0�Œ�                                      �|�W�V�����ԍ��U�Q
	INT  XX_Reserve63;     // 0�Œ�                                      �|�W�V�����ԍ��U�R
	INT  XX_Reserve64;     // 0�Œ�                                      �|�W�V�����ԍ��U�S
} ST_PAGE8;

// --- Page 7 ---
typedef struct {
	INT  HS_VL1;         // �␳�p�^�[���P�@�e��10%                    �|�W�V�����ԍ��U�T  �V�P  �V�V
	INT  HS_VL2;         //   �e��50%                                  �|�W�V�����ԍ��U�U  �V�Q  �V�W
	INT  HS_VL3;         //   �e��100%                                 �|�W�V�����ԍ��U�V  �V�R  �V�X
	INT  HS_Out;         //   �u���[�A�E�g�G�A��                       �|�W�V�����ԍ��U�W  �V�S  �W�O
	INT  HS_Air;         //   �G�A�M���b�v��                           �|�W�V�����ԍ��U�X  �V�T  �W�P
	INT  HS_Tra;         //   �]���                                   �|�W�V�����ԍ��V�O  �V�U  �W�Q
} ST_HOSEI_PTN;

// --- Page 8 ---
typedef struct {
	ST_HOSEI_PTN Hosei[3];
// --- Page 9 ---
// 	INT  H1_C1Vo2;         //   �e��50%                                  �|�W�V�����ԍ��U�U
// 	INT  H1_C1Vo3;         //   �e��100%                                 �|�W�V�����ԍ��U�V
// 	INT  H1_C1Out;         //   �u���[�A�E�g�G�A��                       �|�W�V�����ԍ��U�W
// 	INT  H1_C1Air;         //   �G�A�M���b�v��                           �|�W�V�����ԍ��U�X
// 	INT  H1_C1Tra;         //   �]���                                   �|�W�V�����ԍ��V�O
// 	INT  H1_C2Vo1;         // �␳�p�^�[��2�@�e��10%                     �|�W�V�����ԍ��V�P
// 	INT  H1_C2Vo2;         //   �e��50%                                  �|�W�V�����ԍ��V�Q
// 	INT  H1_C2Vo3;         //   �e��100%                                 �|�W�V�����ԍ��V�R
// 	INT  H1_C2Out;         //   �u���[�A�E�g�G�A��                       �|�W�V�����ԍ��V�S
// 	INT  H1_C2Air;         //   �G�A�M���b�v��                           �|�W�V�����ԍ��V�T
// 	INT  H1_C2Tra;         //   �]���                                   �|�W�V�����ԍ��V�U
// 	INT  H1_C3Vo1;         // �␳�p�^�[��3�@�e��10%                     �|�W�V�����ԍ��V�V
// 	INT  H1_C3Vo2;         //   �e��50%                                  �|�W�V�����ԍ��V�W
// 	INT  H1_C3Vo3;         //   �e��100%                                 �|�W�V�����ԍ��V�X
// 	INT  H1_C3Out;         //   �u���[�A�E�g�G�A��                       �|�W�V�����ԍ��W�O
// 	INT  H1_C3Air;         //   �G�A�M���b�v��                           �|�W�V�����ԍ��W�P
// 	INT  H1_C3Tra;         //   �]���                                   �|�W�V�����ԍ��W�Q
	INT  XX_Reserve83;     // 0�Œ�                                      �|�W�V�����ԍ��W�R
	INT  XX_Reserve84;     // 0�Œ�                                      �|�W�V�����ԍ��W�S
	INT  XX_Reserve85;     // 0�Œ�                                      �|�W�V�����ԍ��W�T
	INT  XX_Reserve86;     // 0�Œ�                                      �|�W�V�����ԍ��W�U
	INT  XX_Reserve87;     // 0�Œ�                                      �|�W�V�����ԍ��W�V
	INT  XX_Reserve88;     // 0�Œ�                                      �|�W�V�����ԍ��W�W
	INT  XX_Reserve89;     // 0�Œ�                                      �|�W�V�����ԍ��W�X
	INT  XX_Reserve90;     // 0�Œ�                                      �|�W�V�����ԍ��X�O
	INT  XX_Reserve91;     // 0�Œ�                                      �|�W�V�����ԍ��X�P
	INT  XX_Reserve92;     // 0�Œ�                                      �|�W�V�����ԍ��X�Q
	INT  XX_Reserve93;     // 0�Œ�                                      �|�W�V�����ԍ��X�R
	INT  XX_Reserve94;     // 0�Œ�                                      �|�W�V�����ԍ��X�S
	INT  XX_Reserve95;     // 0�Œ�                                      �|�W�V�����ԍ��X�T
	INT  XX_Reserve96;     // 0�Œ�                                      �|�W�V�����ԍ��X�U
} ST_PAGE9;

// --- Page 10 ---
typedef struct {
	UB Login;            // 
	UB Reseve[127];      // 
} ST_PAGE10;

// --- Page 90 ---
typedef struct {
//    UB Pno;	// �v���g�R���ԍ�
    UB Cnt;	// �g�[�^���X�e�b�v���i 1 �` 80 �j
    UB Prc;	// �H���L��
    UB Stp;	// �����X�e�b�v
    UB Dmy;	// 
} ST_PROTCOL_DATA;

typedef struct {
	ST_PROTCOL_DATA Dat[20];
	UB Reseve[48];
} ST_PROTCOL_INFO;

typedef struct {
	UB Tit[32];
} ST_PROTCOL_TITLE;
	
// --- Page 100 ---
typedef struct {// 20byte
    UB StCmd;
    UB par02;
    UB par03;
    UB par04;
    UB par05;
    UB par06;
    UB par07;
    UB par08;
    UB par09;
    UB par10;
    UB par11;
    UB par12;
    INT  par13;
    INT  par14;
} ST_PROTCOL_STEP;

typedef struct	{
	ST_PROTCOL_STEP     step[80];// 20*80=1600
} ST_PROTCOL ;

typedef struct {
	INT  sts;		// ���u���
	UB pno;		// �v���g�R���ԍ�
	UB step;		// �v���g�R�����쒆�̏ڍ׃X�e�b�v
	UB cmd;		// �v���g�R�����쒆�̏������(LYSIS,BAIDING,WASH,ELUTE)
	UB dat1;		// �\��
} ST_DEVICE_INFO;

typedef struct	StPulseMotor{
	// 0x0�`0x3
	union	{
		struct	{
			UB	COMB0;						// [W] ����R�}���h
			UB	COMB1;						// [W] ���w��i����R�}���h�̎��s���̎w��j
			UB	OTPB;						// [W] �ėp�o�̓|�[�g�̏�ԕύX
		} stWrite;
		struct	{
			UB	MSTSB0;						// [R] ���C���X�e�[�^�X�i�r�b�g 7�` 0�j
			UB	MSTSB1;						// [R] ���C���X�e�[�^�X�i�r�b�g15�` 8�j
			UB	IOPB;						// [R] �ėp���o�̓|�[�g�̓ǂݏo��
			UB	SSTSB;						// [R] �T�u�X�e�[�^�X�̓ǂݏo��
		} stRead;
	} unCmdSts;
	// 0x4�`0x7
	UB	BUFB0;								// ���o�̓o�b�t�@�i�r�b�g 7�` 0�j
	UB	BUFB1;								// ���o�̓o�b�t�@�i�r�b�g15�` 8�j
	UB	BUFB2;								// ���o�̓o�b�t�@�i�r�b�g23�`16�j
	UB	BUFB3;								// ���o�̓o�b�t�@�i�r�b�g31�`24�j
};
// ��������u���b�N
typedef struct	StCheckControlBlock {
	struct	StCmd {
		long	chkRun;							// �������s
		long	chkCnt;							// ������
		long	chkPmt;							// �����p�����[�^
		long	rsv3;							// �i�\��j
	} stCmd;
	char	cmd[0x40 - sizeof(struct StCmd)];
	
	struct	StSts {
		long	result;							// ��������
		long	status;							// �������
		long	chkCnt;							// ������
		long	errCnt;							// �G���[��
		long	errInfo[8];						// �G���[���[]
		long	curInfo[4];						// ���ݏ��[]
	} stSts;
	char	sts[0xC0 - sizeof(struct StSts)];
};


// �p�g���C�g
typedef struct {
	UB buz;            // �u�U�[�w��  �F0 = �Ȃ��A1 = ����
	UB blk;            // �_�����@�w���F0 = �����A1 = �_���A2 = �_��
	UB col;            // �_���F�w��  �F0 = �ԐF�A1 = ���F�A2 = �ΐF
	UB tim;            // ���ԁi10msec�P�ʁj
} ST_PATLED;

typedef struct {
	unsigned short	target;
	unsigned short	valout;
} ST_VOUT;


typedef struct {
	unsigned short	idl;
	unsigned short	suc;
	unsigned short	err;
	unsigned short	tcnt;
	
	unsigned short	pre[NOZZLE_MAX+1];//�����O
	unsigned short	now[NOZZLE_MAX+1];//����
	unsigned short	pek[NOZZLE_MAX+1];//Peek
	unsigned short	dat[NOZZLE_MAX+1][PRESSMP_MAX];//����
} ST_PSNS;

typedef struct {
	unsigned char	dat[24];
} ST_PSNSCHAR;

typedef struct {
	UB	pno;// �v���g�R���ԍ�
	UB	smp;// ������
	UB	val;// �e��
	UB	dmy;// ��
} ST_PROTCOL_SEL;

typedef struct {
	UB	tmp;   // �w�艷�x
	UB	smp[3];// 
	INT	tim;   // �w�莞��
	INT	cnt;   // �J�E���g�p
	INT	tmo;   // �^�C���A�E�g�p
} ST_INCUV;//incv


typedef struct {
	UB	Req;
	UB	OnPls;
	UB	OfPls;
	UB	OnCnt;
	UB	OfCnt;
	UB	Sts;
	UB	Duty;
	UB	Rsv;
} ST_DPUMP_CNT;//

//--------------------------------------
// �\���̂̒�`
//--------------------------------------
struct	St16550 {
	union {												/* Register 0									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	THR;							/*    [W ] Transmit Holding Register (DLAB==0)	*/
		unsigned char	RBR;							/*    [R ] Receive Buffer Register (DLAB==0)	*/
		unsigned char	DLL;							/*    [RW] Divisor Latch LSB (DLAB==1)			*/
	} REG0;												/*												*/
	union {												/* Register 1									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	DLH;							/*    [RW] Divisor Latch MSB (DLAB==1)			*/
		unsigned char	IER;							/*    [RW] Interrupt Enable Register (DLAB==0)	*/
		struct {										/*  Bit  Access									*/
			unsigned char	RSV		: 4;				/*    Reserve									*/
			unsigned char	EDSSI	: 1;				/*    Enable Modem Status Interrupt				*/
			unsigned char	ERLSI	: 1;				/*    Enable Receiver Line Status Interrupt		*/
			unsigned char	ETBEI	: 1;				/*    Enable Transmitter Holding Register Empty Interrupt	*/
			unsigned char	ERBI	: 1;				/*    Enable Received Data Available Interrupt	*/
		} BIT;											/*												*/
	} REG1;												/*												*/
	union {												/* Register 2									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	FCR;							/*    [W] FIFO Control Register					*/
		struct {										/*  Bit  Access									*/
			unsigned char	RTB		: 2;				/*    Receiver Trigger Bit						*/
			unsigned char	RSV		: 2;				/*    Reserve									*/
			unsigned char	DMS		: 1;				/*    DMA Mode Select							*/
			unsigned char	TFR		: 1;				/*    Transmit FIFO Reset						*/
			unsigned char	RFR		: 1;				/*    Receiver FIFO Reset						*/
			unsigned char	FENA	: 1;				/*    16550 FIFO Enable							*/
		} BITW;											/*												*/
		unsigned char	IIR;							/*    [R] Interrupt Identification Register		*/
		struct {										/*  Bit  Access									*/
			unsigned char	FVLD	: 2;				/*    FIFO Invalid								*/
			unsigned char	RSV		: 2;				/*    Reserve									*/
			unsigned char	IIB		: 3;				/*    Interrupt ID Bit							*/
			unsigned char	IPB		: 1;				/*    Interrupt Pending Bit						*/
		} BITR;											/*												*/
	} REG2;												/*												*/
	union {												/* Register 3									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	LCR;							/*    [RW] Line Control Register				*/
		struct {										/*  Bit  Access									*/
			unsigned char	DLAB	: 1;				/*    Divisor Latch Access Bit					*/
			unsigned char	SB		: 1;				/*    Set Break									*/
			unsigned char	SP		: 1;				/*    Stick Parity								*/
			unsigned char	EPS		: 1;				/*    Even Parity Select						*/
			unsigned char	PEN		: 1;				/*    Parity Enable								*/
			unsigned char	NSTB	: 1;				/*    Number of Stop Bits						*/
			unsigned char	WLSB1	: 1;				/*    Word Length Select Bit #1					*/
			unsigned char	WLSB0	: 1;				/*    Word Length Select Bit #0					*/
		} BIT;											/*												*/
	} REG3;												/*												*/
	union {												/* Register 4									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	MCR;							/*    [RW] Modem Control Register				*/
		struct {										/*  Bit  Access									*/
			unsigned char	RSV		: 2;				/*    Reserve									*/
			unsigned char	AFE		: 1;				/*    Autoflow Control Enable					*/
			unsigned char	LBE		: 1;				/*    Loop-Back Enable							*/
			unsigned char	OUT2	: 1;				/*    OUT 2										*/
			unsigned char	OUT1	: 1;				/*    OUT 1										*/
			unsigned char	RTS		: 1;				/*    Request to Send							*/
			unsigned char	DTR		: 1;				/*    Data Terminal Ready						*/
		} BIT;											/*												*/
	} REG4;												/*												*/
	union {												/* Register 5									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	LSR;							/*    [RW] Line Status Register					*/
		struct {										/*  Bit  Access									*/
			unsigned char	ERF		: 1;				/*    Error in Receiver FIFO					*/
			unsigned char	TEMT	: 1;				/*    Transmitter Empty							*/
			unsigned char	THRE	: 1;				/*    Transmitter Holding Register Empty		*/
			unsigned char	BI		: 1;				/*    Break Interrupt							*/
			unsigned char	FER		: 1;				/*    Framing Error								*/
			unsigned char	PER		: 1;				/*    Parity Error								*/
			unsigned char	OER		: 1;				/*    Overrun Error								*/
			unsigned char	DR		: 1;				/*    Data Ready								*/
		} BIT;											/*												*/
	} REG5;												/*												*/
	union {												/* Register 6									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	MSR;							/*    [RW] Modem Status Register				*/
		struct {										/*  Bit  Access									*/
			unsigned char	DCD		: 1;				/*    Data Carrier Detect						*/
			unsigned char	RI		: 1;				/*    Ring Indicator							*/
			unsigned char	DSR		: 1;				/*    Data Set Ready							*/
			unsigned char	CTS		: 1;				/*    Clear To Send								*/
			unsigned char	DDCD	: 1;				/*    Delta Data Carrier Detect					*/
			unsigned char	TERI	: 1;				/*    Trailing Edge Ring Indicator				*/
			unsigned char	DDSR	: 1;				/*    Delta Data Set Ready						*/
			unsigned char	DCTS	: 1;				/*    Delta Clear To Send						*/
		} BIT;											/*												*/
	} REG6;												/*												*/
	union {												/* Register 7									*/
		unsigned char	w[1];							/*  Area Size									*/
		unsigned char	BYTE;							/*  Byte Access									*/
		unsigned char	SCR;							/*    [RW] Scratch Register						*/
	} REG7;												/*												*/
};



#endif		// _DATASTRUCT_H

