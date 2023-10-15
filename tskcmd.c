





/******************************************************************************
* コマンド実行タスク                                                          *
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
// 定数定義
//*************************************
//#define	SRAM_START_ADDRESS			0x0C000000	// 0C00 0000 - 0C07 FFFF
#define	USER_CT_MAX					10
#define	USER_ID_MAX					8
#define	USER_PW_MAX					4
#define	PROT_CT_MAX					20

#define	PULSEMOTOR_ADDR			0x0C009000		// パルスモータコントローラアドレス
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
#define	PULSEMOTOR_COUNT_ERROR_LIMIT	1		// カウントエラー許容値	[cnt]


//*************************************
// 関数定義
//*************************************
static	void	CmdTskInit();											// コマンドタスクデータ初期化
static	void	CmdTskInitAct();										// コマンドタスク動作データ初期化
static	void	CmdProc(T_CMD_MSG* mrcv);								// タスク別解析処理
static	void	Rx0RcvMsg( T_CMD_MSG* mrcv );							// ＰＣ受信（ＲＸ０）タスクからのメッセージ受信処理
static	void	Tx0RcvMsg( T_CMD_MSG* mrcv );							// ＰＣ送信（ＴＸ０）タスクからのメッセージ受信処理
static	void	AlmRcvMsg( T_CMD_MSG* mrcv );							// アラームハンドラから受信
static	void	EepRcvMsg( T_CMD_MSG* mrcv );							// ＥＥＰＲＯＭ管理タスクからのメッセージ受信処理
static	void	IntRcvMsg( T_CMD_MSG* mrcv );							// ＩＮＴタスクから受信
static	void	MotWaitRcvMsg( T_CMD_MSG* mrcv );						// ウェイトタスクから受信
static	void	MotEndRcvMsg( T_CMD_MSG* mrcv );						// 終了監視タスクから受信
static	void	RxExRcvMsg( T_CMD_MSG* mrcv );							// 受信（ＲＸＥＸ）タスクからのメッセージ受信処理
static	void	PunitRcvMsg( T_CMD_MSG* mrcv );
static	void	TmpCntRcvMsg( T_CMD_MSG* mrcv );

static	void	CmdAnalyze( T_CMD_MSG* mrcv );							// コマンド解析
typedef	void	(*CMD_FUNCTBL)( T_CMD_MSG* mrcv, UB anum, const char** args );// 動作実行ファンクション
typedef	void	(*TP_FUNCTBL) ( T_CMD_MSG* mrcv, UB anum, const char** args );// 動作実行ファンクション
static	ER		ParamDataCheck( UB msg , UB anum ,  const char** args );// 受信パラメータチェック


// コマンド処理
static void CmdDummy  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ダミールーチン                                    */
static void CmdHELLO  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   HELLO   起動通知                                  */
static void CmdVER    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   VER     バージョン確認                            */
static void CmdALLIN  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ALLIN   全I/O入力                                 */
static void CmdBIN    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   BIN     I/O入力                                   */
static void CmdBOUT   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   BOUT    I/O出力                                   */
static void CmdCINIT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   CINIT   入力カウンタの設定                        */
static void CmdCOUNT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   COUNT   入力カウンタの読み出し                    */
static void CmdINT    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   INT     割り込み設定                              */
static void CmdABSMV  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ABSMV   パルスモータ移動(絶対位置)                */
static void CmdABORT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ABORT   パルスモータ即停止                        */
static void CmdENCODER( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   ENCODER エンコーダの設定                          */
static void CmdLIMIT  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   LIMIT   パルスモータ原点復帰                      */
static void CmdMAP    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   MAP     ２軸のパルスモータ移動                    */
static void CmdPDELAY ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   PDELAY  パルスモータのパワー制御待ち時間設定      */
static void CmdPOWER  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   POWER   パルスモータの励磁制御                    */
static void CmdPSET   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   PSET    パルスモータのカウンタリセット            */
static void CmdRELMV  ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   RELMV   パルスモータ移動(相対位置)                */
static void CmdSET    ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   SET     パルスモータ速度設定                      */
static void CmdSTATUS ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   STATUS  パルスモータ状態読み出し                  */
static void CmdSTOP   ( T_CMD_MSG* mrcv, UB anum, const char** args ); 	/*   STOP    パルスモータ減速停止                      */
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

static void SetDevice( UB DevNo );										// デバイス動作中セット
static BOOL GetDeviceStatus( UB DevNo );								// デバイス動作状態取得
static void ClrDevice( UB DevNo );										// デバイス動作中クリア
static ER GetDeviceActionChk ( UB msg, UB bcid, UB mm );				// ＢＣ動作可能／不可判定
static ER LimitAction( UB prid1, UB bcid, UB mm, UB dr, UB md, INT sp );	// リミット動作
static ER RelmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp );				// 相対位置移動動作
static ER AbsmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp );				// 絶対位置移動動作
static ER MapAction( UB prid1, UB bcid, UB mm1, UB mm2, INT pppp1, INT pppp2 );	// MAP移動動作
static ER WaitAction( UB prid1, UB bcid, INT waittim );							// ＷＡＩＴ動作
static void GetBCDevice( UB msg, UB mm, INT *data );					// ＢＣで利用されるデバイス情報の取得

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
// タッチパネル関数定義
//*************************************
       void TpAnalyze( T_CMD_MSG* mrcv );
