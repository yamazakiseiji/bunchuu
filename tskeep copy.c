/******************************************************************************
* EEPROM����^�X�N                                                            *
*                                                                             *
* File name : tskeep.c                                                        *
*                                                                             *
* Copyright (c) 2016 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "define.h"
#include "datastruct.h"
#include "iodefine.h"
#include "spi.h"
#include "sub.h"
#include "tskeep.h"
#include "tblchr.h"


������

#define	SRAM_BASE_ADDRESS	0x0C000000	// Motionnet�A�h���X
#define EEP_DEF_DAT			0xff
#define EEPROM_PAGE_NUM     512
#define EEPSTART_ADRS		0x0000
��
// EEPROM�Ǎ��ݍς݂��m�F���邽�߂�BIT�萔
#defiqne EP_B_USER	0x00000001
#define EP_B_SPED	0x00000002
#define EP_B_POSI	0x00000004
#define EP_B_OPTI	0x00000008
#define EP_B_TITL	0x00000010
#define EP_B_0020	0x00000020
#define EP_B_0040	0x00000040
#define EP_B_0080	0x00000080
#define EP_B_0100	0x00000100
#define EP_B_0200	0x00000200
#define EP_B_0400	0x00000400
#define EP_B_0800	0x00000800
#define EP_B_PR01	0x00001000
#define EP_B_PR02	0x00002000
#define EP_B_PR03	0x00004000
#define EP_B_PR04	0x00008000
#define EP_B_PR05	0x00010000
#define EP_B_PR06	0x00020000
#define EP_B_PR07	0x00040000
#define EP_B_PR08	0x00080000
#define EP_B_PR09	0x00100000
#define EP_B_PR10	0x00200000
#define EP_B_PR11	0x00400000
#define EP_B_PR12	0x00800000
#define EP_B_PR13	0x01000000
#define EP_B_PR14	0x02000000
#define EP_B_PR15	0x04000000
#define EP_B_PR16	0x08000000
#define EP_B_PR17	0x10000000
#define EP_B_PR18	0x20000000
#define EP_B_PR19	0x40000000
#define EP_B_PR20	0x80000000






/*=================================================================================================
  ���[�J���ϐ���`
=================================================================================================*/

static tEEPBUF      eepbuf;        						       	// ��Ɨp�o�b�t�@
static T_MSG_DAT*   mrcv;										// ���b�Z�[�W�f�[�^
static UB			dummy;

static ST_PAGE0 Page0;
static ST_PAGE1 Page1;
static ST_PAGE2 Page2;//ST_SPEED	Y0[5];
static ST_PAGE3 Page3;//ST_SPEED	Z0[5];
static ST_PAGE4 Page4;//ST_SPEED	P0[5];
//static ST_PAGE5 Page5;//ST_SPEED	F0[5];
//static ST_PAGE6 Page6;//ST_SPEED	Y1[5];
static ST_PAGE7 Page7;
static ST_PAGE8 Page8;
static ST_PAGE9 Page9;
static ST_PAGE10 Page10;

//typedef struct {
//    unsigned char Pno;	// �v���g�R���ԍ�
//    unsigned char Cnt;	// �g�[�^���X�e�b�v���i 1 �` 80 �j
//    unsigned char Prc;	// �H���L��
//    unsigned char Dmy;	// 
//} ST_PROTCOL_DATA;
static ST_PROTCOL_INFO  Prtinf; 
static ST_PROTCOL       Prtwk; 
static ST_USERINFO RcvUserInfo;
static UB writebufwk[128];


static int eepread_page;// bit0 = Page0 �` bit11 = Page11�Abit12 = �v���g�R��1�`bit31 =�v���g�R��20

//                            123456789012345678901234567890
const char STRTBL_TEST[]  = {"DBGPROTCOL12345678901234567890"};
const char STRTBL_TEST2[] = {"DBGPROTCOL2"};
const char STRTBL_NASI[]  = {"-"};

//    unsigned char Pno;	// �v���g�R���ԍ�
//    unsigned char Cnt;	// �g�[�^���X�e�b�v���i 1 �` 80 �j
//    unsigned char Prc;	// �H���L��
//    unsigned char Dmy;	// 

const ST_PROTCOL_DATA TEST_DATA_TBL[20] = {
	0x01,0x16,0x0f,0x00,// 01
	0x02,0x16,0x0f,0x00,// 02
	0x00,0x00,0x00,0x00,// 03
	0x00,0x00,0x00,0x00,// 04
	0x00,0x00,0x00,0x00,// 05
	0x00,0x00,0x00,0x00,// 06
	0x00,0x00,0x00,0x00,// 07
	0x00,0x00,0x00,0x00,// 08
	0x00,0x00,0x00,0x00,// 09
	0x00,0x00,0x00,0x00,// 10
	0x00,0x00,0x00,0x00,// 11
	0x00,0x00,0x00,0x00,// 12
	0x00,0x00,0x00,0x00,// 13
	0x00,0x00,0x00,0x00,// 14
	0x00,0x00,0x00,0x00,// 15
	0x00,0x00,0x00,0x00,// 16
	0x00,0x00,0x00,0x00,// 17
	0x00,0x00,0x00,0x00,// 18
	0x00,0x00,0x00,0x00,// 19
	0x00,0x00,0x00,0x00 // 20
};

//
// ������̓f�o�b�N�p ��
//
const ST_PROTCOL_STEP TEST_PROTCOL_TBL[80] = {
//  				CMD		P2		P3		P4		P5		P6		P7		P8		P9		P10		P11		P12		P13			P14
/* �`�b�v�擾 */	0x21,	0x00,	0x00,	0x40,	0x01,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S00
/* �`�b�v�p�� */	0x22,	0x00,	0x00,	0x40,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S01
/* �`�b�v�擾 */	0x21,	0x00,	0x00,	0x80,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S02
/* �`�b�v�p�� */	0x22,	0x00,	0x00,	0x80,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x000001E0,	0x000001E0,// S03
/* �`�b�v�擾 */	0x21,	0x00,	0x00,	0xC0,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S04
/* �`�b�v�p�� */	0x22,	0x00,	0x00,	0xC0,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S05
/* ������     */	0x23,	0x00,	0x00,	0x0F,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S06
/* ������     */	0x23,	0x00,	0x00,	0x10,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S07
/* ������     */	0x23,	0x00,	0x00,	0x11,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x000002DA,// S08
/* ������     */	0x23,	0x00,	0x00,	0x12,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S09
/* ������     */	0x23,	0x00,	0x00,	0x13,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S10
/* ������     */	0x23,	0x00,	0x00,	0x14,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S11
/* �E�F�C�g   */	0x25,	0x00,	0x00,	0x01,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S12
/* �ړ�       */	0x2D,	0x00,	0x00,	0x0D,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x000001F4,	0x000001F4,// S13
/* ���a       */	0x26,	0x00,	0x01,	0x41,	0x11,	0x11,	0x35,	0x36,	0x00,	0x00,	0x00,	0x00,	0x000001F4,	0x000001F4,// S14
/* ���a       */	0x26,	0x00,	0x02,	0x82,	0x11,	0x11,	0x35,	0x36,	0x00,	0x00,	0x00,	0x00,	0x000001F4,	0x000001F4,// S15
/* ���a       */	0x26,	0x00,	0x03,	0xC3,	0x11,	0x11,	0x35,	0x36,	0x00,	0x00,	0x00,	0x00,	0x000001F4,	0x000001F4,// S16
/* �z��       */	0x27,	0x00,	0x01,	0x10,	0x00,	0x11,	0x21,	0x26,	0x00,	0x00,	0x00,	0x00,	0x000001F4,	0x00000000,// S17
/* �f�o       */	0x28,	0x00,	0x01,	0x00,	0x14,	0x00,	0x00,	0x00,	0x11,	0x2B,	0x30,	0x00,	0x00000000,	0x000001F4,// S18
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S19
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S20
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S21
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S22
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S23
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S24
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S25
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S26
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S27
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S28
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S29
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S30
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S31
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S32
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S33
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S34
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S35
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S36
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S37
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S38
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S39
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S40
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S41
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S42
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S43
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S44
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S45
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S46
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S47
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S48
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S49
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S50
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S51
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S52
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S53
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S54
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S55
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S56
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S57
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S58
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S59
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S60
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S61
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S62
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S63
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S64
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S65
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S66
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S67
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S68
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S69
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S70
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S71
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S72
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S73
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S74
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S75
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S76
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S77
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000,// S78
/*            */	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00000000,	0x00000000 // S79
};


