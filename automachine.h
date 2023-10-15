/******************************************************************************
* 装置固有情報                                                                *
*                                                                             *
* File name : machien.h                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _MACHINE_H_
#define _MACHINE_H_


#define DEBUG_SIM_MODE			0		// ☆ 2 = 通信タイムアウト,1 = DEBUG用, 0 = 製品用
#define DEBUG_SIM_PRESS			0		// ☆ 1 = 圧力センサ基板シミュレート, 0 = 圧力センサ基板接続
#define DEBUG_DROPC_OPEN		0		// ☆ 1 = DROPC OPENのまま、0 = 通常使用
#define DEBUG_LOG_SUCK0			0		// ☆ 1 = C-SUCK0トレース出力
#define DEBUG_LOG_MIX00			0		// ☆ 1 = C-MIX00トレース出力
#define DEBUG_TEMP_DEV			0		// ☆ 1 = 温調器シミュレート、0 = 実機
//*******************************
// モータ数
//*******************************

#define DVMIN					1		// モータ番号最小値
#define DVMAX					7		// モータ番号最大値

//*******************************
// チャネル数
//*******************************
#define MIN_CH					1		// チャネル最小数
#define MAX_CH					63		// チャネル最大数
#define	INMIN					1		// 
#define	INMAX					10		// 
#define OUMIN					0		// 
#define	OUMAX					10		// 
#define NZLNUM					1		// ノズル搭載数( 0:搭載数1,1:搭載数2 ) 
#define	NONE					0xff
#define	PROTMAX					20


//*******************************
// モータデバイスタイプ
//*******************************
#define	BCD4020FU				0x00	// MNET-BCD4020FU
#define	BCD4020FUA				0x01	// MNET_BCD4020FUA
#define	D420					0x02	// MNET-D420(入力16ch)
#define	D411					0x03	// MNET-D411(入力8ch,出力8ch)
#define	D402					0x04	// MNET-D402(出力16ch)
#define	D340					0x05	// MNET-D340(入力32ch)
#define	D322					0x06	// MNET-D322(入力16ch,出力16ch)
#define	D304					0x07	// MNET-D304(出力32ch)
#define	M204DUM					0x08	// MNET-M204DUM
#define	PCD4611					0x09	// PCD4611（NICHIYO MOTOR DRIVER）
#define	NCPRSUNIT				0x0A	// ニチリョー圧力ユニット


//*******************************
// モータデバイスＩＤ
// FFh:未実装
//*******************************
#define MT_Y0				1		// Y0軸（分注ヘッド）
#define MT_Z0				2		// Z軸（分注ヘッド）
#define MT_P0				3		// プランジャ軸（分注ヘッド）
#define MT_F0				4		// フォトセンサ稼働軸
#define MT_Y1				5		// Y1軸（トレイ稼働）
#define MT_U0				6		// NICIRYO圧力検知ユニット


//*******************************
// 出力ＣＨ
//*******************************

	enum OUT_CH
	{
		O_DAIAPMP = 1 ,	// ダイアフラムポンプ
		O_LIGHLED = 2 ,	// 照明LED
		O_UVLAMP1 = 3 ,	// UVランプ
		O_DORLOCK = 4 ,	// ドアロック
		O_POWERDW = 5,	// 電源OFF
		O_DCFAN01 = 6 ,	// DCファン
		O_PATLRED = 7 ,	// パトライト1
		O_PATLGRN = 8 ,	// パトライト2
		O_BUZZER1 = 9 ,	// ブザー1
		O_RESRV10 = 10,	// 
		O_RESRV11 = 11,	// 
		O_RESRV12 = 12,	// 
		O_RESRV13 = 13,	// 
		O_RESRV14 = 14,	// 
		O_RESRV15 = 15,	// 
		O_RESRV16 = 16,	// 
		O_RESRV17 = 17,	// 空き
		O_RESRV18 = 18,	// 空き
		O_RESRV19 = 19,	// 空き
		O_RESRV20 = 20,	// 空き
		O_RESRV21 = 21,	// 空き
		O_RESRV22 = 22,	// 空き
		O_RESRV23 = 23,	// 空き
		O_RESRV24 = 24,	// 空き
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
// 入力ＣＨ
//*******************************

	// ＣＨインデックス
	enum IN_CH
	{
		I_DOORSW1 = 1 ,	// ドアスイッチ1
		I_DOORSW2 = 2 ,	// ドアスイッチ2
		I_RESRV03 = 3,	// 予備
		I_RESRV04 = 4,	// 予備
		I_RESRV05 = 5,	// 予備
		I_RESRV06 = 6,	// 予備
		I_RESRV07 = 7,	// 予備
		I_RESRV08 = 8,	// 予備
		I_SPTUB01 = 9 ,	// サンプルチューブ1有無センサ
		I_SPTUB02 = 10,	// サンプルチューブ2有無センサ
		I_SPTUB03 = 11,	// サンプルチューブ3有無センサ
		I_SPTUB04 = 12,	// サンプルチューブ4有無センサ
		I_SPTUB05 = 13,	// サンプルチューブ5有無センサ
		I_SPTUB06 = 14,	// サンプルチューブ6有無センサ
		I_SPTUB07 = 15,	// サンプルチューブ7有無センサ
		I_SPTUB08 = 16,	// サンプルチューブ8有無センサ
		I_TRAYSW1 = 17,	// トレイSW
		I_COLTUB1 = 18,	// 回収チューブ有無センサ
		I_TMPALM1 = 19,	// 温調ALM1
		I_TMPALM2 = 20,	// 温調ALM2
		I_SPTUB09 = 21,	// サンプルチューブ9有無センサ
		I_SPTUB10 = 22,	// サンプルチューブ10有無センサ
		I_SPTUB11 = 23,	// サンプルチューブ11有無センサ
		I_SPTUB12 = 24,	// サンプルチューブ12有無センサ
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
// 入力センサパターン
//*******************************

// 液面検出
enum LIQSNS_PTN
{
	LIQSNS_OFF = 0,		// 
	LIQSNS_ON = 1		// 検出済み
};

// 詰まり検出
enum TUMARI_PTN
{
	TUMARI_OFF = 0,		// 
	TUMARI_ON = 1		// 詰まり状態
};

// 空吸い検出
enum KARASU_PTN
{
	KARASU_OFF = 0,		// 
	KARASU_ON = 1		// 空吸い状態
};

// チップセンサ
enum TIPSNS_PTN
{
	TIPSNS_NON = 0,		// チップ無
	TIPSNS_EXT = 1		// チップ有
};

// ジャミングセンサ
enum DOOR_SW_PTN
{
	DOOR_SW_OFF = 0,	// 
	DOOR_SW_ON = 1		// SW ON
};

// PAUSEボタン
enum PAUSESW_PTN
{
	PAUSESW_OFF = 0,	// 
	PAUSESW_ON = 1		// SW ON
};

// STOPボタン
enum STOP_SW_PTN
{
	STOP_SW_OFF = 0,	// 
	STOP_SW_ON = 1		// SW ON
};

// 開閉栓対象キャップ
enum LED_PTN
{
	LED_OFF = 0,	// 消灯
	LED_ON  = 1		// 点灯
};


// ポジション番号
	enum POS_CH
	{
		POS_Y0_OFFSE = 1 ,   // [Y0] 仮想原点オフセット値               ポジション番号１
		POS_Z0_OFFSE = 2 ,   // [Z0] 仮想原点オフセット値               ポジション番号２
		POS_P0_OFFSE = 3 ,   // [P0] 仮想原点オフセット値               ポジション番号３
		POS_F0_OFFSE = 4 ,   // [F0] 仮想原点オフセット値               ポジション番号４
		POS_Y1_OFFSE = 5 ,   // [Y1] 仮想原点オフセット値               ポジション番号５
		POS_Y0_TAIHI = 6 ,   // [Y0] 退避位置                           ポジション番号６
		POS_Z0_TAIHI = 7 ,   // [Z0] 退避位置                           ポジション番号７
		POS_P0_TAIHI = 8 ,   // [P0] 退避位置                           ポジション番号８
		POS_F0_TAIHI = 9 ,   // [F0] 退避位置                           ポジション番号９
		POS_Y1_TAIHI = 10,   // [Y1] 退避位置                           ポジション番号１０
		POS_STRIPTP1 = 11,   // [Y0] 試薬ストリップ-ショートチップ１    ポジション番号１１
		POS_STRIPTP2 = 12,   // [Y0] 試薬ストリップ-ショートチップ２    ポジション番号１２
		POS_STRIPTPL = 13,   // [Y0] 試薬ストリップ-ロングチップ        ポジション番号１３
		POS_STRIPTUB = 14,   // [Y0] 試薬ストリップ-サンプルチューブ    ポジション番号１４
		POS_STRIPSM6 = 15,   // [Y0] 試薬ストリップ-抽出用試薬6         ポジション番号１５
		POS_STRIPSM5 = 16,   // [Y0] 試薬ストリップ-抽出用試薬5         ポジション番号１６
		POS_STRIPSM4 = 17,   // [Y0] 試薬ストリップ-抽出用試薬4         ポジション番号１７
		POS_STRIPSM3 = 18,   // [Y0] 試薬ストリップ-抽出用試薬3         ポジション番号１８
		POS_STRIPSM2 = 19,   // [Y0] 試薬ストリップ-抽出用試薬2         ポジション番号１９
		POS_STRIPSM1 = 20,   // [Y0] 試薬ストリップ-抽出用試薬1         ポジション番号２０
		POS_STRIPCAT = 21,   // [Y0] 試薬ストリップ-カートリッジ        ポジション番号２１
		POS_STRIPCOL = 22,   // [Y0] 試薬ストリップ-核酸回収液          ポジション番号２２
		POS_Z0_STOPE = 23,   // [Z0] 試薬ストリップ-シール開け最終位置	ポジション番号２３
		POS_Z0_STOP1 = 24,   // [Z0] 試薬ストリップ-シール開け１段位置	ポジション番号２４
		POS_Z0_STRIP = 25,   // [Z0] 試薬ストリップ-加圧位置	        ポジション番号２５
		POS_Z0_TPGTE = 26,   // [Z0] チップ取得最終位置                 ポジション番号２６
		POS_Z0_TPGT1 = 27,   // [Z0] チップ取得１段位置                 ポジション番号２７
		POS_Z0_TPDLE = 28,   // [Z0] チップ廃棄最終位置                 ポジション番号２８
		POS_Z0_TPDL1 = 29,   // [Z0] チップ廃棄１段位置                 ポジション番号２９
		POS_Z0_LIQPU = 30,   // [Z0] 廃液吐出位置                       ポジション番号３０
		POS_Z0_COPOS = 31,   // [Z0] コレクション 回収チューブ位置      ポジション番号３１
		POS_Z0_COLDS = 32,   // [Z0] 廃液チューブ位置                   ポジション番号３２
		//-------------------------------------------------------------------------------
		POS_Z0_SUCS1 = 33,   // [Z0] 吸引開始高さ1                      ポジション番号３３
		POS_Z0_SUCS2 = 34,   // [Z0] 吸引開始高さ2                      ポジション番号３４
		POS_Z0_SUCS3 = 35,   // [Z0] 吸引開始高さ3                      ポジション番号３５
		POS_Z0_SUCS4 = 36,   // [Z0] 吸引開始高さ4                      ポジション番号３６
		POS_Z0_SUCS5 = 37,   // [Z0] 吸引開始高さ5                      ポジション番号３７
		POS_Z0_SUCE1 = 38,   // [Z0] 吸引終了高さ1                      ポジション番号３８
		POS_Z0_SUCE2 = 39,   // [Z0] 吸引終了高さ2                      ポジション番号３９
		POS_Z0_SUCE3 = 40,   // [Z0] 吸引終了高さ3                      ポジション番号４０
		POS_Z0_SUCE4 = 41,   // [Z0] 吸引終了高さ4                      ポジション番号４１
		POS_Z0_SUCE5 = 42,   // [Z0] 吸引終了高さ5                      ポジション番号４２
		POS_Z0_PUTS1 = 43,   // [Z0] 吐出開始高さ1                      ポジション番号４３
		POS_Z0_PUTS2 = 44,   // [Z0] 吐出開始高さ2                      ポジション番号４４
		POS_Z0_PUTS3 = 45,   // [Z0] 吐出開始高さ3                      ポジション番号４５
		POS_Z0_PUTS4 = 46,   // [Z0] 吐出開始高さ4                      ポジション番号４６
		POS_Z0_PUTS5 = 47,   // [Z0] 吐出開始高さ5                      ポジション番号４７
		POS_Z0_PUTE1 = 48,   // [Z0] 吐出終了高さ1                      ポジション番号４８
		POS_Z0_PUTE2 = 49,   // [Z0] 吐出終了高さ2                      ポジション番号４９
		POS_Z0_PUTE3 = 50,   // [Z0] 吐出終了高さ3                      ポジション番号５０
		POS_Z0_PUTE4 = 51,   // [Z0] 吐出終了高さ4                      ポジション番号５１
		POS_Z0_PUTE5 = 52,   // [Z0] 吐出終了高さ5                      ポジション番号５２
		POS_Z0_MIXS1 = 53,   // [Z0] 撹拌吸引開始高さ                   ポジション番号５３
		POS_Z0_MIXE1 = 54,   // [Z0] 撹拌吸引終了高さ                   ポジション番号５４
		POS_P0_TIPDL = 55,   // [P0] 廃棄動作		                    ポジション番号５５
		POS_XX_RSV56 = 56,   // 0固定	 	                            ポジション番号５６
		POS_XX_RSV57 = 57,   // 0固定	 	                            ポジション番号５７
		POS_XX_RSV58 = 58,   // 0固定	 	                            ポジション番号５８
		POS_XX_RSV59 = 59,   // 0固定	 	                            ポジション番号５９
		POS_XX_RSV60 = 60,   // 0固定	 	                            ポジション番号６０
		POS_XX_RSV61 = 61,   // 0固定	 	                            ポジション番号６１
		POS_XX_RSV62 = 62,   // 0固定	 	                            ポジション番号６２
		POS_XX_RSV63 = 63,   // 0固定	 	                            ポジション番号６３
		POS_XX_RSV64 = 64,   // 0固定	 	                            ポジション番号６４
		//-------------------------------------------------------------------------------
		POS_H1_C1VO1 = 65,   // 補正パターン１	容量 10%                ポジション番号６５
		POS_H1_C1VO2 = 66,   //                 容量 50%          		ポジション番号６６
		POS_H1_C1VO3 = 67,   //                 容量100%           		ポジション番号６７
		POS_H1_C1OUT = 68,   //                 ブローアウトエア量      ポジション番号６８
		POS_H1_C1AIR = 69,   //                 エアキャップ量          ポジション番号６９
		POS_H1_C1TRA = 70,   //                 余剰量 			        ポジション番号７０
		POS_H1_C2VO1 = 71,   // 補正パターン２  容量 10%                ポジション番号７１
		POS_H1_C2VO2 = 72,   //                 容量 50%   	            ポジション番号７２
		POS_H1_C2VO3 = 73,   //                 容量100%           		ポジション番号７３
		POS_H1_C2OUT = 74,   //                 ブローアウトエア量      ポジション番号７４
		POS_H1_C2AIR = 75,   //                 エアキャップ量          ポジション番号７５
		POS_H1_C2TRA = 76,   //                 余剰量 			        ポジション番号７６
		POS_H1_C3VO1 = 77,   // 補正パターン３  容量 10%                ポジション番号７７
		POS_H1_C3VO2 = 78,   //                 容量 50%          		ポジション番号７８
		POS_H1_C3VO3 = 79,   //                 容量100%           		ポジション番号７９
		POS_H1_C3OUT = 80,   //                 ブローアウトエア量      ポジション番号８０
		POS_H1_C3AIR = 81,   //                 エアキャップ量          ポジション番号８１
		POS_H1_C3TRA = 82,   //                 余剰量 			        ポジション番号８２
		POS_XX_RSV83 = 83,   // 0固定		                            ポジション番号８３
		POS_XX_RSV84 = 84,   // 0固定		                            ポジション番号８４
		POS_XX_RSV85 = 85,   // 0固定		                            ポジション番号８５
		POS_XX_RSV86 = 86,   // 0固定		                            ポジション番号８６
		POS_XX_RSV87 = 87,   // 0固定		                            ポジション番号８７
		POS_XX_RSV88 = 88,   // 0固定		                            ポジション番号８８
		POS_XX_RSV89 = 89,   // 0固定		                            ポジション番号８９
		POS_XX_RSV90 = 90,   // 0固定		                            ポジション番号９０
		POS_XX_RSV91 = 91,   // 0固定		                            ポジション番号９１
		POS_XX_RSV92 = 92,   // 0固定		                            ポジション番号９２
		POS_XX_RSV93 = 93,   // 0固定		                            ポジション番号９３
		POS_XX_RSV94 = 94,   // 0固定		                            ポジション番号９４
		POS_XX_RSV95 = 95,   // 0固定		                            ポジション番号９５
		POS_XX_RSV96 = 96,   // 0固定		                            ポジション番号９６
	};

// 
enum STOP_STATUS
{
	IDLE_STS = 0,	// 
	STOP_STS = 1,	// 
	PAUS_STS = 2	// 
};


//*******************************
// automachine.c 外部参照
//*******************************


// *********************************
// システム情報
// *********************************

// ファームウェア名称
extern const char FIRMWARE[];

// バージョン
extern const char VER_INFO[];
extern const char VER_INFO_TO_TP[];

// エンコーダー有無情報  0:無、1:有
extern const char ENCODER_EXIST[8];






// *********************************
// デバイス割り当て情報
// *********************************

extern const char MNET_DRV_TYPE[32];
extern volatile const unsigned char MNET_NO[32] ;
extern volatile const unsigned char MNET_M204DUM_ASGN[32];





// *********************************
// Ｉ／Ｏポート割り当て情報
// *********************************

// ポート入力アサイン
extern const char INPUTCH_INFTBL[(MAX_CH+1)][3];

// ポート出力アサイン
extern const char OUTPUTCH_INFTBL[(MAX_CH+1)][3];


#endif  // _MACHINE_H_

