/******************************************************************************
* モーションネット管理タスク                                                  *
*                                                                             *
* File name : mnet.c                                                          *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "datastruct.h" 
#include "mnet.h"
#include "sub.h"
#include "automachine.h"
#include "macro.h"


// *****************************************************
//  コンパイルスイッチ
// *****************************************************
#define WAIT_TYPE_IFB	1	// 定義:IFBフラグ監視
#define WAIT_END_CHK	1	// 定義:完了待ちﾙｰﾌﾟ
#define WAIT_TYPE_RW	1	// 定義:RWディレイ

#define DEBUG_INFO				1		// ☆ 1 = DEBUG用, 0 = 製品用


// *****************************************************
//  関数定義
// *****************************************************
ER	MntInit( void );
BOOL pio_Init();
BOOL motor_Init(void);

void pio_ALLIN(UB *data);
UB pio_BIN(UB ch);
void pio_BOUT(UB ch, UB sw);
void pio_PATOUT(UB sw1, UB sw2, UB sw3);
ER motor_POWER(UB pm, UB sw);
ER motor_LIMIT(UB mm, UB dr, UB md, INT sp );
ER motor_SCAN(UB pm, UH dir);
ER motor_SET(UB pm, ST_MOTOR_PARAM mp);
ER motor_PSET(UB pm, INT cnt, INT enc, INT def);
ER motor_ABSMV(UB pm, INT pos, UB status);
ER motor_RELMV(UB pm, INT pos, UB status);
ER motor_STOP(UB pm);
ER motor_ABORT(UB pm);
ER motor_STATUS(UB pm, ST_MOTOR_STATUS *status);

static BOOL WriteMemory(INT addr, UH data);
static BOOL WriteMemoryInt(INT addr, INT data);
static BOOL ReadMemory(INT addr,  UH *data);
static BOOL SetMainCommand(UH command);
static BOOL GetMainStatus(MNETStatus* status);

// PCD4611 ---
static void WriteMemoryPCD4611(UB data);
static void WriteMemoryIntPCD4611(INT data);
static INT  ReadMemoryPCD4611();
// PCD4611 ---

 BOOL SetMotorRegisterInt(UB dev_no, UH reg, INT value);//★
//static BOOL SetMotorRegisterInt(UB dev_no, UH reg, INT value);
static BOOL SetMotorRegister(UB dev_no, UH reg, UH value);
       BOOL GetMotorStatus(UB dev_no, MotorStatus* status);
static BOOL GetMotorSubStatus(UB dev_no, MNETSubStatus* substs);
static BOOL SetMainRegister(UH reg, UH data);
       BOOL IsExist(UB dev_no);
static BOOL SetMotorCommand(UB dev_no, UH command);
       BOOL	GetMotorRegister(UB dev_no, UH reg, INT* value);
static ER   motor_SetAllRegister(UB pm);
static ER   motor_CalcMotorReg(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg);
static ER motor_CalcMotorRegPCD4611(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg);
static void motor_RegistCommand(UB pm, UH cmd);
       void g_Sleep(INT usec);
static void ClearInterrupt(void);
static BOOL IfbIdleWait(void);	// ＩＦＢアイドリング待ち
static BOOL ChkIFB( void );		// ＩＦＢ状態取得
static BOOL EndIdleWait(void);	// 完了待ち
static BOOL ChkEnd( void );		// エンド状態取得
static BOOL motor_InitController(void);
static UB ChkRbsy( void );
static UB ChkDbsy( void );
static UB ChkSbsy( void );



static ER SetRenv1Fu( UB dev_no );				// RENV1 設定( BCD-4020FUA , BCD-4020FU )
static ER SetRenv1PCD4611( UB dev_no );			// 
static ER SetRenv2Fu( UB dev_no, UB teibai );	// RENV2 設定( BCD-4020FUA , BCD-4020FU )
static ER SetRenv2PCD4611( UB dev_no, UB teibai );
static ER SetRenv3Fu( UB dev_no, UB enc );		// RENV3 設定( BCD-4020FUA , BCD-4020FU )
static ER SetRmdFu( UB dev_no, UB mod );		// RMD   設定( BCD-4020FUA , BCD-4020FU )
static ER SetRmv( UB dev_no, INT pos );			// RMV   設定
static ER SetRmvPCD4611( UB dev_no, INT pos );
void	check_main_pulsemotor_initialize( );

#ifdef DEBUG_INFO
static void DebugMnetInfo( UB mm );
#endif
static ER SetRenv3PCD4611( UB dev_no, UB enc );
static ER SetRmdPCD4611( UB dev_no, UB mod );


/***********************************************************************
* 構造体
***********************************************************************/



/***********************************************************************
* テーブル
***********************************************************************/

static const INT MOTORSPD_TBL[10] = {
	1999,
	 999,
	 399,
	 199,
	  99,
	  39,
	  19,
	   9,
	   3,
	   2
};
static const INT MOTORSPD_TBL_PCD4611[10] = {
	3999,// 0.3
	2399,// 0.5
	1199,// 1
	 599,// 2
	 239,// 5
	  59,// 10
	  29,// 20
	  14,// 50
	   5,// 100
	   2 // 200
};

// LIMITコマンド時のモード選択
static const UB LMMOD_TBL[2][3] = {
//    規定外   CW      CCW
	{ MOD20  , MOD20,  MOD28 },	// GENTEN_POS
	{ MOD22  , MOD22,  MOD2A }	// NUKEDA_POS
};

/***********************************************************************
* データ定義
***********************************************************************/
volatile struct	StMotionnet	*pMotionnet;
volatile struct	StPulseMotor	*pPulseMotor;


static	ST_PM_INFO		_pmInfo[20];			// パルスモータ情報配列
static	BOOL 			Initialized = FALSE;	// 
static 	INT	factor;
static 	INT	param;
static 	INT	RMG;
static 	INT	RFL;
static 	INT	RFH;
static 	INT	RUR;
static 	INT	RUS;
static 	INT	RDS;

#define	PULSEMOTOR_CMD_CUN1R	0x20			// カウンタリセット（COUNTER1）
#define	PULSEMOTOR_CMD_CUN2R	0x21			// カウンタリセット（COUNTER2）
#define	PULSEMOTOR_CMD_STOP		0x49			// 即停止
#define	PULSEMOTOR_CMD_STAFL	0x50			// FL低速スタート
#define	PULSEMOTOR_CMD_WPRFL	0x81			// [W] 初速度
#define	PULSEMOTOR_CMD_WPRMG	0x85			// [W] 速度倍率
#define	PULSEMOTOR_CMD_WPRMD	0x87			// [W] 動作モード
#define	PULSEMOTOR_CMD_WRENV1	0x9C			// [W] 環境設定1
#define	PULSEMOTOR_CMD_WRENV2	0x9D			// [W] 環境設定2
#define	PULSEMOTOR_CMD_RRENV1	0xDC			// [R] 環境設定1
#define	PULSEMOTOR_CMD_RCUN1	0xE3			// [R] COUNTER1（指令位置カウンタ）
#define	PULSEMOTOR_CMD_RCUN2	0xE4			// [R] COUNTER2（機械位置カウンタ）
#define	PULSEMOTOR_CMD_RREST	0xF2			// [R] エラー割り込み要因取得




/******************************************************************************
* 
* [概要]
* 
* 
* 
******************************************************************************/
ER	MntInit( void )
{
	BOOL	blnResult;
	UB		rtycnt = 0;
	int	setValue;

	pMotionnet = (struct StMotionnet *)MNET_BASE_ADDRESS;

	blnResult = SetMainCommand(MNET_CENTER_RESET);				//  100h

	check_main_pulsemotor_initialize();// PCD4611 初期化追加

	// BUSY解除まで待つ
	while( ChkRbsy() ){};
	
	blnResult = SetMainRegister(MNET_CENTER_WRENV0, 0x037F);	// 5500h
	blnResult = SetMainCommand(MNET_CMD_CLEAR_SEND_FIFO);		//  200h
	blnResult = SetMainCommand(MNET_CMD_CLEAR_RECV_FIFO);		//  300h
	blnResult = SetMainCommand(MNET_CENTER_SYSCOM);				// 1000h

	// BUSY解除まで待つ
	while( 1)
	{
		if( ChkDbsy() == CLR ) break;

		rtycnt++;
		dly_tsk(DLY_1M);

		if( rtycnt > SYS_RTY_OVER )
		{
			return( E_CON_SYSCOM );
		}
	};

	// センター通信開始
	rtycnt = 0;
	blnResult = SetMainCommand(MNET_CENTER_STA_IOCOM);			// 3000h
	while(1)
	{
		if( ChkSbsy() ) break;
		rtycnt++;
		dly_tsk(DLY_1M);

		// BUSY解除されず
		if( rtycnt > SYS_RTY_OVER )
		{
			// 通信異常と判断
			return( E_CON_IO_COM );
		}
	}
	
	
	pio_Init();
	motor_Init();

	return( E_SUCCESS );

}
//==============================================================================
//	Function	: check_main_pulsemotor_initialize
//	Description	: メイン基板のパルスモータ検査の初期化
//	Parameters	: なし
//	Return		: なし
//	Note		: 
//	Global		: 
//==============================================================================
void	check_main_pulsemotor_initialize( )
{
//	int	setValue;
	
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRFL, _pmInfo[MT_HEAD_Y0].reg[R1] );	// RFL
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRFH, _pmInfo[MT_HEAD_Y0].reg[R2] );	// RFH
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRUR, _pmInfo[MT_HEAD_Y0].reg[R3] );	// RUR
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDR, 0                           );	// RDR
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRMG, _pmInfo[MT_HEAD_Y0].reg[R4] );	// RMG
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDP, 0                           );	// RDP
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRDS, _pmInfo[MT_HEAD_Y0].reg[R9] );	// RDS
//	SetMotorRegisterInt( MT_HEAD_Y0, WRG_PRMG, 1199                        );	// RMG
#if 1
	SetMotorRegisterInt( MT_Y0, WRG_PRMG,   1199              );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRFL,  50                );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRFH,  300               );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_PRUR,  500               );	// 
	SetMotorRegisterInt( MT_Y0, WPCD_RENV2, 0x00000005        );	// 
	WriteMemoryPCD4611 ( CMD_CUN1R );							// CUN1R : 
	WriteMemoryPCD4611 ( CMD_CUN2R );							// CUN2R : 
