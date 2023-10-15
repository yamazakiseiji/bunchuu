/******************************************************************************
* テーブル定義                                                                *
*                                                                             *
* File name : tblchr.c                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include "define.h"


const char * STRTBL_MSG[45] = 
{
	"HELLO"    , /* 0  HELLO   起動通知                                     */
	"LOG"      , /* 1  LOG     ファームウェア情報の通知                     */
	"VER"      , /* 2  VER     バージョン確認                               */
	"ALLIN"    , /* 3  ALLIN   全I/O入力                                    */
	"BIN"      , /* 4  BIN     I/O入力                                      */
	"BOUT"     , /* 5  BOUT    I/O出力                                      */
	"CINIT"    , /* 6  CINIT   入力カウンタの設定                           */
	"COUNT"    , /* 7  COUNT   入力カウンタの読み出し                       */
	"INT"      , /* 8  INT     割り込み設定                                 */
	"ABSMV"    , /* 9  ABSMV   パルスモータ移動(絶対位置)                   */
	"ABORT"    , /* 10 ABORT   パルスモータ即停止                           */
	"ENCODER"  , /* 11 ENCODER エンコーダの設定                             */
	"LIMIT"    , /* 12 LIMIT   パルスモータ原点復帰                         */
	"MAP"      , /* 13 MAP     ２軸のパルスモータ移動                       */
	"PDELAY"   , /* 14 PDELAY  パルスモータのパワー制御待ち時間設定         */
	"POWER"    , /* 15 POWER   パルスモータの励磁制御                       */
	"PSET"     , /* 16 PSET    パルスモータのカウンタリセット               */
	"RELMV"    , /* 17 RELMV   パルスモータ移動(相対位置)                   */
	"SET"      , /* 18 SET     パルスモータ速度設定                         */
	"STATUS"   , /* 19 STATUS  パルスモータ状態読み出し                     */
	"STOP"     , /* 20 STOP    パルスモータ減速停止                         */
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


// ASC変換用
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
	0xBF,0x86,0xDB,0xBF,0xE6,0xED,0xFD,0xA7,0xFF,0xEF,0xF7,0xFC,0xB9,0xDE,0xF9,0xF1  			/* 10 */	/* Dot付き*/
};