/*=================================================================================================
  ���[�J���֐��v���g�^�C�v�錾
=================================================================================================*/
static ER      write_enable( INT ssl );							// �������݋���
static ER      write_disable( INT ssl );						// �������݋֎~
static ER      busy_wait( INT ssl );							// �r�W�[�҂�
static ER      init_page( INT ssl, INT pno );					// �y�[�W�̏�����
static ER      write_bytes( UH adr, B* buff, INT len );			// �f�[�^�̏�������
static ER      read_bytes( UH adr, B* buff, INT size );			// �f�[�^�̓Ǎ���
static ER      write_page( INT pno, B* buff, INT len ); 		// �f�[�^�̏�������
static ER      read_page( INT pno, B* buff, INT size );			// �f�[�^�̓Ǎ���
static ER      read_status( INT ssl, B* sts );					// �X�e�[�^�X�Ǎ���
static ER      write_status( INT ssl, B status );				// �X�e�[�^�X��������
static void    read_eeprom_1byte( INT addr, B* data );			// EEPROM 1byte���[�h
static void    EepProc(T_CMD_MSG* mrcv);						// EEPROM���䏈��
static void    CmdRcvMsg( T_CMD_MSG* mrcv );					// ���s�^�X�N����̃��b�Z�[�W����
static UB      UserInfoCheck();

void eeprom_init( void );
void etc_memset( B* buff, INT bufsiz, B chr );
static void TestRead(INT pno);
static void TestWrite(INT pno);

void EepBitSet( int dat );
void EepBitClr( int dat );
BOOL EepBitGet( int dat );
void EepBitAllClr();
static ER write_byte_len( INT pno, B* buff, INT len );
void Rcvset_spddat( const char* str );
//void Rcvset_spdptn( const char* str );

/******************************************************************************
* 
* [�T�v]	EEPROM�Ǘ��^�X�N
* 			EEPROM�f�[�^�擾�y�я������݂̊Ǘ����s���B
* [����]	�|
* [�߂�l]	�|
* 
******************************************************************************/
TASK TskEep( void )
{
    T_CMD_MSG*  msg;
    ER          err;

	//SPI�f�[�^������
	spi_init();

	//EEPROM�^�X�N���f�[�^������
	eeprom_init();


//--------------------
//	TestWrite(0);   
//	TestWrite(1);   
//	TestWrite(2);   
//	TestWrite(3);   
//	TestWrite(4);   
//	TestWrite(5);   
//	TestWrite(6);   
//	TestWrite(7);   
//	TestWrite(8);   
//	TestWrite(9);   
//	TestWrite(10);  
//	TestWrite(90);  
//	TestWrite(100); 
//	TestWrite(120); 

//	TestRead(0);
//	TestRead(1);
//	TestRead(2);
//	TestRead(3);
//	TestRead(4);
//	TestRead(5);
//	TestRead(6);
//	TestRead(7);
//	TestRead(8);
//	TestRead(9);
//	TestRead(10);
//	TestRead(90);
//	TestRead(100);
//	TestRead(120);
//	TestRead(480);
//--------------------
	
    for( ;; )
    {
        // �R�}���h�҂�
		err = rcv_mbx( ID_MBX_EEP, (T_MSG**)&msg );
    	if( E_OK != err )		continue;

		EepProc(msg);

		RelMpf( ID_MPF_EEP, (VP)msg );// ���������

    }
}
/******************************************************************************
* 
* [�T�v]	EEPROM�Ǘ��^�X�N������
* 			EEPROM�f�[�^�擾�y�я������݂̊Ǘ����s���B
* [����]	�|
* [�߂�l]	�|
* 
******************************************************************************/
void eeprom_init( void )
{
    etc_memset( eepbuf.format.data, sizeof( eepbuf.format.data ), 0xFF );
//	memset( &eepbuf.format.data, NO_DATA, sizeof( eepbuf.format.data ) );
	
	EepBitAllClr();
}

/******************************************************************************
* 
* [�T�v]	EEPROM����
* [����]	�|
* [�߂�l]	�|
* 
******************************************************************************/
static void EepProc(T_CMD_MSG* mrcv)
{
	switch( mrcv->msgdata.tskid )
	{
	case ID_TSK_RXEX:
		break;
	case ID_TSK_TXEX:
		break;
	case ID_TSK_CMD:
		CmdRcvMsg(mrcv);
		break;
	}
}

static 	int staadr_a;
static 	int page_a;
static 	int pageadr_a;
static 	B *pnter_a;
static 	int staadr_k;
static 	int page_k;
static 	int pageadr_k;
static 	B *pnter_k;
static 	int test_step;
static 	int debug_flg;

static void TestRead(INT pno)
{
	int staadr=0;
	int page=0;
	int pageadr;
	B *pnter;
	int pagecnt;


	pageadr = pno * 0x80;

	switch( pno )
	{
	case 0:// - PAGE0 ----------------

		memset( &Page0.prot.sts, CLR, sizeof( Page0 ) );

// 8byte  109msec
		debug_flg = 1;
	    read_bytes( pageadr, (B*)&Page0.prot.sts, 8 );
		memcpy( &Page0.prot.sts, eepbuf.format.data, 8 );
		debug_flg = 2;

// 1page  1347msec
//		memset( &Page0.prot.sts, CLR, sizeof( Page0 ) );
//		debug_flg = 3;
//		eeprom_read(pageadr, &Page0.prot.sts, 0x80);
//		debug_flg = 4;
		break;
	case 1:
		memset( &Page1.User[0].Id[0], CLR, sizeof( Page1 ) );
		eeprom_read(pageadr, &Page1.User[0].Id[0], 0x80);
		break;
	case 2://���x
		memset( &Page2.Y0[0].ac, CLR, sizeof( Page2 ) );
		eeprom_read(pageadr, &Page2.Y0[0].ac, 0x80);
		break;
	case 3://���x
		memset( &Page3.P0[0].ac, CLR, sizeof( Page3 ) );
		eeprom_read(pageadr, &Page3.P0[0].ac, 0x80);
		break;
	case 4://���x
		memset( &Page4.Y1[0].ac, CLR, sizeof( Page4 ) );
		eeprom_read(pageadr, &Page4.Y1[0].ac, 0x80);
		break;

	case 7:
		memset( &Page7.Y0_Offset, CLR, sizeof( Page7 ) );
		eeprom_read(pageadr, &Page7.Y0_Offset, 0x80);
		break;
	case 8:
		memset( &Page8.Z0_SucStaHei1, CLR, sizeof( Page8 ) );
		eeprom_read(pageadr, &Page8.Z0_SucStaHei1, 0x80);
		break;
	case 9:
		memset( &Page9.Hosei[0].HS_VL1, CLR, sizeof( Page9 ) );
		eeprom_read(pageadr, &Page9.Hosei[0].HS_VL1, 0x80);
		break;

	case 10:
		memset( &Page10.Login, CLR, sizeof( Page10 ) );
		eeprom_read(pageadr, &Page10.Login, 0x80);
		break;
		
	case 90:
		memset( &Prtinf.Dat[0].Cnt, CLR, (4*20) );
		
		pnter = (B* )&Prtinf.Dat[0].Cnt;
		eeprom_read( (pageadr + ( page * 0x80 )), pnter, 0x80 );
		pnter += 0x80 ;
		break;
#if 0
	case 100:
		memset( &Prtwk.step[0].StCmd, CLR, sizeof( ST_PROTCOL ) );
		pnter_a = (B* )&Prtwk.step[0].StCmd;

		eeprom_read( ( 100 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 101 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 102 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 103 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 104 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 105 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 106 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 107 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 108 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 109 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 110 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 111 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 112 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 113 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 114 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 115 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 116 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 117 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 118 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 119 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		break;
	case 120:
		memset( &Prtwk.step[0].StCmd, CLR, sizeof( ST_PROTCOL ) );
		pnter_a = (B* )&Prtwk.step[0].StCmd;
		
		eeprom_read( ( 120 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 121 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 122 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 123 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 124 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 125 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 126 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 127 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 128 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 129 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 130 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 131 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 132 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 133 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 134 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 135 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 136 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 137 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 138 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 139 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		break;
#endif
	case 100:
	case 120:
	case 140:
	case 160:
	case 180:
	case 200:
	case 220:
	case 240:
	case 260:
		
		memset( &Prtwk.step[0].StCmd, CLR, sizeof( ST_PROTCOL ) );
		pnter_a = (B* )&Prtwk.step[0].StCmd;
		
		eeprom_read( ( (pno+0) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+1) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+2) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+3) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+4) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+5) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+6) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+7) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+8) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+9) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+10) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+11) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+12) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+13) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+14) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+15) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+16) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+17) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+18) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( (pno+19) * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		break;
		
		
		
		
	case 480:
		break;
	}

}