#endif

#if 0
	WriteMemoryIntPCD4611( 1199 );
	WriteMemoryPCD4611( WPCD_PRMG );		// PRMG : 速度倍率設定

	WriteMemoryIntPCD4611( 50 );
	WriteMemoryPCD4611( WPCD_PRFL );		// PRFL : 初速設定

	WriteMemoryIntPCD4611( 300 );
	WriteMemoryPCD4611( WPCD_PRFH );		// PRFH : 高速設定

	WriteMemoryIntPCD4611( 500 );
	WriteMemoryPCD4611( WPCD_PRUR );		// PRUR : 

	WriteMemoryIntPCD4611( 500 );
	WriteMemoryPCD4611( WPCD_PRDR );		// PRDR : 

	WriteMemoryIntPCD4611( 0x00000005 );
	WriteMemoryPCD4611( WPCD_RENV2 );		// RENV2 : 
	
	WriteMemoryPCD4611( CMD_CUN1R );		// CUN1R : 
	WriteMemoryPCD4611( CMD_CUN2R );		// CUN2R : 
#endif

#if 0
	// PRMG : 速度倍率設定
	setValue = 1199;							// 倍率 = 1
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRMG;	// 0x85;

	// PRFL : 初速設定
	setValue = 50;								// 初速設定 = 50[pps]
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRFL;	// 0x81;

	// PRFH
	setValue = 300;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRFH;	// 0x82;

	// PRUR
	setValue = 500;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRUR;	// 0x83;

	// PRDR
	setValue = 500;
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_PRDR;	// 0x84;

	// RENV2 : 環境設定2
	setValue = 0x00000005;						// EA/EB: A/B相1逓倍, P1～P0: 汎用出力
	pPulseMotor->BUFB0 = (unsigned char)( setValue >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( setValue >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( setValue >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( setValue >> 24 );
	pPulseMotor->unCmdSts.stWrite.COMB0 = WPCD_RENV2;	// 0x9D
#endif
	
	// 初期状態の設定
	// 汎用出力（"P0", "P1"）を"L"にする ... HOLDで可変
	pPulseMotor->unCmdSts.stWrite.OTPB = 0x00;
}



BOOL motor_Init(void)
{
	UB i;

	motor_InitController();

	for (i=DVMIN+1; i<=DVMAX; i++)
	{
		if (IsExist( i ))
		{
			_pmInfo[i].factor 			= 2;
			_pmInfo[i].pos 				= 0;
			_pmInfo[i].power 			= OFF;
			_pmInfo[i].paused 			= FALSE;
			_pmInfo[i].command.code 	= CMD_NOCOMMAND;
			_pmInfo[i].startParam 		= CMD_NOTYPE;

			_pmInfo[i].reg[R0]			= 0;		// Pls
			_pmInfo[i].reg[R1]			= 200;		// RFL
			_pmInfo[i].reg[R2]			= 800;		// RFH
			_pmInfo[i].reg[R3]			= 500;		// RUR
			_pmInfo[i].reg[R4]			= 500;
			_pmInfo[i].reg[R5]			= 0;
			_pmInfo[i].reg[R6]			= 0;
			_pmInfo[i].reg[R7]			= 0;

			motor_PowerUp(i, _pmInfo[i].power);
		}
	}

	return TRUE;
}
///<summary>
/// モータコントローラを初期化する関数
///</summary>
static BOOL motor_InitController(void)
{
	UB pm;
	UB enc;
	UB mnet_drv_typ;

	// メインデバイスの全割り込みを無効化
	if (!SetMainRegister(MNET_CENTER_WRENV0, 0x037F))				return FALSE;

	for (pm=DVMIN; pm<=DVMAX; pm++)
	{
		if (IsExist( pm ))											// デバイス接続しているか(≠0：接続)
		{
			mnet_drv_typ = MNET_DRV_TYPE[pm];						// デバイス情報取得

			switch( mnet_drv_typ )
			{
			case BCD4020FUA:
				if (!SetMotorCommand(pm, CMD_SRST))					continue;
				if( IsEncoderExist(pm) == TRUE )					enc = EC_3;
				else												enc = EC_N;

				if( SetRenv1Fu( pm ))								return FALSE;// RENV1設定
				if( SetRenv2Fu( pm, enc ))							return FALSE;// RENV2設定
				if( SetRenv3Fu( pm, enc ))							return FALSE;// RENV3設定
				break;

			case BCD4020FU:
				if (!SetMotorCommand(pm, CMD_SRST))					continue;
				if( IsEncoderExist(pm) == TRUE )					enc = EC_3;
				else												enc = EC_N;

				if( SetRenv1Fu( pm ))								return FALSE;// RENV1設定
				if( SetRenv2Fu( pm, enc ))							return FALSE;// RENV2設定
				break;

			case PCD4611:
//				if( SetRenv1PCD4611( pm ))							return FALSE;// RENV1設定
//				if( SetRenv2PCD4611( pm, enc ))						return FALSE;// RENV2設定
				break;

			default:
				// モータドライバ以外は、スキップ
				continue;
				break;
			}
			// RIRQ
			SetMotorRegisterInt(pm, WRG_RIRQ, 0x00000000);
		}
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
// 割り込みハンドラ

///<summary>
/// コマンド実行結果を取得します
///</summary>
void IRQ4_ISR()
{
#if 0	
	MNETStatus status;
	status.UNID.data = pMotionnet->commandStatus0 ;
	ClearInterrupt();
	INTC.IRQRR.BIT.IRQ4F = 0;
#endif
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// 
// IO系
// 
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
///<summary>
/// IOトローラを初期化する関数
///</summary>
BOOL pio_Init()
{
// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return TRUE;
#endif
	// メインデバイスの全割り込みを無効化
	SetMainRegister(MNET_CENTER_WRENV0, 0x037F);

	return TRUE;
}


/******************************************************************************
* 
* [概要]	全入力チャネルの状態を取得する
* [引数]	data    入力値格納先
* [戻り値]	TRUE:成功、FALSE:失敗
* 
******************************************************************************/
void pio_ALLIN(UB *data)
{
	INT Loopn;
	INT amari;
	INT syou;
	INT sftdat;
	UB  sftbit;
	UB  ret;

	sftbit = 0x01;

	pMotionnet = (struct StMotionnet *)MNET_BASE_ADDRESS;

	// ※補足）実装されていない入力個所０
 	// bit1    チャネル2
	// ・
	// bit63   チャネル64

	
	// ※補足）実装されていない入力個所０
	// Ch0は無効のため検査しない
	for( Loopn = MIN_CH; Loopn <= MAX_CH; Loopn++ )	// 2020/2/12 １はじまり対応
	{
		sftbit = 0x01;

		syou    = ( (Loopn - MIN_CH) / 8 );			// 余り取得
		amari   = ( (Loopn - MIN_CH) % 8 );			// 余り取得
		sftdat  = ( sftbit << amari );				// 余りをシフト

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
		ret = 0;
#else
		ret = GetInputDat( Loopn ) ;
#endif

		if( ret )
		{
			data[syou] |= sftdat;
		}
		else
		{
			data[syou] &= (~sftdat);
		}
	}
}

/******************************************************************************
* 
* [概要]	入力チャネルから信号を取得する
* [引数]	ch      入力チャネル(０～)
* 			sw      [out]チャネルの状態(TRUE:ON、FALSE：OFF)
* [戻り値]	1:High、0:Low
* 
******************************************************************************/
UB pio_BIN(UB ch)
{
	UB retdat = OFF;
	
// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	if( ch == 4 )
	{
		return(1);
	}
	else
	{
		return(0);
	}
#endif
	
	if( ( ch == I_DOORSW1 )||( ch == I_DOORSW2 ) )
	{
		
		
		
		
	}
	else
	{
		retdat = GetInputDat( ch );
	}
	
	
	return ( retdat );
}
/******************************************************************************
* 
* [概要]	出力チャネルへの出力データ
* [引数]	ch      出力チャネル
* 			sw      ON:HIGH出力、OFF:LOW出力
* [戻り値]	－
* 
******************************************************************************/
void pio_BOUT(UB ch, UB sw)
{
	INT waittim = WAITTIM;
	INT cnt;
	UB rdata = 0;
	UB wdata = 0;
	UB devno;
	UB portno;
	UB mskbit;

	if(( ch < MIN_CH )||( ch > MAX_CH )) return;	// 2020/2/12 １はじまり対応

	if( ch == O_DAIAPMP )
	{
//		switch( sw )
//		{
//		case 1:		//正転
//			PE.DR.BIT.B0 = 1;
//			PE.DR.BIT.B1 = 0;
//			break;
//		case 2:		//逆転
//			PE.DR.BIT.B0 = 0;
//			PE.DR.BIT.B1 = 1;
//			break;
//		default:	//停止
//			PE.DR.BIT.B0 = 0;
//			PE.DR.BIT.B1 = 0;
//			break;
//		}
		
		switch( sw )
		{
		case 0:
			DpumpStop();
			break;
		default:
			SetDpumpDuty( sw );
			DpumpStart();
			break;
		}
	}
	else
	{
		// CLRリクエスト
		devno  = OUTPUTCH_INFTBL[ch][0];
		portno = OUTPUTCH_INFTBL[ch][1];
		mskbit = OUTPUTCH_INFTBL[ch][2];
		
		// 出力値作成
		if(( devno != NONE )&&( portno != NONE ))
		{
	// 本モード
#if DEBUG_SIM_MODE == 0
			// 書込みデータベース取得
			MAC_READ( pMotionnet->portData[devno][portno], wdata );
#endif

			// SETリクエスト
			if( sw )
			{
				wdata |= mskbit;
			}
			// CLRリクエスト
			else
			{
				wdata &= (~mskbit);
			}

// 本モード
#if DEBUG_SIM_MODE == 0
			// 指定Chへ出力
			for( cnt = 0; cnt < READ_RTY_MAX; cnt++ )
			{
				MAC_WRIT( pMotionnet->portData[devno][portno], wdata  );
				g_Sleep(waittim);
				MAC_READ( pMotionnet->portData[devno][portno], rdata );
				if( wdata == rdata )	break;
			}
#endif
		}
	}
}

/******************************************************************************
* 
* [概要]	パトライト専用出力
* [引数]	sw      ON:HIGH出力、OFF:LOW出力
* [戻り値]	－
* 
******************************************************************************/
void pio_PATOUT(UB sw1, UB sw2, UB sw3)
{
	UB data1 = 0;
	UB data2 = 0;
	UB mskbit = 0;
	UB devcno1 = 0;
	UB portno1 = 0;
	UB devcno2 = 0;
	UB portno2 = 0;

// 本モード
#if DEBUG_SIM_MODE == 0
	devcno1 = OUTPUTCH_INFTBL[O_PATLRED][0];
	portno1 = OUTPUTCH_INFTBL[O_PATLRED][1];
	MAC_READ( pMotionnet->portData[devcno1][portno1], data1 );
	
	devcno2 = OUTPUTCH_INFTBL[O_PATLGRN][0];
	portno2 = OUTPUTCH_INFTBL[O_PATLGRN][1];
	MAC_READ( pMotionnet->portData[devcno2][portno2], data2 );
	
#endif

	// 赤
	mskbit  = OUTPUTCH_INFTBL[O_PATLRED][2];
	if( sw1 )
	{
		data1 |= mskbit;
	}
	else
	{
		data1 &= (~mskbit);
	}

	// 緑
	mskbit  = OUTPUTCH_INFTBL[O_PATLGRN][2];
	if( sw2 )
	{
		data2 |= mskbit;
	}
	else
	{
		data2 &= (~mskbit);
	}
	
// 本モード
#if DEBUG_SIM_MODE == 0
	MAC_WRIT( pMotionnet->portData[devcno1][portno1], data1 );
	MAC_WRIT( pMotionnet->portData[devcno2][portno2], data2 );
#endif
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// 
// モータ系
// 
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
/*-----------------------------------------------------------------------------
  機能：速度の設定
  機能：no      モータ番号
        type    １：Ｓ字、０：台形
        resol   速度倍率
        lspd    初速[Hz]
        hspd    高速[Hz]
        tim     加減速時間
        sarea   Ｓ字変速領域[Hz] ※台形の時は無視
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_SET(UB pm, ST_MOTOR_PARAM mp)
{
	ER Result = E_SUCCESS ;
	PM_REG	pm_reg = {0};

	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}

	// PCD4611通信接続時
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		Result = motor_CalcMotorRegPCD4611(&mp, &pm_reg);
	}
	else
	{
		Result = motor_CalcMotorReg(&mp, &pm_reg);
	}

	if( Result != E_SUCCESS)								return( Result );
	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));	// デバイス接続確認
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));	// ビジー確認

	_pmInfo[pm].reg[R1] = pm_reg.reg[R1];
	_pmInfo[pm].reg[R2] = pm_reg.reg[R2];
	_pmInfo[pm].reg[R3] = pm_reg.reg[R3];
	_pmInfo[pm].reg[R4] = pm_reg.reg[R4];
	_pmInfo[pm].reg[R5] = pm_reg.reg[R5];
	_pmInfo[pm].reg[R6] = pm_reg.reg[R6];

	Result =  motor_SetAllRegister(pm);
	if( Result != E_SUCCESS )								return( Result );

	return Result;
}

/******************************************************************************
* 
* [概要]	アドレスカウンタのリセット
* [引数]	pm:モータ番号、cnt:カウント設定値、enc:エンコーダ設定値、dv:未使用、def:偏差カウンタ
* [戻値]	エラーコード
* 
******************************************************************************/
ER motor_PSET(UB pm, INT cnt, INT enc, INT def)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));	// デバイス接続確認
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));	// ビジー確認

	_pmInfo[pm].pos = cnt;

	// PCD4611通信接続時
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		if (!SetMotorRegisterInt(pm, WRG_RCUN1, cnt))		return(( E_MNT_ACCESS + WRG_RCUN1 ));
	}
	else
	{
		if (!SetMotorRegisterInt(pm, WRG_RCUN1, cnt))		return(( E_MNT_ACCESS + WRG_RCUN1 ));
		if (!SetMotorRegisterInt(pm, WRG_RCUN2, enc))		return(( E_MNT_ACCESS + WRG_RCUN2 ));
		if (!SetMotorRegisterInt(pm, WRG_RCUN3, def))		return(( E_MNT_ACCESS + WRG_RCUN3 ));
	}

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  機能：絶対アドレス移動
  機能：pm      モータ番号
        pos     目的地パルス
        status  動作指定
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_ABSMV(UB pm, INT pos, UB status)
{
	INT rcnt1 = 0;
	ER	Result = E_SUCCESS;
	UH	dir = (pos >=0 ? CW : CCW);

	if (pm < DVMIN || DVMAX < pm)			return((E_PAR_OUTRNG + PARAM1));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))						return(( E_CON_ERR + pm ));		// デバイス接続確認
	if (motor_IsBusy(pm))					return(( E_BUSY + pm ));		// ビジー確認
	if (motor_IsLimit(pm, dir))				return( E_SUCCESS );			// 現在リミットか確認

	motor_SetAllRegister(pm);												// Limit後だとLimit時の速さのままとなるため、ここで通常の設定速度に変更
	SetMotorCommand (pm, CMD_INTRS);										// 割込み出力リセット
	_pmInfo[pm].reg[R0]		= (INT)pos;

	_pmInfo[pm].startParam	= CMD_STAUD;									// スタートコマンド動作指定
	if( status == REQ_STAFH )	_pmInfo[pm].startParam	= CMD_STAFH;
	if( status == REQ_STAFL )	_pmInfo[pm].startParam	= CMD_STAFL;
	_pmInfo[pm].paused		= FALSE;

	
	// PCD4611通信接続時
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_RCUN1);
		rcnt1 = ReadMemoryPCD4611();

		SetMotorRegisterInt( MT_Y0, WRG_RMD, MOD41 );
		SetMotorRegisterInt( MT_Y0, WRG_RMV, (pos - rcnt1) );
		
		WriteMemoryPCD4611( CMD_STAUD );
	}
	// MNET_BCD4020FU(A)接続時
	else
	{
		Result = SetRenv1Fu( pm );			if( Result != E_SUCCESS )	return( Result );	// RENV1設定
		Result = SetRmdFu( pm, MOD42 );		if( Result != E_SUCCESS )	return( Result );	// RMD設定
		Result = SetRmv( pm, pos );			if( Result != E_SUCCESS )	return( Result );	// RMV設定

	}
	motor_RegistCommand(pm, CMD_ABSMV);
	
	return( E_SUCCESS );
}
/*-----------------------------------------------------------------------------
  機能：相対アドレス移動
  機能：pm      モータ番号
        pos     目的地パルス
        status  動作指定
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_RELMV(UB pm, INT pos, UB status)
{
	unsigned long	setValue;
	UB	mod;
	ER	Result = E_SUCCESS;
	UH	dir = (pos >=0 ? CW : CCW);

	if (pm < DVMIN || DVMAX < pm)			return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if (!IsExist( pm ))						return(( E_CON_ERR + pm ));	// デバイス接続確認
	if (motor_IsBusy(pm))					return(( E_BUSY + pm ));	// ビジー確認
	if (motor_IsLimit(pm, dir))				return( E_SUCCESS );		// 現在リミットか確認

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}
	
	// PCD4611通信接続時
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryIntPCD4611( MOD41 );
		WriteMemoryPCD4611( WRG_RMD );

		WriteMemoryIntPCD4611( pos );
		WriteMemoryPCD4611( WRG_RMV );
		
		WriteMemoryPCD4611( CMD_STAUD );
	}
	// MNET_BCD4020FU(A)接続時
	else
	{
		motor_SetAllRegister(pm);											// Limit後だとLimit時の速さのままとなるため、ここで通常の設定速度に変更
		SetMotorCommand (pm, CMD_INTRS);									// 割込み出力リセット
		_pmInfo[pm].reg[R0]		= (INT)pos;

		_pmInfo[pm].startParam	= CMD_STAUD;								// スタートコマンド動作指定
		if( status == REQ_STAFH )	_pmInfo[pm].startParam	= CMD_STAFH;
		if( status == REQ_STAFL )	_pmInfo[pm].startParam	= CMD_STAFL;
		
		_pmInfo[pm].paused		= FALSE;

		Result = SetRenv1Fu( pm );			if( Result != E_SUCCESS )	return( Result );	// RENV1設定
		Result = SetRmdFu( pm, MOD41 );		if( Result != E_SUCCESS )	return( Result );	// RMD設定
		Result = SetRmv( pm, pos );			if( Result != E_SUCCESS )	return( Result );	// RMV設定
		
		motor_RegistCommand(pm, CMD_RELMV);	
	}
	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  機能：リミット位置移動
  機能：no      モータ番号
        dir     方向(MOTOR_CW/MOTOR_CCW)
        spd     速度(定速)
        st      リザーブ
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_LIMIT(UB mm, UB dr, UB md, INT sp )
{
	ER	Result = E_SUCCESS;
	UB	mod;
	
	if (mm < DVMIN || DVMAX < mm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif

	if( MNET_DRV_TYPE[mm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( mm ))										return(( E_CON_ERR + mm ));	// デバイス接続確認
	if (motor_IsBusy(mm))									return(( E_BUSY + mm ));	// ビジー確認

	// PCD4611通信接続時
	if( MNET_DRV_TYPE[mm] == PCD4611 )
	{
//		if( dr == CW )	mod = MOD00;
//		else			mod = MOD08;
		if( dr == CW )	mod = MOD10;
		else			mod = MOD18;

		WriteMemoryIntPCD4611( mod );
		WriteMemoryPCD4611( WRG_RMD );
		
		WriteMemoryPCD4611( CMD_STAFL );
	}
	// MNET_BCD4020FU(A)接続時
	else
	{
		SetMotorCommand (mm, CMD_INTRS);			// 割込み出力リセット

		_pmInfo[mm].reg[R0]		= 0xFFFFFF;
		_pmInfo[mm].startParam	= CMD_STAFL;
		_pmInfo[mm].paused		= FALSE;

		if( MNET_DRV_TYPE[mm] == PCD4611 )			// PCD4611通信接続時
		{
			if( dr == CW )	mod = MOD10;
			else			mod = MOD18;
			
			WriteMemoryIntPCD4611( mod );
			WriteMemoryPCD4611( WRG_RMD );
			WriteMemoryPCD4611( CMD_STAFL );
		}
		// MNET_BCD4020FU(A)接続時
		else
		{
			Result = SetRenv1Fu( mm );							if( Result != E_SUCCESS )	return( Result );	// RENV1設定
			Result = SetRmdFu( mm, LMMOD_TBL[md][dr] );			if( Result != E_SUCCESS )	return( Result );	// RMD設定

			if (!SetMotorRegisterInt(mm, WRG_RFL, sp))			return(( E_MNT_ACCESS + WRG_RFL ));
			if (!SetMotorRegisterInt(mm, WRG_RFH, sp))			return(( E_MNT_ACCESS + WRG_RFH ));
			if (!SetMotorRegisterInt(mm, WRG_RMG, 199))			return(( E_MNT_ACCESS + WRG_RMG ));
			motor_RegistCommand(mm, CMD_LIMIT);
		}
	}
	return( E_SUCCESS );
}


/*-----------------------------------------------------------------------------
  機能：減速停止
  機能：no      モータ番号
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_STOP(UB pm)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}
	

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// デバイス接続確認
	motor_RegistCommand(pm, CMD_STOP);
	if( !SetMotorCommand(pm, CMD_SDSTP) )					return(( E_BUSY + pm ));

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  機能：即停止
  機能：no      モータ番号
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_ABORT(UB pm)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// デバイス接続確認
	if( !SetMotorCommand(pm, CMD_STOP))						return(( E_BUSY + pm ));

	return( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  機能：励磁のON/OFF設定
  機能：no      モータ番号
        sw      １：励磁ON、０：励磁OFF
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_POWER(UB pm, UB sw)
{
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif
	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));		// デバイス接続確認
	if (motor_IsBusy(pm))									return(( E_BUSY + pm ));		// ビジー確認
	if (!motor_PowerUp(pm, sw))								return(( E_CUR_ERR + pm ));		// 励磁

	_pmInfo[pm].power = sw;

	return ( E_SUCCESS );
}

/*-----------------------------------------------------------------------------
  機能：モータ状態の読み出し
  機能：no      モータ番号
  戻値：エラーコード
 ----------------------------------------------------------------------------*/
