/******************************************************************************
* 自動装置固有情報                                                            *
*                                                                             *
* File name : automachine.c                                                   *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include "define.h"
#include "automachine.h"


// *********************************
// システム情報
// *********************************
	// ファームウェア名称("NCP" + "-" + JOB名  ※NCP:NM-CTRL-PCBの略)
	//                        1234567890
	const char FIRMWARE[] = {"NCP-QuickGene12S"};

	// バージョン             123456789012345
	const char VER_INFO[] = {"Ver.0.99 rev.0907A"};
	const char VER_INFO_TO_TP[] = {"0907A"};

	// エンコーダー有無情報  0:無、1:有
	const char ENCODER_EXIST[8]  = 
	{
		0x00,							// [0] DEVICE 07 - DEVICE 00
		0x00,							// [1] DEVICE 15 - DEVICE 08
		0x00,							// [2] DEVICE 23 - DEVICE 16
		0x00,							// [3] DEVICE 31 - DEVICE 24
		0x00,							// [4] DEVICE 39 - DEVICE 32
		0x00,							// [5] DEVICE 47 - DEVICE 40
		0x00,							// [6] DEVICE 55 - DEVICE 48
		0x00							// [7] DEVICE 63 - DEVICE 56
	};

	const char USER_KURABOU[] = {"KURABO"};
	const char PASS_KURABOU[] = {"1212"};


// *********************************
// デバイス割り当て情報
// *********************************
// 0x00:MNET-BCD4020FU
// 0x01:MNET-BCD4020FUA
// 0x02:MNET-D420(入力16ch)
// 0x03:MNET-D411(入力8ch,出力8ch)
// 0x04:MNET-D402(出力16ch)
// 0x05:MNET-D340(入力32ch)
// 0x06:MNET-D322(入力16ch,出力16ch)
// 0x07:MNET-D304(出力32ch)
// 0xFF:NONE
//
// 補足）
//      ------      分注Y0      分注Z                                                       ------         DEVICE 00 ～ DEVICE 07
//      ------      ------      ------      ------      ------      ------      ------      ------         DEVICE 08 ～ DEVICE 15
//      ------      ------      ------      ------      ------      ------      ------      ------         DEVICE 16 ～ DEVICE 23
//      ------      ------      ------      ------      ------      ------      ------      ------         DEVICE 24 ～ DEVICE 31
	const char MNET_DRV_TYPE[32]  = 
	{
		PCD4611   , PCD4611   , BCD4020FUA, BCD4020FUA, BCD4020FUA, BCD4020FUA, NCPRSUNIT , NONE      , // DEVICE 00 ～ DEVICE 07 debug
//		NONE      , BCD4020FUA, BCD4020FUA, BCD4020FUA, BCD4020FUA, BCD4020FUA, BCD4020FUA, NONE      , // DEVICE 00 ～ DEVICE 07
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 08 ～ DEVICE 15
		NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 16 ～ DEVICE 23
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE        // DEVICE 24 ～ DEVICE 31
	};

	// デバイス番号
	volatile const unsigned char MNET_M204DUM_ASGN[32]  = 
	{
//	    NONE      , 1         , 2         , 3         , 4         , 5         , 6         , 7         , // DEVICE 00 ～ DEVICE 07
	    0         , 1         , 2         , 3         , 4         , 5         , 6         , 7         , // DEVICE 00 ～ DEVICE 07
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 08 ～ DEVICE 15
		NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 16 ～ DEVICE 23
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE        // DEVICE 24 ～ DEVICE 31
	};

	// デバイス番号
	volatile const unsigned char MNET_NO[32]  = 
	{
//	    NONE      , 1         , 2         , 3         , 4         , 5         , 6         , 7         , // DEVICE 00 ～ DEVICE 07
	    0         , 1         , 2         , 3         , 4         , 5         , 6         , 7         , // DEVICE 00 ～ DEVICE 07
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 08 ～ DEVICE 15
        NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , // DEVICE 16 ～ DEVICE 23
	    NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE      , NONE        // DEVICE 24 ～ DEVICE 31
	};


// *********************************
// Ｉ／Ｏポート割り当て情報
// *********************************
	// Input Ch
	const char INPUTCH_INFTBL[(MAX_CH+1)][3] = {
	//	Device  Port    Bit
		NONE,   NONE,   NONE,   // 0   空き
		0   ,   0   ,   BIT0,   // 1 ドアスイッチ1
		0   ,   0   ,   BIT1,   // 2 ドアスイッチ2
		0   ,   0   ,   BIT2,   // 3  予備
		0   ,   0   ,   BIT3,   // 4  予備
		0   ,   0   ,   BIT4,   // 5  予備
		0   ,   0   ,   BIT5,   // 6  予備
		0   ,   0   ,   BIT6,   // 7  予備
		0   ,   0   ,   BIT7,   // 8  予備
		2   ,   2   ,   BIT4,   // 9  サンプルチューブ1有無センサ
		2   ,   2   ,   BIT5,   // 10 サンプルチューブ2有無センサ
		2   ,   2   ,   BIT6,   // 11 サンプルチューブ3有無センサ
		2   ,   2   ,   BIT7,   // 12 サンプルチューブ4有無センサ
		3   ,   2   ,   BIT4,   // 33 ジャミングセンサ1
		3   ,   2   ,   BIT5,   // 35 チップセンサ1
		3   ,   2   ,   BIT6,   // 15 サンプルチューブ7有無センサ
		3   ,   2   ,   BIT7,   // 16 サンプルチューブ8有無センサ
		4   ,   2   ,   BIT4,   // 17 トレイSW
		4   ,   2   ,   BIT5,   // 36 チップセンサ2
		4   ,   2   ,   BIT6,   // 19 温調ALM1
		4   ,   2   ,   BIT7,   // 20 温調ALM2
		5   ,   2   ,   BIT4,   // 51 STOPボタン
		5   ,   2   ,   BIT5,   // 52 カスタムボタン
		5   ,   2   ,   BIT6,   // 23 サンプルチューブ11有無センサ
		5   ,   2   ,   BIT7,   // 24 サンプルチューブ12有無センサ
		NONE,   NONE,   NONE,   // 25 
		NONE,   NONE,   NONE,   // 26 
		NONE,   NONE,   NONE,   // 27 
		NONE,   NONE,   NONE,   // 28 
		NONE,   NONE,   NONE,   // 29 
		NONE,   NONE,   NONE,   // 30 
		NONE,   NONE,   NONE,   // 31 
		NONE,   NONE,   NONE,   // 32 
		NONE,   NONE,   NONE,   // 33 
		NONE,   NONE,   NONE,   // 34 
		NONE,   NONE,   NONE,   // 35 
		NONE,   NONE,   NONE,   // 36 
		NONE,   NONE,   NONE,   // 37 
		NONE,   NONE,   NONE,   // 38 
		NONE,   NONE,   NONE,   // 39 
		NONE,   NONE,   NONE,   // 40 
		NONE,   NONE,   NONE,   // 41 
		NONE,   NONE,   NONE,   // 42 
		NONE,   NONE,   NONE,   // 43 
		NONE,   NONE,   NONE,   // 44 
		NONE,   NONE,   NONE,   // 45 
		NONE,   NONE,   NONE,   // 46 
		NONE,   NONE,   NONE,   // 47 
		NONE,   NONE,   NONE,   // 48 
		NONE,   NONE,   NONE,   // 49 
		NONE,   NONE,   NONE,   // 50 
		NONE,   NONE,   NONE,   // 51 
		NONE,   NONE,   NONE,   // 52 
		NONE,   NONE,   NONE,   // 53 
		NONE,   NONE,   NONE,   // 54 
		NONE,   NONE,   NONE,   // 55 
		NONE,   NONE,   NONE,   // 56 
		NONE,   NONE,   NONE,   // 57 
		NONE,   NONE,   NONE,   // 58 
		NONE,   NONE,   NONE,   // 59 
		NONE,   NONE,   NONE,   // 60 
		NONE,   NONE,   NONE,   // 61 
		NONE,   NONE,   NONE,   // 62 
		NONE,   NONE,   NONE    // 63 
	};

	// Output Ch
	const char OUTPUTCH_INFTBL[(MAX_CH+1)][3] = {
	//	Device  Port    Bit
		NONE,   NONE,   NONE,   // 0  空き
		0	,	0	,	BIT0,   // 1  ダイアフラムポンプ
		2	,	3	,	BIT1,   // 2  LED照明
		2	,	3	,	BIT2,   // 3  UVランプ
		3	,	3	,	BIT1,   // 4  ドアロック1
		3	,	3	,	BIT2,   // 5  ドアロック2
		4	,	3	,	BIT1,   // 6  DCファン
		4	,	3	,	BIT2,   // 7  パトライト1
		5	,	3	,	BIT1,   // 8  パトライト2
		5	,	3	,	BIT2,   // 9  パトライトBz1
		NONE,	NONE,	NONE,   // 10 
		NONE,	NONE,	NONE,   // 11 
		NONE,	NONE,	NONE,   // 12 
		NONE,	NONE,	NONE,   // 13 
		NONE,	NONE,	NONE,   // 14 
		NONE,	NONE,	NONE,   // 15 
		NONE,	NONE,	NONE,   // 16 
		NONE,	NONE,	NONE,   // 17 
		NONE,	NONE,	NONE,   // 18 
		NONE,	NONE,	NONE,   // 19 
		NONE,	NONE,	NONE,   // 20 
		NONE,	NONE,	NONE,   // 21 
		NONE,	NONE,	NONE,   // 22 
		NONE,	NONE,	NONE,   // 23 
		NONE,	NONE,	NONE,   // 24 
		NONE,	NONE,	NONE,   // 25 
		NONE,	NONE,	NONE,   // 26 
		NONE,	NONE,	NONE,   // 27 
		NONE,	NONE,	NONE,   // 28 
		NONE,	NONE,	NONE,   // 29 
		NONE,	NONE,	NONE,   // 30 
		NONE,	NONE,	NONE,   // 31 
		NONE,	NONE,	NONE,   // 32 
		NONE,	NONE,	NONE,   // 33 
		NONE,	NONE,	NONE,   // 34 
		NONE,	NONE,	NONE,   // 35 
		NONE,	NONE,	NONE,   // 36 
		NONE,	NONE,	NONE,   // 37 
		NONE,	NONE,	NONE,   // 38 
		NONE,	NONE,	NONE,   // 39 
		NONE,	NONE,	NONE,   // 40 
		NONE,	NONE,	NONE,   // 41 
		NONE,	NONE,	NONE,   // 42 
		NONE,	NONE,	NONE,   // 43 
		NONE,	NONE,	NONE,   // 44 
		NONE,	NONE,	NONE,   // 45 
		NONE,	NONE,	NONE,   // 46 
		NONE,	NONE,	NONE,   // 47 
		NONE,	NONE,	NONE,   // 48 
		NONE,	NONE,	NONE,   // 49 
		NONE,	NONE,	NONE,   // 50 
		NONE,	NONE,	NONE,   // 51 
		NONE,	NONE,	NONE,   // 52 
		NONE,	NONE,	NONE,   // 53 
		NONE,	NONE,	NONE,   // 54 
		NONE,	NONE,	NONE,   // 55 
		NONE,	NONE,	NONE,   // 56 
		NONE,	NONE,	NONE,   // 57 
		NONE,	NONE,	NONE,   // 58 
		NONE,	NONE,	NONE,   // 59 
		NONE,	NONE,	NONE,   // 60 
		NONE,	NONE,	NONE,   // 61 
		NONE,	NONE,	NONE,   // 62 
		NONE,	NONE,	NONE    // 63 
	};