static void TestWrite(INT pno)
{
	int staadr=0;
	int page=0;
	int loop=0;
	int pageadr;
	B *pnter;

	memset( &Page0, CLR, sizeof( Page0 ) );
	memset( &Page1, CLR, sizeof( Page1 ) );
	memset( &Page2, CLR, sizeof( Page2 ) );
	memset( &Page3, CLR, sizeof( Page3 ) );
	memset( &Page4, CLR, sizeof( Page4 ) );
	memset( &Page8, CLR, sizeof( Page8 ) );
	memset( &Page9, CLR, sizeof( Page9 ) );
	memset( &Page10, CLR, sizeof( Page10 ) );

	
	switch( pno )
	{
	case 0:// - PAGE0 ----------------
		// 8byte 139msec
		Page0.prot.sts  = E_TIP_DPFALL; // ���
		Page0.prot.no   = 1;            // �v���g�R���ԍ�
		Page0.prot.step = 4;            // �����X�e�b�v
		Page0.prot.proc = PROCESS_LYSI; // �H��
		write_byte_len( EEPROM_PAGE0_ADR , (B*)&Page0.prot.sts, 8 );
debug_flg = 1;

//		// 128byte 1346msec
//		Page0.prot.sts  = E_TIP_NTHING; // ���
//		Page0.prot.no   = 2;            // �v���g�R���ԍ�
//		Page0.prot.step = 5;            // �����X�e�b�v
//		Page0.prot.proc = PROCESS_BILD; // �H��
//		eeprom_write( EEPROM_PAGE0_ADR , &Page0.prot.sts, 0x80 );
//		debug_flg = 3;
		break;

	case 1:// - PAGE1 ----------------
		Page1.User[0].Id[0] = 0x61;
		Page1.User[0].Id[1] = 0x64;
		Page1.User[0].Id[2] = 0x6d;
		Page1.User[0].Id[3] = 0x69;
		Page1.User[0].Id[4] = 0x6e;
		Page1.User[0].Pw[0] = 0x31;
		Page1.User[0].Pw[1] = 0x32;
		Page1.User[0].Pw[2] = 0x33;
		Page1.User[0].Pw[3] = 0x34;
		
		Page1.User[1].Id[0] = 0x75;
		Page1.User[1].Id[1] = 0x73;
		Page1.User[1].Id[2] = 0x65;
		Page1.User[1].Id[3] = 0x72;
		Page1.User[1].Id[4] = 0x31;
		Page1.User[1].Pw[0] = 0x31;
		Page1.User[1].Pw[1] = 0x32;
		Page1.User[1].Pw[2] = 0x33;
		Page1.User[1].Pw[3] = 0x34;

		Page1.User[9].Id[0] = 0x75;
		Page1.User[9].Id[1] = 0x73;
		Page1.User[9].Id[2] = 0x65;
		Page1.User[9].Id[3] = 0x72;
		Page1.User[9].Id[4] = 0x39;
		Page1.User[9].Pw[0] = 0x31;
		Page1.User[9].Pw[1] = 0x32;
		Page1.User[9].Pw[2] = 0x33;
		Page1.User[9].Pw[3] = 0x34;
		eeprom_write( ( 1 * 0x80) , &Page1.User[0].Id[0], 0x80 );
		break;

	case 2:// - PAGE2 ----------------
		for( loop = 0; loop < 5; loop++ )
		{
			Page2.Y0[loop].ac = 1;
			Page2.Y0[loop].gl = 500;
			Page2.Y0[loop].fl = 300;
			Page2.Y0[loop].fh = 1000;

			Page2.Z0[loop].ac = 1;
			Page2.Z0[loop].gl = 500;
			Page2.Z0[loop].fl = 300;
			Page2.Z0[loop].fh = 1000;

		}
		eeprom_write( ( 2 * 0x80) , &Page2.Y0[0].ac, 0x80 );
		break;

	case 3:// - PAGE3 ----------------
		for( loop = 0; loop < 5; loop++ )
		{
			Page3.P0[loop].ac = 1;
			Page3.P0[loop].gl = 500;
			Page3.P0[loop].fl = 300;
			Page3.P0[loop].fh = 1000;

			Page3.F0[loop].ac = 1;
			Page3.F0[loop].gl = 500;
			Page3.F0[loop].fl = 300;
			Page3.F0[loop].fh = 1000;
		}
		eeprom_write( ( 3 * 0x80) , &Page3.P0[0].ac, 0x80 );
		break;
	
	case 4:// - PAGE4 ----------------
		for( loop = 0; loop < 5; loop++ )
		{
			Page4.Y1[loop].ac = 1;
			Page4.Y1[loop].gl = 500;
			Page4.Y1[loop].fl = 300;
			Page4.Y1[loop].fh = 1000;
		}
		Page4.SpPtn.Y0_SpdIniPtn       = 1;
		Page4.SpPtn.Z0_SpdIniPtn       = 1;
		Page4.SpPtn.P0_SpdIniPtn       = 1;
		Page4.SpPtn.Y1_SpdIniPtn       = 1;
		Page4.SpPtn.F0_SpdIniPtn       = 1;
		Page4.SpPtn.Y0_SpdNmlPtn       = 2;
		Page4.SpPtn.Z0_SpdNmlPtn       = 2;
		Page4.SpPtn.P0_SpdNmlPtn       = 2;
		Page4.SpPtn.Y1_SpdNmlPtn       = 2;
		Page4.SpPtn.F0_SpdNmlPtn       = 2;
		Page4.SpPtn.P0_SpdMovAirS      = 3;
		Page4.SpPtn.P0_SpdMovAirP      = 3;
		Page4.SpPtn.Z0_SpdTipGetDownE  = 3;
		Page4.SpPtn.Z0_SpdTipGetDown1  = 3;
		Page4.SpPtn.Z0_SpdTipDelDownE  = 3;
		Page4.SpPtn.Z0_SpdTipDelDown1  = 3;
		Page4.SpPtn.Z0_SpdTipDownE     = 3;
		Page4.SpPtn.Z0_SpdTipDown1     = 3;
		Page4.SpPtn.Y0_SpdSucMov       = 3;
		Page4.SpPtn.Y0_SpdPutMov       = 3;
		Page4.SpPtn.Z0_SpdPutDown      = 3;
		Page4.SpPtn.Z0_SpdSucDown      = 3;
		Page4.SpPtn.P0_SpdPut          = 3;
		Page4.SpPtn.P0_SpdSuc          = 3;
		Page4.SpPtn.P0_SpdTipDel       = 3;
		
		eeprom_write( ( 4 * 0x80) , &Page4.Y1[0].ac, 0x80 );
		break;

	case 5:// - PAGE5 ----------------
		break;

	case 6:// - PAGE6 ----------------
		break;

	case 7:// - PAGE7 ----------------		
		Page7.Y0_Offset     = 20    ; // [Y0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��P
		Page7.Z0_Offset     = 20    ; // [Z0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��Q
		Page7.P0_Offset     = 20    ; // [P0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��R
		Page7.F0_Offset     = 20    ; // [F0] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��S
		Page7.Y1_Offset     = 20    ; // [Y1] ���z���_�I�t�Z�b�g�l               �|�W�V�����ԍ��T
		Page7.Y0_Taihi      = 100   ; // [Y0] �ޔ��ʒu                           �|�W�V�����ԍ��U
		Page7.Z0_Taihi      = 100   ; // [Z0] �ޔ��ʒu                           �|�W�V�����ԍ��V
		Page7.P0_Taihi      = 100   ; // [P0] �ޔ��ʒu                           �|�W�V�����ԍ��W
		Page7.F0_Taihi      = 100   ; // [F0] �ޔ��ʒu                           �|�W�V�����ԍ��X
		Page7.Y1_Taihi      = 100   ; // [Y1] �ޔ��ʒu                           �|�W�V�����ԍ��P�O
		Page7.StripTp1      = 140   ; // [Y0] ����X�g���b�v-�V���[�g�`�b�v�P    �|�W�V�����ԍ��P�P
		Page7.StripTp2      = 180   ; // [Y0] ����X�g���b�v-�V���[�g�`�b�v�Q    �|�W�V�����ԍ��P�Q
		Page7.StripTpL      = 220   ; // [Y0] ����X�g���b�v-�����O�`�b�v        �|�W�V�����ԍ��P�R
		Page7.StripTub      = 290   ; // [Y0] ����X�g���b�v-�T���v���`���[�u    �|�W�V�����ԍ��P�S
		Page7.StripSm6      = 340   ; // [Y0] ����X�g���b�v-���o�p����6         �|�W�V�����ԍ��P�T
		Page7.StripSm5      = 385   ; // [Y0] ����X�g���b�v-���o�p����5         �|�W�V�����ԍ��P�U
		Page7.StripSm4      = 430   ; // [Y0] ����X�g���b�v-���o�p����4         �|�W�V�����ԍ��P�V
		Page7.StripSm3      = 475   ; // [Y0] ����X�g���b�v-���o�p����3         �|�W�V�����ԍ��P�W
		Page7.StripSm2      = 520   ; // [Y0] ����X�g���b�v-���o�p����2         �|�W�V�����ԍ��P�X
		Page7.StripSm1      = 565   ; // [Y0] ����X�g���b�v-���o�p����1         �|�W�V�����ԍ��Q�O
		Page7.StripCat      = 620   ; // [Y0] ����X�g���b�v-�J�[�g���b�W        �|�W�V�����ԍ��Q�P
		Page7.StripCol      = 665   ; // [Y0] ����X�g���b�v-�j�_����t          �|�W�V�����ԍ��Q�Q
		Page7.Z0_StripOpnE  = 3200  ; // [Z0] ����X�g���b�v-�V�[���J���ŏI�ʒu	�|�W�V�����ԍ��Q�R
		Page7.Z0_StripOpn1  = 3000  ; // [Z0] ����X�g���b�v-�V�[���J���P�i�ʒu	�|�W�V�����ԍ��Q�S
		Page7.Z0_StripPres  = 3000  ; // [Z0] ����X�g���b�v-�����ʒu	        �|�W�V�����ԍ��Q�T
		Page7.Z0_TipGetE    = 3200  ; // [Z0] �`�b�v�擾�ŏI�ʒu                 �|�W�V�����ԍ��Q�U
		Page7.Z0_TipGet1    = 3200  ; // [Z0] �`�b�v�擾�P�i�ʒu                 �|�W�V�����ԍ��Q�V
		Page7.Z0_TipDelE    = 3000  ; // [Z0] �`�b�v�p���ŏI�ʒu                 �|�W�V�����ԍ��Q�W
		Page7.Z0_TipDel1    = 3200  ; // [Z0] �`�b�v�p���P�i�ʒu                 �|�W�V�����ԍ��Q�X
		Page7.Z0_LiqPut     = 3000  ; // [Z0] �p�t�f�o�ʒu                       �|�W�V�����ԍ��R�O
		Page7.Y1_ColTube    = 100   ; // [Z0] �R���N�V���� ����`���[�u�ʒu      �|�W�V�����ԍ��R�P
		Page7.Y1_ColDspTube = 200   ; // [Z0] �p�t�`���[�u�ʒu                   �|�W�V�����ԍ��R�Q

		eeprom_write( ( 7 * 0x80) , &Page7.Y0_Offset, 0x80 );
		break;

	case 8:// - PAGE8 ----------------
		Page8.Z0_SucStaHei1 = 3000  ; // [Z0] �z���J�n����1                      �|�W�V�����ԍ��R�R
		Page8.Z0_SucStaHei2 = 3010  ; // [Z0] �z���J�n����2                      �|�W�V�����ԍ��R�S
		Page8.Z0_SucStaHei3 = 3020  ; // [Z0] �z���J�n����3                      �|�W�V�����ԍ��R�T
		Page8.Z0_SucStaHei4 = 3030  ; // [Z0] �z���J�n����4                      �|�W�V�����ԍ��R�U
		Page8.Z0_SucStaHei5 = 3040  ; // [Z0] �z���J�n����5                      �|�W�V�����ԍ��R�V
		Page8.Z0_SucEndHei1 = 3050  ; // [Z0] �z���I������1                      �|�W�V�����ԍ��R�W
		Page8.Z0_SucEndHei2 = 3060  ; // [Z0] �z���I������2                      �|�W�V�����ԍ��R�X
		Page8.Z0_SucEndHei3 = 3070  ; // [Z0] �z���I������3                      �|�W�V�����ԍ��S�O
		Page8.Z0_SucEndHei4 = 3080  ; // [Z0] �z���I������4                      �|�W�V�����ԍ��S�P
		Page8.Z0_SucEndHei5 = 3090  ; // [Z0] �z���I������5                      �|�W�V�����ԍ��S�Q
		Page8.Z0_PutStaHei1 = 3000  ; // [Z0] �f�o�J�n����1                      �|�W�V�����ԍ��S�R
		Page8.Z0_PutStaHei2 = 3010  ; // [Z0] �f�o�J�n����2                      �|�W�V�����ԍ��S�S
		Page8.Z0_PutStaHei3 = 3030  ; // [Z0] �f�o�J�n����3                      �|�W�V�����ԍ��S�T
		Page8.Z0_PutStaHei4 = 3040  ; // [Z0] �f�o�J�n����4                      �|�W�V�����ԍ��S�U
		Page8.Z0_PutStaHei5 = 3050  ; // [Z0] �f�o�J�n����5                      �|�W�V�����ԍ��S�V
		Page8.Z0_PutEndHei1 = 3000  ; // [Z0] �f�o�I������1                      �|�W�V�����ԍ��S�W
		Page8.Z0_PutEndHei2 = 3010  ; // [Z0] �f�o�I������2                      �|�W�V�����ԍ��S�X
		Page8.Z0_PutEndHei3 = 3030  ; // [Z0] �f�o�I������3                      �|�W�V�����ԍ��T�O
		Page8.Z0_PutEndHei4 = 3040  ; // [Z0] �f�o�I������4                      �|�W�V�����ԍ��T�P
		Page8.Z0_PutEndHei5 = 3050  ; // [Z0] �f�o�I������5                      �|�W�V�����ԍ��T�Q
		Page8.Z0_MixStaSuc  = 2500  ; // [Z0] �h�a�z���J�n����                   �|�W�V�����ԍ��T�R
		Page8.Z0_MixEndSuc  = 2500  ; // [Z0] �h�a�z���I������                   �|�W�V�����ԍ��T�S
        Page8.P0_TpDispose  = 10    ; // [P0] �p������		                     �|�W�V�����ԍ��T�T
		
		eeprom_write( ( 8 * 0x80) , &Page8.Z0_SucStaHei1, 0x80 );
		break;

	case 9:// - PAGE9 ----------------
		Page9.Hosei[0].HS_VL1  =  1000;   // �␳�p�^�[���P	�e�� 10%                �|�W�V�����ԍ��U�T
		Page9.Hosei[0].HS_VL2  =  5000;   //                 �e�� 50%          		�|�W�V�����ԍ��U�U
		Page9.Hosei[0].HS_VL3  =  10000;  //                 �e��100%           		�|�W�V�����ԍ��U�V
		Page9.Hosei[0].HS_Out  =  100;    //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��U�W
		Page9.Hosei[0].HS_Air  =  300;    //                 �G�A�L���b�v��          �|�W�V�����ԍ��U�X
		Page9.Hosei[0].HS_Tra  =  100;    //                 �]��� 			        �|�W�V�����ԍ��V�O
		Page9.Hosei[1].HS_VL1  =  1000;   // �␳�p�^�[���Q  �e�� 10%                �|�W�V�����ԍ��V�P
		Page9.Hosei[1].HS_VL2  =  5000;   //                 �e�� 50%   	            �|�W�V�����ԍ��V�Q
		Page9.Hosei[1].HS_VL3  =  10000;  //                 �e��100%           		�|�W�V�����ԍ��V�R
		Page9.Hosei[1].HS_Out  =  100;    //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��V�S
		Page9.Hosei[1].HS_Air  =  300;    //                 �G�A�L���b�v��          �|�W�V�����ԍ��V�T
		Page9.Hosei[1].HS_Tra  =  100;    //                 �]��� 			        �|�W�V�����ԍ��V�U
		Page9.Hosei[2].HS_VL1  =  1000;   // �␳�p�^�[���R  �e�� 10%                �|�W�V�����ԍ��V�V
		Page9.Hosei[2].HS_VL2  =  5000;   //                 �e�� 50%          		�|�W�V�����ԍ��V�W
		Page9.Hosei[2].HS_VL3  =  10000;  //                 �e��100%           		�|�W�V�����ԍ��V�X
		Page9.Hosei[2].HS_Out  =  100;    //                 �u���[�A�E�g�G�A��      �|�W�V�����ԍ��W�O
		Page9.Hosei[2].HS_Air  =  300;    //                 �G�A�L���b�v��          �|�W�V�����ԍ��W�P
		Page9.Hosei[2].HS_Tra  =  100;    //                 �]��� 			        �|�W�V�����ԍ��W�Q
		
		eeprom_write( ( 9 * 0x80) , &Page9.Hosei[0].HS_VL1, 0x80 );
		break;

	case 10:// - PAGE10 ----------------
		Page10.Login       = 1;
		eeprom_write( ( 10 * 0x80) , &Page10.Login, 0x80 );
		break;

	case 90:// - PAGE90 ----------------
		memcpy( &Prtinf.Dat[0].Cnt, &TEST_DATA_TBL[0], 0x80 );
		staadr = 90*0x80;
		pnter = (B* )&Prtinf.Dat[0].Cnt;
		eeprom_write( (staadr + ( page * 0x80 )) , pnter, 0x80 );
		break;

	case 100:// - PAGE100 ----------------
		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
		memcpy( &Prtwk.step[0].StCmd,      &TEST_PROTCOL_TBL[0], sizeof( ST_PROTCOL ) );


		staadr = 100*0x80;
		pnter = (B* )&Prtwk.step[0].StCmd;
		for( page = 0; page < 20; page++ )
		{
			eeprom_write( (staadr + ( page * 0x80 )) , pnter, 0x80 );
			pnter += 0x80 ;
		}
		break;
	case 120:// - PAGE120 ----------------
		// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
//		memcpy( &Prtwk.info.ProtcolName[0], &STRTBL_TEST2[0], sizeof( STRTBL_TEST ) );
		memcpy( &Prtwk.step[0].StCmd,      &TEST_PROTCOL_TBL[0], sizeof( ST_PROTCOL ) );

		staadr_k = 120*0x80;
		pnter_k = (B* )&Prtwk.step[0].StCmd;
		for( page_k = 0; page_k < 20; page_k++ )
		{
			eeprom_write( (staadr_k + ( page_k * 0x80 )) , pnter_k, 0x80 );
			pnter_k += 0x80 ;
		}
		break;
	}
}
static UB	inf2;
static 	int staadr;
static 		B   *pnter;
static 		int page;
static 	int dbgadr;
static 		B   *dbgpnter;
static 		UB	dbgpagecnt;
static 			int dbgbytcnt;
static 		int dbgpage;