ER motor_STATUS(UB pm, ST_MOTOR_STATUS *status)
{
	INT pos  = 0;
	INT enc  = 0;
	INT dev  = 0;
	INT rsts = 0;
	INT rest = 0;
	INT rcnt1= 0;
	UB mnet_drv_typ;
	MNETSubStatus substs;
	
	// モータ番号範囲外
	if (pm < DVMIN || DVMAX < pm)							return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	// ステータスセット
	status->busy      = 0;
	status->cw_limit  = 0;
	status->ccw_limit = 0;
	status->enc       = 0;
	status->dev       = 0;
	status->pos       = _pmInfo[pm].pos = (INT)pos;
	return (E_SUCCESS );
#endif

	if (!IsExist( pm ))										return(( E_CON_ERR + pm ));				// デバイス接続異常

	mnet_drv_typ = MNET_DRV_TYPE[pm];						// デバイス情報取得

	if( mnet_drv_typ == PCD4611 )							// PCD4611通信接続時
	{
		WriteMemoryPCD4611(RRG_REST);
		rest = ReadMemoryPCD4611();

		status->cw_limit  = (rest & 0x00000001) ? 1 : 0;
		status->ccw_limit = (rest & 0x00000002) ? 1 : 0;
		if((rest & 0x00000004))		return( E_CON_HEAD_Y0 );
		
//		WriteMemoryPCD4611(RRG_RSTS);
//		pcd_rsts = ReadMemoryPCD4611();
		
		WriteMemoryPCD4611(RRG_RCUN1);
		rcnt1 = ReadMemoryPCD4611();
		
		// ステータスセット
		status->busy      = motor_IsBusy(pm);
		status->cw_limit  = motor_IsLimit(pm , CW );
		status->ccw_limit = motor_IsLimit(pm , CCW);
		status->enc       = 0;
		status->dev       = 0;
		status->pos       = rcnt1;
	}
	else
	{
#ifdef DEBUG_INFO
		DebugMnetInfo( pm );
#endif

		// エラー割込要因取得
		if (!GetMotorRegister(pm, RRG_REST, &rest))				return(( E_MNT_ACCESS + RRG_REST ));	// レジスタアクセス異常

		if (!GetMotorRegister(pm, RRG_RSTS, &rsts))	// 状態取得
		{
			return(( E_MNT_ACCESS + RRG_RSTS ));	// レジスタアクセス異常
		}
		
		if (!GetMotorRegister(pm, RRG_RCUN1, &pos))	// 移動量取得
		{
			return(( E_MNT_ACCESS + RRG_RCUN1 ));	// レジスタアクセス異常
		}
		
		if( IsEncoderExist(pm) == TRUE )			// エンンコーダ付き
		{
			// エンコーダ値取得
			if (!GetMotorRegister(pm, RRG_RCUN2, &enc))			return(( E_MNT_ACCESS + RRG_RCUN2 ));	// レジスタアクセス異常

			// 偏差値取得
			if (!GetMotorRegister(pm, RRG_RCUN3, &dev))			return(( E_MNT_ACCESS + RRG_RCUN3 ));	// レジスタアクセス異常

			// 脱調検知
			if(( rest & 0x00000004))							return(( E_DTY_ERR | pm ));				// REST bit2(ESC3)

			// 脱調検知
			if(( rsts & 0xf0000000) == 0)		// 固定値０エリアに書き込まれている場合は、FIFOの読込が出来なかったと判断し監視しない
			{
				if( (rsts & 0x00400000) )							
				{
//				return(( E_DTY_ERR | pm ));				// RSTS bit22(SCP3)
				}
			}
		}

		// 通信エラー
		if( (rest & 0x00000400) )								return(( E_CON_ERR | pm ));				// REST bit10(ESNT)

		// 脱調検知(オーバーライド失敗のため脱調と判断)
		if( (rest & 0x00001000) )								return(( E_DTY_ERR | pm ));				// REST bit12(ESOR)

		// ステータスセット
		status->busy      = motor_IsBusy(pm);
		status->cw_limit  = (rsts & 0x00000040) ? 1 : 0;
		status->ccw_limit = (rsts & 0x00000080) ? 1 : 0;
		status->enc       = enc;
		status->dev       = dev;
		status->pos       = _pmInfo[pm].pos = (INT)pos;
		
	}

	if( status->busy == TRUE )									return(( E_BUSY + pm ));				// デバイスビジー

	return ( E_SUCCESS );
}
/******************************************************************************
* 
* [概要]	エンコーダ設定
* [引数]	mm:モータ番号、ec:逓倍設定値、dtyp:未使用、dv:未使用、so:脱腸検出ﾊﾟﾙｽ
* [戻値]	エラーコード
* 
******************************************************************************/
ER motor_ENCODER(UB mm, UB ec, UB dtyp, UB dv, INT so)
{
	RENV2		renv2;
	RENV4		renv4;
	RENV2PCD	renv2pcd;
	INT 		pos = 0;
	UB 			mnet_drv_typ;

	// モータ番号範囲外
	if (mm < DVMIN || DVMAX < mm)									return(( E_PAR_OUTRNG + PARAM1 ));

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return (E_SUCCESS );
#endif

	if( MNET_DRV_TYPE[mm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}
	
	if (!IsExist( mm ))												return(( E_CON_ERR + mm ));
	if (motor_IsBusy(mm))											return(( E_BUSY + mm ));

	mnet_drv_typ = MNET_DRV_TYPE[mm];	// デバイス情報取得

	if(
		( mnet_drv_typ == BCD4020FUA )||
		( mnet_drv_typ == BCD4020FU )
	)
	{
		if (!GetMotorRegister(mm, RRG_RENV2, &renv2.UNID.data))		return(( E_MNT_ACCESS + RRG_RENV2 ));// RENV2読込
		if (!GetMotorRegister(mm, RRG_RENV4, &renv4.UNID.data))		return(( E_MNT_ACCESS + RRG_RENV4 ));// RENV4読込
		
		// Encoder 逓倍設定
		switch( ec )
		{
			case EC_1:// １逓倍
			default:
				renv2.UNID.BIT.EIM1 = 0;
				renv2.UNID.BIT.EIM0 = 0;
				break;
			case EC_2:// ２逓倍
				renv2.UNID.BIT.EIM1 = 0;
				renv2.UNID.BIT.EIM0 = 1;
				break;
			case EC_3:// ４逓倍
				renv2.UNID.BIT.EIM1 = 1;
				renv2.UNID.BIT.EIM0 = 0;
				break;
		}
		if (!SetMotorRegisterInt(mm, WRG_RENV2, renv2.UNID.data))	return(( E_MNT_ACCESS + WRG_RENV2 ));

		// 脱腸検出設定
		if (!GetMotorRegister(mm, RRG_RCUN1, &pos))					return(( E_MNT_ACCESS + RRG_RCUN1 ));		// カウンタ合わせ
		if (!SetMotorRegisterInt(mm, WRG_RCUN2, pos))				return(( E_MNT_ACCESS + WRG_RCUN2 ));
		if( !SetMotorCommand(mm, CMD_CUN3R) )						return(( E_BUSY + mm ));					// COUNTER3リセット
		if (!SetMotorRegisterInt(mm, WRG_RCMP3, so))				return(( E_MNT_ACCESS + WRG_RCMP3 ));		// RCMP3データ
		if(( so )&&(ec != EC_N))	// 脱調パルス有かつエンコーダ無効以外
		{
			// 脱腸検出ﾊﾟﾙｽ≠0の場合、RCMP3 ＜ 比較カウンタで条件成立
			renv4.UNID.BIT.C3S0 = 1;
			renv4.UNID.BIT.C3S1 = 0;
			renv4.UNID.BIT.C3S2 = 1;
			renv4.UNID.BIT.C3S3 = 0;
		}
		else
		{
			// 脱腸検出ﾊﾟﾙｽ＝0の場合、常に比較条件不成立にしておく
			renv4.UNID.BIT.C3S0 = 0;
			renv4.UNID.BIT.C3S1 = 0;
			renv4.UNID.BIT.C3S2 = 0;
			renv4.UNID.BIT.C3S3 = 0;
		}
		if (!SetMotorRegisterInt(mm,WRG_RENV4,renv4.UNID.data))		return(( E_MNT_ACCESS + WRG_RENV4 ));		// 設定
	}
	// PCD4611通信接続時
	else if( MNET_DRV_TYPE[mm] == PCD4611 )
	{
		// 本案件は、エンコードないため処理不要
	}

	return( E_SUCCESS );
}



/******************************************************************************
* 
* [概要]	モータ速度倍率取得
* [引数]	速度番号
* [戻値]	倍率値
* 
******************************************************************************/
INT GetFactor( UB factor )
{
	INT Result = 0;

	if( ( factor >= 0)&&( factor <= 10 ))
	{
		Result = MOTORSPD_TBL[factor];
	}
	return( Result );
}
/******************************************************************************
* 
* [概要]	モータ速度倍率取得
* [引数]	速度番号
* [戻値]	倍率値
* 
******************************************************************************/
INT GetFactorPCD4611( UB factor )
{
	INT Result = 0;

	if( ( factor >= 0)&&( factor <= 10 ))
	{
		Result = MOTORSPD_TBL_PCD4611[factor];
	}
	return( Result );
}

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■


/***********************************************************************
*
* [概要]  センターデバイスのメモリーにデータを書き込みます
* [引数]  addr：書込みアドレス、data：書込みデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static BOOL WriteMemory(INT addr, UH data)
{
	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);
	ER	result = E_SUCCESS;

	// バス経由によるデータ通知のためアクセス頻度がある
	// 場合、データ競合の可能性もでてくるため排他制御追加
	MAC_WAI_SEM( ID_SEM_MEMRW );

	*p++ = (UB)(data >> 0);
	*p   = (UB)(data >> 8);

#ifdef WAIT_TYPE_IFB
	// TRUE:ビジー	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}
#endif

#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif

	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [概要]  センターデバイスのメモリーにデータを書き込みます
* [引数]  addr：書込みアドレス、data：書込みデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static BOOL WriteMemoryInt(INT addr, INT data)
{	
	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);
	ER	result = E_SUCCESS;

	// バス経由によるデータ通知のためアクセス頻度がある
	// 場合、データ競合の可能性もでてくるため排他制御追加
	MAC_WAI_SEM( ID_SEM_MEMRW );

	*(p+0) = (UB)(data >> 0);
	*(p+1) = (UB)(data >> 8);
	*(p+0) = (UB)(data >> 16);
	*(p+1) = (UB)(data >> 24);

	// TRUE:ビジー	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}

#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif
	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [概要]  センターデバイスのメモリーからデータを読み出します
* [引数]  addr：読み出しアドレス、data：読み出しデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static BOOL ReadMemory(INT addr,  UH *data)
{
	UB	b0;
	UB	b1;
	ER	result = E_SUCCESS;

	volatile UB *p = (UB *)(MNET_BASE_ADDRESS + addr);

	// バス経由によるデータ通知のためアクセス頻度がある
	// 場合、データ競合の可能性もでてくるため排他制御追加
	MAC_WAI_SEM( ID_SEM_MEMRW );

	b0 = (UB)*p++;
	b1 = (UB)*p;
	*data      = (b1 << 8) | b0;

#ifdef WAIT_TYPE_IFB
	// TRUE:ビジー	
	if( IfbIdleWait() == TRUE )
	{
		return FALSE;
	}
#endif
#ifdef WAIT_TYPE_RW
	g_Sleep(50);
#endif
	MAC_SIG_SEM( ID_SEM_MEMRW );

	return TRUE;
}
/***********************************************************************
*
* [概要]  センターデバイスにコマンドを書き込みます
* [引数]  command：コマンド
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static BOOL  SetMainCommand(UH command)
{
	return WriteMemory(MNET_COMMAND, command);
}
/***********************************************************************
*
* [概要]  センターデバイスのステータスを取得します
* [引数]  status：MNET状態格納先
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static BOOL GetMainStatus(MNETStatus* status)
{
	return ReadMemory(MNET_STATUS, &status->UNID.data);
}



// --- PCD4611 ---------------------------------------------------------------------------------------------

/***********************************************************************
*
* [概要]  センターデバイスのメモリーにデータを書き込みます
* [引数]  addr：書込みアドレス、data：書込みデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static void WriteMemoryPCD4611(UB data)
{
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	pPulseMotor->unCmdSts.stWrite.COMB0 = (unsigned char)data;
}
/***********************************************************************
*
* [概要]  センターデバイスのメモリーにデータを書き込みます
* [引数]  addr：書込みアドレス、data：書込みデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static void WriteMemoryIntPCD4611(INT data)
{	
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	pPulseMotor->BUFB0 = (unsigned char)( data >> 0 );
	pPulseMotor->BUFB1 = (unsigned char)( data >> 8 );
	pPulseMotor->BUFB2 = (unsigned char)( data >> 16 );
	pPulseMotor->BUFB3 = (unsigned char)( data >> 24 );
}

/***********************************************************************
*
* [概要]  センターデバイスのメモリーからデータを読み出します
* [引数]  addr：読み出しアドレス、data：読み出しデータ
* [戻値]  TRUE:成功、FALSE:失敗
* 
***********************************************************************/
static INT ReadMemoryPCD4611()
{
	INT result = 0x00;
	pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

	result |=	( (INT)( pPulseMotor->BUFB0 ) << 0 );
	result |=	( (INT)( pPulseMotor->BUFB1 ) << 8 );
	result |=	( (INT)( pPulseMotor->BUFB2 ) << 16 );
	result |=	( (INT)( pPulseMotor->BUFB3 ) << 24 );
	
	return result;
}
// --- PCD4611 ---------------------------------------------------------------------------------------------



///<summary>
/// メインデバイスのレジスターにデータを書き込みます
///</summary>
static BOOL  SetMainRegister(UH reg, UH data)
{
	// FALSE:書込み失敗?
	if( WriteMemory(MNET_IO_BUFFER, data) == FALSE )
	{
		return FALSE;
	}
	return SetMainCommand(reg);
}


///<summary>
/// 指定モーターデバイスにコマンドを書き込みます
///</summary>
static BOOL   SetMotorCommand(UB dev_no, UH command)
{
	if( MNET_DRV_TYPE[dev_no] == PCD4611 )
	{
		WriteMemoryPCD4611( (UB)command );
	}
	else
	{
		WriteMemory(MNET_FIFO, command);								// レジスタ書き込み

		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + dev_no)) )	// FALSE:コマンドセット失敗?
		{
			return FALSE;
		}

