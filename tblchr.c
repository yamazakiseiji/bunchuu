/******************************************************************************
* �e�[�u����`                                                                *
*                                                                             *
* File name : tblchr.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include "define.h"


const char * STRTBL_MSG[45] = 
{
	"HELLO"    , /* 0  HELLO   �N���ʒm                                     */
	"LOG"      , /* 1  LOG     �t�@�[���E�F�A���̒ʒm                     */
	"VER"      , /* 2  VER     �o�[�W�����m�F                               */
	"ALLIN"    , /* 3  ALLIN   �SI/O����                                    */
	"BIN"      , /* 4  BIN     I/O����                                      */
	"BOUT"     , /* 5  BOUT    I/O�o��                                      */
	"CINIT"    , /* 6  CINIT   ���̓J�E���^�̐ݒ�                           */
	"COUNT"    , /* 7  COUNT   ���̓J�E���^�̓ǂݏo��                       */
	"INT"      , /* 8  INT     ���荞�ݐݒ�                                 */
	"ABSMV"    , /* 9  ABSMV   �p���X���[�^�ړ�(��Έʒu)                   */
	"ABORT"    , /* 10 ABORT   �p���X���[�^����~                           */
	"ENCODER"  , /* 11 ENCODER �G���R�[�_�̐ݒ�                             */
	"LIMIT"    , /* 12 LIMIT   �p���X���[�^���_���A                         */
	"MAP"      , /* 13 MAP     �Q���̃p���X���[�^�ړ�                       */
	"PDELAY"   , /* 14 PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�         */
	"POWER"    , /* 15 POWER   �p���X���[�^�̗㎥����                       */
	"PSET"     , /* 16 PSET    �p���X���[�^�̃J�E���^���Z�b�g               */
	"RELMV"    , /* 17 RELMV   �p���X���[�^�ړ�(���Έʒu)                   */
	"SET"      , /* 18 SET     �p���X���[�^���x�ݒ�                         */
	"STATUS"   , /* 19 STATUS  �p���X���[�^��ԓǂݏo��                     */
	"STOP"     , /* 20 STOP    �p���X���[�^������~                         */
	"BZCNT"    , /* 21 BZCNT                                                */
	"EEPREAD"  , /* 22 EEPREAD                                              */
	"EEPWRIT"  , /* 23 EEPWRIT                                              */
	"reserve"  , /* 24 reserve                                              */
	"reserve"  , /* 25 reserve                                              */
	"TMPSET"   , /* 26 reserve                                              */
	"TMPGET"   , /* 27 reserve                                              */
	"reserve"  , /* 26 reserve                                              */
	"VINI"     , /* 29 reserve                                              */
	"PINI"     , /* 30 reserve                                              */
	"PREN"     , /* 31 reserve                                              */
	"PTAN"     , /* 32 reserve                                              */
	"PCAN"     , /* 33 reserve                                              */
	"VOUT"     , /* 34 reserve                                              */
	"PSNS"     , /* 35 reserve                                              */
	"IMPPDAT"  , /* 36 IMPPDAT                                              */
	"IMPSPED"  , /* 37 IMPSPED                                              */
	"IMPPOSI"  , /* 38 IMPPOSI                                              */
	"IMPOPTN"  , /* 39 IMPOPTN                                              */
	"EXPRUNL"  , /* 40 EXPRUNL                                              */
	"EXPCOML"  , /* 41 EXPCOML                                              */
	"reserve"  , /* 27 reserve                                              */
	"reserve"  , /* 28 reserve                                              */
	"reserve"    /* 29 reserve                                              */
};



const char STRTBL_CW[] = {"CW"};
const char STRTBL_BCERR[] = {"ERROR"};
const char STRTBL_BCCOM[] = {"COMER"};


// ASC�ϊ��p
const char ASC_CHG[16]  = 
{
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};
const char ASC_CHG_LOWER[16]  = 
{
	'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
};

//
// === cyc handler =====================================================
//



//
// === seg proc =====================================================
//
const char num_seg[] = 
{ 
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,			/* 00 */	
	0xBF,0x86,0xDB,0xBF,0xE6,0xED,0xFD,0xA7,0xFF,0xEF,0xF7,0xFC,0xB9,0xDE,0xF9,0xF1  			/* 10 */	/* Dot�t��*/
};