/******************************************************************************
* 
* [�T�v]	���s�^�X�N����̃��b�Z�[�W��M����
* [����]	mrcv:���b�Z�[�W�f�[�^
* [�߂�l]	�|
* 
******************************************************************************/
static void CmdRcvMsg(T_CMD_MSG* mrcv)
{
	UB	msgid;
	UB	prid1;
	UB	bcid;
	UB	inf;
	int cnt;
	int loop;
	int index;
	int pageadr;
	ST_CNTLDATA			msgdat;										// ���b�Z�[�W�f�[�^
	int bytcnt;
	UB	pagecnt;
	UB	pstepcnt;


	// memcpy(�R�s�[��A�h���X�A�R�s�[���A�h���X�A�R�s�[�T�C�Y)
	memcpy( &msgdat.rsp.buf[0],   &mrcv->msgdata.data[0],CMD_BUFSIZ );

	msgid = mrcv->msgdata.msgid;
	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	inf   = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	
	if( msgid == ID_MSG_EEPREAD )
	{
		switch( prid1 )
		{
		case ID_TPL_LGIN1:
			if( EepBitGet( EP_B_USER ) == FALSE )
			{
				eeprom_read(EEPROM_PAGE1_ADR, &Page1.User[0].Id[0], sizeof( Page1 ));
				EepBitSet( EP_B_USER );
			}
			break;

		case ID_TPL_FWVER:
			if( (inf2 & EEPROM_SPED_PTRN) )	// ���x���iPage2�`Page7�j
			{
				if( EepBitGet( EP_B_SPED ) == FALSE )
				{
					eeprom_read(EEPROM_PAGE2_ADR, &Page2.Y0[0].ac, sizeof( Page2 ));
					eeprom_read(EEPROM_PAGE3_ADR, &Page3.P0[0].ac, sizeof( Page3 ));
					eeprom_read(EEPROM_PAGE4_ADR, &Page4.Y1[0].ac, sizeof( Page4 ));
					EepBitSet( EP_B_SPED );
//debug
//TestRead(2);
//TestRead(3);
//TestRead(4);
//debug
				}
			}
			
			if( (inf2 & EEPROM_POSI_INFO) )	// �ʒu���iPage8�`Page9�j
			{
				if( EepBitGet( EP_B_POSI ) == FALSE )
				{
					eeprom_read(EEPROM_PAGE7_ADR, &Page7.Y0_Offset,       sizeof( Page7 ));
					eeprom_read(EEPROM_PAGE8_ADR, &Page8.Z0_SucStaHei1,   sizeof( Page8 ));
					eeprom_read(EEPROM_PAGE9_ADR, &Page9.Hosei[0].HS_VL1, sizeof( Page9 ));
					EepBitSet( EP_B_POSI );
				}
			}
			break;

		case ID_TPL_OPTIF:
			if( EepBitGet( EP_B_OPTI ) == FALSE )
			{
				eeprom_read(EEPROM_PAGE10_ADR, &Page10.Login, sizeof( Page10 ));
				EepBitSet( EP_B_OPTI );
			}
			break;
			
		case ID_TPL_PRTCL:
			if( EepBitGet( EP_B_PR01 ) == FALSE )
			{
				if( inf2 >= 1 )
				{
					index = (inf2 - 1);
					EepBitSet( ( EP_B_PR01 << index ) );

					// �v���g�R���f�[�^�擾�i�v���g�R���ԍ��A���X�e�b�v���A�H���j
					//                       Cnt           , Stp,          Prc
					pnter_a = (B* )&Prtinf.Dat[index].Cnt;
					staadr  = EEPROM_PAGE90_ADR + index * sizeof(ST_PROTCOL_DATA);
					eeprom_read(  staadr , pnter_a, sizeof(ST_PROTCOL_DATA) );

					// �v���g�R���f�[�^�X�e�b�v�擾
					pstepcnt = Prtinf.Dat[index].Cnt;
					if( pstepcnt <= PROTCOL_MAX )
					{
						bytcnt  = pstepcnt * sizeof(ST_PROTCOL_STEP);
						pagecnt = (UB) bytcnt / EEPROM_SIZE_MAX;
						if( (bytcnt % EEPROM_SIZE_MAX) )	pagecnt++;
						
						pnter_a = (B* )&Prtwk.step[0].StCmd;
//						page = EEPROM_PAGE100_ADR + index * sizeof(ST_PROTCOL_STEP);
						page = EEPROM_PAGE100_ADR + index * 20 * EEPROM_SIZE_MAX;
						
						for( loop = 0; loop < pagecnt ;loop++ )
						{
							eeprom_read(  page +  (loop * EEPROM_SIZE_MAX ), pnter_a, EEPROM_SIZE_MAX );
							pnter_a += EEPROM_SIZE_MAX ;
						}
					}
				}
			}
			break;
			
			
		default:
			break;
		}
	}
	
	if( msgid == ID_MSG_IMPPDAT )
	{
        //  --------------------
        // ProcNo  page    Adr(hex)
        //   1     100     3200
        //   2     120     3C00
        //   3     140     4600
        //   4     160     5000
        //   5     180     5400
        //   6     200     5800
        //   7     220     5C00
        //   8     240     6000
        //   9     260     6400
        //  10     280     6800
        //  11     300     6C00
        //  12     320     7000
        //  13     340     7400
        //  14     360     7800
        //  15     380     7C00
        //  16     400     8000
        //  17     420     8400
        //  18     440     8800
        //  19     460     8C00
        //  20     480     9000
        //  --------------------
		
		pstepcnt = Get_pcnt();
		if( pstepcnt <= PROTCOL_MAX )
		{
			if( inf2 >= 1 )
			{
				// �v���g�R���X�e�b�v��������
				index = (inf2 - 1);
				Prtinf.Dat[index].Cnt = pstepcnt;
				pnter_a = (B* )&Prtinf.Dat[index].Cnt;
				staadr = EEPROM_PAGE90_ADR + index * 4;

				write_byte_len( staadr , pnter_a, 1 );
				
				
// ----- debug -----
#if 0
TestRead(90);
#endif
// ----- debug -----

				
				// �v���g�R��������
				bytcnt  = pstepcnt * sizeof(ST_PROTCOL_STEP);
dbgbytcnt = bytcnt;

				pagecnt = bytcnt / EEPROM_SIZE_MAX;

dbgpagecnt = pagecnt;
				
				if( (bytcnt % EEPROM_SIZE_MAX) )	pagecnt++;

dbgpagecnt = pagecnt;
				
//				staadr = (100 + (inf2 - 1) * sizeof(ST_PROTCOL_STEP)) * EEPROM_SIZE_MAX;
				staadr = (100 + (inf2 - 1) * 20 ) * EEPROM_SIZE_MAX;
dbgadr = staadr ;

				pnter = (B* )( SRAM_START_ADDRESS + sizeof( ST_PROTCOL )*( inf2 - 1));
dbgpnter=pnter ;
				
				for( page = 0; page < pagecnt; page++ )
				{
dbgpage=page;
					eeprom_write( (staadr + ( page * EEPROM_SIZE_MAX )) , pnter, EEPROM_SIZE_MAX );
					pnter += EEPROM_SIZE_MAX ;
				}
				
				
// ----- debug -----
#if 0
		memset( &Prtwk.step[0].StCmd, CLR, sizeof( ST_PROTCOL ) );
		pnter_a = (B* )&Prtwk.step[0].StCmd;

		eeprom_read( ( 140 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 141 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 142 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 143 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 144 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 145 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 146 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 147 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 148 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 149 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 150 * 0x80 ), pnter_a, 0x80 );	pnter_a += 0x80 ;
		eeprom_read( ( 151 * 0x80 ), pnter_a, 0x80 );	
#endif
// ----- debug -----
				
				
				
			}
		}
		
	}
	
	
	if( msgid == ID_MSG_IMPSPED )
	{
		eeprom_write( ( 2 * EEPROM_SIZE_MAX) , &Page2.Y0[0].ac, EEPROM_SIZE_MAX );
		eeprom_write( ( 3 * EEPROM_SIZE_MAX) , &Page3.P0[0].ac, EEPROM_SIZE_MAX );
		eeprom_write( ( 4 * EEPROM_SIZE_MAX) , &Page4.Y1[0].ac, EEPROM_SIZE_MAX );
	}
	
	if( msgid == ID_MSG_IMPPOSI )
	{
		eeprom_write( ( 7 * EEPROM_SIZE_MAX) , &Page7.Y0_Offset,      EEPROM_SIZE_MAX );
		eeprom_write( ( 8 * EEPROM_SIZE_MAX) , &Page8.Z0_SucStaHei1,  EEPROM_SIZE_MAX );
		eeprom_write( ( 9 * EEPROM_SIZE_MAX) , &Page9.Hosei[0].HS_VL1,EEPROM_SIZE_MAX );
	}
	
	
	
	if( msgid == ID_MSG_EEPWRIT_STS )
	{
		memcpy( &writebufwk[0], &mrcv->msgdata.data[0], sizeof(ST_DEVICE_INFO));
		write_byte_len( EEPROM_PAGE0_ADR , (B*)&writebufwk[0], sizeof(ST_DEVICE_INFO) );
		
		
// ��������̓f�o�b�N�Ŋm�F�p
// ��������̓f�o�b�N�Ŋm�F�p		memset( &Page0.prot.sts, CLR, sizeof( Page0 ) );
// ��������̓f�o�b�N�Ŋm�F�p	    read_bytes( EEPROM_PAGE0_ADR, (B*)&Page0.prot.sts, 8 );
// ��������̓f�o�b�N�Ŋm�F�p		memcpy( &Page0.prot.sts, eepbuf.format.data, 8 );
// ��������̓f�o�b�N�Ŋm�F�p		debug_flg = 1;
// ��������̓f�o�b�N�Ŋm�F�p
		return;
	}

	
	
	
	
	
	
    // �������b�Z�[�W���M
	CmdEndResponse	(
						ID_TSK_EEP,
						prid1,
						bcid,
						msgid,
						inf,
						inf2,
						&msgdat
					);
}
/******************************************************************************
* 
* [�T�v]	EEPROM�����݊Ǘ��f�[�^������
* [����]	�|
* [�߂�l]	�|
* 
******************************************************************************/
static UB UserInfoCheck()
{
	int i;

//	if(
//		( Strncmp( &RcvUserInfo.Id[0], &Page0.Mast.Id[0], 8 ))||
//		( Strncmp( &RcvUserInfo.Pw[0], &Page0.Mast.Pw[0], 4 ))
//	)
//	{
		for( i = 0; i < 10; i++ )
		{
			if(
				( Strncmp( &RcvUserInfo.Id[0], &Page1.User[i].Id[0], 8 ))||
				( Strncmp( &RcvUserInfo.Pw[0], &Page1.User[i].Pw[0], 4 ))
			)
			{
				return(1);
			}
		}
//	}
	return(0);
}