#ifdef WAIT_END_CHK
		
		if( EndIdleWait()  == TRUE )									// TRUE:ビジー	
		{
			return FALSE;
		}
#endif
	}
	return TRUE;
}

///<summary>
/// 指定モーターデバイスのレジスターにデータを書き込みます
///</summary>
static BOOL   SetMotorRegisterInt(UB dev_no, UH reg, INT value)
{
	if( MNET_DRV_TYPE[dev_no] == PCD4611 )
	{
		WriteMemoryIntPCD4611(value);
		WriteMemoryPCD4611((UB)reg);
	}
	else
	{
		WriteMemory   (MNET_FIFO, reg);		// レジスタ書き込み
		WriteMemoryInt(MNET_FIFO, value);	// データ書き込み
		
		if( EndIdleWait()  == TRUE )		// TRUE:ビジーのまま
		{
			return ( FALSE );
		}
	}
	
	return TRUE;
}

///<summary>
/// 指定モーターデバイスのレジスターにデータを書き込みます
///</summary>
static BOOL   SetMotorRegister(UB dev_no, UH reg, UH value)
{
	UB mnetno;

	mnetno = MNET_NO[dev_no];

	if( MNET_DRV_TYPE[mnetno] == PCD4611 )
	{
		WriteMemoryPCD4611(value);
		WriteMemoryPCD4611(reg);
	}
	else
	{
		WriteMemory   (MNET_FIFO, reg);		// レジスタ書き込み
		WriteMemoryInt(MNET_FIFO, value);	// データ書き込み

		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + mnetno)) )// FALSE:コマンドセット失敗?
		{
			return ( FALSE );
		}
		
		if( EndIdleWait()  == TRUE )		// TRUE:ビジー	
		{
			return ( FALSE );
		}
	}


	return TRUE;
}
///<summary>
/// 指定モーターデバイスのレジスターからデータを読み込みます
///</summary>
// FALSE:異常、TRUE:成功
BOOL	GetMotorRegister(UB dev_no, UH reg, INT* value)
{
	INT	fifo_dat;
	UB mnetno;
	MNETStatus status;

	mnetno = MNET_NO[dev_no];

	if( MNET_DRV_TYPE[mnetno] == PCD4611 )
	{
		WriteMemoryPCD4611((UB)reg);
		*value  =  ReadMemoryPCD4611();
	}
	else
	{
		if (!SetMainCommand(MNET_CMD_CLEAR_RECV_FIFO))		return FALSE;

		// リードコマンドをＦＩＦＯへ書き込む
		WriteMemory(MNET_FIFO, reg);
		// リードコマンドをＦＩＦＯへ書き込む
		if( !SetMainCommand((MNET_DATA_COMMUNICATION_BASE + mnetno)) )
		{
			return ( FALSE );
		}
		// ステータスリード処理
		while (TRUE)
		{
			// ステータスリード
			GetMainStatus(&status);

			// CEND = 1?
			if( status.UNID.BIT.CEND == 1 )
			{
				// EDTE = 0?
				if(status.UNID.BIT.EDTE == 0 )
				{
					// 正常
					break;
				}
				else
				{
					// 異常
					return FALSE;
				}
			}
		}
		*value  = ((INT)(pMotionnet->fifo0) <<  0);
		*value |= ((INT)(pMotionnet->fifo1) <<  8);
#ifdef WAIT_TYPE_IFB
		// TRUE:ビジー	
		if( IfbIdleWait() == TRUE )
		{
			return FALSE;
		}
#endif

		fifo_dat  =  (pMotionnet->fifo0 & 0x000000ff);
		fifo_dat |= ((pMotionnet->fifo1 <<  8) & 0x0000ff00);
		g_Sleep(30);
		fifo_dat |= ((pMotionnet->fifo0 << 16) & 0x00ff0000);
		fifo_dat |= ((pMotionnet->fifo1 << 24) & 0xff000000);
		*value  =  fifo_dat;
#ifdef WAIT_END_CHK
		// TRUE:ビジー	
		if( EndIdleWait()  == TRUE )
		{
			return FALSE;
		}
#endif
	}

	
	return( TRUE );
}