static void TplDummy( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 00 ダミー                                             */
static void TplFwver( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 01 FWバージョン情報                                   */
static void TplOptif( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 02 オプション情報                                     */
static void TplLgin1( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 03 ログイン認証1                                      */
static void TplLgin2( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 04 ログイン認証2                                      */
static void TplInita( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 05 初期化指示                                         */
static void TplTitle( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 06 プロトコルタイトル要求                             */
static void TplPrExt( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 07 プロトコル有無                                     */
static void TplUsral( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 08 ユーザー情報全取得                                 */
static void TplInf01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 11 ユーザー1情報                                      */
static void TplInf02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 12 ユーザー2情報                                      */
static void TplInf03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 13 ユーザー3情報                                      */
static void TplInf04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 14 ユーザー4情報                                      */
static void TplInf05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 15 ユーザー5情報                                      */
static void TplInf06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 16 ユーザー6情報                                      */
static void TplInf07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 17 ユーザー7情報                                      */
static void TplInf08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 18 ユーザー8情報                                      */
static void TplInf09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 19 ユーザー9情報                                      */
static void TplInf10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 1A ユーザー10情報                                     */
static void TplRec01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 21 ユーザー1登録                                      */
static void TplRec02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 22 ユーザー2登録                                      */
static void TplRec03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 23 ユーザー3登録                                      */
static void TplRec04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 24 ユーザー4登録                                      */
static void TplRec05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 25 ユーザー5登録                                      */
static void TplRec06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 26 ユーザー6登録                                      */
static void TplRec07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 27 ユーザー7登録                                      */
static void TplRec08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 28 ユーザー8登録                                      */
static void TplRec09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 29 ユーザー9登録                                      */
static void TplRec10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 2A ユーザー10登録                                     */
static void TplChg01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 31 ユーザー1変更                                      */
static void TplChg02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 32 ユーザー2変更                                      */
static void TplChg03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 33 ユーザー3変更                                      */
static void TplChg04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 34 ユーザー4変更                                      */
static void TplChg05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 35 ユーザー5変更                                      */
static void TplChg06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 36 ユーザー6変更                                      */
static void TplChg07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 37 ユーザー7変更                                      */
static void TplChg08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 38 ユーザー8変更                                      */
static void TplChg09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 39 ユーザー9変更                                      */
static void TplChg10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 3A ユーザー10変更                                     */
static void TplDel01( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 41 ユーザー1削除                                      */
static void TplDel02( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 42 ユーザー2削除                                      */
static void TplDel03( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 43 ユーザー3削除                                      */
static void TplDel04( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 44 ユーザー4削除                                      */
static void TplDel05( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 45 ユーザー5削除                                      */
static void TplDel06( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 46 ユーザー6削除                                      */
static void TplDel07( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 47 ユーザー7削除                                      */
static void TplDel08( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 48 ユーザー8削除                                      */
static void TplDel09( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 49 ユーザー9削除                                      */
static void TplDel10( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 4A ユーザー10削除                                     */
static void TplFwpol( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 50 FW状態（ポーリング）                               */
static void TplPrtcl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 51 プロトコル実行                                     */
static void TplResta( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 52 リスタート実行                                     */
static void TplPause( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 53 一時停止                                           */
static void TplResum( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 54 再開                                               */
static void TplAbort( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 55 中断                                               */
static void TplOptcd( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 58 オプションバーコード実行                           */
static void TplSmbar( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 59 サンプルチューブQRコード                           */
static void TplSmsgl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5A サンプルチューブ有無確認（単）                     */
static void TplSmall( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5B サンプルチューブ有無確認（全）                     */
static void TplCllne( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5C 回収チューブレーンQRコード                         */
static void TplClqrc( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5D 回収チューブQRコード                               */
static void TplClsgl( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5E 回収チューブ有無（単）                             */
static void TplClall( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 5F 回収チューブ有無（全）                             */
static void TplLdlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 60 LED                                                */
static void TplUvlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 61 UV点灯                                             */
static void TplPtlmp( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 62 パトランプ                                         */
static void TplPowof( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 63 電源OFF                                            */
static void TplExRun( T_CMD_MSG* mrcv, UB anum, const char** args );	/* 6B 運転履歴                                           */
static void TplCommDataSend( UB *tpdat, INT tpsiz );					/* */
static void TplCommDataSendImp( UB *tpdat, INT tpsiz, UB msg );
static void TplRidDataSend( UB *txdat, UB id );
static ER   TpCmdCheck( T_CMD_MSG* mrcv );								/* タッチパネルコマンド確認                              */
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


static ER  CmdActionStatus( T_CMD_MSG* mrcv );	//状態取得
static ER  CmdActionTipGet( T_CMD_MSG* mrcv );	//チップ取得
static ER  CmdActionTipDsp( T_CMD_MSG* mrcv );	//チップ廃棄
static ER  CmdActionStrOpn( T_CMD_MSG* mrcv );	//穴あけ
static ER  CmdActionPress ( T_CMD_MSG* mrcv );	//加圧
static ER  CmdActionWait  ( T_CMD_MSG* mrcv );	//ウェイト
static ER  CmdActionMix   ( T_CMD_MSG* mrcv );	//攪拌
static ER  CmdActionSuc   ( T_CMD_MSG* mrcv );	//吸引
static ER  CmdActionPut   ( T_CMD_MSG* mrcv );	//吐出
static ER  CmdActionPrep1 ( T_CMD_MSG* mrcv );	//分注１
static ER  CmdActionPrep2 ( T_CMD_MSG* mrcv );	//分注２
static ER  CmdActionIncub ( T_CMD_MSG* mrcv );	//インキュベーション
static ER  CmdActionMove  ( T_CMD_MSG* mrcv );	//移動

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
// テーブル
//*************************************
// コールファンクション
static CMD_FUNCTBL	CmdFuncTbl[ID_MSG_BASE_CMD_MAX] = 
{
	/*===========================================================================================================================*/
	/*  基本コマンド                                                       No. Command 内容                                      */
	/*===========================================================================================================================*/
	&CmdVER    ,														/* 0   HELLO   起動通知                                  */
	&CmdDummy  ,														/* 1   dummy                                             */
	&CmdVER    ,														/* 2   VER     バージョン確認                            */
	&CmdALLIN  ,														/* 3   ALLIN   全I/O入力                                 */
	&CmdBIN    ,														/* 4   BIN     I/O入力                                   */
	&CmdBOUT   ,														/* 5   BOUT    I/O出力                                   */
	&CmdCINIT  ,														/* 6   CINIT   入力カウンタの設定                        */
	&CmdCOUNT  ,														/* 7   COUNT   入力カウンタの読み出し                    */
	&CmdINT    ,														/* 8   INT     割り込み設定                              */
	&CmdABSMV  ,														/* 9   ABSMV   パルスモータ移動(絶対位置)                */
	&CmdABORT  ,														/* 10  ABORT   パルスモータ即停止                        */
	&CmdENCODER,														/* 11  ENCODER エンコーダの設定                          */
	&CmdLIMIT  ,														/* 12  LIMIT   パルスモータ原点復帰                      */
	&CmdMAP    ,														/* 13  MAP     ２軸のパルスモータ移動                    */
	&CmdPDELAY ,														/* 14  PDELAY  パルスモータのパワー制御待ち時間設定      */
	&CmdPOWER  ,														/* 15  POWER   パルスモータの励磁制御                    */
	&CmdPSET   ,														/* 16  PSET    パルスモータのカウンタリセット            */
	&CmdRELMV  ,														/* 17  RELMV   パルスモータ移動(相対位置)                */
	&CmdSET    ,														/* 18  SET     パルスモータ速度設定                      */
	&CmdSTATUS ,														/* 19  STATUS  パルスモータ状態読み出し                  */
	&CmdSTOP   ,														/* 20  STOP    パルスモータ減速停止                      */
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
//        タッチパネル通信
//-----------------------------
// コールファンクション
TP_FUNCTBL	TpFuncTbl[TP_ID_PROC_MAX] = 
{
	/*===========================================================================================================================*/
	/*  基本コマンド                                                       No. Command 内容                                      */
	/*===========================================================================================================================*/
	&TplDummy ,                                                         /* 00 ダミー                                             */
	&TplFwver ,                                                         /* 01 FWバージョン情報                                   */
	&TplOptif ,                                                         /* 02 オプション情報                                     */
	&TplLgin1 ,                                                         /* 03 ログイン認証1                                      */
	&TplLgin2 ,                                                         /* 04 ログイン認証2                                      */
	&TplInita ,                                                         /* 05 初期化指示                                         */
	&TplDummy ,                                                         /* 06 ダミー                                             */
	&TplDummy ,                                                         /* 07 ダミー                                             */
	&TplUsral ,                                                         /* 08 ユーザー情報全取得                                 */
	&TplDummy ,                                                         /* 09 ダミー                                             */
	&TplDummy ,                                                         /* 0A ダミー                                             */
	&TplDummy ,                                                         /* 0B ダミー                                             */
	&TplDummy ,                                                         /* 0C ダミー                                             */
	&TplDummy ,                                                         /* 0D ダミー                                             */
	&TplDummy ,                                                         /* 0E ダミー                                             */
	&TplDummy ,                                                         /* 0F ダミー                                             */
	&TplDummy ,                                                         /* 10 ダミー                                             */
	&TplInf01 ,                                                         /* 11 ユーザー1情報                                      */
	&TplInf02 ,                                                         /* 12 ユーザー2情報                                      */
	&TplInf03 ,                                                         /* 13 ユーザー3情報                                      */
	&TplInf04 ,                                                         /* 14 ユーザー4情報                                      */
	&TplInf05 ,                                                         /* 15 ユーザー5情報                                      */
	&TplInf06 ,                                                         /* 16 ユーザー6情報                                      */
	&TplInf07 ,                                                         /* 17 ユーザー7情報                                      */
	&TplInf08 ,                                                         /* 18 ユーザー8情報                                      */
	&TplInf09 ,                                                         /* 19 ユーザー9情報                                      */
	&TplInf10 ,                                                         /* 1A ユーザー10情報                                     */
	&TplDummy ,                                                         /* 1B ダミー                                             */
	&TplDummy ,                                                         /* 1C ダミー                                             */
	&TplDummy ,                                                         /* 1D ダミー                                             */
	&TplDummy ,                                                         /* 1E ダミー                                             */
	&TplDummy ,                                                         /* 1F ダミー                                             */
	&TplDummy ,                                                         /* 20 ダミー                                             */
	&TplRec01 ,                                                         /* 21 ユーザー1登録                                      */
	&TplRec02 ,                                                         /* 22 ユーザー2登録                                      */
	&TplRec03 ,                                                         /* 23 ユーザー3登録                                      */
	&TplRec04 ,                                                         /* 24 ユーザー4登録                                      */
	&TplRec05 ,                                                         /* 25 ユーザー5登録                                      */
	&TplRec06 ,                                                         /* 26 ユーザー6登録                                      */
	&TplRec07 ,                                                         /* 27 ユーザー7登録                                      */
	&TplRec08 ,                                                         /* 28 ユーザー8登録                                      */
	&TplRec09 ,                                                         /* 29 ユーザー9登録                                      */
	&TplRec10 ,                                                         /* 2A ユーザー10登録                                     */
	&TplDummy ,                                                         /* 2B ダミー                                             */
	&TplDummy ,                                                         /* 2C ダミー                                             */
	&TplDummy ,                                                         /* 2D ダミー                                             */
	&TplDummy ,                                                         /* 2E ダミー                                             */
	&TplDummy ,                                                         /* 2F ダミー                                             */
	&TplDummy ,                                                         /* 30 ダミー                                             */
	&TplChg01 ,                                                         /* 31 ユーザー1変更                                      */
	&TplChg02 ,                                                         /* 32 ユーザー2変更                                      */
	&TplChg03 ,                                                         /* 33 ユーザー3変更                                      */
	&TplChg04 ,                                                         /* 34 ユーザー4変更                                      */
	&TplChg05 ,                                                         /* 35 ユーザー5変更                                      */
	&TplChg06 ,                                                         /* 36 ユーザー6変更                                      */
	&TplChg07 ,                                                         /* 37 ユーザー7変更                                      */
	&TplChg08 ,                                                         /* 38 ユーザー8変更                                      */
	&TplChg09 ,                                                         /* 39 ユーザー9変更                                      */
	&TplChg10 ,                                                         /* 3A ユーザー10変更                                     */
	&TplDummy ,                                                         /* 3B ダミー                                             */
	&TplDummy ,                                                         /* 3C ダミー                                             */
	&TplDummy ,                                                         /* 3D ダミー                                             */
	&TplDummy ,                                                         /* 3E ダミー                                             */
	&TplDummy ,                                                         /* 3F ダミー                                             */
	&TplDummy ,                                                         /* 40 ダミー                                             */
	&TplDel01 ,                                                         /* 41 ユーザー1削除                                      */
	&TplDel02 ,                                                         /* 42 ユーザー2削除                                      */
	&TplDel03 ,                                                         /* 43 ユーザー3削除                                      */
	&TplDel04 ,                                                         /* 44 ユーザー4削除                                      */
	&TplDel05 ,                                                         /* 45 ユーザー5削除                                      */
	&TplDel06 ,                                                         /* 46 ユーザー6削除                                      */
	&TplDel07 ,                                                         /* 47 ユーザー7削除                                      */
	&TplDel08 ,                                                         /* 48 ユーザー8削除                                      */
	&TplDel09 ,                                                         /* 49 ユーザー9削除                                      */
	&TplDel10 ,                                                         /* 4A ユーザー10削除                                     */
	&TplDummy ,                                                         /* 4B ダミー                                             */
	&TplDummy ,                                                         /* 4C ダミー                                             */
	&TplDummy ,                                                         /* 4D ダミー                                             */
	&TplDummy ,                                                         /* 4E ダミー                                             */
	&TplDummy ,                                                         /* 4F ダミー                                             */
	&TplFwpol ,                                                         /* 50 FW状態（ポーリング）                               */
	&TplPrtcl ,                                                         /* 51 プロトコル実行                                     */
	&TplResta ,                                                         /* 52 リスタート実行                                     */
	&TplPause ,                                                         /* 53 一時停止                                           */
	&TplResum ,                                                         /* 54 再開                                               */
	&TplAbort ,                                                         /* 55 中断                                               */
	&TplDummy ,                                                         /* 56 ダミー                                             */
	&TplDummy ,                                                         /* 57 ダミー                                             */
	&TplOptcd ,                                                         /* 58 オプションバーコード実行                           */
	&TplSmbar ,                                                         /* 59 サンプルチューブQRコード                           */
	&TplSmsgl ,                                                         /* 5A サンプルチューブ有無確認（単）                     */
	&TplSmall ,                                                         /* 5B サンプルチューブ有無確認（全）                     */
	&TplCllne ,                                                         /* 5C 回収チューブレーンQRコード                         */
	&TplClqrc ,                                                         /* 5D 回収チューブQRコード                               */
	&TplClsgl ,                                                         /* 5E 回収チューブ有無（単）                             */
	&TplClall ,                                                         /* 5F 回収チューブ有無（全）                             */
	&TplLdlmp ,                                                         /* 60 LED                                                */
	&TplUvlmp ,                                                         /* 61 UV点灯                                             */
	&TplPtlmp ,                                                         /* 62 パトランプ                                         */
	&TplPowof ,                                                         /* 63 電源OFF                                            */
	&TplDummy ,                                                         /* 64 ダミー                                             */
	&TplDummy ,                                                         /* 65 ダミー                                             */
	&TplDummy ,                                                         /* 66 ダミー                                             */
	&TplDummy ,                                                         /* 67 ダミー                                             */
	&TplDummy ,                                                         /* 68 ダミー                                             */
	&TplDummy ,                                                         /* 69 ダミー                                             */
	&TplDummy ,                                                         /* 6A ダミー                                             */
	&TplExRun ,                                                         /* 6B 運転履歴                                           */
	&TplDummy ,                                                         /* 6C ダミー                                             */
	&TplDummy ,                                                         /* 6D ダミー                                             */
	&TplDummy ,                                                         /* 6E ダミー                                             */
	&TplDummy                                                           /* 6F ダミー                                             */
};

// データチェックテーブル
// num:パラメータ数（CMD分を含む）
const ST_DATACHK DATCHK_TBL[44] = {
/*======================================================================================================================================================================================================================================================================*/
/*  基本コマンド                                                               基本コマンド                                                                                                                                                                             */
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
// データ定義
//*************************************
static	ST_CNTLDATA			stCtl;										// 実行タスク制御用ﾃﾞｰﾀ

static	char				RecvData[CMD_BUFSIZ];						// 
static	const	char*		RecvArgs[MAX_ARGS];							// 
static	UB 					bufwk[CMD_BUFSIZ];							// 
static	ST_EEPBUF			eepb;
static	ST_USERINFO			RcvUserInfo;
static	INT 				DeviceStatus[2];							// モータ動作中状態  BIT=1:動作中
static	ER 					MnetStatus;
static	ST_POWER			power[(DVMAX+1)];
static	ST_PROTCOL			*pProcAdr;
static	ST_PROTCOL_DATA		*pProcDat;
static	ST_BZCNT			bzcnt;
static	UB					PauseStopFlg;
static	unsigned int 		RcvDataSize;
static	unsigned char 		ExpRunlSeqno;
static	unsigned char 		ExpRunlRetry;
static	ST_CNTLDATA			tp;											// 実行タスク制御用ﾃﾞｰﾀ
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

// debug用
static unsigned debug1_flg;


/***********************************************************************
* 
* [概要]コマンド管理タスク
* 
* 
* 
***********************************************************************/
TASK TskCmd( void )
{
    T_CMD_MSG*  	msg;
	ER          	err;

	// ※注意
	// スレーブよりメイン基板のモーションネットが、先に立ち上がった場合、スレーブ未接続と判断されるため
	// スレーブが立ち上がる時間分を待ってから起動
	dly_tsk(1000);

	// コマンドタスク初期化
	CmdTskInit();

	// モーションネット初期化
	MnetStatus = MntInit();

	// コマンドタスク初期動作
	if( MnetStatus != E_SUCCESS )
	{
		CmdTskInitAct();
	}
	
	for( ;; )
    {
        // コマンド待ち
        err = rcv_mbx( ID_MBX_CMD, (T_MSG**)&msg);

    	// メッセージ処理
    	CmdProc(msg);

    	RelMpf( ID_MPF_CMD, (VP)msg );// メモリ解放
    }
}
/***********************************************************************
* 
* [概要]	実行タスク初期化
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void CmdTskInit()
{
	INT Loop;

	// データクリア
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

	// 7SEG待機表示
	*( (UB *)LED_ADDR ) = CLR;

}
/***********************************************************************
* 
* [概要]	コマンドタスク初期動作
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
static void CmdTskInitAct()
{
	INT Loop;
    ST_MOTOR_PARAM	mp;

	// 初期速度設定
	mp.accel_type = 1;												// 駆動タイプ(０：台形、１：Ｓ字)
	mp.factor     = GetFactor(3);									// モータ倍率
	mp.FL         = 300;											// 初速[pls](0～100,000)
	mp.FH         = 1000;											// 最高速[pls](0～100,000)
	mp.utime      = 500;											// 加速時間[ms](１～65535)
	mp.dtime      = 500;											// 減速時間[ms](１～65535)
	mp.us         = 500;											// 加速時Ｓ字範囲
	mp.ds         = 500;											// 減速時Ｓ字範囲
	
	for( Loop = DVMIN; Loop <= DVMAX; Loop++ )
	{
		motor_SET( Loop, mp );
	}

}
/***********************************************************************
* 
* [概要]	タスク別解析処理
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	ＰＣ受信（ＲＸ０）タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void Rx0RcvMsg( T_CMD_MSG* mrcv )
{
	UB			msgid;

	// メッセージＩＤ取得
	msgid = mrcv->msgdata.msgid;

	switch( msgid )
	{
	case ID_MSG_ACK:
		// 制御データ初期化
		memset( &stCtl,  CLR,  sizeof( stCtl ) );
		break;

	case ID_MSG_NAK:
		if( stCtl.rsp.rty > RTY_OVER ) 
		{
			// リトライオーバー
			// 制御データ初期化
			memset( &stCtl,  CLR,  sizeof( stCtl ) );
		}
		else
		{
			// 再送
			CmdSendResponse( &stCtl );
		}
		break;

	default:
		// 基本コマンド
		if( msgid < ID_MSG_BASE_CMD_MAX )
		{
			CmdAnalyze( mrcv );
		}
		else
		{
			// ありえない
		}
		break;
	}
}

/***********************************************************************
* 
* [概要]	ＰＣ送信（ＴＸ０）タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void Tx0RcvMsg( T_CMD_MSG* mrcv )
{
}

/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void AlmRcvMsg( T_CMD_MSG* mrcv )
{
}

/***********************************************************************
* 
* [概要]	ＥＥＰＲＯＭ管理タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
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
			if( (inf2 & EEPROM_SPED_PTRN) )	// 速度情報（Page2～Page4）
			{
				eep_GetSpeedData( &Sp[MT_Y0][0].ac );
				eep_GetSpeedPattern( &SpPtn.Y0_SpdIniPtn );
			}
			if( (inf2 & EEPROM_POSI_INFO) )	// 位置情報（Page7～Page9）
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
				// UB Cnt; トータルステップ数（ 1 ～ 80 ）
				// UB Prc; 工程有無
				// UB Stp; 処理ステップ
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
				               inf2           // inf2(プロトコル番号)
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,                                         // 
							PARAM2, 										// パラメータ数
							STRTBL_MSG[msgid], 								// コマンド文字列
							E_SUCCESS,										// エラーコード

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
		CmdSendResponse( &stCtl );											// 応答送信
	}

}

/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void IntRcvMsg( T_CMD_MSG* mrcv )
{
	ER result = E_SUCCESS;
	UB Loopd;
	UB intbuf[8];
	INT TargetDevice[2];
	INT CompareDevice[2];
	
	// 今回停止させたデバイスが関連するBLKCMDがある場合にBLKCMD管理テーブルの登録から外す
	memcpy( &intbuf[0], &mrcv->msgdata.data[0], 8 );
	
	// 上位
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
* [概要]	タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
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
			
			// 応答メッセージ作成
			CmdMakeResponse(	&stCtl,
								PARAM12, 								// パラメータ数
								STRTBL_MSG[msg], 						// コマンド文字列
								result, 								// エラーコード

								mm1,									// モータ１番号
								sts.pos, 								// パルスカウント１
								sts.enc,								// エンコーダ１
								sts.cw_limit, 							// CWリミットセンサ(0:OFF,1:ON)
								sts.ccw_limit, 							// CCWリミットセンサ(0:OFF,1:ON)

								mm2, 									// モータ２番号
								sts2.pos, 								// パルスカウント２
								sts2.enc,								// エンコーダ２
								sts2.cw_limit, 							// CWリミットセンサ(0:OFF,1:ON)
								sts2.ccw_limit							// CCWリミットセンサ(0:OFF,1:ON)
							);
			break;


		case ID_MSG_ABORT:
		case ID_MSG_STOP:
			// 今回停止させたデバイスが関連するＢＣがある場合にＢＣ管理テーブルの登録から外す

			// 対象デバイス初期化
			memset( &TargetDevice[0], CLR, sizeof( TargetDevice ) );

			// 対象デバイス取得
			BitSet_INT( mm1, &TargetDevice[0] );

			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

		default:
			// レスポンスデータ取得
			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], stCtl.rsp.siz );
			break;
		}
		
		// 応答送信
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
* [概要]	拡張シリアル受信タスクからのメッセージ受信処理
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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

			// 応答メッセージ作成
			CmdMakeResponsePsns( msg,                                           // message
								STRTBL_MSG[msg], 								// コマンド文字列
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
// 本メッセージは、仕様上受信ないのでコメント
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
		
			// 応答メッセージ作成
			CmdMakeResponse( 	&stCtl,                                         // 
								PARAM2, 										// パラメータ数
								STRTBL_MSG[msg], 								// コマンド文字列
								result,											// エラーコード

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
		// 応答メッセージ作成
//		CmdMakeResponse( 	&msgdat,
//							PARAM7, 									// パラメータ数
//							STRTBL_MSG[msg], 							// コマンド文字列
//							result, 									// エラーコード
//
//							(INT)mm,									// モータ１番号
//							sts.pos, 									// パルスカウント１
//							sts.enc,									// エンコーダ１
//							sts.cw_limit, 								// CWリミットセンサ(0:OFF,1:ON)
//							sts.ccw_limit, 								// CCWリミットセンサ(0:OFF,1:ON)
//
//							0, 											// モータ２番号
//							0, 											// パルスカウント２
//							0,											// エンコーダ２
//							0, 											// CWリミットセンサ(0:OFF,1:ON)
//							0			 								// CCWリミットセンサ(0:OFF,1:ON)
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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
//			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], stCtl.rsp.siz );	// レスポンスデータ取得
			stCtl.rsp.siz = mrcv->msgdata.siz;
			memcpy( &stCtl.rsp.buf[0], &mrcv->msgdata.data[0], mrcv->msgdata.siz );	// レスポンスデータ取得
			CmdSendResponse( &stCtl );											// 応答メッセージ送信
			break;


		default:
			break;
	}
}
/***********************************************************************
* 
* [概要]	コマンドリクエスト受信監視用アラームハンドラ
* [引数]	
* [戻値]	－
* 
***********************************************************************/
void AlmComR0Ack( VP_INT exinf )
{
   int save_i;
	exinf = 0;

#if 0// 2022.03.02
	stp_alm( ID_ALM_R0ACK );												// 
#endif
    // 割込み禁止処理
    save_i = get_imask();
 	set_imask(0x0f);

	if( stCtl.rsp.rty > RTY_OVER ) 
	{
		// リトライオーバー
		
		
		// 制御データ初期化
		memset( &stCtl,  CLR,  sizeof( stCtl ) );
	}
	else
	{
		// 再送
		CmdSendResponse( &stCtl );
	}
	set_imask(save_i);
}
/***********************************************************************
* 
* [概要]	バーコードレスポンスタイムアウト
*           一定時間経過してもバーコードからレスポンスがない場合のタイム
*           アウト処理
* [引数]	
* [戻値]	－
* 
***********************************************************************/
void AlmCom2RspTmo( VP_INT exinf )
{
//    RelMpf( ID_MPF_CMD, (VP)msg );// メモリ解放
}

/***********************************************************************
* 
* [概要]	コマンドの解析
* [引数]	data:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static void CmdAnalyze( T_CMD_MSG* mrcv )
{
	UB			msgid;
	UB			anum;

	// メッセージＩＤ取得
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
		// ImpExpRcvDataは、受信処理で既に大文字しているので不要。
		anum = SplitOnly( &ImpExpRcvData[0], RecvArgs );
	}
	else
	{
		memcpy( &RecvData[0],     &mrcv->msgdata.data[0],CMD_BUFSIZ );		// コマンド電文コピー
		ToUpper( (char*)&RecvData );										// 大文字
		anum = Split( &RecvData[0], RecvArgs );								// 分解 ＆ パラメータ数取得
	}
	
	// コールファンクション
	(*CmdFuncTbl[msgid])( mrcv, anum, RecvArgs  );
}

/***********************************************************************
* 
* [概要]	動作終了通知
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void CmdEndResponse( ID txkid, UB prid1, UB bcid, UB msgid, UB inf1, UB inf2, ST_CNTLDATA* cntl )
{
	mbx_send	(
					txkid,												// ﾀｽｸID
					ID_MPF_CMD,											// ﾌﾟｰﾙID
					ID_MBX_CMD,											// MBOXID
					prid1,												// PROCID
					bcid,												// BCID
					msgid,												// ﾒｯｾｰｼﾞID
					inf1,												// 情報1
					inf2,												// 情報2
					&cntl->rsp.buf[0],									// DATA
					cntl->rsp.siz										// DATAサイズ
				);
}
/***********************************************************************
* 
* [概要]	コマンド応答の送信
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void CmdSendResponse( ST_CNTLDATA* cntl )
{
	// レスポンス送信
	SendResponse( cntl );
	cntl->rsp.rty++;
}
/***********************************************************************
* 
* [概要]	レスポンス送信
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void SendResponse( ST_CNTLDATA* cntl )
{
	//( ﾀｽｸID, ﾒﾓﾘﾌﾟｰﾙID, ﾒｯｾｰｼﾞBOXID,ﾒｯｾｰｼﾞID, 状態, DATA, DATAサイズ)
	mbx_send	(
					ID_TSK_CMD,               // ID tskid,    ﾀｽｸID
					ID_MPF_TX0,               // ID mpfid,    ﾌﾟｰﾙID
					ID_MBX_TX0,               // ID mbxid,    MBOXID
					NO_DATA,                  // UB prid1,    PROCID
					NO_DATA,                  // UB bcid,     BCID
					ID_MSG_CMD,               // UB msgid,    ﾒｯｾｰｼﾞID
					0,                        // UB sts,      情報1
					0,                        // UB sts2,     情報2
					&cntl->rsp.buf[0],        // UB* data,    DATA
					cntl->rsp.siz             // INT siz      DATAサイズ
				);
}
/***********************************************************************
* 
* [概要]	コマンド応答の送信
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void CmdSendResponseEtc( ST_CNTLDATA* cntl, UB msgid )
{
	//( ﾀｽｸID, ﾒﾓﾘﾌﾟｰﾙID, ﾒｯｾｰｼﾞBOXID,ﾒｯｾｰｼﾞID, 状態, DATA, DATAサイズ)
	mbx_send	(
					ID_TSK_CMD,               // ID tskid,    ﾀｽｸID
					ID_MPF_TX0,               // ID mpfid,    ﾌﾟｰﾙID
					ID_MBX_TX0,               // ID mbxid,    MBOXID
					NO_DATA,                  // UB prid1,    PROCID
					NO_DATA,                  // UB bcid,     BCID
					msgid,                    // UB msgid,    ﾒｯｾｰｼﾞID
					0,                        // UB sts,      情報1
					0,                        // UB sts2,     情報2
					&cntl->rsp.buf[0],        // UB* data,    DATA
					cntl->rsp.siz             // INT siz      DATAサイズ
				);
	
	cntl->rsp.rty++;
}

/***********************************************************************
* 
* [概要]	コマンド応答の送信
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void CmdSendResponse2( ST_CNTLDATA* cntl )
{
	// レスポンス送信
	SendResponse2( cntl );
	cntl->rsp.rty++;
}
/***********************************************************************
* 
* [概要]	レスポンス送信
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void SendResponse2( ST_CNTLDATA* cntl )
{
	//( ﾀｽｸID, ﾒﾓﾘﾌﾟｰﾙID, ﾒｯｾｰｼﾞBOXID,ﾒｯｾｰｼﾞID, 状態, DATA, DATAサイズ)
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
* [概要]	応答データ作成
* [引数]	
* [戻値]	－
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
	memcpy( &psnsdat[1], &rspbuf2, len );		// コマンド～データまでコピー
	psnsdat[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( (UB*)&psnsdat[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	psnsdat[(len+2)] = bcch;					// BCC1
	psnsdat[(len+3)] = bccl;					// BCC2
	
	
	mbx_send	(
				ID_TSK_CMD,               // ID tskid,    ﾀｽｸID
				ID_MPF_TX0,               // ID mpfid,    ﾌﾟｰﾙID
				ID_MBX_TX0,               // ID mbxid,    MBOXID
				NO_DATA,                  // UB prid1,    PROCID
				NO_DATA,                  // UB bcid,     BCID
				msgid,               // UB msgid,    ﾒｯｾｰｼﾞID
				0,                        // UB sts,      情報1
				0,                        // UB sts2,     情報2
				&dmyadr,                  // UB* data,    DATA
				1                         // INT siz      DATAサイズ
			);
}
/***********************************************************************
* 
* [概要]	応答データ作成
* [引数]	
* [戻値]	－
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
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );			// コマンド～データまでコピー
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2の4byte分をレングスに加算
}
/***********************************************************************
* 
* [概要]	応答データ作成
* [引数]	
* [戻値]	－
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
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// コマンド～データまでコピー
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2の4byte分をレングスに加算
}
/***********************************************************************
* 
* [概要]	応答データ作成
* [引数]	
* [戻値]	－
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
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// コマンド～データまでコピー
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2の4byte分をレングスに加算
}
/***********************************************************************
* 
* [概要]	応答データ作成(入力カウンタ読み出し用)
* [引数]	
* [戻値]	－
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
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// コマンド～データまでコピー
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2の4byte分をレングスに加算
}
/***********************************************************************
* 
* [概要]	応答データ作成
* [引数]	
* [戻値]	－
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
	memcpy( &cntl->rsp.buf[1], &rspbuf, len );		// コマンド～データまでコピー
	cntl->rsp.buf[(len+1)] = ETX;					// ETX

	// BCC計算
	bcc = CalcBCC( &cntl->rsp.buf[1], (len+1));

	// 10進数→16進数→ASC変換
	DectoHexASC( bcc, &bcch, &bccl );

	cntl->rsp.buf[(len+2)] = bcch;					// BCC1
	cntl->rsp.buf[(len+3)] = bccl;					// BCC2
	cntl->rsp.siz          = len+4;					// stx,etx,bcc1,bcc2の4byte分をレングスに加算
}
/***********************************************************************
* 
* [概要]	受信パラメータチェック
* [引数]	msg:コマンドＩＤ、anum:パラメータ数、args:パラメータデータアドレス
* [戻値]	状態
* 
***********************************************************************/
static ER ParamDataCheck( UB msg , UB anum ,  const char** args )
{
	INT ParDat;
	INT Loop;
	INT argnum;

	// パラメータ過不足チェック
	if( anum < DATCHK_TBL[msg].argnum )		return( E_PAR_SHORTG );		// パラメータ数過不足

	// パラメータテーブル定義分の数値チェック
	argnum = DATCHK_TBL[msg].argnum;

	// コマンド以外のパラメータチェック
	if( argnum > 1 )
	{
		// anumは、コマンド分も含まれたパラメータ数なので、コマンド以外のパラメータの上下限についてチェック
		for( Loop = PARAM1; Loop < argnum; Loop++ )
		{
			if( 
				( DATCHK_TBL[msg].range[Loop].min != CLR )||
				( DATCHK_TBL[msg].range[Loop].max != CLR )
			)
			{
				// パラメータ範囲チェック
				ParDat = Atoi( args[Loop], N_DEC, sizeof(INT), N_SIG );
				if( 
					( ParDat < DATCHK_TBL[msg].range[Loop].min )||
					( ParDat > DATCHK_TBL[msg].range[Loop].max )
				)
				{
					return( ( E_PAR_OUTRNG + Loop ));					// パラメータ範囲異常
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
/*  基本コマンド   ここから                                                                                                                                                                                     基本コマンド   */
/*=============================================================================================================================================================================================================================*/

/***********************************************************************
* 
* [概要]	ダミールーチン
* [引数]	cmd:コマンドリクエスト電文
* [戻値]	－
* 
***********************************************************************/
static void CmdDummy( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// 必要であればエラー出力
}


static	UB temp1;
static	UB temp2;

/***********************************************************************
*
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdVini   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  データチェック
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPini   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  データチェック
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPren   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  データチェック
	result = ParamDataCheck( ID_MSG_PRESSREN_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPtan   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  データチェック
	result = ParamDataCheck( ID_MSG_PRESSTAN_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPcan   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB dumy = CLR;
	
	//  データチェック
	result = ParamDataCheck( ID_MSG_PRESSCAN_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdVout  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;

	//  データチェック
	result = ParamDataCheck( ID_MSG_VALVEOUT_HS, anum, args );

	//  データ正常か
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
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPsns  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	dummy  = CLR;
	
	result = ParamDataCheck( ID_MSG_PRESSURE_HS, anum, args );

	//  データ正常か
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
* [概要]  HELLO   起動通知
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdHELLO  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
 	// 応答メッセージ作成
	CmdMakeResponseStr(		&stCtl,
							PARAM4,
							STRTBL_MSG[ID_MSG_HELLO],
							MnetStatus,
							(char*)&FIRMWARE,
							(char*)&VER_INFO
						);

	// レスポンスデータ送信
	SendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  VER     バージョン確認
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdVER    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// 応答メッセージ作成
	CmdMakeResponseStr(		&stCtl,
							PARAM4, 
							args[0], 
							MnetStatus,
							(char*)&FIRMWARE, 
							(char*)&VER_INFO
						);

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  ALLIN   全I/O入力
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdALLIN  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	allsts[INDAT_CNTRL_MAX];
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  初期化
	memset( &allsts, CLR,  sizeof( allsts ) );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// 全入力ポート状態取得
		pio_ALLIN( &allsts[0] );
	}
	// 応答メッセージ作成
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

	// 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  BIN     I/O入力
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdBIN    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	sts  = CLR;
	INT	par  = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		par = Atoi( args[PARAM1], N_DEC, sizeof(INT), N_SIG );
	
		// 入力状態取得
	    sts = pio_BIN( par );	
	}

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM4, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						par, 											//3  入力ｃｈ
						sts, 											//4  入力状態取得
						0,												//5  
						0, 												//6  
						0, 												//7  

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);


    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  BOUT    I/O出力
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdBOUT   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	INT	ch  = CLR;
	INT	act = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		ch  = Atoi( args[PARAM1], N_DEC, sizeof(INT), N_SIG );
		act = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );

		// I/O出力
		pio_BOUT( ch, act );
	}
	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM6, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						ch, 											//3  入力ｃｈ
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

	
    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  CINIT   入力カウンタの設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdCINIT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	nn  = CLR;
	UB	f   = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		nn  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		f   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );

		// カウント開始
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
		// カウント停止
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
	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						nn, 											//3  入力ｃｈ
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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}
/***********************************************************************
*
* [概要]  COUNT   入力カウンタの読み出し
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdCOUNT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER	result = E_SUCCESS;
	UB	nn  = CLR;
	UINT	cccc = CLR;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		nn  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		
		// カウント取得
		cccc = GetCount(nn);
	}
	// 応答メッセージ作成
	CmdMakeResponseCount( 	&stCtl,
							PARAM4, 									//0  パラメータ数
							args[0], 									//1  コマンド文字列
							result,										//2  エラーコード

							nn, 										//3  入力ｃｈ
							cccc 										//4  カウント値
						);

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  INT     割り込み設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdINT    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	ST_INTINF			Intinf;							// 
	UB  msg;

	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &bufwk,  CLR, sizeof( bufwk ) );
	memset( &Intinf, CLR, sizeof( ST_INTINF ) );

	//  データチェック
	result = ParamDataCheck( msg, anum, args );
	
	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		Intinf.nn   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		Intinf.trg  = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		
		// 一時停止するモータ指定
		SetByteArray( args[PARAM3],&Intinf.mmm[0]);
		
		// 出力チャネルへ出力する値
		SetByteArray( args[PARAM4],&Intinf.dat[0]);

		// 割り込み発生時に出力するチャネル指定
		SetByteArray( args[PARAM5],&Intinf.msk[0]);

		// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
		memcpy( &bufwk[0], (B*)&Intinf, sizeof( ST_INTINF ) );

		// 待ちタスクへ通知
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

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						Intinf.nn,										//3  入力ｃｈ
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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  ABSMV   パルスモータ移動(絶対位置)
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdABSMV  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_ABSMV absmv;
	UB  msg;

	msg = mrcv->msgdata.msgid;
	
	// 初期化
	memset( &bufwk, 0, sizeof( bufwk ) );
	memset( &absmv,  0,  sizeof( ST_ABSMV ) );

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		absmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		absmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		absmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, absmv.mm );

		if( result == E_SUCCESS )
		{
			result = AbsmvAction( NO_DATA, NO_DATA, absmv.mm, absmv.st, absmv.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  パラメータ数
							args[0], 									//1  コマンド文字列
							result,										//2  エラーコード

							(INT)absmv.mm,								//3  モータ１番号
							0, 											//4  パルスカウント１
							0,											//5  エンコーダ１
							0, 											//6  CWリミットセンサ(0:OFF,1:ON)
							0, 											//7  CCWリミットセンサ(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  ABORT   パルスモータ即停止
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdABORT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_STOP abort;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		abort.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
		memcpy( &bufwk[0], (B*)&abort, sizeof( ST_STOP ) );

		// 既に励磁ONの場合は、実行タスクへ
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 										//0  パラメータ数
							args[0], 										//1  コマンド文字列
							result,											//2  エラーコード

							0,												//3  モータ１番号
							0, 												//4  パルスカウント１
							0,												//5  エンコーダ１
							0, 												//6  CWリミットセンサ(0:OFF,1:ON)
							0, 												//7  CCWリミットセンサ(0:OFF,1:ON)

							0, 												//8  
							0, 												//9  
							0,												//10 
							0, 												//11 
							0 												//12 
						);

		// 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  ENCODER エンコーダの設定
* [引数]  メッセージ、引数の数、ｺﾏﾝﾄﾞ電文
* [戻値]  エラー
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

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		ec   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		dtyp = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );
		dv   = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );
		so   = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );
		
		result = motor_ENCODER( mm, ec, dtyp, dv, so );
	}

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						mm, 											//3  モータ番号
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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  LIMIT   パルスモータ原点復帰
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdLIMIT  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER       result = E_SUCCESS;
	ST_LIMIT limit;
	UB       msg;

	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &bufwk, 0, sizeof( bufwk ) );
	memset( &limit,  0,  sizeof( ST_LIMIT ) );

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		limit.mm  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		limit.dr = GetCwInfo( args[PARAM2 ] );						// 移動方向		CW : 正転方向, CCW : 逆転方向
		limit.sp = Atoi( args[PARAM3], N_DEC, sizeof(INT), N_SIG );
		limit.md = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, limit.mm );

		if( result == E_SUCCESS )
		{
			// デバイス動作中にセット
			result = LimitAction(	
							NO_DATA,
							NO_DATA,
							limit.mm,
							limit.dr, 
							GENTEN_POS,
							limit.sp
						);												// リミット位置動作
		}
	}

	if( result != E_SUCCESS )
	{
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM3, 									//0  パラメータ数
							args[0], 									//1  コマンド文字列
							result,										//2  エラーコード

							(INT)limit.mm,								//3  モータ１番号
							0, 											//4  パルスカウント１
							0,											//5  エンコーダ１
							0, 											//6  CWリミットセンサ(0:OFF,1:ON)
							0, 											//7  CCWリミットセンサ(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}
/***********************************************************************
*
* [概要]  MAP     ２軸のパルスモータ移動
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
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
	
	// 初期化
	memset( &bufwk,  CLR, sizeof( bufwk ) );
	memset( &sts1,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &sts2,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &absmv,  CLR, sizeof( ST_ABSMV ) );
	memset( &absmv2, CLR, sizeof( ST_ABSMV ) );
	
	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		absmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		absmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		absmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// パラメータ取得
		absmv2.mm   = Atoi( args[PARAM4], N_DEC, sizeof(UB), N_SIG );
		absmv2.pppp = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );
		absmv2.st   = Atoi( args[PARAM6], N_DEC, sizeof(UB), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, absmv.mm );

		if( result == E_SUCCESS )
		{
			result = MapAction( NO_DATA, NO_DATA, absmv.mm, absmv2.mm, absmv.pppp, absmv2.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM12, 									//0  パラメータ数
							args[0], 									//1  コマンド文字列
							result, 									//2  エラーコード

							absmv.mm,									//3  モータ１番号
							sts1.pos, 									//4  パルスカウント
							sts1.enc,									//5  エンコーダ
							sts1.cw_limit, 								//6  CWリミットセンサ(0:OFF,1:ON)
							sts1.ccw_limit, 							//7  CCWリミットセンサ(0:OFF,1:ON)

							absmv2.mm,									//8  モータ２番号
							sts2.pos, 									//9  パルスカウント
							sts2.enc,									//10 エンコーダ
							sts2.cw_limit, 								//11 CWリミットセンサ(0:OFF,1:ON)
							sts2.ccw_limit 								//12 CCWリミットセンサ(0:OFF,1:ON)
						);

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  PDELAY  パルスモータのパワー制御待ち時間設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPDELAY ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_PDELAY	pdelay;
	UB msg;

	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &bufwk, 0, sizeof( bufwk ) );
	
	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		pdelay.mm    = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		pdelay.tttt  = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, pdelay.mm );

		if( result == E_SUCCESS )
		{
			// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
			memcpy( &bufwk[0], (B*)&pdelay, sizeof( ST_PDELAY ) );

			// 待ちタスクへ通知
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

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						(INT)pdelay.mm,									//3  モータ１番号
						0, 												//4  パルスカウント１
						0,												//5  エンコーダ１
						0, 												//6  CWリミットセンサ(0:OFF,1:ON)
						0, 												//7  CCWリミットセンサ(0:OFF,1:ON)

						0, 												//8  
						0, 												//9  
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  POWER   パルスモータの励磁制御
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdPOWER  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	UB mm     = CLR;
	UB sw;
	UB msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		sw   = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// 励磁
			result = motor_POWER( mm, sw );

			// POWER状態登録
			power[mm].mm = mm;
			power[mm].sw = sw;
		}
	}

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						(INT)mm, 										//3  モータ１番号
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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}

/***********************************************************************
*
* [概要]  PSET    パルスモータのカウンタリセット
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
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

	
	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		cnt  = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		enc  = Atoi( args[PARAM3], N_DEC, sizeof(INT), N_SIG );
		def  = Atoi( args[PARAM4], N_DEC, sizeof(INT), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// パルスモータのカウンタリセット
			result = motor_PSET( mm, cnt, enc, def );
		}
	}

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM4, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						(INT)mm, 										//3  モータ１番号
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
	
    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}
/***********************************************************************
*
* [概要]  RELMV   パルスモータ移動(相対位置)
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdRELMV  ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ST_RELMV relmv;
	ER result = E_SUCCESS;
	UB msg;

	msg = mrcv->msgdata.msgid;
	
	// 初期化
	memset( &bufwk,CLR, sizeof( bufwk ) );
	memset( &relmv,CLR, sizeof( ST_RELMV ) );

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		relmv.mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		relmv.pppp = Atoi( args[PARAM2], N_DEC, sizeof(INT), N_SIG );
		relmv.st   = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, relmv.mm );

		if( result == E_SUCCESS )
		{
			result = RelmvAction( NO_DATA, NO_DATA, relmv.mm, relmv.st, relmv.pppp );
		}
	}

	if( result != E_SUCCESS )
	{
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  パラメータ数
							args[0], 									//1  コマンド文字列
							result,										//2  エラーコード

							(INT)relmv.mm,								//3  モータ１番号
							0, 											//4  パルスカウント
							0,											//5  エンコーダ
							0, 											//6  CWリミットセンサ(0:OFF,1:ON)
							0, 											//7  CCWリミットセンサ(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  SET     パルスモータ速度設定
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdSET    ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB		mm     = CLR;		// モータ番号
    ST_MOTOR_PARAM		mp;		// 
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得   sample: SET,1,1,3,500,7500,500,500
		mm            = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		mp.accel_type = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );	// 駆動タイプ(０：台形、１：Ｓ字)
		// デバイス:PCD4611
		if( MNET_DRV_TYPE[mm] == PCD4611 )
		{
			mp.factor     = GetFactorPCD4611( Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG ) );    // モータ倍率
		}
		else
		{
			mp.factor     = GetFactor( Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG ) );    // モータ倍率
		}
		mp.FL         = Atoi( args[PARAM4], N_DEC, sizeof(INT), N_SIG );// 初速[pls](0～100,000)
		mp.FH         = Atoi( args[PARAM5], N_DEC, sizeof(INT), N_SIG );// 最高速[pls](0～100,000)
		mp.utime      = Atoi( args[PARAM6], N_DEC, sizeof(INT), N_SIG );// 加速時間[ms](１～65535)
		mp.dtime      = mp.utime;                                       // 減速時間[ms](１～65535)
		mp.us         = Atoi( args[PARAM7], N_DEC, sizeof(INT), N_SIG );// 加速時Ｓ字範囲
		mp.ds         = mp.us;                                          // 減速時Ｓ字範囲

		// デバイス動作状態確認
		result = GetDeviceActionChk( msg, NO_DATA, mm );

		if( result == E_SUCCESS )
		{
			// 設定
			motor_SET( mm, mp );
		}
	}

	CmdMakeResponse( 	&stCtl,
						PARAM3, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						mm, 											//3  モータ１番号
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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );

}

/***********************************************************************
*
* [概要]  STATUS  パルスモータ状態読み出し
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdSTATUS ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;	// エラー情報
	UB		mm     = CLR;		// モータ番号
	ST_MOTOR_STATUS sts;		// モータ状態
	UB  msg;

	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &sts,  CLR,  sizeof( ST_MOTOR_STATUS ) );


	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{

		// パラメータ取得
		mm   = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// 状態取得
	    result  = motor_STATUS( mm, &sts );
	}

	// 応答メッセージ作成
	CmdMakeResponse( 	&stCtl,
						PARAM9, 										//0  パラメータ数
						args[0], 										//1  コマンド文字列
						result,											//2  エラーコード

						mm,	 											//3  モータ１番号
						(INT)sts.busy, 									//4  動作状態
						sts.pos,										//5  パルスカウンタ値
						sts.enc, 										//6  エンコーダ値
						sts.dev, 										//7  偏差カウンタ値

						sts.cw_limit, 									//8  CWリミットセンサ値(0:OFF,1:ON)
						sts.ccw_limit, 									//9  CCWリミットセンサ値(0:OFF,1:ON)
						0,												//10 
						0, 												//11 
						0 												//12 
					);

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );
}


/***********************************************************************
*
* [概要]  STOP    パルスモータ減速停止
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdSTOP   ( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER result = E_SUCCESS;
	ST_MOTOR_STATUS sts;
	ST_STOP stop;
	UB msg;

	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &sts,   CLR, sizeof( ST_MOTOR_STATUS ) );
	memset( &bufwk, CLR, sizeof( bufwk ) );
	memset( &stop,  CLR, sizeof( ST_STOP ) );

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		stop.mm  = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );

		// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
		memcpy( &bufwk[0], (B*)&stop, sizeof( ST_STOP ) );

		// 既に励磁ONの場合は、実行タスクへ
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

		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM7, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_STOP], 					//1  コマンド文字列
							result,										//2  エラーコード

							(INT)stop.mm,								//3  モータ１番号
							sts.pos, 									//4  パルスカウント１
							sts.pos,									//5  エンコーダ１
							sts.cw_limit, 								//6  CWリミットセンサ(0:OFF,1:ON)
							sts.ccw_limit, 								//7  CCWリミットセンサ(0:OFF,1:ON)

							0, 											//8  
							0, 											//9  
							0,											//10 
							0, 											//11 
							0 											//12 
						);

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );

		ClrDevice( stop.mm );
	}
}

/***********************************************************************
*
* [概要]  
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdBZCNT( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER  result = E_SUCCESS;

	ST_BZCNT bzcnt;
	UB  msg;

	msg = mrcv->msgdata.msgid;

	//  データチェック
	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
	if( result == E_SUCCESS )
	{
		// パラメータ取得
		bzcnt.Act    = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		bzcnt.Colptn = Atoi( args[PARAM2], N_DEC, sizeof(UB), N_SIG );
		bzcnt.Buzptn = Atoi( args[PARAM3], N_DEC, sizeof(UB), N_SIG );

		// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
		memcpy( &bufwk[0], (B*)&bzcnt, sizeof( ST_BZCNT ) );

		// タスク通知
		mbx_send(
						ID_TSK_CMD,										// 送信元ﾀｽｸID
						ID_MPF_LED,										// ﾒﾓﾘﾌﾟｰﾙID
						ID_MBX_LED,										// ﾒｰﾙﾎﾞｯｸｽID
						ID_TPL_PTLMP,									// PROCID
						mrcv->msgdata.bcid,								// BCID
						ID_MSG_CBZCNT,									// message
						NO_DATA,										// 情報1(汎用)
						NO_DATA,										// 情報2(汎用)
						&bufwk[0],										// ﾃﾞｰﾀ
						sizeof( bufwk )									// ﾃﾞｰﾀｻｲｽﾞ
					);
		
	}

	CmdMakeResponse( 	&stCtl,
						PARAM2, 										//0  パラメータ数
						STRTBL_MSG[ID_MSG_CBZCNT], 						//1  コマンド文字列
						result,											//2  エラーコード

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

    // 応答メッセージ送信
	CmdSendResponse( &stCtl );

}

/***********************************************************************
*
* [概要]  EEPROM 指定ページの１ページ(128byte)データを読み込む
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdEEPREAD( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB  msg;
	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &eepb,  0xff, sizeof( ST_EEPBUF ) );

	//  データチェック
//	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
//	if( result == E_SUCCESS )
	{
		// パラメータ取得
		eepb.no = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		eepb.siz = EEP_RWSIZE;
		
		// 書込みデータ配列化
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_EEPREAD], 				//1  コマンド文字列
							result,										//2  エラーコード

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

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  EEPROM 指定ページに１ページ(128byte)データを書き込む
* [引数]  ｺﾏﾝﾄﾞ電文引数ﾃﾞｰﾀ
* [戻値]  -
* 
***********************************************************************/
static void CmdEEPWRIT( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	ER		result = E_SUCCESS;
	UB  msg;
	msg = mrcv->msgdata.msgid;

	// 初期化
	memset( &eepb,  0xff, sizeof( ST_EEPBUF ) );

	//  データチェック
//	result = ParamDataCheck( msg, anum, args );

	//  データ正常か
//	if( result == E_SUCCESS )
	{
		// パラメータ取得
		eepb.no = Atoi( args[PARAM1], N_DEC, sizeof(UB), N_SIG );
		eepb.siz = EEP_RWSIZE;
		
		// 書込みデータ配列化
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_EEPWRIT], 				//1  コマンド文字列
							result,										//2  エラーコード

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

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
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

	// タッチパネルへそのままデータ通知
	TplRpDataSend();
	TplRidDataSend ( &tp.rsp.buf[0] , ID_TPL_PRINF );

	// EEPへ書込み指示
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_IMPPDAT], 				//1  コマンド文字列
							result,										//2  エラーコード

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

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}

/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_IMPSPED], 				//1  コマンド文字列
							result,										//2  エラーコード

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

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}


/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
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
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,
							PARAM2, 									//0  パラメータ数
							STRTBL_MSG[ID_MSG_IMPPOSI], 				//1  コマンド文字列
							result,										//2  エラーコード

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

	    // 応答メッセージ送信
		CmdSendResponse( &stCtl );
	}
}


/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
* 
***********************************************************************/
static void CmdImpOptn( T_CMD_MSG* mrcv, UB anum, const char** args )
{
}


/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
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
* [概要]  
* [引数]  
* [戻値]  -
* 
***********************************************************************/
static void CmdExpComl( T_CMD_MSG* mrcv, UB anum, const char** args )
{
}


/***********************************************************************
*
* [概要]  
* [引数]  
* [戻値]  -
* 
***********************************************************************/
static void CmdExpClog( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	// タッチパネルへそのままデータ通知
	TplRpDataSend();
	TplRidDataSend ( &tp.rsp.buf[0] , ID_TPL_PRINF );
	
	
	
	
	
}






/*=============================================================================================================================================================================================================================*/
/*  基本コマンド   ここまで                                                                                                                                                                                     基本コマンド   */
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
* [概要]  デバイス動作中セット
* [引数]  
* [戻値]  -
* 
***********************************************************************/
static void SetDevice( UB DevNo )
{
	// 未登録・規定外
	if( DevNo == NO_DATA )	return;
	if( DevNo > DVMAX )		return;

	
	return;//debug
	
	BitSet_INT( DevNo, &DeviceStatus[0] );
}

/***********************************************************************
*
* [概要]  デバイス動作状態取得
* [引数]  
* [戻値]  TRUE:動作中、FALSE:待機中
* 
***********************************************************************/
static BOOL GetDeviceStatus( UB DevNo )
{
	// 未登録や規定外は動作中扱いとする
	if( DevNo == NO_DATA )	return( TRUE );
	if( DevNo > DVMAX )		return( TRUE );

	return(( BitGet_INT( DevNo, &DeviceStatus[0] ) > 0 ? TRUE : FALSE ));
}

/***********************************************************************
*
* [概要]  デバイス動作中クリア
* [引数]  
* [戻値]  -
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
* [概要]	パルスモータを励磁
* [引数]	モーター番号、励磁動作指示
* [戻値]	TRUE:成功、FALSE:失敗
* 
* 
***********************************************************************/
static ER GetDeviceActionChk ( UB msg, UB bcid, UB mm )
{
	ER result = E_SUCCESS;
#if 0	// ★
	INT AsnDevice[2];
	INT CmpDevice[2];
	INT TargetDevice;
	UB devnowk;

	// **************************************************************
	// 現在、動作中のデバイスとかぶらないか確認
	// ※基本コマンドはbcctrlに登録されていないので動作中フラグで確認
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
* [概要]	パルスモータを励磁
* [引数]	モーター番号、励磁動作指示
* [戻値]	TRUE:成功、FALSE:失敗
* 
******************************************************************************/
BOOL GetPowerSts(UB pm)
{
	BOOL result;// port状態 H = TRUE、L = FALSE

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
* [概要]	リミット動作
* [引数]	id   : ﾌﾞﾛｯｸｺﾏﾝﾄﾞID
*           mm   : ﾃﾞﾊﾞｲｽNO
*           dr   : 方向
*           md   : 動作ﾓｰﾄﾞ
*           sp   : 速度
* [戻値]	エラーコード
* 
******************************************************************************/
static ER LimitAction( UB prid1, UB bcid, UB mm, UB dr, UB md, INT sp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_LIMIT lmt;

	// LIMITデータコピー
	lmt.mm = mm;
	lmt.dr = dr;
	lmt.md = md;
	lmt.sp = sp;
	
	memcpy( &bufwk[0], &lmt, sizeof( ST_LIMIT )); 
	
	// 励磁ＯＮ中であれば実行タスクへ通知
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// 励磁ＯＦＦであればウェイトタスクへ通知
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// メッセージ通知
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
* [概要]	相対位置移動動作
* [引数]	id   : ﾌﾞﾛｯｸｺﾏﾝﾄﾞID
*           mm   : ﾃﾞﾊﾞｲｽNO
*           pppp : mm移動量
* [戻値]	エラーコード
* 
******************************************************************************/
static ER RelmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_RELMV rel;

	// 基本コマンドはデバイス状態を確認
	if( bcid == NO_DATA )
	{
		if( GetDeviceStatus(mm) ==TRUE )
		{
			return( (E_BUSY + mm) );
		}
	}

	// RELMVデータコピー
	rel.mm = mm;
	rel.st = 0;
	rel.pppp = pppp;
	
	memcpy( &bufwk[0], &rel, sizeof( ST_RELMV )); 
	
	// 励磁ＯＮ中であれば実行タスクへ通知
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// 励磁ＯＦＦであればウェイトタスクへ通知
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// メッセージ通知
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
* [概要]	絶対位置移動動作
* [引数]	id   : ﾌﾞﾛｯｸｺﾏﾝﾄﾞID
*           mm   : ﾃﾞﾊﾞｲｽNO
*           pppp : mm移動量
* [戻値]	エラーコード
* 
******************************************************************************/
static ER AbsmvAction( UB prid1, UB bcid, UB mm, UB st, INT pppp )
{
	ER result = E_SUCCESS;
	ID mpfid;
	ID mbxid;
	ST_ABSMV abs;

	// 基本コマンドはデバイス状態を確認
	if( bcid == NO_DATA )
	{
		if( GetDeviceStatus(mm) ==TRUE )
		{
			return( (E_BUSY + mm) );
		}
	}

	// ABSMVデータコピー
	abs.mm = mm;
	abs.st = st;
	abs.pppp = pppp;
	
	memcpy( &bufwk[0], &abs, sizeof( ST_ABSMV )); 
	
	// 励磁ＯＮ中であれば実行タスクへ通知
	if(power[mm].sw == SET)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// 励磁ＯＦＦであればウェイトタスクへ通知
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// メッセージ通知
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
* [概要]	MAP移動動作
* [引数]	id   : ﾌﾞﾛｯｸｺﾏﾝﾄﾞID
*           mm1  : ﾃﾞﾊﾞｲｽNO
*           mm2  : ﾃﾞﾊﾞｲｽNO
*           pppp1: mm1移動量
*           pppp2: mm2移動量
* [戻値]	エラーコード
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
	
	// 基本コマンドはデバイス状態を確認
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

	// 初期化
	memset( &bufwk,  CLR, sizeof( bufwk ) );

    absmv.mm   = mm1; 
    absmv.st   = 0;
    absmv.pppp = pppp1;

	absmv2.mm   = mm2; 
    absmv2.st   = 0;
    absmv2.pppp = pppp2;

	// 励磁ＯＮ中であれば実行タスクへ通知
	if(
		(power[absmv.mm].sw == SET)&&
		(power[absmv2.mm].sw == SET)
	)
	{
		mpfid = ID_MPF_MOTACT;
		mbxid = ID_MBX_MOTACT;
	}
	// 励磁ＯＦＦであればウェイトタスクへ通知
	else
	{
		mpfid = ID_MPF_MOTWAIT;
		mbxid = ID_MBX_MOTWAIT;
	}

	// -------------------
	// １つ目のデバイス
	// -------------------
	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &bufwk[0], (B*)&absmv, sizeof( ST_ABSMV ) );
	
	// メッセージ通知
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
	// ２つ目のデバイス
	// -------------------
	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &bufwk[0], (B*)&absmv2, sizeof( ST_ABSMV ) );

	// メッセージ通知
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
	
	// エラーチェック(エラー時は、「1」側優先でログに出力)
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
* [概要]	ＷＡＩＴ動作
* [引数]	id   : ﾌﾞﾛｯｸｺﾏﾝﾄﾞID
*           mm   : ﾃﾞﾊﾞｲｽNO
*           pppp : mm移動量
* [戻値]	エラーコード
* 
******************************************************************************/
static ER WaitAction( UB prid1, UB bcid, INT waittim )
{
	ER result = E_SUCCESS;

	memcpy( &bufwk[0], &waittim, sizeof( INT )); 

	// メッセージ通知
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
* [概要]  ＢＣで利用されるデバイス情報の取得
* [引数]  msg:メッセージＩＤ，mm:デバイス番号，*data:格納先
* [戻値]  －
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
//        タッチパネル通信
//-------------------------------------------------------------------------------------------------------------------------

/***********************************************************************
* 
* [概要]	コマンドの解析
* [引数]	data:コマンドリクエスト電文
* [戻値]	－
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
	
	if( IsQgProcList( procid ) == FALSE )				// 重複なし
	{
		mrcv->msgdata.bcid = GetQgProcList( procid ) ;	// コマンドリスト空きindex取得
	}
	else
	{
		mrcv->msgdata.bcid = NO_DATA;					// 重複あり
	}
	
	if( procid == ID_TPL_EXRUN )						// 運転履歴受信
	{
		
		
		
	}
	else
	{
		memset( &TpRecvData,    CLR,  sizeof( TpRecvData ) );
		memset( &TpRecvArgs,    CLR,  sizeof( TpRecvArgs ) );

		// コマンド電文コピー
		memcpy( &TpRecvData[0],     &mrcv->msgdata.data[0],CMD_BUFSIZ );

		// 大文字
//	ToUpper( (char*)&TpRecvData );

		// 分解 ＆ パラメータ数取得
		anum = SplitSpace( &TpRecvData[0], TpRecvArgs );
	}

	// コールファンクション
	(*TpFuncTbl[procid])( mrcv, anum, TpRecvArgs  );
}


/***********************************************************************
* 	
* [概要]	
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDummy( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	








}

/***********************************************************************
* 	
* [概要]	01 FWバージョン情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplFwver( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );

	mrcv->msgdata.inf2 = (EEPROM_SPED_PTRN+EEPROM_POSI_INFO);

	// EEPROM データ取得指示
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
* [概要]	02 オプション情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	

	// 固定値
	tp.rsp.buf[8] = '0';
***********************************************************************/
static void TplOptif( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	memset( &bufwk[0],     CLR,  CMD_BUFSIZ );

	mrcv->msgdata.inf2 = CLR;

	// EEPROM データ取得指示
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
* [概要]	03 ログイン認証1
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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

	// EEPROM データ取得指示
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
* [概要]	04 ログイン認証2
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	05 初期化指示
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
	
//axsis =0x1f;// ★デバック
	
	
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
* [概要]	08 ユーザー情報全取得
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplUsral( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	

/***********************************************************************
* 	
* [概要]	11 ユーザー1情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	12 ユーザー2情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	13 ユーザー3情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	 14 ユーザー4情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	15 ユーザー5情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	16 ユーザー6情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	17 ユーザー7情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	18 ユーザー8情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	19 ユーザー9情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	1A ユーザー10情報
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplInf10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	21 ユーザー1登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	22 ユーザー2登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	23 ユーザー3登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	24 ユーザー4登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	25 ユーザー5登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	26 ユーザー6登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	27 ユーザー7登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	28 ユーザー8登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	29 ユーザー9登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	2A ユーザー10登録
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplRec10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	31 ユーザー1変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	32 ユーザー2変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	33 ユーザー3変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	34 ユーザー4変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	35 ユーザー5変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	36 ユーザー6変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	37 ユーザー7変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	38 ユーザー8変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	39 ユーザー9変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	3A ユーザー10変更
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplChg10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	41 ユーザー1削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel01( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	42 ユーザー2削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel02( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	43 ユーザー3削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel03( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	44 ユーザー4削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel04( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	45 ユーザー5削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel05( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	46 ユーザー6削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel06( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	47 ユーザー7削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel07( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	48 ユーザー8削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel08( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	49 ユーザー9削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel09( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	4A ユーザー10削除
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplDel10( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	50 FW状態（ポーリング）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplFwpol( T_CMD_MSG* mrcv, UB anum, const char** args )	
{
	TpRspFWPOL();
}

/***********************************************************************
* 	
* [概要]	51 プロトコル実行
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* [備考]
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
		// EEPROM データ取得指示
		mbx_send( ID_TSK_CMD,          // tskid
	               ID_MPF_EEP,         // mpfid
	               ID_MBX_EEP,         // mbxid
	               mrcv->msgdata.prid1,// prid1  タッチパネル指示
	               mrcv->msgdata.bcid, // bcid
	               ID_MSG_EEPREAD,     // msgid
	               mrcv->msgdata.inf,  // sts
	               mrcv->msgdata.inf2, // sts2   プロトコル番号
	               &bufwk[0],          // data   null
	               CMD_BUFSIZ          // siz
	             );

		// プロトコル応答
		TpRspPRCTL( E_SUCCESS );
	}
}

/***********************************************************************
* 	
* [概要]	52 リスタート実行
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplResta( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	53 一時停止
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	54 再開
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	55 中断
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	 58 オプションバーコード実行
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplOptcd( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	59 サンプルチューブQRコード
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplSmbar( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	5A サンプルチューブ有無確認（単）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplSmsgl( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	result = NG;
	UB	param1;

	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		param1 = AsctoHex(param1);

	if(( param1 >= 1 )&&( param1 <= 12 ))
	{
		result = OK;//あり
	}
	
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_SMSGL );
}

/***********************************************************************
* 	
* [概要]	5B サンプルチューブ有無確認（全）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplSmall( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	UB	result = NG;
	UB	allsts[INDAT_CNTRL_MAX];

	// 全入力ポート状態取得
	pio_ALLIN( &allsts[0] );

	if(( allsts[0] == 0xff )&&( allsts[1] == 0xc0 ))
	{
		result = OK;//あり
	}
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_SMALL );
}

/***********************************************************************
* 	
* [概要]	5C 回収チューブレーンQRコード
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplCllne( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	5D 回収チューブQRコード
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplClqrc( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	5E 回収チューブ有無（単）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplClsgl( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	5F 回収チューブ有無（全）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplClall( T_CMD_MSG* mrcv, UB anum, const char** args )	
{	
	
}	
	
/***********************************************************************
* 	
* [概要]	60 LED
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	61 UV点灯
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
* [概要]	62 パトランプ
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplPtlmp( T_CMD_MSG* mrcv, UB anum, const char** args )	
{
	UB param1;
	UB param2;
	UB param3;
	UB result;

	// データ取得
	Strcpy( (char*)&param1 , TpRecvArgs[1]  );		bzcnt.Act    = AsctoHex(param1);
	Strcpy( (char*)&param2 , TpRecvArgs[2]  );		bzcnt.Colptn = AsctoHex(param2);
	Strcpy( (char*)&param3 , TpRecvArgs[3]  );		bzcnt.Buzptn = AsctoHex(param3);

	// データチェック
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

	// タッチパネルへ返信
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_PTLMP );

	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &bufwk[0], (B*)&bzcnt, sizeof( ST_BZCNT ) );

	// タスク通知
	mbx_send(
					ID_TSK_CMD,										// 送信元ﾀｽｸID
					ID_MPF_LED,										// ﾒﾓﾘﾌﾟｰﾙID
					ID_MBX_LED,										// ﾒｰﾙﾎﾞｯｸｽID
					ID_TPL_PTLMP,									// PROCID
					mrcv->msgdata.bcid,								// BCID
					ID_MSG_CLEDON,									// message
					NO_DATA,										// 情報1(汎用)
					NO_DATA,										// 情報2(汎用)
					&bufwk[0],										// ﾃﾞｰﾀ
					sizeof( bufwk )									// ﾃﾞｰﾀｻｲｽﾞ
				);
}
/***********************************************************************
* 	
* [概要]	63 電源OFF
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
* 	
***********************************************************************/
static void TplPowof( T_CMD_MSG* mrcv, UB anum, const char** args )
{
	UB result = OK;

	pio_BOUT( O_POWERDW, ON );

	// タッチパネルへ返信
	TplRpMakeSend( PARAM1, result );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_POWOF );

}
/***********************************************************************
* 	
* [概要]	6B 運転履歴（タッチパネルより運転履歴受信）
* [引数]	電文、パラメータ数、アドレス
* [戻値]	－
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
			// エラー応答メッセージ作成
			CmdMakeResponse( 	&stCtl,                                         // 
								PARAM2, 										// パラメータ数
								STRTBL_MSG[ID_MSG_EXPRUNL], 					// コマンド文字列
								E_RECV_FAIL,									// エラーコード

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
			// タッチパネルへリトライ
			memcpy( &err, &mrcv->msgdata.data[0], siz );

			TplExRunSend();
			TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_EXRUN );
		}
	}
	else
	{
		// 応答メッセージ作成
		CmdMakeResponse( 	&stCtl,                                         // 
							PARAM2, 										// パラメータ数
							STRTBL_MSG[ID_MSG_EXPRUNL], 					// コマンド文字列
							E_SUCCESS,										// エラーコード

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
	    // 応答メッセージ送信
		CmdSendResponseEtc( &stCtl, ID_MSG_EXPRUNL );
		
		
		// タッチパネルへ次レコードを依頼
		ExpRunlSeqno++;
		TplExRunSend();
		TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_EXRUN );
	}
}

/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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
	
	// パラメータフォーム
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void TpRspLGIN2()
{
}

/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* [備考]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspINITA( ER result )
{
	INT data;

	if( result == E_SUCCESS )		data = 0;	// 成功
	else							data = 1;	// 失敗
	
	TplRpMakeSend( 1, data );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_INITA );
}
/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* [備考]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspPRCTL( ER result )
{
	INT data;

	if( result == E_SUCCESS )		data = 0;	// 成功
	else							data = 1;	// 失敗

	TplRpMakeSend( 1, data );
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_PRTCL );
}
/***********************************************************************
* 
* [概要]	ポーリング
* [引数]	メッセージデータ
* [戻値]	－
* [備考]
*                        0   1    2    3    4    5    6    7    8     9    10     11    12
*  TP_RP_SEND_FORM[1]:  STX,'W', 'N', ' ', 'R', 'P', '1', ' ', '*',  ETX,  0x30, 0x30, 0x0d
* 
***********************************************************************/
static void TpRspFWPOL()
{
	TplRpMakeSend( 1, DeviceProtcolNoGet() );		// パラメータ１電文作成
	TplRpMakeSend( 2, DeviceStatusGet() );			// パラメータ２電文作成
	TplRpMakeSend( 3, (INT)DeviceStepGet() );		// パラメータ３電文作成
	
	TplRidDataSend( &tp.rsp.buf[0] , ID_TPL_FWPOL );
}
/***********************************************************************
* 
* [概要]	LED点灯／消灯指示
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
static void TpRspLDLMP()
{
}

/***********************************************************************
* 	
* [概要]	タッチパネルパラメータ電文作成＆送信
* [引数]	パラメータ番号、パラメータデータ
* [戻値]	－
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
* [概要]	タッチパネルへ運転履歴要求電文作成＆送信
* [引数]	
* [戻値]	－
* [備考]	2WN RP1 XX3XXD
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
* [概要]	タッチパネルパラメータ電文作成＆送信
* [引数]	パラメータ番号、パラメータデータ
* [戻値]	－
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
* [概要]	タッチパネルへエクスポート要求作成＆送信
* [引数]	
* [戻値]	－
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
* [概要]	タッチパネル識別ID電文作成
* [引数]	電文、識別ID
* [戻値]	－
* 	
***********************************************************************/
static void TplRidDataSend( UB *txdat, UB id )
{
// 識別01 コマンド                                        0    1    2    3    4    5    6    7    8    9   10
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
* [概要]	タッチパネル送信データを送信タスクへ通知
* [引数]	電文、識別ID
* [戻値]	－
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
* [概要]	タッチパネル送信データを送信タスクへ通知
* [引数]	電文、識別ID
* [戻値]	－
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
* [概要]	
* [引数]	
* [戻値]	0xff:電文異常
* 
***********************************************************************/
static ER TpCmdCheck( T_CMD_MSG* mrcv )
{
	ER	err = E_COM_CMDDEF;
	UB	id;
 	UB	cmd1wk = 'A';
 	UB	cmd2wk = '$';

	
	// TPコマンドチェック
	if( 
		( mrcv->msgdata.data[0]  == cmd1wk  )&&
		( mrcv->msgdata.data[1]  == cmd2wk  ) 
	)
	{
		id    = ((AsctoHex( mrcv->msgdata.data[2] ) << 4) & 0xf0);
		id   |= ((AsctoHex( mrcv->msgdata.data[3] )     ) & 0x0f);

		// 識別IDチェック
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
			// 正常
			err = E_SUCCESS;
		}
	}

	return( err );
}
/***********************************************************************
* 
* [概要]	QG12S処理リスト
* [引数]	
* [戻値]	NO_DATA：空き無し
* 
***********************************************************************/
static ER QgProcList( T_CMD_MSG* mrcv )
{
	ER  result = E_SUCCESS;
	UB  prid1;
	
	prid1 = mrcv->msgdata.prid1;

	switch( prid1 )
	{
		case ID_TPL_FWVER:        /* FWバージョン情報                */
			break;
		case ID_TPL_OPTIF:        /* オプション情報                  */
			break;
		case ID_TPL_LGIN1:        /* ログイン認証1                   */
		case ID_TPL_LGIN2:        /* ログイン認証2                   */
			break;
		case ID_TPL_INITA:        /* 初期化指示                      */
			result = QgProcList_INITA( mrcv );
			break;
		case ID_TPL_USRAL:        /* ユーザー情報全取得              */
			break;
		case ID_TPL_INF01:        /* ユーザー1情報                   */
		case ID_TPL_INF02:        /* ユーザー2情報                   */
		case ID_TPL_INF03:        /* ユーザー3情報                   */
		case ID_TPL_INF04:        /* ユーザー4情報                   */
		case ID_TPL_INF05:        /* ユーザー5情報                   */
		case ID_TPL_INF06:        /* ユーザー6情報                   */
		case ID_TPL_INF07:        /* ユーザー7情報                   */
		case ID_TPL_INF08:        /* ユーザー8情報                   */
		case ID_TPL_INF09:        /* ユーザー9情報                   */
		case ID_TPL_INF10:        /* ユーザー10情報                  */
			break;
		case ID_TPL_REC01:        /* ユーザー1登録                   */
		case ID_TPL_REC02:        /* ユーザー2登録                   */
		case ID_TPL_REC03:        /* ユーザー3登録                   */
		case ID_TPL_REC04:        /* ユーザー4登録                   */
		case ID_TPL_REC05:        /* ユーザー5登録                   */
		case ID_TPL_REC06:        /* ユーザー6登録                   */
		case ID_TPL_REC07:        /* ユーザー7登録                   */
		case ID_TPL_REC08:        /* ユーザー8登録                   */
		case ID_TPL_REC09:        /* ユーザー9登録                   */
		case ID_TPL_REC10:        /* ユーザー10登録                  */
			break;
		case ID_TPL_CHG01:        /* ユーザー1変更                   */
		case ID_TPL_CHG02:        /* ユーザー2変更                   */
		case ID_TPL_CHG03:        /* ユーザー3変更                   */
		case ID_TPL_CHG04:        /* ユーザー4変更                   */
		case ID_TPL_CHG05:        /* ユーザー5変更                   */
		case ID_TPL_CHG06:        /* ユーザー6変更                   */
		case ID_TPL_CHG07:        /* ユーザー7変更                   */
		case ID_TPL_CHG08:        /* ユーザー8変更                   */
		case ID_TPL_CHG09:        /* ユーザー9変更                   */
		case ID_TPL_CHG10:        /* ユーザー10変更                  */
			break;
		case ID_TPL_DEL01:        /* ユーザー1削除                   */
		case ID_TPL_DEL02:        /* ユーザー2削除                   */
		case ID_TPL_DEL03:        /* ユーザー3削除                   */
		case ID_TPL_DEL04:        /* ユーザー4削除                   */
		case ID_TPL_DEL05:        /* ユーザー5削除                   */
		case ID_TPL_DEL06:        /* ユーザー6削除                   */
		case ID_TPL_DEL07:        /* ユーザー7削除                   */
		case ID_TPL_DEL08:        /* ユーザー8削除                   */
		case ID_TPL_DEL09:        /* ユーザー9削除                   */
		case ID_TPL_DEL10:        /* ユーザー10削除                  */
			break;
		case ID_TPL_FWPOL:        /* FW状態（ポーリング）            */
			break;
		case ID_TPL_PRTCL:        /* プロトコル実行                  */
			result = QgProcList_PRTCL( mrcv );
			break;
		case ID_TPL_RESTA:        /* リスタート実行                  */
			break;
		case ID_TPL_PAUSE:        /* 一時停止                        */
			break;
		case ID_TPL_RESUM:        /* 再開                            */
			break;
		case ID_TPL_ABORT:        /* 中断                            */
			break;
		case ID_TPL_OPTCD:        /* オプションバーコード実行        */
			break;
		case ID_TPL_SMBAR:        /* サンプルチューブQRコード        */
			break;
		case ID_TPL_SMSGL:        /* サンプルチューブ有無確認（単）  */
		case ID_TPL_SMALL:        /* サンプルチューブ有無確認（全）  */
			break;
		case ID_TPL_CLLNE:        /* 回収チューブレーンQRコード      */
			break;
		case ID_TPL_CLQRC:        /* 回収チューブQRコード            */
			break;
		case ID_TPL_CLSGL:        /* 回収チューブ有無（単）          */
		case ID_TPL_CLALL:        /* 回収チューブ有無（全）          */
			break;
		case ID_TPL_LDLMP:        /* LED                             */
			break;
		case ID_TPL_UVLMP:        /* UV点灯                          */
			break;
		case ID_TPL_PTLMP:        /* パトランプ                      */
//			result = QgProcList_PTLMP( mrcv );
			break;
	}
	return( result );
}

/***********************************************************************
* 
* [概要]	初期化指示
* [引数]	
* [戻値]	NO_DATA：空き無し
* [備考]
*
* typedef struct {
* 	UB	msgid;		ID_MSG_LIMIT
* 	UB	devno;		軸
* 	UB	actstp;		動作ステップ
* 	UB	prid1;		プロセス番号（ID_TPL_INITA）
* 	UB	inf;		次初期化軸
* 	UB	inf2;		未使用
* 	UB	inf3;		未使用
* 	UB	inf4;		未使用
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
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// データ初期化
		InitQgProcList(bcid);
		return( E_CON_ERR );
	}
	
	switch( bcctrl[bcid].actstp )
	{
	case 0:// アイドリング
		// －－－
		// －－－  アイドリング中（動作指示なし）  －－－
		// －－－
		break;
	
	case 1:// 原点センサを踏むまで移動(早く)
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
	
	case 2:// 原点センサを抜けるまで移動
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

	case 3:// 内部カウンターリセット
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 4:// 仮想原点位置まで移動
		posadr = &BkP7.Y0_Offset + mm - 1;
		offs = *posadr;
		
		result = RelmvAction( prid1, bcid, mm, REQ_STAUD, offs );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;

	case 5:// 原点センサを踏むまで移動(ゆっくり)
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
	
	case 6:// 内部カウンターリセット
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	
	case 7:// 仮想原点位置まで移動
		posadr = &BkP7.Y0_Offset + mm - 1;
		offs = *posadr;
		
		result = RelmvAction( prid1, bcid, mm, REQ_STAUD, offs );
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		bcctrl[bcid].actstp++;
		break;
	
	case 8:// 内部カウンターリセット
		motor_PSET( mm, 0, 0, 0 );
		bcctrl[bcid].actstp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	
	case 9:// 終了判定
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
	
	default:// 終了処理
		result = E_SUCCESS;
		break;
	}

	if( result != E_CONTINUE )
	{
		TpRspINITA( result );
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// データ初期化
	}

	return( result );	
}
/***********************************************************************
* 
* [概要]	プロトコル動作
* [引数]	
* [戻値]	NO_DATA：空き無し
* [備考]
*
* typedef struct {
* 	UB	msgid;		ID_MSG_LIMIT
* 	UB	devno;		軸
* 	UB	actstp;		動作ステップ
* 	UB	prid1;		プロセス番号（ID_TPL_INITA）
* 	UB	inf;		未使用
* 	UB	inf2;		未使用
* 	UB	inf3;		未使用
* 	UB	inf4;		未使用
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
		// －－－
		// －－－  アイドリング中（動作指示なし）  －－－
		// －－－
		return( result );	
	}

	// 中断確認
	if( GetPauseStop() == STOP_STS )			return( E_PROTCOLSTOP );

	// 一時停止確認
	if( GetPauseStop() == PAUS_STS )			return( E_PROTCOLPAUSE );

//
#if 0
	//  ★★★ デバック ★★★ 加圧
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd = P_PRS;  // 状態
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par02 = 1;      // 加速パターン  ポンプ速度
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par03 = 1;      // ポジション
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par04 = 123;    // PEEK加圧             kPa
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par05 = 4;      // PEEK加圧タイムアウト sec
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par06 = 56;     // 減圧（00H～FFH）     kPa
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par07 = 7;      // 減圧タイムアウト     sec
	pProcAdr->step[(bcctrl[bcid].actstp - 1)].par08 = 89;     // 減圧変化             kPa


	// ★★★ デバック ★★★ 攪拌
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
//★★★ デバック ★★★
	
	// --- ここから動作開始 ----------------------------------------------------------------
	Cmd = ( pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd & P_MSK );
	Sts =   pProcAdr->step[(bcctrl[bcid].actstp - 1)].StCmd;
	
	switch( Cmd )
	{
		case P_TGT://チップ取得
			result = CmdActionTipGet( mrcv );
			break;
		case P_TDS://チップ廃棄
			result = CmdActionTipDsp( mrcv );
			break;
		case P_OPN://穴あけ
			result = CmdActionStrOpn( mrcv );
			break;
		case P_PRS://加圧
			result = CmdActionPress( mrcv );
			break;
		case P_WAI://ウェイト
			result = CmdActionWait( mrcv );
			break;
		case P_MIX://攪拌
			result = CmdActionMix( mrcv );
			break;
		case P_SUC://吸引
			result = CmdActionSuc( mrcv );
			break;
		case P_PUT://吐出
			result = CmdActionPut( mrcv );
			break;
		case P_PR1://分注１
			result = CmdActionPrep1( mrcv );
			break;
		case P_PR2://分注２
			result = CmdActionPrep2( mrcv );
			break;
		case P_INC://インキュベーション
			result = CmdActionIncub( mrcv );
			break;
		case P_MOV://移動
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

			if( bcctrl[bcid].actstp > pProcDat->Cnt )		// 全行程終了
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
		memset( &bcctrl[bcid].msgid,  CLR,  sizeof( ST_BCCTRL ) );				// データ初期化
	}
	else
	{
		DeviceInfoSet( result, bcctrl[bcid].inf2, bcctrl[bcid].actstp, Sts );
	}
	
	return( result );
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* [備考]	
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
* [概要]	
* [引数]	
* [戻値]	
* [備考]	
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
		bcctrl[bcid].inf2   = inf2;// プロトコル実行時は、プロトコル番号
	}
}

/***********************************************************************
* 
* [概要]	QG12S処理リスト空き確認
* [引数]	
* [戻値]	NO_DATA：空き無し
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
				return( index );//最初にみつけた空き位置
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
* [概要]	QG12S処理リスト重複確認
* [引数]	
* [戻値]	TRUE：重複もしくはID規定外
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
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static void SetUserInfoSorce( ST_USERINFO *user )
{
	// memcpy(コピー先アドレス、コピー元アドレス、コピーサイズ)
	memcpy( &RcvUserInfo.Id[0], user, sizeof(ST_USERINFO) );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static void ClrUserInfoSorce()
{
	memset( &RcvUserInfo.Id[0], CLR, sizeof( ST_USERINFO ) );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
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
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static UB GetProtocolStatus( UB dev, UB ptn )
{
	UB result = E_OK;
	
	// ステップから状態読込み
	
	
	
	
	return( result );
}
/***********************************************************************
* 
* [概要]	状態取得
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER  CmdActionStatus( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;
	return( result );
}

/***********************************************************************
* 
* [概要]	チップ取得
* [引数]	
* [戻値]	
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
	case 0:	// チップ未装着確認（チップ装着済みの場合、チップ装着エラー通知し終了）
		// ★チップ有無確認したいが、センサ無しのため、スルー
		
		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	case 1:	// Z軸速度設定・退避位置移動
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

	case 2:	// Y0軸速度設定・チップ装着位置移動
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
		
	case 3:	// Z軸チップ取得速度（1段目）設定・1段目位置移動
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}


	case 4:	// Z軸チップ取得速度（最終）設定・最終位置移動
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}

	case 5:	// Z軸速度設定・退避位置移動
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

	case 6:	// チップ装着確認（装着失敗時、チップ未装着エラー通知し終了）
		// ★チップ有無確認

		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [概要]	チップ廃棄
* [引数]	
* [戻値]	
* [備考]	inf3：プランジャリトライカウンタに使用
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
	case 0:	// チップ未装着確認（チップ装着済みの場合、チップ装着エラー通知し終了）
		// ★チップ有無確認したいが、センサ無しのため、スルー
		
		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	case 1:	// Z軸速度設定・退避位置移動
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

	case 2:	// Y0軸速度設定・チップ廃棄位置移動
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

	case 3:	// Z軸チップ廃棄速度（1段目）設定・1段目位置移動
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//		}


	case 4:	// Z軸チップ廃棄速度（最終）設定・最終位置移動
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}
		
		
	case 5:	// 廃棄動作 プランジャを下へ動作
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}


	case 6:	// 廃棄動作 プランジャを上へ動作
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}


	case 7:	// Z軸速度設定・退避位置移動
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

	case 8:	// チップ装着確認（装着失敗時、チップ未装着エラー通知し終了）
		// ★チップ有無確認

		result = E_SUCCESS;
		pProcDat->Stp++;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [概要]	穴あけ
* [引数]	
* [戻値]	
* [備考]	
* 		pProcAdr->step[actstep].par03 == STRIP_S1 )	// ショート１
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
	case 0:	// 試薬ストリッバ有無確認
		
		// ★試薬ストリッバ有無確認
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	case 1: // Z軸速度設定・退避位置移動
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

	case 2:	// Y0軸速度設定後、試薬ストリッパブレーク位置に移動
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

	case 3:	// Z軸シールブレーク1段位置（現在位置から1段目まで）
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}
		
		
	case 4:	// Z軸シールブレーク最終位置（1段目から最終位置まで）
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
			// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}


	case 5:	// Z軸速度設定・退避位置移動
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
* [概要]	加圧
* [引数]	
* [戻値]	
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
	case 0:	// ポンプ速度設定、加圧前圧力取得指示
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

	case 1:	// 正常ノズルのみ電磁弁を流路側に指示
		// typedef struct {
		// 	unsigned short	idl;
		// 	unsigned short	suc;
		// 	unsigned short	err;
		// 	unsigned short	tcnt;
		// 	unsigned short	pre[NOZZLE_MAX+1];//加圧前
		// 	unsigned short	now[NOZZLE_MAX+1];//現在
		// 	unsigned short	pek[NOZZLE_MAX+1];//Peek
		// 	unsigned short	dat[NOZZLE_MAX+1][PRESSMP_MAX];//履歴
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

	case 2:	// ポンプ駆動（加圧開始）
		pio_BOUT( O_DAIAPMP, 5 );// ★速度パターンいれる
		WaitAction( prid1, bcid, PRESSMP_WAIT );
		pProcDat->Stp++;
		break;

	case 3:	// 圧力値取得
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

	case 4:	// 加圧判定
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

	case 5:	// 圧力値取得 減圧
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

	case 6:	// 減圧判定
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
		if( PressStatus(&pProcAdr->step[actstep].par03) != E_SUCCESS )//★
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
* [概要]	
* [引数]	
* [戻値]	
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

	if( psns.tcnt >= ( 5 * pProcAdr->step[actstep].par05 ))//  Timeout時間(msec) : Tsec * 1000、Interval:200msec、タイムアウトカウント：1000T/200 = 5T
	{
		// タイムアウトした場合は、非対象ch・成功ch以外エラー
		wkA = ( psns.idl | psns.suc );
		wkB = ( wkA      ^ 0x0fff   );
		wkC = ( wkB      ^ psns.err );
		psns.err = wkC;
		
		if( ( wkC & 0x0fff ) == 0x0fff )	// 全てエラーは終了
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
					// エラーもしくは成功済みの場合は、判定不要
				}
				else
				{
					peekdat = psns.now[nzl];
					if( peekdat >= pProcAdr->step[actstep].par04 )	// 加圧ピーク越え
					{
						psns.suc |= sftbit;
						psns.pek[nzl] = peekdat;// 加圧ピーク登録
					}
				}
			}
			sftbit = ( sftbit << 1 );
		}
		
		wkA = (psns.idl | psns.suc | psns.err);
		
		if( ( wkA & 0x0fff ) == 0x0fff )	// 全て終了
		{
			result = E_SUCCESS;
debug1_flg=1;
		}
	}
	return( result );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
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

	if( psns.tcnt >= ( 5 * pProcAdr->step[actstep].par07 ))//  Timeout時間(msec) : Tsec * 1000、Interval:200msec、タイムアウトカウント：1000T/200 = 5T
	{
		// タイムアウトした場合は、非対象ch・成功ch以外エラー
		wkA = ( psns.idl | psns.suc );
		wkB = ( wkA      ^ 0x0fff   );
		wkC = ( wkB      ^ psns.err );
		psns.err = wkC;
		
		if( ( wkC & 0x0fff ) == 0x0fff )	// 全てエラーは終了
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
					if( psns.now[nzl] <= pProcAdr->step[actstep].par06 )	// 減圧したか
					{
						psns.suc |= sftbit;
					}
					else if( (psns.pek[nzl]-psns.now[nzl]) >= pProcAdr->step[actstep].par08 )	// 急激に減圧したか
					{
						psns.err |= sftbit;
					}
				}
				sftbit = ( sftbit << 1 );
			}
		}

		if( ( psns.err & 0x0fff ) == 0x0fff )	// 全てエラーは終了
		{
			result = E_SUCCESS;
		}
		
		wkA = (psns.idl | psns.suc | psns.err);
		if( ( wkA & 0x0fff ) == 0x0fff )		// 全て終了
		{
			result = E_SUCCESS;
		}
	}
	return( result );
}

/***********************************************************************
* 
* [概要]	ウェイト
* [引数]	
* [戻値]	
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
	case 0:	// 1. ウェイト時間設定後、タイマスタート
		bcid    = mrcv->msgdata.bcid;
		prid1   = mrcv->msgdata.prid1;
		actstep = bcctrl[bcid].actstp -1 ;
		waittim = ( pProcAdr->step[actstep].par03 * 60 * 1000 ) / DLY_10M ;// 10msec周期

		WaitAction( prid1, bcid, waittim );
		pProcDat->Stp++;
		break;

	case 1:	// 2. ウェイト終了待ち
		result = E_SUCCESS;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [概要]	攪拌
* [引数]	
* [戻値]	
* 


// 3．分注Z軸｢撹拌吸引高さ｣位置移動
// 4．撹拌容量＋余剰量分（初回のみ）を吸引と同時に分注Z軸を下降させる。
// 5．吸引後ウェイト
// 6．現在の分注Z軸位置から撹拌容量を吐出と同時に分注Z軸上昇させる。
// 7．撹拌容量を吐出　※攪拌最終回の吐出時のみ余剰量分も含め吐出
// 8．吐出後ウェイト
// 9．｢4～8｣を設定回数繰り返す
// 10．分注Z軸ブローアウト高さ位置移動
// 11．ポンプリセット
// 12．分注Z軸チップタッチ位置移動  (チップタッチ有り)
// 13．分注Z軸ホームポジション移動
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

	UB  z_sucdn;// par4  bit7～bit4：[Z]吸引下降速度パターン
	UB  p_sucsp;// par4  bit3～bit0：[P]吸引速度パターン
	UB  z_sucup;// par5  bit7～bit4：[Z]吐出上昇速度パターン
	UB  p_putsp;// par5  bit3～bit0：[P]吐出速度パターン
	UB  staposi;// par6  [Z]攪拌開始位置ポジション番号
	UB  endposi;// par7  [Z]攪拌終了位置ポジション番号
	INT mixcnt; // par1  bit5～bit0：攪拌回数
	INT zancnt;
	INT stahei;// par6   [Z]攪拌開始位置
	INT endhei;// par7   [Z]攪拌終了位置
	INT sucval;// par13  [P}吸引量
	INT putval;// par14  [P}吐出量
	
	prid1 = mrcv->msgdata.prid1;
	bcid  = mrcv->msgdata.bcid;
	inf1  = mrcv->msgdata.inf;
	inf2  = mrcv->msgdata.inf2;
	actstep = bcctrl[bcid].actstp - 1 ;
	
	switch( pProcDat->Stp )
	{
	case 0:	// [攪拌処理] 
		z_sucdn = ((pProcAdr->step[actstep].par05 >> 4) & 0x0f);
		p_sucsp = ((pProcAdr->step[actstep].par05 >> 0) & 0x0f);
		z_sucup = ((pProcAdr->step[actstep].par05 >> 4) & 0x0f);
		p_putsp = ((pProcAdr->step[actstep].par05 >> 0) & 0x0f);
		stahei  = GetPosiData( pProcAdr->step[actstep].par07 );
		endhei  = GetPosiData( pProcAdr->step[actstep].par08 );
		sucval  = pProcAdr->step[actstep].par13;
		putval  = pProcAdr->step[actstep].par14;
		
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		
		
	case 1:// [攪拌処理] 1．ブローアウトエア高さ（退避高さ）移動
		MAC_SPEED_SET( MT_Z0, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].ac, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fl, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fh, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 2:// [攪拌処理] 2．圧力基板制御：未指定のノズルチャンネルの電磁弁をONにする
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
		
	case 3:// [攪拌処理] 3．P軸ブローアウトエア吸引速度設定
		// ブローアウトエア吸引
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
		
	case 4:	// [攪拌処理]  4．分注Z軸｢撹拌吸引高さ｣位置移動
		if( tskid == ID_TSK_PUNIT )
		{
			// ★圧力監視記述
			
			
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
		
	case 5 :	// [攪拌処理]  5 ．撹拌容量＋余剰量分（初回のみ）を吸引と同時に分注Z軸を下降させる。& 吸引中圧力監視
		if( tskid == ID_TSK_PUNIT )
		{
			// 読み捨て
		}
		else
		{
			devno  = MT_Z0;
			spdptn = (pProcAdr->step[actstep].par05 >> 4);
			movptn = pProcAdr->step[actstep].par07;				// 吸引終了高さ
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
		
	case 6 :	// [攪拌処理]  6 ．吸引後ウェイト
		if( tskid == ID_TSK_PUNIT )
		{
			// ★圧力監視記述
			
			
			
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
		
	case 7 :	// [攪拌処理]  7 ．現在の分注Z軸位置から撹拌容量を吐出と同時に分注Z軸上昇させる。
		if( tskid == ID_TSK_PUNIT )
		{
			// 読み捨て
		}
		else
		{
			devno  = MT_Z0;
			spdptn = (pProcAdr->step[actstep].par05 >> 4);// 吐出上昇速度パターン
			movptn = pProcAdr->step[actstep].par06;		  // 攪拌開始高さ
			movval = GetPositionValue(movptn);

			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );

			devno  = MT_P0;
			spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// 吐出速度パターン
			MAC_SPEED_SET( devno, 
			               Sp[devno][spdptn].ac, 
			               Sp[devno][spdptn].fl, 
			               Sp[devno][spdptn].fh, 
			               Sp[devno][spdptn].gl );
	
			// ★最終の吐出か（余剰量＋吐出量を吐き出す）
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
		
	case 8 :	// [攪拌処理]  8 ．撹拌容量を吐出
		if( tskid == ID_TSK_PUNIT )
		{
			// 読み捨て
		}
		else
		{
			pProcDat->Stp++;
			WaitAction( prid1, bcid, 10 );
		}
		break;
		
	case 9 :	// [攪拌処理]  9 ．吐出後ウェイト
		if( tskid == ID_TSK_PUNIT )
		{
			// 読み捨て
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
				pProcDat->Stp = 5;// ｢5～9｣を設定回数繰り返す
			}
			WaitAction( prid1, bcid, 2 );
		}
		break;
		
	case 10:	// [攪拌処理]  10．分注Z軸ブローアウト高さ位置移動
		MAC_SPEED_SET( MT_Z0, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].ac, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fl, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].fh, 
		               Sp[MT_Z0][SpPtn.Z0_SpdNmlPtn].gl );

		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 11:	// [攪拌処理]  11．ポンプリセット
		// ブローアウトエア吸引
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
		
	case 12:	// [攪拌処理]  12．圧力基板制御：未指定のノズルチャンネルの電磁弁をONにする
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	case 13:	// [攪拌処理]  13．分注Z軸チップタッチ位置移動  (チップタッチ有り)
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	case 14:	// [攪拌処理]  14．分注Z軸退避位置移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, MT_Z0, REQ_STAUD, BkP7.Z0_Taihi );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 15:	// [攪拌処理] 
		result = E_SUCCESS;
		break;
	}
	
	return( result );
}
/***********************************************************************
* 
* [概要]	吸引
* [引数]	
* [戻値]	
* [補足]	P1  P2   P3     P4        P5  P6       P7       P8        P9  P10  P11  P12  P13    P14
*           *8       ﾊﾟﾀ-ﾝ  吸引位置  -   吸引速度 吸引高開 吸引高終  -   -    -    -    吸引量 -
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
	case 0: // 0． 待機
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 1: // 1． Z軸通常速度設定
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;
		movval = GetPositionValue(POS_Z0_TAIHI);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 2: // 2． Z軸退避高さへ移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3． Y0軸通常速度設定
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;
		movptn = pProcAdr->step[actstep].par03;
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 4: // 4． Y0軸指定位置移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5． Z軸吸引高さ位置移動
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6． 圧力基板制御：未指定電磁弁ON、指定電磁弁OFF
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

	case 7: // 7． P軸吸引速度設定
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);
		movptn = pProcAdr->step[actstep].par06;				// 吸引終了高さ
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
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 8: // 8． 吸引開始（P軸吸引 & Z軸下降）
//      MapAction( UB prid1, UB bcid, UB mm1, UB mm2, INT pppp1, INT pppp2 );
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );

		
		
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 9: // 9． 圧力基板制御：吸引動作中に指定したノズルチャンネルの圧力を監視
	        //     ※各チャンネルの圧力センサ値が設定値を上回った場合、エラーとし電磁弁をONにする。
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

	case 10:// 10．吸引動作終了
		pProcDat->Stp++;


	case 11:// 11．圧力基板制御：圧力監視終了。全てのノズルチャンネルの電磁弁をOFFにする。
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

	case 12:// 12．指定時間ウェイト
		pProcDat->Stp++;
		WaitAction( prid1, bcid, 10 );
		break;

	case 13:// 13．Z軸退避高さへ移動
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
* [概要]	吐出
* [引数]	
* [戻値]	
* [補足]	P1  P2    P3    P4       P5   P6   P7   P8   P9   P10   P11   P12   P13   P14
*           *8       ﾊﾟﾀ-ﾝ  吸引位置 -    -    -    -
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
	case 0: // 0． 待機
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	//****************************************************
	//***                    吸引                      ***
	//****************************************************
	case 1: // 1． Z軸通常速度設定
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;                    // [Z]通常速度
		movval = GetPositionValue(POS_Z0_TAIHI);        // [Z]退避位置

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 2: // 2． Z軸退避高さへ移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3． Y0軸通常速度設定
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;                    // [Y0]通常速度
		movptn = pProcAdr->step[actstep].par03;         // [Y0]吸引位置
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 4: // 4． Y0軸指定位置移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5． Z軸吸引高さ位置移動
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;         // [Z]吸引開始高さ
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6． 圧力基板制御：未指定電磁弁ON、指定電磁弁OFF
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

	case 7: // 7． P軸吸引速度設定
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]吸引下降速度パターン
		movptn = pProcAdr->step[actstep].par07;         // [Z]吸引終了高さ
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// [P]吸引速度パターン
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 8: // 8． 吸引開始（P軸吸引 & Z軸下降）
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 9: // 9． 圧力基板制御：吸引動作中に指定したノズルチャンネルの圧力を監視
	        //     ※各チャンネルの圧力センサ値が設定値を上回った場合、エラーとし電磁弁をONにする。
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

	case 10:// 10．吸引動作終了
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

	case 11:  // 11．指定時間ウェイト待ち
		if( msgid == ID_MSG_PRESSTAN )
		{
			//読み捨て
			break;
		}
		if( msgid == ID_MSG_TIMWAIT )
		{
			pProcDat->Stp++;
		}
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		
	case 12:// 12．Z軸退避高さへ移動
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

	case 13:  // 13．終了待ち
		result = E_SUCCESS;
		break;
	}

	return( result );
}
/***********************************************************************
* 
* [概要]	分注１
* [引数]	
* [戻値]	
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
	case 0: // 0． 待機
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	//****************************************************
	//***                    吸引                      ***
	//****************************************************
	case 1: // 1． Z軸通常速度設定
		devno  = MT_Z0;
		spdptn = SpPtn.Z0_SpdNmlPtn;                    // [Z]通常速度
		movval = GetPositionValue(POS_Z0_TAIHI);        // [Z]退避位置

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 2: // 2． Z軸退避高さへ移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 3: // 3． Y0軸通常速度設定
		devno  = MT_Y0;
		spdptn = SpPtn.Y0_SpdNmlPtn;                    // [Y0]通常速度
		movptn = pProcAdr->step[actstep].par03;         // [Y0]吸引位置
		movval = GetPositionValue(movptn);
		
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 4: // 4． Y0軸指定位置移動
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 5: // 5． Z軸吸引高さ位置移動
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par06;         // [Z]吸引開始高さ
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 6: // 6． 圧力基板制御：未指定電磁弁ON、指定電磁弁OFF
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

	case 7: // 7． P軸吸引速度設定
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]吸引下降速度パターン
		movptn = pProcAdr->step[actstep].par07;         // [Z]吸引終了高さ
		movval = GetPositionValue(movptn);

		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );

		devno  = MT_P0;
		spdptn = (pProcAdr->step[actstep].par05 & 0x0f);// [P]吸引速度パターン
		MAC_SPEED_SET( devno, 
		               Sp[devno][spdptn].ac, 
		               Sp[devno][spdptn].fl, 
		               Sp[devno][spdptn].fh, 
		               Sp[devno][spdptn].gl );
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 8: // 8． 吸引開始（P軸吸引 & Z軸下降）
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par13 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 9: // 9． 圧力基板制御：吸引動作中に指定したノズルチャンネルの圧力を監視
	        //     ※各チャンネルの圧力センサ値が設定値を上回った場合、エラーとし電磁弁をONにする。
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

	case 10:// 10．吸引動作終了
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

	case 11:  // 11．指定時間ウェイト待ち
		if( msgid == ID_MSG_PRESSTAN )
		{
			//読み捨て
			break;
		}
		if( msgid == ID_MSG_TIMWAIT )
		{
			pProcDat->Stp++;
		}
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		
	case 12:// 12．Z軸退避高さへ移動
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
	//***                    吐出                      ***
	//****************************************************
	case 13:   // 13．Y0軸指定位置移動
		movptn = pProcAdr->step[actstep].par04;
		movval = GetPositionValue(movptn);
		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;

	case 14:   // 14．Z軸吐出開始高さへ移動
		devno  = MT_Z0;
		movptn = pProcAdr->step[actstep].par09;					// 吐出開始高さ
		movval = GetPositionValue(movptn);

		result = AbsmvAction( ID_TPL_PRTCL, bcid, devno, REQ_STAUD, movval );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		break;
		
	case 15:   // 15．P軸吐出速度設定、Z軸上昇速度設定
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par08 >> 4);
		movptn = pProcAdr->step[actstep].par10;					// 吐出終了位置
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
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 16:   // 16．吐出開始（P軸吐出・Z軸吐出上昇同時駆動）、同時に圧力基板へ圧力値取得指示
		MapAction( prid1, bcid, MT_Z0, MT_P0, movval, pProcAdr->step[actstep].par14 );
		if( result == E_SUCCESS ) result = E_CONTINUE;
		pProcDat->Stp++;
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	case 17:   // 17．P軸・Z軸動作終わるまで、圧力状態監視を行う。閾値以下を検出した場合は、吐出失敗。
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

	case 18:   // 18．（P軸・Z軸動作終了後）圧力基板制御：圧力監視終了。全てのノズルチャンネルの電磁弁をOFFにする。
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
		
	case 19:  // 19．指定時間ウェイト
		if( msgid == ID_MSG_PRESSTAN )
		{
			//読み捨て
		}
		if( msgid == ID_MSG_VALVEOUT )
		{
			pProcDat->Stp++;
			WaitAction( prid1, bcid, 10 );
		}
		break;
		
		
	case 20:  // 20．Z軸通常速度設定・退避位置移動
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
		
		
	case 21:  // 21．終了待ち
		result = E_SUCCESS;
		break;
	}
	
	return( result );
}

/***********************************************************************
* 
* [概要]	吸引位置取得
* [引数]	
* [戻値]	
* 
***********************************************************************/
static INT  GetPositionValue( UB index )
{
	INT *posadr;
	INT result = CLR;
	
	if(( index >= POS_Y0_OFFSE )&&( index <= POS_Z0_COLDS ))	// 1～32
	{
		posadr = &BkP7.Y0_Offset + (index - POS_Y0_OFFSE) ;
		result = *posadr;
	}

	if(( index >= POS_Z0_SUCS1 )&&( index <= POS_XX_RSV64 ))	// 33～
	{
		posadr = &BkP8.Z0_SucStaHei1 + (index - POS_Z0_SUCS1) ;
		result = *posadr;
	}

	if(( index >= POS_H1_C1VO1 )&&( index <= POS_XX_RSV96 ))	// 65～
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
* [概要]	分注２
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER  CmdActionPrep2( T_CMD_MSG* mrcv )
{
	ER result = E_CONTINUE;

	return( result );
}
/***********************************************************************
* 
* [概要]	インキュベーション
* [引数]	
* [戻値]	
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
	case 0:// [Z]ノズル高さ移動
		IncuvDataInit(pProcAdr->step[actstep].par03 , pProcAdr->step[actstep].par04);
		
		devno  = MT_Z0;
		spdptn = (pProcAdr->step[actstep].par05 >> 4);  // [Z]吸引下降速度パターン
		movptn = pProcAdr->step[actstep].par06;         // [Z]吸引終了高さ
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
		
	case 1:	// 温調器へ指定した温度に設定通知を行う。
		// 指定温度通知
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

	case 2:	// 指定温度を指定時間になるまで監視
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
		// ↓↓↓  下に続く  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		}

	case 3:	// 終了待ち
		result = E_SUCCESS;
		break;
	}
	return( result );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	TRUE = インキュベーション終了 
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
* [概要]	
* [引数]	
* [戻値]	TRUE = インキュベーション終了 
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
* [概要]	移動
* [引数]	
* [戻値]	
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
* [概要]	
* [引数]	
* [戻値]	
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
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static UB  GetMoveDevice( UB posi )
{
	UB devno = CLR;
	
	switch( posi )
	{
		case POS_P0_OFFSE :   // [P0] 仮想原点オフセット値               ポジション番号３
		case POS_P0_TAIHI :   // [P0] 退避位置                           ポジション番号８
			devno = MT_P0;
			break;

		case POS_F0_OFFSE :   // [F0] 仮想原点オフセット値               ポジション番号４
		case POS_F0_TAIHI :   // [F0] 退避位置                           ポジション番号９
			devno = MT_F0;
			break;
		
		case POS_Y1_OFFSE :   // [Y1] 仮想原点オフセット値               ポジション番号５
		case POS_Y1_TAIHI :   // [Y1] 退避位置                           ポジション番号１０
			devno = MT_Y1;
			break;

		case POS_Y0_OFFSE :   // [Y0] 仮想原点オフセット値               ポジション番号１
		case POS_Y0_TAIHI :   // [Y0] 退避位置                           ポジション番号６
		case POS_STRIPTP1 :   // [Y0] 試薬ストリップ-ショートチップ１    ポジション番号１１
		case POS_STRIPTP2 :   // [Y0] 試薬ストリップ-ショートチップ２    ポジション番号１２
		case POS_STRIPTPL :   // [Y0] 試薬ストリップ-ロングチップ        ポジション番号１３
		case POS_STRIPTUB :   // [Y0] 試薬ストリップ-サンプルチューブ    ポジション番号１４
		case POS_STRIPSM6 :   // [Y0] 試薬ストリップ-抽出用試薬6         ポジション番号１５
		case POS_STRIPSM5 :   // [Y0] 試薬ストリップ-抽出用試薬5         ポジション番号１６
		case POS_STRIPSM4 :   // [Y0] 試薬ストリップ-抽出用試薬4         ポジション番号１７
		case POS_STRIPSM3 :   // [Y0] 試薬ストリップ-抽出用試薬3         ポジション番号１８
		case POS_STRIPSM2 :   // [Y0] 試薬ストリップ-抽出用試薬2         ポジション番号１９
		case POS_STRIPSM1 :   // [Y0] 試薬ストリップ-抽出用試薬1         ポジション番号２０
		case POS_STRIPCAT :   // [Y0] 試薬ストリップ-カートリッジ        ポジション番号２１
		case POS_STRIPCOL :   // [Y0] 試薬ストリップ-核酸回収液          ポジション番号２２
			devno = MT_Y0;
			break;

		case POS_Z0_OFFSE :   // [Z0] 仮想原点オフセット値               ポジション番号２
		case POS_Z0_TAIHI :   // [Z0] 退避位置                           ポジション番号７
		case POS_Z0_STOPE :   // [Z0] 試薬ストリップ-シール開け最終位置  ポジション番号２３
		case POS_Z0_STOP1 :   // [Z0] 試薬ストリップ-シール開け１段位置  ポジション番号２４
		case POS_Z0_STRIP :   // [Z0] 試薬ストリップ-加圧位置	         ポジション番号２５
		case POS_Z0_TPGTE :   // [Z0] チップ取得最終位置                 ポジション番号２６
		case POS_Z0_TPGT1 :   // [Z0] チップ取得１段位置                 ポジション番号２７
		case POS_Z0_TPDLE :   // [Z0] チップ廃棄最終位置                 ポジション番号２８
		case POS_Z0_TPDL1 :   // [Z0] チップ廃棄１段位置                 ポジション番号２９
		case POS_Z0_LIQPU :   // [Z0] 廃液吐出位置                       ポジション番号３０
		case POS_Z0_COPOS :   // [Z0] コレクション 回収チューブ位置      ポジション番号３１
		case POS_Z0_COLDS :   // [Z0] 廃液チューブ位置                   ポジション番号３２
		case POS_Z0_SUCS1 :   // [Z0] 吸引開始高さ1                      ポジション番号３３
		case POS_Z0_SUCS2 :   // [Z0] 吸引開始高さ2                      ポジション番号３４
		case POS_Z0_SUCS3 :   // [Z0] 吸引開始高さ3                      ポジション番号３５
		case POS_Z0_SUCS4 :   // [Z0] 吸引開始高さ4                      ポジション番号３６
		case POS_Z0_SUCS5 :   // [Z0] 吸引開始高さ5                      ポジション番号３７
		case POS_Z0_SUCE1 :   // [Z0] 吸引終了高さ1                      ポジション番号３８
		case POS_Z0_SUCE2 :   // [Z0] 吸引終了高さ2                      ポジション番号３９
		case POS_Z0_SUCE3 :   // [Z0] 吸引終了高さ3                      ポジション番号４０
		case POS_Z0_SUCE4 :   // [Z0] 吸引終了高さ4                      ポジション番号４１
		case POS_Z0_SUCE5 :   // [Z0] 吸引終了高さ5                      ポジション番号４２
		case POS_Z0_PUTS1 :   // [Z0] 吐出開始高さ1                      ポジション番号４３
		case POS_Z0_PUTS2 :   // [Z0] 吐出開始高さ2                      ポジション番号４４
		case POS_Z0_PUTS3 :   // [Z0] 吐出開始高さ3                      ポジション番号４５
		case POS_Z0_PUTS4 :   // [Z0] 吐出開始高さ4                      ポジション番号４６
		case POS_Z0_PUTS5 :   // [Z0] 吐出開始高さ5                      ポジション番号４７
		case POS_Z0_PUTE1 :   // [Z0] 吐出終了高さ1                      ポジション番号４８
		case POS_Z0_PUTE2 :   // [Z0] 吐出終了高さ2                      ポジション番号４９
		case POS_Z0_PUTE3 :   // [Z0] 吐出終了高さ3                      ポジション番号５０
		case POS_Z0_PUTE4 :   // [Z0] 吐出終了高さ4                      ポジション番号５１
		case POS_Z0_PUTE5 :   // [Z0] 吐出終了高さ5                      ポジション番号５２
		case POS_Z0_MIXS1 :   // [Z0] 撹拌吸引開始高さ                   ポジション番号５３
		case POS_Z0_MIXE1 :   // [Z0] 撹拌吸引終了高さ                   ポジション番号５４
			devno = MT_Z0;
			break;
		
		default:
			break;
	}
	return( devno );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
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
* [概要]	圧力ユニットへ加圧指示
* [引数]	
* [戻値]	
* 
***********************************************************************/
static ER  PressReq( UB *par )
{
	ER result = E_CONTINUE;
	// ★後日検討
	
	return( result );
}
/***********************************************************************
* 
* [概要]	圧力ユニットへ加圧指示
* [引数]	
* [戻値]	
* 
***********************************************************************/
static INT  PressStatus( UB *par )
{
	INT result = E_CONTINUE;
	
	// ★後日検討
	result = E_SUCCESS;
	
	return( result );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static void  SetPauseStop( UB act )
{
	if( act > PAUS_STS )	PauseStopFlg = PAUS_STS;
	else 					PauseStopFlg = act;
	
	// 状態書き込み
	
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static UB  GetPauseStop()
{
	return( PauseStopFlg );
}
/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static UB  InitPauseStop()
{
	PauseStopFlg = IDLE_STS;
}

/***********************************************************************
* 
* [概要]	ブローアウトエア量
* [引数]	補正前容量
* [戻値]	補正後容量
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
* [概要]	エアギャップ量
* [引数]	補正前容量
* [戻値]	補正後容量
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
* [概要]	余剰量
* [引数]	補正前容量
* [戻値]	補正後容量
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
* [概要]	容量補正計算
* [引数]	補正前容量
* [戻値]	補正後容量
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
	
	if( ( vl0_base >= 0 )&&( ( valcal < vl1_base ) ))			//  0%以上 ～ 10%未満
	{
		factor = adr->HS_VL1;
	}
	if( ( valcal >= vl1_base )&&( ( valcal < vl2_base ) ))	// 10%以上 ～ 50%未満
	{
		factor = adr->HS_VL2;
	}
	if( ( valcal >= vl2_base )&&( ( valcal < vl3_base ) ))	// 50%以上 ～ 100%未満
	{
		factor = adr->HS_VL3;
	}
	if( valcal >= vl3_base )										// 100%以上
	{
		factor = adr->HS_VL3;
	}
	
	result = factor * valcal;
	result = result / 1000;

	return( result);
}

/***********************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static unsigned short  GetNzlStsAll()
{
	return( nzlsts );
}
/***********************************************************************
* 
* [概要]	指定ノズルの状態を取得する
* [引数]	ノズル番号
* [戻値]	ノズル状態（0 ≠  エラー状態、0 = 正常状態）
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
* [概要]	ノズル状態セット
* [引数]	ノズル番号
* [戻値]	－
* [備考]	bit0:ノズル1・・・・bit11:ノズル12  ノズル状態（1 =  エラー状態、0 = 正常状態）
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
* [概要]	ノズル状態クリア
* [引数]	ノズル番号
* [戻値]	－
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
* [概要]	
* [引数]	
* [戻値]	
* 
***********************************************************************/
static unsigned short  NzlTargetGet()
{
	unsigned short nzltarget;
	unsigned short nzlerror;
	unsigned short nzlmask;
	unsigned short nzlresult;
	
	nzltarget = IdleBitSet( psel.smp );	// ターゲット取得
	nzlerror  = GetNzlStsAll() ;		// エラー状態取得
	nzlmask   = ( nzlerror ^ 0xffff );	// エラーチャネルマスクデータ作成
	nzlresult = ( nzltarget & nzlmask );// エラー以外のターゲット取得
	
	return( nzlresult );
}

/***********************************************************************
* 
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
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
* [概要]	
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
INT GetRunData( unsigned char *buf )
{
	memcpy( buf, &ImpExpRcvData[0], RcvDataSize );
	
	return( RcvDataSize );
}

/***********************************************************************
* 
* [概要]	大容量データ扱う場合、一時的にRcvDataに格納 ※競合しない設計必要
* [引数]	メッセージデータ
* [戻値]	－
* 
***********************************************************************/
void SetRcvData( unsigned char *buf, unsigned int len )
{
	memset( &ImpExpRcvData[0], CLR, sizeof( ImpExpRcvData ) );

	RcvDataSize = len;
	memcpy( &ImpExpRcvData[0], buf, len );
}