/*-------------------------------------------------------------------------------------------------
  �@�\�FEEPROM�̃f�[�^�N���A
  �����F�Ȃ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
ER eeprom_clear( void )
{
    ER     err;
    INT     i;

    for( i = 0; i < EEPROM_PAGE_NUM; i++ )
    {
        err = init_page( SPI_SSL2, i );
        if( err ) return err;
    }
    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̏�������
  �����Fpno         �y�[�W�ԍ�
        buf         �������ރf�[�^(1-120)
        len         �f�[�^�̒���
        nopowchk    TRUE:�d��OFF�`�F�b�N�Ȃ��AFALSE:�d��OFF�`�F�b�N����
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
ER eeprom_write( INT pno, const B* buff, INT len )
{
    INT     err;

    err = write_page( pno, buff, len );

	return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̓Ǎ���
  �����Fpno     �y�[�W�ԍ�
        buff    �ǂݍ��񂾃f�[�^���i�[����o�b�t�@
        size    �o�b�t�@�̃T�C�Y
        len     �ǂݍ��񂾃T�C�Y
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
ER eeprom_read( INT pno, B* buff, INT size )
{
    INT     err = E_SUCCESS;
    INT     adr;
	UH		cnt;
	UH		cntmax;
	INT		readsiz;

#if 1

	adr = pno;
	cntmax = size / PAGEMAX ;
	readsiz = size % PAGEMAX;

	for( cnt = 0;cnt <= cntmax; cnt++,adr=adr+PAGEMAX )
	{
		if(cnt == cntmax) 	size = readsiz;
		else				size = PAGEMAX;

		err = read_page( adr, buff, size );
	}

#else
	int cnt;
	
	RSPI.SPCR.BIT.SPE = 1;						// RSPI�@�\�C�l�[�u�� : RSPI�@�\��L��������
	for( cnt = 0; cnt < size; cnt++,pno++,buff++ )
	{
		read_eeprom_1byte((INT)pno, (B *)buff);
	}

#endif
	return err;
}
static void read_eeprom_1byte(INT addr, B *data )
{
	// �ǂݍ���
	RSPI.SPCMD0.BIT.SPB = 2;					// RSPI�f�[�^���ݒ� : 32�r�b�g
	RSPI.SPSR.BIT.OVRF = 0;						// �I�[�o�����G���[�t���O���N���A����
	RSPI.SPSR.BIT.SPRF = 0;						// ����Ŏ�M�̏������s��
	RSPI.SPDR.LONG = ( (INT)CMD_READ_ARRAY << 24 ) | ( (INT)addr << 8 ) | 0x00; 
	RSPI.SPSR.BIT.SPTEF = 0;					// ����ő��M���J�n�����
	dly_tsk(DLY_10M);
	
	// �ǂݍ��񂾃f�[�^���擾����
	*data = RSPI.SPDR.LONG;
}
/*=================================================================================================
  ���[�J���֐�
=================================================================================================*/