///<summary>
/// 指定モーターデバイスのステータスを読み込みます( PCD4611以外 )
///</summary>
BOOL GetMotorStatus(UB dev_no, MotorStatus* status)
{
	UB Data_H = 0;
	UB Data_L = 0;
	UH Data_HL;

	if (!IsExist( dev_no ))
	{
		return FALSE;
	}
	
	//	struct
	//	{
	//		UH	reserve2: 7;	// 常に0になります。
	//		UH	SBSY	: 1;	// パルス出力開始で１になります。動作停止で０になります。（＝ＢＳＹ）
	//		UH	reserve1: 4;	// 常に0になります。
	//		UH	SEVT	: 1;	// イベント割り込み発生により１になり、ＲＩＳＴ読み出しで０になります。
	//		UH	SERR	: 1;	// エラー割り込み発生により１になり、ＲＥＳＴ読み出しで０になります。
	//		UH	SEND	: 1;	// 動作停止により割り込みを発生し、１になります。割り込みリセットコマンド(0008h)で０になります。
	//		UH	SINT	: 1;	// 割り込み発生により１になります。(ビット１,２,３のいずれかが１)
	//	}BIT;
	MAC_READ( pMotionnet->portData[dev_no][0], Data_L )
	MAC_READ( pMotionnet->portData[dev_no][1], Data_H )

	status->UNID.data = ( ( Data_H * 0x100 ) + Data_L ) ;

	return TRUE;
}
/******************************************************************************
* 
* [概要]	指定デバイスが存在するかチェックします
* [引数]	モーター番号
* [戻値]	TRUE:デバイス有、FALSE:デバイス無
* 
******************************************************************************/
BOOL IsExist(UB dev_no)
{
	UB result = 0;
	UB mnetno ;

	// 接続上限を超えている？
	if( ( dev_no > DVMAX )||( ( dev_no < DVMIN ) ))
	{
		return( FALSE );
	}

	switch( MNET_DRV_TYPE[dev_no] )
	{
	case BCD4020FU:
	case BCD4020FUA:
		mnetno = dev_no;
		break;

	case PCD4611:
		return TRUE;
		break;

	case M204DUM:
		mnetno = MNET_NO[dev_no]; 
		break;
	case NCPRSUNIT:
		return TRUE;
		break;
	}

// シュミレーションモード
#if DEBUG_SIM_MODE != 0
	return( TRUE ) ;
#endif
	
	MAC_READ( pMotionnet->deviceInformation[mnetno], result );

	return result != 0;
}

