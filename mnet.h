/******************************************************************************
* モーションネット管理タスク                                                  *
*                                                                             *
* File name : tskmnt.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _MNTTASK_H_
#define _MNTTASK_H_


//--------------------------------------
// マクロ置換の定義
//--------------------------------------
#define	MNET_BASE_ADDRESS			0x0C008000	// Motionnetアドレス
#define	PULSEMOTOR_ADDR				0x0C009000	// パルスモータコントローラアドレス

#define	MOTIONNET_DEVICE_MAX		64			// Motionnetのデバイス数
#define	MOTIONNET_PORT_MAX			4			// Motionnetのポート数

#define	MOTIONNET_CMD_CYCLIC_START	0x3000		// サイクリック通信の開始
#define	MOTIONNET_CMD_CYCLIC_STOP	0x3100		// サイクリック通信の停止
#define	MOTIONNET_CMD_DATA_START	0x4000		// データ通信
#define	MOTIONNET_CMD_DATA_CANCEL	0x4100		// データ通信キャンセル
#define	MOTIONNET_CMD_WRENV0		0x5500		// [W] 環境設定0


#define	ON							1
#define	OFF							0

// 逓倍設定
#define	EC_N						0			// 無効
#define	EC_1						1			// １逓倍
#define	EC_2						2			// ２逓倍
#define	EC_3						3			// ４逓倍

// カウンタ設定
#define	ENC_COUNTER					1			// エンコーダ
#define	PLS_COUNTER					2			// 出力パルス





//--------------------------------------
// 構造体の定義
//--------------------------------------
 ///<summary>
/// モーターコマンド
///</summary>
enum MOTOR_COMMAND
{
	CMD_NOTYPE	= 0,			// 無効コマンド
	CMD_STAFL	= 0x0050,		// ＦＬ定速スタート
	CMD_STAFH	= 0x0051,		// ＦＨ定速スタート
	CMD_STAUD	= 0x0053,		// 高速スタート(加速→FH 定速→減速)
	CMD_CNTFL	= 0x0054,		// 残量ＦＬ定速スタート
	CMD_CNTFH	= 0x0055,		// 残量ＦＨ定速スタート
	CMD_CNTUD	= 0x0057,		// 残量高速スタート
	CMD_CMSTA	= 0x0006,		// ＃ＳＴＡ出力(同時スタート)
	CMD_SPSTA	= 0x002A,		// ＃ＳＴＡ入力代行
	CMD_FCHGL	= 0x0040,		// ＦＬ定速へ瞬時速度変更
	CMD_FCHGH	= 0x0041,		// ＦＨ定速へ瞬時速度変更
	CMD_FSCHL	= 0x0042,		// ＦＬ速度まで減速
	CMD_FSCHH	= 0x0043,		// ＦＨ速度まで加速
	CMD_STOP	= 0x0049,		// 即停止
	CMD_SDSTP	= 0x004A,		// 減速停止
	CMD_CMSTP	= 0x0007,		// ＃ＳＴＰ出力(同時停止)
	CMD_CMEMG	= 0x0005,		// 非常停止
	CMD_NOP		= 0x0000,		// 無効コマンド
	CMD_INTRS	= 0x0008,		// 割り込み出力(ﾒｲﾝｽﾃｰﾀｽ_Bit0)リセット
	CMD_SRST	= 0x0004,		// ソフトウェアリセット
	CMD_CUN1R	= 0x0020,		// COUNTER1(指令位置)リセット
	CMD_CUN2R	= 0x0021,		// COUNTER2(機械位置)リセット
	CMD_CUN3R	= 0x0022,		// COUNTER3(汎用・偏差)リセット
	CMD_ERCOUT	= 0x0024,		// ＥＲＣ信号の出力
	CMD_ERCRST	= 0x0025,		// ＥＲＣ信号のリセット
	CMD_STAON	= 0x0028,		// ＰＣＳ入力代行
	CMD_LTCH	= 0x0029,		// ＬＴＣ入力代行
};

struct StMotionnet {
	// 0x000～0x001
	B	commandStatus0;						// コマンド＆ステータス（ビット7～0）
	B	commandStatus1;						// コマンド＆ステータス（ビット15～8）
	
	// 0x002～0x003
	B	intrStatus0;						// 割り込みステータス（ビット7～0）
	B	intrStatus1;						// 割り込みステータス（ビット15～8）
	
	// 0x004～0x005
	B	inputOutputBuff0;					// 入出力バッファ（ビット7～0）
	B	inputOutputBuff1;					// 入出力バッファ（ビット15～8）
	
	// 0x006～0x007
	B	fifo0;								// データ送受信用FIFO（ビット7～0）
	B	fifo1;								// データ送受信用FIFO（ビット15～8）
	
	// 0x008～0x077
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
	B	rsv08[100];							// 未定義
	
	// 0x078～0x0B7
	B	deviceInformation[MOTIONNET_DEVICE_MAX];
												// デバイス情報
	// 0x0B8～0x0BF
	B	cyclicCommErrorFlg[MOTIONNET_DEVICE_MAX / 8];
												// サイクリック通信エラーフラグ
	// 0x0C0～0x0DF
	B	inputChangeIntrSettting[MOTIONNET_DEVICE_MAX / 2];
												// 入力変化割り込み設定
	// 0x0E0～0x0FF
	B	inputChangeIntrFlg[MOTIONNET_DEVICE_MAX / 2];
												// 入力変化割り込みフラグ
	// 0x100～0x1FF
	B	portData[MOTIONNET_DEVICE_MAX][MOTIONNET_PORT_MAX];
												// ポートデータ
};

///<summary>
/// RSTSレジスタ：拡張ステータスを確認できます。（読み出し専用）
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve1: 4;	// （常に０になります）
			INT	SPH4	: 1;	// 励磁信号出力ＰＨ４がＯＮ状態の時に１になります。
			INT	SPH3	: 1;	// 励磁信号出力ＰＨ３がＯＮ状態の時に１になります。
			INT	SPH2	: 1;	// 励磁信号出力ＰＨ２がＯＮ状態の時に１になります。
			INT	SPH1	: 1;	// 励磁信号出力ＰＨ１がＯＮ状態の時に１になります。
			INT	SPLS	: 1;	// パルス出力(±)がＯＮ状態の時に１になります。
			INT	SCP3	: 1;	// ＣＭＰ３比較条件成立時に１になります。
			INT	SCP2	: 1;	// ＣＭＰ２比較条件成立時に１になります。
			INT	SCP1	: 1;	// ＣＭＰ１比較条件成立時に１になります。
			INT	SINP	: 1;	// ＩＮＰ入力信号がＯＮ状態の時に１になります。
			INT	SLTC	: 1;	// ＬＴＣ入力信号がＯＮ状態の時に１になります。
			INT	SCLR	: 1;	// ＣＬＲ入力信号がＯＮ状態の時に１になります。
			INT	SEZ		: 1;	// ＥＺ入力信号がＯＮ状態の時に１になります。
			INT	SERC	: 1;	// ＥＲＣ出力信号がＯＮ状態の時に１になります。
			INT	SPCS	: 1;	// ＰＣＳ入力信号がＯＮ状態の時に１になります。
			INT	SEMG	: 1;	// ＃ＥＭＧ入力信号がＯＮ状態の時に１になります。
			INT	SSTP	: 1;	// ＃ＳＴＰ入力信号がＯＮ状態の時に１になります。
			INT	SSTA	: 1;	// ＃ＳＴＡ入力信号がＯＮ状態の時に１になります。
			INT	SDIN	: 1;	// ＳＤ入力信号がＯＮ状態の時に１になります。（ＳＤ入力端子の状態）
			INT	SSD		: 1;	// ＳＤ入力信号がＯＮ状態の時に１になります。（ＳＤラッチの状態）
			INT	SORG	: 1;	// ＯＲＧ入力信号がＯＮ状態の時に１になります。
			INT	SMEL	: 1;	// －ＥＬ入力信号がＯＮ状態の時に１になります。
			INT	SPEL	: 1;	// ＋ＥＬ入力信号がＯＮ状態の時に１になります。
			INT	SALM	: 1;	// ＡＬＭ入力信号がＯＮ状態の時に１になります。
			INT	SDIR	: 1;	// 動作方向（０：＋方向 １：－方向）
			INT	CND		: 4;	// 動作状態を表します。
		}BIT;
	}UNID;
} RSTS;

///<summary> 
/// RSTSレジスタ  PCD4611：拡張ステータスを確認できます。（読み出し専用）
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
/// RENV1レジスタ
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
			INT	EROE      : 1;	// b10  FU / FU-A  EA/EB 入力仕様を設定(00:1逓倍,01:2逓倍,10:4逓倍)
			INT	ALML      : 1;	// b9   FU / FU-A   〃
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
/// RENV1レジスタ  PCD4611
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
/// RENV2レジスタ
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
			INT	EROE      : 1;	// b10 EA/EB 入力仕様を設定(00:1逓倍,01:2逓倍,10:4逓倍)
			INT	ALML      : 1;	// b9  〃
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
/// RENV2レジスタ
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve31 : 1;	// b31（0固定）
			INT	reserve30 : 1;	// b30（0固定）
			INT	reserve29 : 1;	// b29（0固定）
			INT	IMSK      : 1;	// b28（G9003B:0固定） 
			INT	EXER      : 1;	// b27（G9003B:0固定） 
			INT	GN2	      : 1;	// b26（G9003B:0固定） 
			INT	GN1	      : 1;	// b25（G9003B:0固定） 
			INT	GN0	      : 1;	// b24（G9003B:0固定） 
			INT	SIFM      : 1;	// b23（G9003B:0固定）
			INT	IDL2      : 1;	// b22 
			INT	IDL1      : 1;	// b21 
			INT	IDL0      : 1;	// b20 
			INT	ROMB      : 1;	// b19（G9003B:0固定）
			INT	P0FF      : 1;	// b18 
			INT	E0FF      : 1;	// b17 
			INT	PDIR      : 1;	// b16 
			INT	PIM1      : 1;	// b15 
			INT	PIM0      : 1;	// b14 
			INT	PINF      : 1;	// b13 
			INT	EZL	      : 1;	// b12 
			INT	EDIR      : 1;	// b11 
			INT	EIM1      : 1;	// b10 EA/EB 入力仕様を設定(00:1逓倍,01:2逓倍,10:4逓倍)
			INT	EIM0      : 1;	// b9  〃
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
/// RENV2レジスタ  PCD4611
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
/// RENV3レジスタ
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{                       //           ※参考.RENV2
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
/// RENV3レジスタ
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve6: 1;	//（常に０になります）
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
			INT	CI32	: 1;	// b12 ＣＯＵＮＴＥＲ３（汎用・偏差）のカウント入力を設定
			INT	CI31	: 1;	// b11 〃
			INT	CI30	: 1;	// b10 〃
			INT	CI21	: 1;	// b9  ＣＯＵＮＴＥＲ２（機械位置）のカウント入力を設定
			INT	CI20	: 1;	// b8  〃
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
/// RENV3レジスタ  PCD4611
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
/// RENV4レジスタ
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
/// RENV4レジスタ
///</summary>
typedef struct
{
	union
	{
		INT	data;

		struct
		{
			INT	reserve3: 4;	//（常に０になります）
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
/// コマンド構造体
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
/// (M204DUM)RENV5レジスタ
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
/// (M204DUM)RENV6レジスタ
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
/// (M204DUM)RENV7レジスタ
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
/// コマンド構造体
///</summary>
typedef struct
{
	UH		code;			// コマンドコード
//1004	UH		combi_pm;		// 連携PM
	UH		status;			// モータステータス
//1004	BOOL	slave;

} ST_COMMAND;

// <summary>
// モーターステータス構造体
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	reserve2: 7;	// 常に0になります。
			UH	SBSY	: 1;	// パルス出力開始で１になります。動作停止で０になります。（＝ＢＳＹ）
			UH	reserve1: 4;	// 常に0になります。
			UH	SEVT	: 1;	// イベント割り込み発生により１になり、ＲＩＳＴ読み出しで０になります。
			UH	SERR	: 1;	// エラー割り込み発生により１になり、ＲＥＳＴ読み出しで０になります。
			UH	SEND	: 1;	// 動作停止により割り込みを発生し、１になります。割り込みリセットコマンド(0008h)で０になります。
			UH	SINT	: 1;	// 割り込み発生により１になります。(ビット１,２,３のいずれかが１)
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
// （４軸）サブステータス構造体
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	SSD : 1;	// BIT15 :SSD SD入力ON時に1になります。(SD入力のラッチ信号)
			UH	SORG: 1;	// BIT14 :SORG ORG入力ON時に1になります。
			UH	SMEL: 1;	// BIT13 :SMEL -EL入力ON時に1になります。
			UH	SPEL: 1;	// BIT12 :SPEL +EL入力ON時に1になります。
			UH	SALM: 1;	// BIT11 :SALM ALM入力ON時に1になります。
			UH	SFC : 1;	// BIT10 :SFC 定速動作中に1になります。
			UH	SFD : 1;	// BIT9  :SFD 減速中に1になります。
			UH	SFU : 1;	// BIT8  :SFU 加速中に1になります。
			UH	IOP : 8;	// BIT7-0:IOP7～0 P7～P0端子の状態を読み出します。(0:Lレベル、1:Hレベル)
		}BIT;
	}UNID;
} MNETSubStatus;

///<summary>
/// モータパラメータ構造体
///</summary>
typedef struct 
{
	INT			factor;		// モータ動作倍率
	UH	accel_type;			// 加減速特性
	INT			FL;			// 高速動作速度(pps)
	INT			FH;			// 定速動作速度(pps)
	INT			utime;		// 加速時間(ms)
	INT			dtime;		// 減速時間(ms)
	INT			us;			// 加速時Ｓ字範囲
	INT			ds;			// 減速時Ｓ字範囲
} ST_MOTOR_PARAM;


///<summary>
/// モータ情報構造体
///</summary>
typedef struct
{
	INT			factor;		// モータ動作倍率
	ST_MOTOR_PARAM	param;	// モータパラメータ
	INT			reg[10];		// モータコントローラレジスタ
	UH			dir;		// モータ移動方向
//1004	BOOL		stopCW;		// CW方向動作禁止フラグ
//1004	BOOL		stopCCW;	// CCW方向動作禁止フラグ
	INT			pos;		// モータ絶対位置
	UH			power;		// 励磁設定
	BOOL		paused;		// 一時停止フラグ
//1004	BOOL		active;		// 
//1004	BOOL		enableDelay;// 遅延有効フラグ
//1004	INT			delay;		// 遅延時間
//1004	INT			dcounter;	// 遅延カウンタ
//1004	BOOL		interrupt;
//1004	BOOL		notified;
//1004	BOOL		not_notify;
	ST_COMMAND		command;	// コマンド
	UH	startParam;	// 動作開始コマンドパラメータ

} ST_PM_INFO;


// <summary>
// センターデバイスステータス構造体
// </summary>
// <remarks></remarks>
typedef struct
{
	union
	{
		UH	data;
		struct
		{
			UH	BBSY	: 1;	// RENV0(8)= "1" でブレーク通信コマンド（0610h）を発行したとき、
									// ブレーク通信が完了するまで "1" となります。
			UH	DBSY	: 1;	// システム通信中、またはデータ通信中に1 になります。
			UH	RBSY	: 1;	// リセット処理中に1 になります。
			UH	SBSY	: 1;	// サイクリック通信スタート中に1 になります。
			UH	reserve2: 1;	// 常に0 になります。
			UH	RDBB	: 1;	// データ受信用FIFO に受信データがある時に1 になります。
			UH	TDBB	: 1;	// データ送信用FIFO に送信データがある時に1 になります。
			UH	REF		: 1;	// 未送信の出力ポートデータがある時1 になります。
			UH	reserve1: 1;	// 常に0 になります。
			UH	CAER	: 1;	// CPU のアクセスエラーです。
			UH	ERAE	: 1;	// ﾛｰｶﾙ側受信処理ｴﾗｰ発生時に1 になり、割り込み信号(INT)を出力します。
			UH	EDTE	: 1;	// データ通信エラー発生時に1 になり、割り込み信号(INT)を出力します。
			UH	EIOE	: 1;	// サイクリック通信ｴﾗｰ発生時に1 になり、割り込み信号(INT)を出力します。
			UH	IOPC	: 1;	// 「入力変化割り込み設定」を1 にセットした入力ポートの状態が変化した時に1 になり、
									// 割り込み信号(INT)を出力します。
			UH	BRKF	: 1;	// ブレークフレーム受信時に1 になり、割り込み信号(INT)を出力します。
			UH	CEND	: 1;	// データ送信用FIFO 書き込み可能時に1 になります。
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
			UH	SPRF	: 1;	// 動作用プリレジスタが満杯 = 1
			UH	SEOR	: 1;	// 目標位置オーバライドが実行出来ない = 1
			UH	reserve3: 1;	// 
			UH	reserve2: 1;	// 
			UH	reserve1: 1;	// 
			UH	SCP2	: 1;	// 
			UH	SCP1	: 1;	// 
			UH	SSC1	: 1;	// 
			UH	SSC0	: 1;	// 
			UH	SINT	: 1;	// イベント割込み発生 = 1、RIST読込で 0となる
			UH	SERR	: 1;	// エラー割込み発生 = 1、REST読込で 0となる
			UH	SEND	: 1;	// スタートコマンド書き込み時 = 0、動作停止で1となる
			UH	SENI	: 1;	// 
			UH	SRUN	: 1;	// パルス出力開始時 = 1、動作停止で0となる
			UH	SSCM	: 1;	// スタートコマンド書き込み時 = 1、動作停止で0となる
		}BIT;
	}UNID;
} MNETStatusPCD4611;

///<summary>
/// モータレジスタ構造体
///</summary>
typedef struct
{
	INT			reg[10];
} PM_REG;

///<summary>
/// RMDレジスタ：動作モードを設定するレジスタです。
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
/// RMDレジスタ：動作モードを設定するレジスタです。
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
/// モータステータス構造体
///</summary>
typedef struct
{
	union
	{
		INT	data;
		struct
		{
			INT	reserve	: 4;	// （常に０に設定して下さい。）
			INT	MPIE	: 1;	// 
			INT	MADJ	: 1;	// ＦＨ補正機能の設定。(０：ＯＮ １：ＯＦＦ）
			INT	MSPO	: 1;	// １：異常停止時に＃ＳＴＰ（同時停止）信号を出力します。
			INT	MSPE	: 1;	// １：＃ＳＴＰ入力により減速停止または即停止させます。ほかの軸の異常停止による＃ＳＴＰ発生時、
			INT	MAX		: 4;	// 
			INT	MSY		: 2;	// 
			INT	MSN		: 2;	// 
			INT	MIPF	: 1;	// 
			INT	MPCS	: 1;	// ＰＣＳ入力の有効／無効設定。(１：位置決め動作時に、ＰＣＳ入力ＯＮからパルス数管理を行います。)［目標位置のオーバーライド２］
			INT	MSDP	: 1;	// 高速動作時のスローダウンポイントを設定。位置決め動作時に有効。０：自動設定 １：マニュアル設定

			INT	METM	: 1;	// 動作完了タイミングを設定。(０：周期完了 １：パルス完了)
			INT	MCCE	: 1;	// 
			INT	MSMD	: 1;	// 高速動作時の加減速特性を設定。(０：直線加減速 １：Ｓ時加減速)
			INT	MINP	: 1;	// ０：ＩＮＰ入力による遅延は無効になります。(RSTS での確認は可) 
									//１：ＩＮＰ入力ＯＮで動作完了になります。
			INT	MSDE	: 1;	// ０：ＳＤ入力は無効になります。（サブステータス、RSTS での確認は可）
									// １：ＳＤ入力ＯＮにより減速（減速停止）します。
			INT	MENI	: 1;	// 
			INT	MOD		: 7;	// 動作モードを設定します。
		}BIT;
	}UNID;
} RMDDUM;

///<summary>
/// モータステータス構造体
///</summary>
typedef struct
{
	INT			pos;				// パルス出力値
	INT			enc;				// エンコーダ値
	UINT		dev;				// パルス出力値とエンコーダ値の偏差
	BOOL		busy;
	BOOL		cw_limit;
	BOOL		ccw_limit;

} ST_MOTOR_STATUS;

	///<summary>
	/// 加減速特性
	///</summary>
	enum ACCEL_TYPE 
	{
		ACCEL_LINEAR = 0,			// 直線加減速
		ACCEL_STYPE  = 1,			// S字加減速
	};

	///<summary>
	/// パルスモータNo
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
	/// レジスタ
	///</summary>
	typedef enum							
	{
		R0 = 0,		// プリセット量設定／残パルス確認	R/W  0～0xFFFFFF
		R1 = 1,		// FL速度設定						W(R) 1～0x1FFFF
		R2 = 2,		// FH速度設定						W(R) 1～0x1FFFF
		R3 = 3,		// 加速度レート設定					W(R) 2～0x3FF
		R4 = 4,		// 倍率設定							W(R) 2～0x3FF
		R5 = 5,		// スローダウンポイント設定			W(R) 0～0xFFFF
		R6 = 6,		// アイドリングパルス設定			W(R) 0～7
		R7 = 7,		// 環境データ設定(PCD4541のみ)		W(R) 0, 1
		R8 = 8,		// 加速Ｓ字範囲設定					W(R) 1～0xC350
		R9 = 9,		// 減速Ｓ字範囲設定					W(R) 1～0xC350

	} REGISTER;


	///<summary>
	/// 書き込み用レジスタ
	///</summary>
	enum WRITE_REGISTER
	{
		WRG_RMVOR	= 0x0080,	// RMV レジスタ へのオーバーライド書き込み
		WRG_WPRCP5  = 0x008B,	// RCMP5レジスタ へのオーバーライド書き込み
		WRG_RMV		= 0x0090,	// RMV レジスタ への書き込み
		WRG_RFL		= 0x0091,	// RFL レジスタ への書き込み
		WRG_RFH		= 0x0092,	// RFH レジスタ への書き込み
		WRG_RDP		= 0x0096,	// RDP レジスタ への書き込み
		WRG_RMD		= 0x0097,	// RMD レジスタ への書き込み
		WRG_RFA		= 0x009B,	// RFA レジスタ への書き込み
		WRG_RENV1	= 0x009C,	// RENV1 レジスタ への書き込み
		WRG_RENV2	= 0x009D,	// RENV2 レジスタ への書き込み
		WRG_RENV3	= 0x009E,	// RENV3 レジスタ への書き込み
		WRG_RENV4	= 0x009F,	// RENV4 レジスタ への書き込み
		WRG_RENV5	= 0x00A0,	// RENV5 レジスタ への書き込み
		WRG_RENV6	= 0x00A1,	// RENV6 レジスタ への書き込み
		WRG_RENV7	= 0x00A2,	// RENV7 レジスタ への書き込み
		WRG_RCUN1	= 0x00A3,	// RCUN1 レジスタ への書き込み
		WRG_RCUN2	= 0x00A4,	// RCUN2 レジスタ への書き込み
		WRG_RCMP1	= 0x00A7,	// RCMP1 レジスタ への書き込み
		WRG_RCMP2	= 0x00A8,	// RCMP2 レジスタ への書き込み
		WRG_RCMP3	= 0x00A9,	// RCMP3 レジスタ への書き込み
		WRG_RCMP5	= 0x00AB,	// RCMP5 レジスタ への書き込み
		WRG_RIRQ	= 0x00AC,	// RIRQ レジスタ への書き込み
	};

	///<summary>
	/// 書き込み用レジスタ
	///</summary>
	enum WRITE_REGISTER16
	{
		WRG_RUR		= 0x0093,	// RUR レジスタ への書き込み
		WRG_RDR		= 0x0094,	// RDR レジスタ への書き込み
		WRG_RMG		= 0x0095,	// RMG レジスタ への書き込み
		WRG_RUS		= 0x0099,	// RUS レジスタ への書き込み
		WRG_RDS		= 0x009A,	// RDS レジスタ への書き込み
		WRG_RCUN3	= 0x00A5,	// RCUN3 レジスタ への書き込み
	};

	///<summary>
	/// 書き込み用レジスタ
	///</summary>
	enum WRITE_PREREGISTER
	{
		WRG_PRMV	= 0x0080,	// RMV レジスタ への書き込み
		WRG_PRFL	= 0x0081,	// RFL レジスタ への書き込み
		WRG_PRFH	= 0x0082,	// RFH レジスタ への書き込み
		WRG_PRUR	= 0x0083,	// RUS レジスタ への書き込み
		WRG_PRDR	= 0x0084,	// RDR レジスタ への書き込み
		WRG_PRMG	= 0x0085,	// RMG レジスタ への書き込み
		WRG_PRDP	= 0x0086,	// RDP レジスタ への書き込み
		WRG_PRMD	= 0x0087,	// RMD レジスタ への書き込み
		WRG_PRIP	= 0x008B,	// RIP レジスタ への書き込み
		WRG_PRUS	= 0x0089,	// RUS レジスタ への書き込み
		WRG_PRDS	= 0x008A,	// RDS レジスタ への書き込み
	};

	///<summary>
	/// 読み込み用レジスタ
	///</summary>
	enum READ_REGISTER
	{
		RRG_RMV		= 0x00D0,	//  RMV レジスタ の読み出し
		RRG_RFL		= 0x00D1,	//  RFL レジスタ の読み出し
		RRG_RFH		= 0x00D2,	//  RFH レジスタ の読み出し
		RRG_RUR		= 0x00D3,	//  RUR レジスタ の読み出し
		RRG_RDR		= 0x00D4,	//  RDR レジスタ の読み出し
		RRG_RMG		= 0x00D5,	//  RMG レジスタ の読み出し
		RRG_RDP		= 0x00D6,	//  RDP レジスタ の読み出し
		RRG_RMD		= 0x00D7,	//  RMD レジスタ の読み出し
		RRG_RUS		= 0x00D9,	//  RUS レジスタ の読み出し
		RRG_RDS		= 0x00DA,	//  RDS レジスタ の読み出し
		RRG_RFA		= 0x00DB,	//  RFA レジスタ の読み出し
		RRG_RENV1	= 0x00DC,	//  RENV1 レジスタ の読み出し
		RRG_RENV2	= 0x00DD,	//  RENV2 レジスタ の読み出し
		RRG_RENV3	= 0x00DE,	//  RENV3 レジスタ の読み出し
		RRG_RENV4	= 0x00DF,	//  RENV4 レジスタ の読み出し
		RRG_RENV5	= 0x00E0,	//  RENV5 レジスタ の読み出し
		RRG_RENV6	= 0x00E1,	//  RENV6 レジスタ の読み出し
		RRG_RCUN1	= 0x00E3,	//  RCUN1 レジスタ の読み出し
		RRG_RCUN2	= 0x00E4,	//  RCUN2 レジスタ の読み出し
		RRG_RCUN3	= 0x00E5,	//  RCUN3 レジスタ の読み出し
		RRG_RCMP1	= 0x00E7,	//  RCMP1 レジスタ の読み出し
		RRG_RCMP2	= 0x00E8,	//  RCMP2 レジスタ の読み出し
		RRG_RCMP3	= 0x00E9,	//  RCMP3 レジスタ の読み出し
		RRG_RIRQ	= 0x00EC,	//  RIRQ レジスタ の読み出し
		RRG_RLTC1	= 0x00ED,	//  RLTC1 レジスタ の読み出し
		RRG_RLTC2	= 0x00EE,	//  RLTC2 レジスタ の読み出し
		RRG_RLTC3	= 0x00EF,	//  RDTC3 レジスタ の読み出し
		RRG_RSTS	= 0x00F1,	//  RSTS レジスタ の読み出し
		RRG_REST	= 0x00F2,	//  REST レジスタ の読み出し
		RRG_RIST	= 0x00F3,	//  RIST レジスタ の読み出し
		RRG_RPLS	= 0x00F4,	//  RPLS レジスタ の読み出し
		RRG_RSPD	= 0x00F5,	//  RSPD レジスタ の読み出し
		RRG_RSDC	= 0x00F6,	//  RSDC レジスタ の読み出し
	};
	enum READ_PREREGISTER
	{
		RRG_PRMV	= 0x00C0,	//  PRMV レジスタ の読み出し
		RRG_PRFL	= 0x00C1,	//  PRFL レジスタ の読み出し
		RRG_PRFH	= 0x00C2,	//  PRFH レジスタ の読み出し
		RRG_PRUR	= 0x00C3,	//  PRUR レジスタ の読み出し
		RRG_PRDR	= 0x00C4,	//  PRDR レジスタ の読み出し
		RRG_PRMG	= 0x00C5,	//  PRMG レジスタ の読み出し
		RRG_PRDP	= 0x00C6,	//  PRDP レジスタ の読み出し
		RRG_PRMD	= 0x00C7,	//  PRMD レジスタ の読み出し
		RRG_PRUS	= 0x00C9,	//  PRUS レジスタ の読み出し
		RRG_PRDS	= 0x00CA,	//  PRDS レジスタ の読み出し
	};

	// PCD ---
	enum WRITE_REGISTER_PCD
	{
		WPCD_RMVOR	= 0x80,	// RMV レジスタ へのオーバーライド書き込み
		WPCD_WPRCP5 = 0x8B,	// RCMP5レジスタ へのオーバーライド書き込み
		WPCD_RMV	= 0x90,	// RMV レジスタ への書き込み
		WPCD_RFL	= 0x91,	// RFL レジスタ への書き込み
		WPCD_RFH	= 0x92,	// RFH レジスタ への書き込み
		WPCD_RUR	= 0x93,	// RUR レジスタ への書き込み
		WPCD_RDR	= 0x94,	// RDR レジスタ への書き込み
		WPCD_RMG	= 0x95,	// RMG レジスタ への書き込み
		WPCD_RUS	= 0x99,	// RUS レジスタ への書き込み
		WPCD_RDS	= 0x9A,	// RDS レジスタ への書き込み
		WPCD_RFA	= 0x9B,	// RFA レジスタ への書き込み
		WPCD_RENV1	= 0x9C,	// RENV1 レジスタ への書き込み
		WPCD_RENV2	= 0x9D,	// RENV2 レジスタ への書き込み
		WPCD_RENV3	= 0x9E,	// RENV3 レジスタ への書き込み
		WPCD_RENV4	= 0x9F,	// RENV4 レジスタ への書き込み
		WPCD_RENV5	= 0xA0,	// RENV5 レジスタ への書き込み
		WPCD_RENV6	= 0xA1,	// RENV6 レジスタ への書き込み
		WPCD_RENV7	= 0xA2,	// RENV7 レジスタ への書き込み
		WPCD_RCUN1	= 0xA3,	// RCUN1 レジスタ への書き込み
		WPCD_RCUN2	= 0xA4,	// RCUN2 レジスタ への書き込み
		WPCD_RCUN3	= 0xA5,	// RCUN3 レジスタ への書き込み
		WPCD_RCMP1	= 0xA7,	// RCMP1 レジスタ への書き込み
		WPCD_RCMP2	= 0xA8,	// RCMP2 レジスタ への書き込み
		WPCD_RCMP3	= 0xA9,	// RCMP3 レジスタ への書き込み
		WPCD_RCMP5	= 0xAB,	// RCMP5 レジスタ への書き込み
		WPCD_RIRQ	= 0xAC,	// RIRQ レジスタ への書き込み
	};

	///<summary>
	/// 書き込み用プリレジスタ
	///</summary>
	enum WRITE_PREREGISTER_PCD
	{
		WPCD_PRMV	= 0x80,	// RMV レジスタ への書き込み
		WPCD_PRFL	= 0x81,	// RFL レジスタ への書き込み
		WPCD_PRFH	= 0x82,	// RFH レジスタ への書き込み
		WPCD_PRUR	= 0x83,	// RUS レジスタ への書き込み
		WPCD_PRDR	= 0x84,	// RDR レジスタ への書き込み
		WPCD_PRMG	= 0x85,	// RMG レジスタ への書き込み
		WPCD_PRDP	= 0x86,	// RDP レジスタ への書き込み
		WPCD_PRMD	= 0x87,	// RMD レジスタ への書き込み
		WPCD_PRIP	= 0x8B,	// RIP レジスタ への書き込み
		WPCD_PRUS	= 0x89,	// RUS レジスタ への書き込み
		WPCD_PRDS	= 0x8A,	// RDS レジスタ への書き込み
	};

	///<summary>
	/// 読み込み用レジスタ
	///</summary>
	enum READ_REGISTER_PCD
	{
		RPCD_RMV	= 0xD0,	//  RMV レジスタ の読み出し
		RPCD_RFL	= 0xD1,	//  RFL レジスタ の読み出し
		RPCD_RFH	= 0xD2,	//  RFH レジスタ の読み出し
		RPCD_RUR	= 0xD3,	//  RUR レジスタ の読み出し
		RPCD_RDR	= 0xD4,	//  RDR レジスタ の読み出し
		RPCD_RMG	= 0xD5,	//  RMG レジスタ の読み出し
		RPCD_RDP	= 0xD6,	//  RDP レジスタ の読み出し
		RPCD_RMD	= 0xD7,	//  RMD レジスタ の読み出し
		RPCD_RUS	= 0xD9,	//  RUS レジスタ の読み出し
		RPCD_RDS	= 0xDA,	//  RDS レジスタ の読み出し
		RPCD_RFA	= 0xDB,	//  RFA レジスタ の読み出し
		RPCD_RENV1	= 0xDC,	//  RENV1 レジスタ の読み出し
		RPCD_RENV2	= 0xDD,	//  RENV2 レジスタ の読み出し
		RPCD_RENV3	= 0xDE,	//  RENV3 レジスタ の読み出し
		RPCD_RENV4	= 0xDF,	//  RENV4 レジスタ の読み出し
		RPCD_RENV5	= 0xE0,	//  RENV5 レジスタ の読み出し
		RPCD_RENV6	= 0xE1,	//  RENV6 レジスタ の読み出し
		RPCD_RCUN1	= 0xE3,	//  RCUN1 レジスタ の読み出し
		RPCD_RCUN2	= 0xE4,	//  RCUN2 レジスタ の読み出し
		RPCD_RCUN3	= 0xE5,	//  RCUN3 レジスタ の読み出し
		RPCD_RCMP1	= 0xE7,	//  RCMP1 レジスタ の読み出し
		RPCD_RCMP2	= 0xE8,	//  RCMP2 レジスタ の読み出し
		RPCD_RCMP3	= 0xE9,	//  RCMP3 レジスタ の読み出し
		RPCD_RIRQ	= 0xEC,	//  RIRQ レジスタ の読み出し
		RPCD_RLTC1	= 0xED,	//  RLTC1 レジスタ の読み出し
		RPCD_RLTC2	= 0xEE,	//  RLTC2 レジスタ の読み出し
		RPCD_RLTC3	= 0xEF,	//  RDTC3 レジスタ の読み出し
		RPCD_RSTS	= 0xF1,	//  RSTS レジスタ の読み出し
		RPCD_REST	= 0xF2,	//  REST レジスタ の読み出し
		RPCD_RIST	= 0xF3,	//  RIST レジスタ の読み出し
		RPCD_RPLS	= 0xF4,	//  RPLS レジスタ の読み出し
		RPCD_RSPD	= 0xF5,	//  RSPD レジスタ の読み出し
		RPCD_RSDC	= 0xF6,	//  RSDC レジスタ の読み出し
	};
	enum READ_PREREGISTER_PCD
	{
		RPCD_PRMV	= 0xC0,	//  PRMV レジスタ の読み出し
		RPCD_PRFL	= 0xC1,	//  PRFL レジスタ の読み出し
		RPCD_PRFH	= 0xC2,	//  PRFH レジスタ の読み出し
		RPCD_PRUR	= 0xC3,	//  PRUR レジスタ の読み出し
		RPCD_PRDR	= 0xC4,	//  PRDR レジスタ の読み出し
		RPCD_PRMG	= 0xC5,	//  PRMG レジスタ の読み出し
		RPCD_PRDP	= 0xC6,	//  PRDP レジスタ の読み出し
		RPCD_PRMD	= 0xC7,	//  PRMD レジスタ の読み出し
		RPCD_PRUS	= 0xC9,	//  PRUS レジスタ の読み出し
		RPCD_PRDS	= 0xCA,	//  PRDS レジスタ の読み出し
	};
	// PCD ---
	
	
	///<summary>
	/// 動作モード
	///</summary>
	enum MODE
	{
		MOD00	= 0x00,	// コマンド制御による (＋)方向連続動作
		MOD08	= 0x08,	// コマンド制御による (－)方向連続動作
		MOD01	= 0x01,	// パルサ(PA/PB)入力による連続動作
		MOD10	= 0x10,	// (＋)方向 原点復帰動作                       o
		MOD18	= 0x18,	// (－)方向 原点復帰動作                       o
		MOD12	= 0x12,	// (＋)方向 原点抜け出し動作                   o
		MOD1A	= 0x1A,	// (－)方向 原点抜け出し動作                   o
		MOD15	= 0x15,	// (＋)方向 原点サーチ動作
		MOD1D	= 0x1D,	// (－)方向 原点サーチ動作
		MOD20	= 0x20,	// ＋ＥＬまたは＋ＳＬ位置まで動作
		MOD28	= 0x28,	// －ＥＬまたは－ＳＬ位置まで動作
		MOD22	= 0x22,	// －ＥＬまたは－ＳＬ抜け出し動作
		MOD2A	= 0x2A,	// ＋ＥＬまたは＋ＳＬ抜け出し動作
		MOD24	= 0x24,	// (＋)方向にＥＺカウント分だけ動作
		MOD2C	= 0x2C,	// (－)方向にＥＺカウント分だけ動作
		MOD41	= 0x41,	// 位置決め動作(目標相対位置指定)              o
		MOD42	= 0x42,	// 位置決め動作 (COUNTER1 絶対位置指定)        o
		MOD43	= 0x43,	// 位置決め動作 (COUNTER2 絶対位置指定)
		MOD44	= 0x44,	// 指令位置(COUNTER1)０点復帰動作
		MOD45	= 0x45,	// 機械位置(COUNTER2)０点復帰動作
		MOD46	= 0x46,	// (＋)方向 １パルス動作
		MOD4E	= 0x4E,	// (－)方向 １パルス動作
		MOD47	= 0x47,	// タイマ動作
		MOD51	= 0x51,	// パルサ(PA/PB)入力による位置決め動作
		MOD52	= 0x52,	// PA/PB 同期の 位置決め動作 (COUNTER1 絶対位置指定)
		MOD53	= 0x53,	// PA/PB 同期の 位置決め動作 (COUNTER2 絶対位置指定)
		MOD54	= 0x54,	// パルサ(PA/PB)入力による指令位置０点復帰動作
		MOD55	= 0x55,	// パルサ(PA/PB)入力による機械位置０点復帰動作
	};

	// <summary>
    // アドレスマップ定数
    // </summary>
    // <remarks></remarks>
    enum AddressMap
	{
        MNET_COMMAND					= 0,		// コマンド
        MNET_STATUS						= 0,		// ステータス
        MNET_INTERRUPT_STATUS			= 0x2,		// 割込みステータス
        MNET_IO_BUFFER					= 0x4,		// 入出力バッファ
        MNET_FIFO						= 0x6,		// FIFO
        MNET_DEVICE_INFO_BASE			= 0x78,		// デバイス情報開始アドレス
        MNET_ERROR_FLAG_BASE			= 0xB8,		// 通信エラーフラグ開始アドレス
        MNET_INTERRUPT_SETTING_BASE		= 0xC0,		// 割込み設定開始アドレス
        MNET_INTERRUPT_FLAG_BASE		= 0xE0,		// 割込みフラグ開始アドレス
        MNET_PORT_DATA_BASE				= 0x100,	// ポートデータ開始アドレス
	};

	// <summary>
    // 通信速度定数
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
    // センターコマンド
    // </summary>
    // <remarks></remarks>
	enum CenterCommand
	{
		MNET_CENTER_RESET				= 0x100,    // リセット
		MNET_CMD_CLEAR_SEND_FIFO		= 0x200,    // 送信用FIFOリセット
		MNET_CMD_CLEAR_RECV_FIFO		= 0x300,    // 受信用FIFOリセット
		MNET_CENTER_CLR_STS				= 0x400,    // ステータスクリア(ステータスリードによるビットクリア無効設定時(RENV0-Bit9:1)に有効,
		MNET_CENTER_CLR_ERCNT			= 0x600,	// エラーカウンタクリア
		MNET_CENTER_BRKCOM				= 0x610,    // ブレーク通信発生(ブレイク機能OFF時(RENV0-Bit8:1)に有効)
		MNET_CENTER_SYSCOM				= 0x1000,	// システム通信
		MNET_CMD_GET_ATTRIBUTE			= 0x1300,   // デバイスの属性情報の取得
		MNET_CENTER_STA_IOCOM			= 0x3000,   // I/O通信スタート
		MNET_CENTER_STP_IOCOM			= 0x3100,   // I/O通信ストップ
		MNET_DATA_COMMUNICATION_BASE	= 0x4000,	// データ通信
		MNET_DATA_COMMUNICATION_CANCEL	= 0x4100,	// データ通信キャンセル

		// 入出力バッファ使用コマンド
		MNET_CMD_DEVICE_INFO_BASE		= 0x5000,	// デバイス情報書き込み
	};

	// <summary>
    // メインレジスター
    // </summary>
    // <remarks></remarks>
	enum MainRegister
	{
		MNET_CENTER_WRENV0				= 0x5500,   // RENV0書き込み
		MNET_CENTER_RRENV0				= 0x6500,   // RENV0読み出し
		MNET_CENTER_RRERCNT				= 0x6501,   // エラーカウンタ読み出し
		MNET_CENTER_RRCYCLE				= 0x6502,   // サイクリック周期読み出し
		MNET_CENTER_RRDADR				= 0x6503,   // 最終データ通信デバイスアドレス読み出し
		MNET_CENTER_RRVER				= 0x6504,   // バージョン読み出し
	};

	// <summary>
    // INT形ステータスのクリアビット(MNET_CENTER_CLR_STSコマンドに加算)
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
    // メインメモリーアクセスタイプ定数
    // </summary>
    // <remarks></remarks>
    enum AccessType
	{
        MNET_LOCAL_PORT0				= 0,		// ポート0
        MNET_LOCAL_PORT1				= 1,		// ポート1
        MNET_LOCAL_PORT2				= 2,		// ポート2
        MNET_LOCAL_PORT3				= 3,		// ポート3
        MNET_LOCAL_ERRFLG				= 4,		// エラーフラグ
        MNET_LOCAL_INTSET				= 5,		// 入力変化割込設定
        MNET_LOCAL_INTFLG				= 6,		// 入力変化割込フラグ
        MNET_LOCAL_DEVINFO				= 7,		// デバイス情報
        MNET_LOCAL_PORTL				= 10,		// 下位ポート(PORT1-0)
        MNET_LOCAL_PORTH				= 11,		// 上位ポート(PORT3-2)
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
	// デバイスタイプ定数
	// </summary>
	// <remarks></remarks>
	enum Type	// DeviceType value
	{
		IO		= 0x01,		// IOデバイス
		DATA	= 0x81,		// データデバイス
	};

	// <summary>
	// デバイス名定数
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
	/// モータ移動方向
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
	/// アドレスマップ
	///</summary>
	enum ADRRESS_MAP
	{                           // [読み込み処理]          / [書き込み処理]
		ADR_RW_CRTCMD = 0x0000, // メインステータスB07-B00 / 制御コマンド書き込み   
		ADR_RW_JIKREQ = 0x0001, // メインステータスB15-B08 / 軸指定                 
		ADR_RW_IOPORT = 0x0002, // 汎用入出力ポート        / 汎用入出力ポート       
		ADR_RW_SUBSTS = 0x0003, // サブステータス          / 無効                   
		ADR_RW_BUFF01 = 0x0004, // バッファ書き込みB07-B00 / バッファ書き込みB07-B00
		ADR_RW_BUFF02 = 0x0005, // バッファ書き込みB15-B08 / バッファ書き込みB15-B08
		ADR_RW_BUFF03 = 0x0006, // バッファ書き込みB23-B16 / バッファ書き込みB23-B16
		ADR_RW_BUFF04 = 0x0007  // バッファ書き込みB31-B24 / バッファ書き込みB31-B24
	};

	///<summary>
	/// 処理内容
	///</summary>
	enum PROCESS_CONT
	{
		ADRFIX = 0x0000,		// アドレス固定
		ADRINC = 0x0080,		// アドレス加算
		ADRDEC = 0x00C0			// アドレス減算
	};

	///<summary>
	/// 処理内容設定
	///</summary>
	enum PROCESS_SET
	{
		NPSET = 0x0000,			// NOP
		WRSET = 0x0900,			// 書き込み処理
		RDSET = 0x0A00,			// 読み込み処理
		DTSET = 0x0300			// データ受信処理
	};
		
//#define E_SUCCESS           0x0000          // 成功
//#define E_SYS_ERROR         0x0001          // システムコールエラー
//#define E_PMSTPHND_ERROR    0x0002          // パルスモータ停止ハンドラでの異常
//#define E_EMC_EMPTY         0x0003          // EMCモード：テストデータなし
//#define E_CMD_UNKOWN        0x0101          // 定義されていないコマンド
//#define E_CMD_PARAM         0x0102          // パラメータ異常
//#define E_PIO_CHANNEL       0x0200          // チャネル番号異常
//#define E_MTR_PARAM         E_CMD_PARAM     // パラメータ異常
//#define E_MTR_INT           0x0301          // INTコマンドによる一時停止
//#define E_MTR_SOUT          0x0302          // 脱調
//#define E_DEV_NORES         0x0411          // コントローラ無応答
//#define E_DEV_RESET         0x0412          // コントローラリセット
//#define E_DAC_BUSY          0x0500          // DACビジー
//#define E_DAC_NO            E_MTR_NO        // モータ番号異常
//#define E_ROM_ADDRESS       E_CMD_PARAM     // アドレス異常
//#define E_ROM_BUSY          E_DAC_BUSY      // BUSY
//#define E_ADC_CHANNEL       E_PIO_CHANNEL
//#define E_ADC_PARAM         E_CMD_PARAM
//#define E_MTR_BUSY          E_DAC_BUSY      // モータBUSY
//#define E_MTR_LIMIT         0x0600          // リミットエラー
//#define E_SCI_OVFW          0x0700          // バッファオーバーフロー
//#define E_SCI_ORN           0x0701          // オーバーランエラー
//#define E_SCI_FER           0x0702          // フレーミングエラー
//#define E_SCI_PER           0x0703          // パリティーエラー

TASK TskMnt( void );
ER MntInit( void );
void IRQ4_ISR();
BOOL IsExist(UB dev_no);

#endif  // _MNTTASK_H_