/*-------------------------------------------------------------------------------------------------
  �@�\�F�������݋���
  �����Fssl     SPI��/CS�ԍ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER write_enable( INT ssl )
{
    B    cmd;
    ER     err;

    cmd = CMD_WRITE_ENABLE;
    err = spi_send( ssl, &cmd, 1 );
    return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�������݋֎~
  �����Fssl     SPI��/CS�ԍ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER write_disable( INT ssl )
{
    B    cmd;
    ER     err;

    cmd = CMD_WRITE_DISABLE;
    err = spi_send( ssl, &cmd, 1 );
    return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�r�W�[�҂�
  �����Fssl     SPI��/CS�ԍ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER busy_wait( INT ssl )
{
    ER     err;
	B	rty = 0;
	B	sts;

    err = read_status( ssl, &sts );
    if( err ) return err;

	while( 0 != ( sts & STS_MASK_BUSY ) )
	{
		rty++;
		if( rty > SPI_TIMOUT)
		{
			return E_COM_SPITMO;
		}
		dly_tsk(DLY_10M);
	}
    return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�y�[�W�̏�����
  �����Fssl     SPI��/CS�ԍ�
        pno     �y�[�W�ԍ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER init_page( INT ssl, INT pno )
{
    ER     err;
    UH    adr;

    etc_memset( eepbuf.format.data, sizeof( eepbuf.format.data ), 0xFF );

    adr = (UH)pno;
    err = write_bytes( adr, NULL,sizeof( eepbuf.format.data ) );
    if( err ) return err;

    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̏�������
  �����Fssl         SPI��/CS�ԍ�
        adr         �A�h���X(�y�[�W�̐擪�A�h���X)
        buf         �������ރf�[�^  (NULL���w�肷���eepbuf�̓��e���g�p����)
        len         �f�[�^�̒���
        nopowchk    TRUE:�d��OFF�`�F�b�N�Ȃ��AFALSE:�d��OFF�`�F�b�N����
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER write_bytes( UH adr, B* buff, INT len )
{
    ER     err;

    // EEPROM�̃R�}���h
    eepbuf.format.cmd.cmd  = CMD_WRITE_ARRAY;
    eepbuf.format.cmd.adrh = (B)((adr >>  8) & 0xff);
    eepbuf.format.cmd.adrl = (B)( adr        & 0xff);

	err = write_enable( SPI_SSL2 );
    if( err ) return err;
    err = spi_send( SPI_SSL2, eepbuf.buff, sizeof( tEEPCMD ) + len );
    if( err ) return err;
    err = busy_wait( SPI_SSL2 );
    if( err ) return err;

    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̓Ǎ���
  �����Fssl     SPI��/CS�ԍ�
        adr     �A�h���X(�y�[�W�̐擪�A�h���X)
        buff    �ǂݍ��񂾃f�[�^���i�[����o�b�t�@(NULL�w��\)
        size    �o�b�t�@�̃T�C�Y
        len     �ǂݍ��񂾃T�C�Y
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER read_bytes( UH adr, B* buff, INT size )
{
    ER     err;

    // EEPROM�̃R�}���h
    eepbuf.format.cmd.cmd  = CMD_READ_ARRAY;
    eepbuf.format.cmd.adrh = (B)((adr >>  8) & 0xff);
    eepbuf.format.cmd.adrl = (B)( adr        & 0xff);

    // �ǂݍ���
    err = spi_send_recv( (B*)    &eepbuf.format.cmd
                        , sizeof( eepbuf.format.cmd )
                        , (B*)   &eepbuf.format.data
                        , size);

	
	if( err ) return err;

	return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̏�������(�P�y�[�W:120�o�C�g)
  �����Fssl         SPI��/CS�ԍ�
        pno         �y�[�W�ԍ�
        buf         �������ރf�[�^(NULL���w�肷���eepbuf�̓��e���g�p����)
        len         �f�[�^�̒���
        nopowchk    TRUE:�d��OFF�`�F�b�N�Ȃ��AFALSE:�d��OFF�`�F�b�N����
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER write_page( INT pno, B* buff, INT len )
{
//k    ER     err = E_ERR;
    ER     err = E_SUCCESS;
    UH    adr;

    adr = (UH)pno;

    if( buff )
    {
        etc_memset( eepbuf.format.data, sizeof( eepbuf.format.data ), 0xFF );
        etc_memcpy( eepbuf.format.data, sizeof( eepbuf.format.data ), buff, len );
		err = write_bytes( adr, buff, sizeof( eepbuf.format.data ) );
    }

	return err;
}
static ER write_byte_len( INT pno, B* buff, INT len )
{
//k    ER     err = E_ERR;
    ER     err = E_SUCCESS;
    UH    adr;

    adr = (UH)pno;

    if( buff )
    {
        etc_memset( eepbuf.format.data, sizeof( eepbuf.format.data ), 0xFF );
        etc_memcpy( eepbuf.format.data, sizeof( eepbuf.format.data ), buff, len );
		err = write_bytes( adr, buff, len );
    }

	return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̓Ǎ���
  �����Fssl     SPI��/CS�ԍ�
        pno     �y�[�W�ԍ�
        buff    �ǂݍ��񂾃f�[�^���i�[����o�b�t�@
        size    �o�b�t�@�̃T�C�Y
        len     [out]�ǂݍ��񂾃T�C�Y(0-120)
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER      read_page( INT pno, B* buff, INT size )
{
    ER     err;
    UH    adr;
    adr  = (UH)pno;

    // �P�y�[�W�ǂݍ���
    err = read_bytes( adr, NULL, size );
    if( err ) return err;

	memcpy( buff, eepbuf.format.data, size );

    return E_SUCCESS;
}
/*-------------------------------------------------------------------------------------------------
  �@�\�F�X�e�[�^�X�Ǎ���
  �����Fssl     SPI��/CS�ԍ�
  �����Fsts     [out]�ǂݍ��񂾃X�e�[�^�X
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER read_status( INT ssl, B* sts )
{
    ER     err;
    B    cmd;

    cmd = CMD_READ_STATUS;
    err = spi_send_recv(&cmd, 1, sts, 1 );
    return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�X�e�[�^�X��������
  �����Fssl     SPI��/CS�ԍ�
        sts     �������ރX�e�[�^�X
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER write_status( INT ssl, B sts )
{
    B    comd[2];
    ER     err;

    comd[0] = CMD_WRITE_STATUS;
    comd[1] = sts;

    err = write_enable( ssl );
    if( err ) return err;
    err = spi_send( ssl, comd, sizeof( comd ) );
    if( err ) return err;
    err = busy_wait( ssl );
    if( err ) return err;
    return E_SUCCESS;
}
/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̃Z�b�g
  �����Fbuff        �f�[�^���Z�b�g����o�b�t�@
        bufsi       �f�[�^���Z�b�g����o�b�t�@�̃T�C�Y
        chr         �Z�b�g����f�[�^
  �ߒl�F�Ȃ�
-------------------------------------------------------------------------------------------------*/
void etc_memset( B* buff, INT bufsiz, B chr )
{
    INT i;

    for( i = 0; i < bufsiz; i++ )
    {
        *buff = chr;
        buff++;
    }
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^�̃R�s�[
  �����Fdbuf        �R�s�[��o�b�t�@
        dsiz        �R�s�[��o�b�t�@�̃T�C�Y
        sbuf        �R�s�[���o�b�t�@
        ssiz        �R�s�[�T�C�Y
  �ߒl�F�R�s�[�����T�C�Y
-------------------------------------------------------------------------------------------------*/
INT etc_memcpy( B* dbuf, INT dsiz, const B* sbuf, INT ssiz )
{
    INT     siz;
    INT     i;

    if( dsiz > ssiz )
    {
        siz = ssiz;
    }
    else
    {
        siz = dsiz;
    }

    for( i = 0; i < siz; i++ )
    {
        *dbuf = *sbuf;
        dbuf++;
        sbuf++;
    }
    return siz;
}
/***********************************************************************
* 
* [�T�v]���[�U�[���
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetAllInfo( UB *getdat  )
{
	ER	result = E_SUCCESS;
	int datasiz;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

//	datasiz = sizeof( ST_PAGE1 ) + sizeof( ST_PAGE2 ) + sizeof( ST_PAGE3 ) + sizeof( ST_PAGE4 ) + sizeof( ST_PAGE5 ) + sizeof( ST_PAGE6 ) + sizeof( ST_PAGE7 ) + sizeof( ST_PAGE8 ) + sizeof( ST_PAGE9 ) + sizeof( ST_PAGE10 ) ;
	datasiz = sizeof( ST_PAGE1 ) + sizeof( ST_PAGE2 ) + sizeof( ST_PAGE3 ) + sizeof( ST_PAGE4 ) +  sizeof( ST_PAGE8 ) + sizeof( ST_PAGE9 ) + sizeof( ST_PAGE10 ) ;
	memcpy( getdat, &Page1.User[0].Id[0] , datasiz );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
/***********************************************************************
* 
* [�T�v]���[�U�[���
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetUserInfo( UB *getdat  )
{
	ER	result = E_SUCCESS;
	int datasiz;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	datasiz = sizeof( ST_PAGE1 );
	memcpy( getdat, &Page1.User[0].Id[0] , datasiz );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
/***********************************************************************
* 
* [�T�v]���x�p�^�[��
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetSpeedData( UB *getdat  )
{
	ER	result = E_SUCCESS;
	INT datasiz;
	UB *adr;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	datasiz = (sizeof( ST_SPEED )) * 5;

	adr = getdat;
	memcpy( adr, &Page2.Y0[0].ac,  datasiz );//1

	adr += datasiz;
	memcpy( adr, &Page2.Z0[0].ac,  datasiz );//2
	
	adr += datasiz;
	memcpy( adr, &Page3.P0[0].ac,  datasiz );//3
	
	adr += datasiz;
	memcpy( adr, &Page3.F0[0].ac,  datasiz );//4
	
	adr += datasiz;
	memcpy( adr, &Page4.Y1[0].ac,  datasiz );//5
	
	
	
	
//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
static INT datasiz_dbg;

/***********************************************************************
* 
* [�T�v]���x�p�^�[��
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetSpeedPattern( UB *getdat  )
{
	ER	result = E_SUCCESS;
	INT datasiz;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	datasiz = sizeof( ST_SPEED_PTN );
	memcpy( getdat, &Page4.SpPtn.Y0_SpdIniPtn,  datasiz );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
/***********************************************************************
* 
* [�T�v]�ʒu���
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetPositionInfo( UB *getdat  )
{
	ER	result = E_SUCCESS;
	INT datasiz;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	datasiz = sizeof( ST_PAGE7 ) ;
	memcpy( getdat, &Page7.Y0_Offset,  datasiz );
	getdat += datasiz;

	datasiz = sizeof( ST_PAGE8 ) ;
	memcpy( getdat, &Page8.Z0_SucStaHei1,  datasiz );
	getdat += datasiz;
	
	datasiz = sizeof( ST_PAGE9 ) ;
	memcpy( getdat, &Page9.Hosei[0].HS_VL1,  datasiz );
	
	
//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
/***********************************************************************
* 
* [�T�v]�I�v�V�������
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetOptionInfo( UB *getdat  )
{
	ER	result = E_SUCCESS;
	INT datasiz;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	datasiz = sizeof( ST_PAGE10 ) ;
	memcpy( getdat, &Page10.Login,  datasiz );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}

/***********************************************************************
* 
* [�T�v]�v���g�R���^�C�g���擾
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetProtocolData( UB *getdat ,  UB index )
{
	ER	result = E_SUCCESS;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	Prtinf.Dat[index].Stp = CLR;
	Prtinf.Dat[index].Prc = CLR;
	memcpy( getdat, &Prtinf.Dat[index].Cnt, 0x80 );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}

/***********************************************************************
* 
* [�T�v]�v���g�R���擾
* �����F*getdat    �i�[�� �擪�A�h���X
* 
* 
***********************************************************************/
ER	eep_GetProtocol( UB *getdat  )
{
	ER	result = E_SUCCESS;

//	MAC_WAI_SEM( ID_SEM_HDMOV );

	memcpy( getdat, &Prtwk, sizeof( ST_PROTCOL ) );

//	MAC_SIG_SEM( ID_SEM_HDMOV );

	return( result );
}
/******************************************************************************
* 
* [�T�v]	EEPROM�Ǎ��ݍς݃Z�b�g
* [����]	�Ώۃr�b�g
* [�ߒl]	�|
* 
******************************************************************************/
void EepBitSet( int dat )
{
	eepread_page |= dat;
}
/******************************************************************************
* 
* [�T�v]	EEPROM�Ǎ��ݍς݃N���A
* [����]	�Ώۃr�b�g
* [�ߒl]	�|
* 
******************************************************************************/
void EepBitClr( int dat )
{
	eepread_page &= (0xffffffff ^ dat);
}
/******************************************************************************
* 
* [�T�v]	�Ǎ��ݍς݂��m�F
* [����]	�Ώۃr�b�g
* [�ߒl]	TRUE:�Ǎ��ݍς�
* 
******************************************************************************/
BOOL EepBitGet( int dat )
{
	int wk;
	
	wk = eepread_page & dat;
	if( wk )	return( TRUE );
	
	return( FALSE );
}
/******************************************************************************
* 
* [�T�v]	EEPROM�Ǎ��ݍς݃N���A
* [����]	�Ώۃr�b�g
* [�ߒl]	�|
* 
******************************************************************************/
void EepBitAllClr()
{
	eepread_page = CLR;
}


void Rcvset_spddat( const char* str )
{
	//      s1:�R�s�[��A      s2:�R�s�[��
	StrHexcpy( (char * )&Page2.Y0[0].ac, (char * )str );
}

//void Rcvset_spdptn( const char* str )
//{
//	//      s1:�R�s�[��A      s2:�R�s�[��
//	StrHexcpy( (char * )&Page4.Y1[0].ac, (char * )str );
//}

void Rcvset_posdat( const char* str )
{
	//      s1:�R�s�[��A      s2:�R�s�[��
	StrHexcpy( (char * )&Page7.Y0_Offset, (char * )str );
}