static	INT pcd_rsts;

/******************************************************************************
* 
* [概要]	パルスモータが実行状態を返す
* [引数]	モーター番号
* [戻値]	TRUE:実行中、FALSE:アイドリング
* 
******************************************************************************/
BOOL motor_IsBusy(UB pm)
{
	INT Loop;
	UH  wk = 0;
	MotorStatus	mstatus;

	if (!IsExist( pm ))
	{
		return FALSE;
	}
	
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_RSTS);
		pcd_rsts = ReadMemoryPCD4611();
		
		if(
			( (pcd_rsts & 0x0000000A) == 0x0000000A )||
			( (pcd_rsts & 0x0000000B) == 0x0000000B )||
			( (pcd_rsts & 0x0000000C) == 0x0000000C )||
			( (pcd_rsts & 0x0000000D) == 0x0000000D )
		)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		for( Loop = 0; Loop <= 3; Loop++ )
		{
			GetMotorStatus(pm, (MotorStatus* )&mstatus);

			if ( wk != mstatus.UNID.BIT.SBSY )
			{
				wk = mstatus.UNID.BIT.SBSY;
				Loop = 0;
			}
		}
		
		if( wk == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/******************************************************************************
* 
* [概要]	パルスモータが指定方向のリミット位置の場合TRUEを返す関数
* [引数]	モーター番号、方向
* [戻値]	TRUE:リミット、FALSE:リミット以外
* 
******************************************************************************/
BOOL motor_IsLimit(UB pm, UH dir)
{
	BOOL result = FALSE;
	RSTS rsts;
	INT  rest= 0;

	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		WriteMemoryPCD4611(RRG_REST);
		rest = ReadMemoryPCD4611();
		
		if( dir == CW )
		{
			if((rest & 0x00000001))		return( TRUE );
		}
		if( dir == CCW )
		{
			if((rest & 0x00000002))		return( TRUE );
		}
	}
	else
	{
		if (!GetMotorRegister(pm, RRG_RSTS, &rsts.UNID.data))
		{
			return( result );
		}

		if( dir == CW )
		{
			if( rsts.UNID.BIT.SPEL )
			{
				result = TRUE;
			}
		}
		if( dir == CCW )
		{
			if( rsts.UNID.BIT.SMEL )
			{
				result = TRUE;
			}
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	指定パルスモータがリミット位置の場合TRUEを返す関数
* [引数]	モーター番号
* [戻値]	TRUE:リミット、FALSE:リミット以外
* 
******************************************************************************/
BOOL motor_IsLimitW(UB pm)
{
	if( motor_IsLimit(pm,CW) == TRUE )	return( TRUE );
	if( motor_IsLimit(pm,CCW) == TRUE )	return( TRUE );

	return( FALSE );
}
/******************************************************************************
* 
* [概要]	エンコーダ有無を取得
* [引数]	モーター番号
* [戻値]	TRUE:有、FALSE:無
* 
******************************************************************************/
BOOL IsEncoderExist(UB pm)
{
	BOOL	result = FALSE;
	UB		index;
	UB		bitsfh = 0x00;
	UB		bitdat = 0x01;
	UB		pmwk;

	// モータ範囲外
	if( pm > DVMAX )	return( result );
	
	// bit0    チャネル1
 	// bit1    チャネル2
	// ・
	// bit63   チャネル64
	if( pm == 0 )		return( result );
	pmwk = pm - 1;

	index  = ( pmwk / 8 );
	bitsfh = ( pmwk % 8 );
	bitdat = (bitdat << bitsfh);
	
	// エンコーダ付き
	if( (ENCODER_EXIST[index] & bitdat) )
	{
		result = TRUE;
	}
	
	return( result );
}
/******************************************************************************
* 
* [概要]	パルスモータを励磁
* [引数]	モーター番号、励磁動作指示
* [戻値]	TRUE:成功、FALSE:失敗
* 
******************************************************************************/
BOOL motor_PowerUp(UB pm, UB sw)
{
	INT cnt;
	INT rcnt;
	UB DataCmp;
	UB DataSet;
	UB mnet_drv_typ;

	mnet_drv_typ = MNET_DRV_TYPE[pm];

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( TRUE );
	}

	if(
		( mnet_drv_typ != BCD4020FUA )&&
		( mnet_drv_typ != BCD4020FU )
	)
	{
		return FALSE;
	}
	
	MAC_READ( pMotionnet->portData[pm][2], DataSet );

	switch( mnet_drv_typ )
	{
	case BCD4020FUA:
		if (sw)
		{
			DataSet &= (~0x01);	// 励磁ON
		}
		else
		{
			DataSet |= 0x01;	// 励磁OFF
		}
		break;
	case BCD4020FU:
		if (sw)
		{
			DataSet |= 0x40;	// 励磁ON
		}
		else
		{
			DataSet &= (~0x40);	// 励磁OFF
		}
		break;
	}

//x		g_Sleep(20);
//x		g_Sleep(40);
//o		g_Sleep(50);
//o		g_Sleep(60);
//o		g_Sleep(500);
//o		g_Sleep(250);
//o		g_Sleep(120);
	for( cnt = 0; cnt < READ_RTY_MAX; cnt++ )
	{
		MAC_WRIT( pMotionnet->portData[pm][3], DataSet  );

		for( rcnt = 0; rcnt < READ_RTY_MAX; rcnt++ )
		{
			g_Sleep(WAITTIM);
			MAC_READ( pMotionnet->portData[pm][2], DataCmp );
			if( DataSet == DataCmp )
			{
				return TRUE;
			}
			else
			{
			}
		}
	}
	
	
	return FALSE;
}
///<summary>
/// 全てのレジスタに値を書き込む関数
///</summary>
ER motor_SetAllRegister(UB pm)
{
	RMD		rmd;

	if( MNET_DRV_TYPE[pm] == NCPRSUNIT )// 圧力ユニット
	{
		return( E_SUCCESS );
	}

	// デバイス:PCD4611
	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		pPulseMotor = (struct StPulseMotor *)PULSEMOTOR_ADDR;

//		WriteMemoryPCD4611( CMD_CUN1R );		// CUN1R : 
//		if( !SetMotorRegisterInt(pm, WRG_RFA, 1                  ) )	return(( E_MNT_ACCESS + WRG_RFA ));// 9Bh
		SetMotorRegisterInt( pm, WRG_PRFL, _pmInfo[pm].reg[R1] );	// RFL
		SetMotorRegisterInt( pm, WRG_PRFH, _pmInfo[pm].reg[R2] );	// RFH
		SetMotorRegisterInt( pm, WRG_PRUR, _pmInfo[pm].reg[R3] );	// RUR
		SetMotorRegisterInt( pm, WRG_PRDR, 0                   );	// RDR
		SetMotorRegisterInt( pm, WRG_PRMG, _pmInfo[pm].reg[R4] );	// RMG
		SetMotorRegisterInt( pm, WRG_PRDP, 0                   );	// RDP
		SetMotorRegisterInt( pm, WRG_PRDS, _pmInfo[pm].reg[R9] );	// RDS
		SetMotorRegisterInt( pm, WRG_PRMG, 1199                );	// RMG

	}
	// デバイス:FU,FU-A
	else
	{
		if( !GetMotorRegister(pm, RRG_RMD, &rmd.UNID.data))				return(( E_MNT_ACCESS + RRG_RMD ));

		rmd.UNID.BIT.MSMD = (_pmInfo[pm].param.accel_type == ACCEL_STYPE ? 1 : 0);
		rmd.UNID.BIT.MINT = 1;
		if( !SetMotorRegisterInt(pm, WRG_RMD, rmd.UNID.data) )			return(( E_MNT_ACCESS + WRG_RMD ));// 97h
		if( !SetMotorRegisterInt(pm, WRG_RFL, _pmInfo[pm].reg[R1]) )	return(( E_MNT_ACCESS + WRG_RFL ));// 91h
		if( !SetMotorRegisterInt(pm, WRG_RFH, _pmInfo[pm].reg[R2]) )	return(( E_MNT_ACCESS + WRG_RFH ));// 92h
		if( !SetMotorRegister(pm,   WRG_RUR, _pmInfo[pm].reg[R3]) )		return(( E_MNT_ACCESS + WRG_RUR ));// 93h
		if( !SetMotorRegister(pm,   WRG_RDR, 0) )						return(( E_MNT_ACCESS + WRG_RDR ));// 94h
		if( !SetMotorRegisterInt(pm, WRG_RMG, _pmInfo[pm].reg[R4]) )	return(( E_MNT_ACCESS + WRG_RMG ));// 95h
		if( !SetMotorRegisterInt(pm, WRG_RDP, 0) )						return(( E_MNT_ACCESS + WRG_RDP ));// 96h
		if( !SetMotorRegisterInt(pm, WRG_RFA, 1) )						return(( E_MNT_ACCESS + WRG_RFA ));// 9Bh
		if( !SetMotorRegisterInt(pm, WRG_RUS, _pmInfo[pm].reg[R8]) )	return(( E_MNT_ACCESS + WRG_RUS ));// 99h
		if( !SetMotorRegisterInt(pm, WRG_RDS, _pmInfo[pm].reg[R9]) )	return(( E_MNT_ACCESS + WRG_RDS ));// 9Ah
	}

	return( E_SUCCESS );
}
///<summary>
/// モータ動作パラメータから各レジスタの値を計算する関数
///</summary>
static ER motor_CalcMotorReg(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg)
{
	if( mp->accel_type > 1 )		return(( E_PAR_OUTRNG + PARAM1 ));
	if( mp->FH == 0 )				return(( E_PAR_OUTRNG + PARAM2 ));
	if( mp->FL == 0 )				return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->FH <= mp->FL )			return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->utime <= 0)				return(( E_PAR_OUTRNG + PARAM4 ));

	factor	= mp->factor;
	param	= (mp->accel_type == ACCEL_LINEAR ? 5000 : 2500);
	RMG		= factor;
	RFL		= (mp->FL * (RMG + 1)) / 200;
	RFH		= (mp->FH * (RMG + 1)) / 200;
	RUR		= ((mp->utime * param) / (RFH - RFL)) - 1;
	RUS		= (mp->us * (RMG + 1)) / 200;
	RDS		= (mp->ds * (RMG + 1)) / 200;
	
	pm_reg->reg[R1]	= RFL;
	pm_reg->reg[R2]	= RFH;
	pm_reg->reg[R3]	= RUR;
	pm_reg->reg[R4]	= RMG;
	pm_reg->reg[R5]	= 0;
	pm_reg->reg[R6]	= 2;
	pm_reg->reg[R7]	= 0;
	pm_reg->reg[R8]	= RUS;
	pm_reg->reg[R9]	= RDS;

	return ( E_SUCCESS );
}
///<summary>
/// モータ動作パラメータから各レジスタの値を計算する関数
///</summary>
static ER motor_CalcMotorRegPCD4611(const ST_MOTOR_PARAM* mp, PM_REG* pm_reg)
{
	if( mp->accel_type > 1 )		return(( E_PAR_OUTRNG + PARAM1 ));
	if( mp->FH == 0 )				return(( E_PAR_OUTRNG + PARAM2 ));
	if( mp->FL == 0 )				return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->FH <= mp->FL )			return(( E_PAR_OUTRNG + PARAM3 ));
	if( mp->utime <= 0)				return(( E_PAR_OUTRNG + PARAM4 ));

	factor	= MOTORSPD_TBL_PCD4611[2];
	param	= (mp->accel_type == ACCEL_LINEAR ? 5000 : 2500);
	RMG		= factor;
	RFL		= (mp->FL * (RMG + 1)) / 2441;						// 40,000,000/ 16384 = 2441
	RFH		= (mp->FH * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	RUR		= ((mp->utime * param) / (RFH - RFL)) - 1;
	RUS		= (mp->us * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	RDS		= (mp->ds * (RMG + 1)) / 2441;						// 40,000,000/ 16384
	
	pm_reg->reg[R1]	= RFL;
	pm_reg->reg[R2]	= RFH;
	pm_reg->reg[R3]	= RUR;
	pm_reg->reg[R4]	= RMG;
	pm_reg->reg[R5]	= 0;
	pm_reg->reg[R6]	= 2;
	pm_reg->reg[R7]	= 0;
	pm_reg->reg[R8]	= RUS;
	pm_reg->reg[R9]	= RDS;

	return ( E_SUCCESS );
}
/////<summary>
/// コマンドをセットする関数
///</summary>
static void motor_RegistCommand(UB pm, UH cmd)
{
	_pmInfo[pm].command.code     = cmd;
	_pmInfo[pm].command.status   = PSTAT_READY;

	if( MNET_DRV_TYPE[pm] == PCD4611 )
	{
		// PCD4611は、処理不要。前段階で実行済み
	}
	else
	{
		SetMotorCommand(pm, _pmInfo[pm].startParam);
	}
}
///<summary>
/// 指定μs待機します
///</summary>
static void g_Sleep(INT usec)
{
    INT i;
    
    i = 40000;          // 6.25 ns/instruction * 4 instructions * loop_count = 1ms = 1000000 ns
    i *= usec;
    i /= 1000;
    while (i--);        /* loop count */
}
///<summary>
/// 割り込み要因をクリアします
///</summary>
static void ClearInterrupt(void)
{
}

/******************************************************************************
* 
* [概要]	ＩＦＢアイドリング待ち
* [引数]	－
* [戻値]	IFBビット(TRUE = ビジー , FALSE = アイドリング)
* 
******************************************************************************/
static BOOL IfbIdleWait(void)
{
	INT	loop;
	
	for( loop = 0; loop < MNET_CMD_RTY_MAX; loop++ )
	{
		if( ChkIFB() == TRUE )	return( FALSE );
		g_Sleep(5);
	}
	
	return( TRUE );
}
/******************************************************************************
* 
* [概要]	ＩＦＢ状態取得
* [引数]	－
* [戻値]	IFBビット(TRUE = 1 , FALSE = 0)
* 
******************************************************************************/
static BOOL ChkIFB( void )
{
	BOOL	result; 

	if( PB.DR.BIT.B19 )
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}

	return( result );
}
/******************************************************************************
* 
* [概要]	完了待ち
* [引数]	－
* [戻値]	TRUE = ビジー , FALSE = アイドリング
* 
******************************************************************************/
static BOOL EndIdleWait(void)
{
	INT	loop;
	
	for( loop = 0; loop < MNET_CMD_RTY_MAX; loop++ )
	{
		if( ChkEnd() == TRUE )	return( FALSE );
		g_Sleep(5);
	}

	return( TRUE );
}
/******************************************************************************
* 
* [概要]	エンド状態取得
* [引数]	－
* [戻値]	TRUE:完了、FALSE:未完了
* 
******************************************************************************/
static BOOL ChkEnd( void )
{
	BOOL	result; 
	MNETStatus status;

	GetMainStatus(&status);

	if (status.UNID.BIT.CEND)
	{
		if (status.UNID.BIT.EDTE)
		{
			result = FALSE;	//エラー
		}
		else
		{
			result = TRUE;
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
* [概要]	RBSY状態取得
* [引数]	－
* [戻値]	1:ON、0:OFF
* 
******************************************************************************/
static UB ChkRbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.RBSY ? SET : CLR );

	return( result );
}
/******************************************************************************
* 
* [概要]	DBSY状態取得
* [引数]	－
* [戻値]	1:ON、0:OFF
* 
******************************************************************************/
static UB ChkDbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.DBSY ? SET : CLR );

	return( result );
}
/******************************************************************************
* 
* [概要]	SBSY状態取得
* [引数]	－
* [戻値]	1:ON、0:OFF
* 
******************************************************************************/
static UB ChkSbsy( void )
{
	UB	result; 
	MNETStatus status;

	GetMainStatus(&status);

	result = ( status.UNID.BIT.SBSY ? SET : CLR );

	return( result );
}
static UB dev_nowk;
/******************************************************************************
* 
* [概要]	RENV1 設定( BCD-4020FUA , BCD-4020FU )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv1Fu( UB dev_no )
{
	RENV1		renv1;
	ER 			result = E_SUCCESS;
dev_nowk = dev_no;
	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// 圧力ユニット
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv1, CLR, sizeof( RENV1 ) );

		renv1.UNID.BIT.PDTC = 1;

		//FU-A
		if( MNET_DRV_TYPE[dev_no] == BCD4020FUA )
		{
			// 自動カレントダウン
			renv1.UNID.BIT.CDWN = 1;
		}

		// 9Ch
		if (!SetMotorRegisterInt( dev_no, WRG_RENV1, renv1.UNID.data ) )
		{
			result = E_MNT_ACCESS + WRG_RENV1;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RENV2 設定( BCD-4020FUA , BCD-4020FU )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv2Fu( UB dev_no, UB teibai )
{
	ER 			result = E_SUCCESS;
	RENV2		renv2;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// 圧力ユニット
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv2, CLR, sizeof( RENV2 ) );
		
		// P0M 励磁出力
		renv2.UNID.BIT.P0M  = 1;
		
		if( MNET_DRV_TYPE[dev_no] == BCD4020FUA )
		{
			// P1M 汎用出力
			renv2.UNID.BIT.P1M  = 1;

			// P2M 汎用出力
			renv2.UNID.BIT.P2M  = 1;
		}
		else
		{
			// P6M 自動カレントダウン
			renv2.UNID.BIT.P6M  = 1;
		}
		
		// EOF
		renv2.UNID.BIT.E0FF = 0;

		// EA/EB入力仕様
		switch( teibai )
		{
		// 1逓倍
		case EC_1:
			renv2.UNID.BIT.EIM0 = 0;
			renv2.UNID.BIT.EIM1 = 0;
			break;
		// 2逓倍
		case EC_2:
			renv2.UNID.BIT.EIM0 = 1;
			renv2.UNID.BIT.EIM1 = 0;
			break;
		// 4逓倍
		case EC_3:
		default:
			renv2.UNID.BIT.EIM0 = 0;
			renv2.UNID.BIT.EIM1 = 1;
			break;
		}
		
		// 9Dh
		if (!SetMotorRegisterInt( dev_no, WRG_RENV2, renv2.UNID.data ) )
		{
			result = E_MNT_ACCESS + WRG_RENV2;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RENV3 設定( BCD-4020FUA , BCD-4020FU )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv3Fu( UB dev_no, UB enc )
{
	ER 			result = E_SUCCESS;
	RENV3		renv3;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// 圧力ユニット
	{
		return( result );
	}

	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv3, CLR, sizeof( RENV3 ) );

		// 原点復帰動作設定：原点復帰２
		renv3.UNID.BIT.ORM0 = 0;
		renv3.UNID.BIT.ORM1 = 0;
		renv3.UNID.BIT.ORM2 = 1;
		renv3.UNID.BIT.ORM3 = 1;

		// COUNTER2カウント設定：EA/EB入力
		renv3.UNID.BIT.CI20 = 0;
		renv3.UNID.BIT.CI21 = 0;

		// COUNTER3カウント設定：出力パルスとEA/EB入力（偏差）
		renv3.UNID.BIT.CI30 = 0;
		renv3.UNID.BIT.CI31 = 0;
		renv3.UNID.BIT.CI32 = 1;

		// 9Eh
		if (!SetMotorRegisterInt(dev_no, WRG_RENV3, renv3.UNID.data) )
		{
			result = E_MNT_ACCESS + WRG_RENV3;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RENV4 設定( BCD-4020FUA , BCD-4020FU )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv4Fu( UB dev_no, INT so )
{
	ER 			result = E_SUCCESS;
	RENV4		renv4;
	UB 			sts;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// 圧力ユニット
	{
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &renv4, CLR, sizeof( RENV4 ) );
		
		sts = ( so > 0 ? SET : CLR );

		// コンパレータ３の比較カウンタ選択：COUNTER3(汎用・偏差)
		renv4.UNID.BIT.C3C0 = 0;
		renv4.UNID.BIT.C3C1 = sts;

		// コンパレータ３の比較方法選択：RCMP3＝比較カウンタ
		renv4.UNID.BIT.C3S0 = sts;
		renv4.UNID.BIT.C3S1 = 0;
		renv4.UNID.BIT.C3S2 = 0;
		renv4.UNID.BIT.C3S3 = 0;

		// コンパレータ３条件成立時の処理選択：即停止
		renv4.UNID.BIT.C3D0 = sts;
		renv4.UNID.BIT.C3D1 = 0;
		
		// 9Fh
		if (!SetMotorRegisterInt(dev_no, WRG_RENV4, renv4.UNID.data) )
		{
			result = E_MNT_ACCESS + WRG_RENV4;
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RMD 設定( BCD-4020FUA , BCD-4020FU )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRmdFu( UB dev_no, UB mod )
{
	ER 			result = E_SUCCESS;
	RMD			rmd;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )// 圧力ユニット
	{
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		memset( &rmd, CLR, sizeof( RMD ) );
		
		if (!GetMotorRegister(dev_no, RRG_RMD, &rmd.UNID.data))
		{
			return(( E_MNT_ACCESS + RRG_RMD ));
		}
		rmd.UNID.BIT.MOD = mod;
		rmd.UNID.BIT.MINT = 1;

		if (!SetMotorRegisterInt(dev_no, WRG_RMD, rmd.UNID.data))
		{
			return(( E_MNT_ACCESS + WRG_RMD ));
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RMV 設定
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRmv( UB dev_no, INT pos )
{
	ER 			result = E_SUCCESS;

	if( MNET_DRV_TYPE[dev_no] == NCPRSUNIT )
	{
		// 圧力ユニット
		return( result );
	}
	
	if( MNET_DRV_TYPE[dev_no] != PCD4611 )
	{
		if (!SetMotorRegisterInt(dev_no, WRG_RMV, pos))
		{
			return(( E_MNT_ACCESS + WRG_RMV ));
		}
	}
	return( result );
}
/******************************************************************************
* 
* [概要]	RMV 設定
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRmvPCD4611( UB dev_no, INT pos )
{
	WriteMemoryIntPCD4611( pos );
	WriteMemoryPCD4611( WRG_RMV );

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [概要]	RENV1 設定( PCD4611 )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv1PCD4611( UB dev_no )
{

	return( E_SUCCESS );
}

/******************************************************************************
* 
* [概要]	RENV2 設定( PCD4611 )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv2PCD4611()
{
	WriteMemoryIntPCD4611( 0x00000005 );// EA/EB: A/B相1逓倍, P1～P0: 汎用出力
	WriteMemoryPCD4611( WPCD_RENV2 );	// 0x9D

	return( E_SUCCESS );
}
/******************************************************************************
* 
* [概要]	RENV3 設定( PCD4611 )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRenv3PCD4611( UB dev_no, UB enc )
{
	return( E_SUCCESS );
}
/******************************************************************************
* 
* [概要]	RMD 設定( PCD4611 )
* [引数]	－
* [戻値]	－
* 
******************************************************************************/
static ER SetRmdPCD4611( UB dev_no, UB mod )
{
	WriteMemoryIntPCD4611( mod );
	WriteMemoryPCD4611( WPCD_PRMD );

	return( E_SUCCESS );
}

#ifdef DEBUG_INFO
static 	INT dbgread;

/******************************************************************************
* 
* [概要]	SBSY状態取得
* [引数]	－
* [戻値]	1:ON、0:OFF
* 
******************************************************************************/
static void DebugMnetInfo( UB mm )
{

	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMV, &dbgread);           //  RRG_PRMV    = 0x00C0,   PRMV レジスタ の読み出し

	dbgread = 0;
//	GetMotorRegister(mm, RRG_PRFL, &dbgread);           //  RRG_PRFL    = 0x00C1,   PRFL レジスタ の読み出し
//	GetMotorRegister(mm, RRG_PRFH, &dbgread);           //  RRG_PRFH    = 0x00C2,   PRFH レジスタ の読み出し
//	GetMotorRegister(mm, RRG_PRUR, &dbgread);           //  RRG_PRUR    = 0x00C3,   PRUR レジスタ の読み出し
//	GetMotorRegister(mm, RRG_PRDR, &dbgread);           //  RRG_PRDR    = 0x00C4,   PRDR レジスタ の読み出し

	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMG, &dbgread);           //  RRG_PRMG    = 0x00C5,   PRMG レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRDP, &dbgread);           //  RRG_PRDP    = 0x00C6,   PRDP レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRMD, &dbgread);           //  RRG_PRMD    = 0x00C7,   PRMD レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRUS, &dbgread);           //  RRG_PRUS    = 0x00C9,   PRUS レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_PRDS, &dbgread);           //  RRG_PRDS    = 0x00CA,   PRDS レジスタ の読み出し
	
	dbgread = 0;
	GetMotorRegister(mm, RRG_RMV, &dbgread);			//  RRG_RMV     = 0x00D0,   RMV レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RFL, &dbgread);            //  RRG_RFL     = 0x00D1,   RFL レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RFH, &dbgread);            //  RRG_RFH     = 0x00D2,   RFH レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RUR, &dbgread);            //  RRG_RUR     = 0x00D3,   RUR レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RDR, &dbgread);            //  RRG_RDR     = 0x00D4,   RDR レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RMG, &dbgread);            //  RRG_RMG     = 0x00D5,   RMG レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RDP, &dbgread);            //  RRG_RDP     = 0x00D6,   RDP レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RMD, &dbgread);            //  RRG_RMD     = 0x00D7,   RMD レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RUS, &dbgread);            //  RRG_RUS     = 0x00D9,   RUS レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RDS, &dbgread);            //  RRG_RDS     = 0x00DA,   RDS レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RFA, &dbgread);            //  RRG_RFA     = 0x00DB,   RFA レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV1, &dbgread);          //  RRG_RENV1   = 0x00DC,   RENV1 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV2, &dbgread);          //  RRG_RENV2   = 0x00DD,   RENV2 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV3, &dbgread);          //  RRG_RENV3   = 0x00DE,   RENV3 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV4, &dbgread);          //  RRG_RENV4   = 0x00DF,   RENV4 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV5, &dbgread);          //  RRG_RENV5   = 0x00E0,   RENV5 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RENV6, &dbgread);          //  RRG_RENV6   = 0x00E1,   RENV6 レジスタ の読み出し
//	GetMotorRegister(mm, RRG_RENV7, &dbgread);          //  RRG_RENV7   = 0x00E2,   RENV7 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN1, &dbgread);          //  RRG_RCUN1   = 0x00E3,   RCUN1 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN2, &dbgread);          //  RRG_RCUN2   = 0x00E4,   RCUN2 レジスタ の読み出し
	dbgread = 0;
	GetMotorRegister(mm, RRG_RCUN3, &dbgread);          //  RRG_RCUN3   = 0x00E5,   RCUN3 レジスタ の読み出し

}
#endif

//	struct
//	{
//		UH	BBSY	: 1;	// RENV0(8)= "1" でブレーク通信コマンド（0610h）を発行したとき、
//								// ブレーク通信が完了するまで "1" となります。
//		UH	DBSY	: 1;	// システム通信中、またはデータ通信中に1 になります。
//		UH	RBSY	: 1;	// リセット処理中に1 になります。
//		UH	SBSY	: 1;	// サイクリック通信スタート中に1 になります。
//		UH	reserve2: 1;	// 常に0 になります。
//		UH	RDBB	: 1;	// データ受信用FIFO に受信データがある時に1 になります。
//		UH	TDBB	: 1;	// データ送信用FIFO に送信データがある時に1 になります。
//		UH	REF		: 1;	// 未送信の出力ポートデータがある時1 になります。
//		UH	reserve1: 1;	// 常に0 になります。
//		UH	CAER	: 1;	// CPU のアクセスエラーです。
//		UH	ERAE	: 1;	// ﾛｰｶﾙ側受信処理ｴﾗｰ発生時に1 になり、割り込み信号(INT)を出力します。
//		UH	EDTE	: 1;	// データ通信エラー発生時に1 になり、割り込み信号(INT)を出力します。
//		UH	EIOE	: 1;	// サイクリック通信ｴﾗｰ発生時に1 になり、割り込み信号(INT)を出力します。
//		UH	IOPC	: 1;	// 「入力変化割り込み設定」を1 にセットした入力ポートの状態が変化した時に1 になり、
//								// 割り込み信号(INT)を出力します。
//		UH	BRKF	: 1;	// ブレークフレーム受信時に1 になり、割り込み信号(INT)を出力します。
//		UH	CEND	: 1;	// データ送信用FIFO 書き込み可能時に1 になります。
//	}BIT;

