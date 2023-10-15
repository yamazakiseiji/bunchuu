/******************************************************************************
* 定数定義                                                                    *
*                                                                             *
* File name : define.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _DEFEINE_H_
#define _DEFEINE_H_






//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// 条件付きコンパイル定義

#define EEP_RWSIZE_TYPE		1			// 0:1byte RW, 1:8byte RW
#define LOGSEND				1			// 


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// 共通定義

// セット・クリア
#define SET					1
#define CLR					0

#define ON					1
#define OFF					0

#define OK					0
#define NG					1


// モータ駆動時状態
#define DEFDAT				0xff
#define REQIDL				0x00		// 
#define REQRDY				0x01		// 動作前準備
#define REQACT				0x02		// 動作実行
#define REQEND				0xEE		// 動作終了

// 
#define NO_DATA				0xff


// BIT情報
#define BIT0				0x0001
#define BIT1				0x0002
#define BIT2				0x0004
#define BIT3				0x0008
#define BIT4				0x0010
#define BIT5				0x0020
#define BIT6				0x0040
#define BIT7				0x0080
#define BIT8				0x0100
#define BIT9				0x0200
#define BIT10				0x0400
#define BIT11				0x0800
#define BIT12				0x1000
#define BIT13				0x2000
#define BIT14				0x4000
#define BIT15				0x8000


// 実行処理結果
#define E_OK				0           // 実行完了
#define E_SUC				0           // 実行完了                         = PSTAT_OK相当
#define E_NON				6           // 処理なし

#define N_HEX				16
#define N_DEC				10

#define N_SIG				1
#define N_UNSIG				0

// Timeout
#define DLY_TMOUT			-1

#define PARAM0				0	// 第０パラメータ
#define PARAM1				1	// 第１パラメータ
#define PARAM2				2	// 第２パラメータ
#define PARAM3				3	// 第３パラメータ
#define PARAM4				4	// 第４パラメータ
#define PARAM5				5	// 第５パラメータ
#define PARAM6				6	// 第６パラメータ
#define PARAM7				7	// 第７パラメータ
#define PARAM8				8	// 第８パラメータ
#define PARAM9				9	// 第９パラメータ
#define PARAM10				10	// 第１０パラメータ
#define PARAM11				11	// 第１１パラメータ
#define PARAM12				12	// 第１２パラメータ
#define PARAM13				13	// 第１３パラメータ
#define PARAM14				14	// 第１４パラメータ
#define PARAM15				15	// 第１５パラメータ
#define PARAM16				16	// 第１６パラメータ
#define PARAM17				17	// 第１７パラメータ
#define PARAM18				18	// 第１８パラメータ
#define PARAM19				19	// 第１９パラメータ
#define PARAM20				20	// 第２０パラメータ
#define PARAM21				21	// 第２１パラメータ
#define PARAM22				22	// 第２２パラメータ
#define PARAM23				23	// 第２３パラメータ
#define PARAM24				24	// 第２４パラメータ
#define PARAM25				25	// 第２５パラメータ
#define PARAM26				26	// 第２６パラメータ
#define PARAM27				27	// 第２７パラメータ
#define PARAM28				28	// 第２８パラメータ
#define PARAM29				29	// 第２９パラメータ


// IOデータ数
#define INDAT_CNTRL_MAX		8
#define OTDAT_CNTRL_MAX		8

// プラス・マイナス
#define MINUS				1
#define PLUS				0

// Ｈ／Ｌ
#define HIGH				1
#define LOW					0


#define POMPVALUE_BASE 1000
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// タスク・ハンドラ設定

//*************************************
// タスクＩＤ
//*************************************
#define ID_TSK_RX0          1           // 制御ＰＣ通信受信タスク
#define ID_TSK_TX0          2           // 制御ＰＣ通信送信タスク
#define ID_TSK_RXEX         3           // 通信受信タスク
#define ID_TSK_TXEX         4           // 通信送信タスク
#define ID_TSK_CMD          5           // コマンド実行タスク
#define ID_TSK_EEP          6           // EEPROM実行タスク
#define ID_TSK_IO           7           // 入力監視タスク
#define ID_TSK_MOTWAIT      8           // モータ動作前ディレイタスク
#define ID_TSK_MOTACT       9           // モータ動作制御タスク
#define ID_TSK_MOTEND      10           // モータ動作終了監視タスク
#define ID_TSK_INT         11           // INTコマンド監視用タスク
#define ID_TSK_LED         12           // 
#define ID_TSK_PUNIT       13
#define ID_TSK_TMPCNT      14
#define TSK_NUM            14           // タスク数

//*************************************
// ハンドラＩＤ
//*************************************
#define ID_HDR_CYC          1           // 周期起床ハンドラ
#define HDR_NUM             2           // ハンドラ数 @+1

//*************************************
// 割り込みサービスルーチン
//*************************************
#define ID_ISR_RX0          1
#define ID_ISR_ER0          2
#define ID_ISR_RX2          3
#define ID_ISR_ER2          4

//*************************************
// データキュー
//*************************************
#define ID_DTQ_RXD          1           // SIO0 受信データキュー
#define ID_DTQ_RXEX         2           // SIO拡張 受信データキュー
#define DTQ_NUM             3           // 

//*************************************
// メールBOX
//*************************************
#define ID_MBX_RX0          1           // 
#define ID_MBX_TX0          2           // SIO0 送信データ
#define ID_MBX_RXEX         3           // 
#define ID_MBX_TXEX         4           // SIO拡張 送信データ
#define ID_MBX_CMD          5           // COM  コマンド
#define ID_MBX_EEP          6           // 
#define ID_MBX_IO           7           // ＩＯ監視
#define ID_MBX_MOTWAIT      8           // 
#define ID_MBX_MOTACT       9           // モータ制御
#define ID_MBX_MOTEND      10           // 
#define ID_MBX_INT         11           // INTコマンド監視
#define ID_MBX_LED         12           // 
#define ID_MBX_PUNIT       13           // 
#define ID_MBX_TMPCNT      14           // 
#define MBX_NUM      	   14           // 

//*************************************
// メモリプール
//*************************************
//#define ID_MPF_RS0          1           // ->SIO0
#define ID_MPF_TX0          2           // SIO0 送信データ
#define ID_MPF_TXEX         3           // ->SIO2
#define ID_MPF_CMD          4           // 
#define ID_MPF_EEP          5           // 
#define ID_MPF_IO           6           // 
#define ID_MPF_MOTWAIT      7           // 
#define ID_MPF_MOTACT       8           // 
#define ID_MPF_MOTEND       9           // 
#define ID_MPF_INT          10          // 
#define ID_MPF_LED          11          // 
#define ID_MPF_PUNIT        12          // 
#define ID_MPF_TMPCNT       13          // 
#define MPF_NUM     	    13          // プール数

//*************************************
// セマフォ
//*************************************
#define ID_SEM_INIT0		1 			// C-INIT0
#define ID_SEM_PAUSE		2 			// C-PAUSE@
#define ID_SEM_HDMOV		3 			// C-HDMOV
#define ID_SEM_TPGET		4 			// C-TPGET
#define ID_SEM_TPRLS		5 			// C-TPRLS
#define ID_SEM_DROPN		6 			// C-DROPN
#define ID_SEM_SKSET		7 			// C-SKSET
#define ID_SEM_SUCK0		8 			// C-SUCK0
#define ID_SEM_DTSET		9 			// C-DTSET
#define ID_SEM_DOT00		10			// C-DOT00
#define ID_SEM_MXSET		11			// C-MXSET
#define ID_SEM_MIX00		12			// C-MIX00
#define ID_SEM_THMOV		13			// C-THMOV
#define ID_SEM_CPOPN		14			// C-CPOPN
#define ID_SEM_CPCLS		15			// C-CPCLS
#define ID_SEM_BCDSP		16			// C-BCDSP
#define ID_SEM_TURN1		17			// C-TURN1
#define ID_SEM_BZCNT		18			// C-BZCNT
#define ID_SEM_LEDON		19			// C-LEDON
#define ID_SEM_MEMRW		20			// C-MEMRW
#define SEM_NUM       (ID_SEM_MEMRW+1)  // セマフォ数

//*************************************
// アラームハンドラ
//*************************************
#define ID_ALM_R0DAT        1           // SIO0 データ受信タイムアウト処理
#define ID_ALM_R0RSP        2           // SIO0 レスポンス受信タイムアウト処理
#define ID_ALM_R0ACK        3           // 
#define ID_ALM_R2DAT        4           // SIO2 データ受信タイムアウト処理
#define ID_ALM_R2RSP        5           // SIO2 レスポンス受信タイムアウト処理
#define ID_ALM_R2ACK        6           // 
#define ID_ALM_CPOPN        7           // 
#define ID_ALM_BARRD        8           // 
#define ALM_NUM     	    9           // アラーム数
							
//*************************************
// メッセージＩＤ
//*************************************

// RX0 TASK -> CMD TASK

// 基本コマンド
                                        /*==========================================================*/
                                        /* No. Command 内容                                         */
                                        /*==========================================================*/
#define	ID_MSG_HELLO		0           /*  HELLO   起動通知                                      */
#define	ID_MSG_LOG			1           /*  reserve                                               */
#define	ID_MSG_VER			2           /*  VER     バージョン確認                                */
#define	ID_MSG_ALLIN		3           /*  ALLIN   全I/O入力                                     */
#define	ID_MSG_BIN			4           /*  BIN     I/O入力                                       */
#define	ID_MSG_BOUT			5           /*  BOUT    I/O出力                                       */
#define	ID_MSG_CINIT		6           /*  CINIT   入力カウンタの設定                            */
#define	ID_MSG_COUNT		7           /*  COUNT   入力カウンタの読み出し                        */
#define	ID_MSG_INT			8           /*  INT     割り込み設定                                  */
#define	ID_MSG_ABSMV		9           /*  ABSMV   パルスモータ移動(絶対位置)                    */
#define	ID_MSG_ABORT		10          /*  ABORT   パルスモータ即停止                            */
#define	ID_MSG_ENCODER		11          /*  ENCODER エンコーダの設定                              */
#define	ID_MSG_LIMIT		12          /*  LIMIT   パルスモータ原点復帰                          */
#define	ID_MSG_MAP			13          /*  MAP     ２軸のパルスモータ移動                        */
#define	ID_MSG_PDELAY		14          /*  PDELAY  パルスモータのパワー制御待ち時間設定          */
#define	ID_MSG_POWER		15          /*  POWER   パルスモータの励磁制御                        */
#define	ID_MSG_PSET			16          /*  PSET    パルスモータのカウンタリセット                */
#define	ID_MSG_RELMV		17          /*  RELMV   パルスモータ移動(相対位置)                    */
#define	ID_MSG_SET			18          /*  SET     パルスモータ速度設定                          */
#define	ID_MSG_STATUS		19          /*  STATUS  パルスモータ状態読み出し                      */
#define	ID_MSG_STOP			20          /*  STOP    パルスモータ減速停止                          */
#define	ID_MSG_BZCNT		21          /*  BZCNT                                                 */
#define	ID_MSG_EEPREAD		22          /*  EEPREAD   */
#define	ID_MSG_EEPWRIT		23          /*  EEPWRIT   */
#define	ID_MSG_INTRPT		24          /*  INTERRUPT                                             */
#define	ID_MSG_TMPSET_HS	26          /*                                                        */
#define	ID_MSG_TMPGET_HS	27          /*                                                        */
                                        /*                                                        */
#define	ID_MSG_VALVEINI_HS	29          /*                                                        */
#define	ID_MSG_PRESSINI_HS	30          /*                                                        */
#define	ID_MSG_PRESSREN_HS	31          /*                                                        */
#define	ID_MSG_PRESSTAN_HS	32          /*                                                        */
#define	ID_MSG_PRESSCAN_HS	33          /*                                                        */
#define	ID_MSG_VALVEOUT_HS	34          /*                                                        */
#define	ID_MSG_PRESSURE_HS	35          /*                                                        */
#define	ID_MSG_IMPPDAT		36          /*                                                        */
#define	ID_MSG_IMPSPED		37          /*                                                        */
#define	ID_MSG_IMPPOSI		38          /*                                                        */
#define	ID_MSG_IMPOPTN		39          /*                                                        */
#define	ID_MSG_EXPRUNL		40          /*                                                        */
#define	ID_MSG_EXPCOML		41          /*                                                        */
#define	ID_MSG_EXPPOSI		42          /*                                                        */
#define	ID_MSG_EXPOPTN		43          /*                                                        */
#define	ID_MSG_EXPCLOG		44          /*                                                        */





#define	ID_MSG_CBZCNT		50          /*  C-BZCNT                                               */
#define	ID_MSG_CLEDON		51          /*  C-LEDON                                               */

#define	ID_MSG_BASE_CMD_MAX	45          /* 基本コマンド総数                                       */


//Response
#define	ID_MSG_CMD			100         /* コマンドレスポンス                                       */
#define	ID_MSG_ACK			101         /* ＡＣＫ                                                   */
#define	ID_MSG_NAK			102         /* ＮＡＫ                                                   */
#define	ID_MSG_CMD_BAR		103         /* コマンドレスポンス                                       */
#define	ID_MSG_RUNLOGERR	104          /*  */

// etc
#define	ID_MSG_VALVEINI	150          /*  */
#define	ID_MSG_PRESSINI	151          /*  */
#define	ID_MSG_PRESSREN	152          /*  */
#define	ID_MSG_PRESSTAN	153          /*  */
#define	ID_MSG_PRESSCAN	154          /*  */
#define	ID_MSG_VALVEOUT	155          /*  */
#define	ID_MSG_PRESSURE	156          /*  */
#define	ID_MSG_TMPSET	157          /*  */
#define	ID_MSG_TMPGET	158          /*  */
#define	ID_MSG_TMPGETR	159          /*  */



// etc
#define	ID_MSG_TIMWAIT		200         /* ＷＡＩＴ                                                 */
#define	ID_MSG_EEPWRIT_STS	201         /* デバイス状態書込み（応答不要）                           */



//CMD TASK->COUNT TASK



//  -> MOT TASK
#define ID_MSG_CNT_START	0
#define ID_MSG_CNT_STOP		1


//*************************************
// タッチパネルＩＤ
//*************************************
#if 0
enum TPID_KIND
{
	TPID_FWVER = 0x01,
	TPID_OPTIF = 0x02,
	TPID_LGIN1 = 0x03,
	TPID_LGIN2 = 0x04,
	TPID_INITA = 0x05
};
#endif
#if 1
#define	ID_TPL_FWVER		0x01        /* FWバージョン情報                                         */
#define	ID_TPL_OPTIF		0x02        /* オプション情報                                           */
#define	ID_TPL_LGIN1		0x03        /* ログイン認証1                                            */
#define	ID_TPL_LGIN2		0x04        /* ログイン認証2                                            */
#define	ID_TPL_INITA		0x05        /* 初期化指示                                               */
#define	ID_TPL_TITLE		0x06        /* プロトコルタイトル要求                                   */
#define	ID_TPL_PREXT		0x07        /* プロトコル有無                                           */
#define	ID_TPL_USRAL		0x08        /* ユーザー情報全取得                                       */
#define	ID_TPL_INF01		0x11        /* ユーザー1情報                                            */
#define	ID_TPL_INF02		0x12        /* ユーザー2情報                                            */
#define	ID_TPL_INF03		0x13        /* ユーザー3情報                                            */
#define	ID_TPL_INF04		0x14        /* ユーザー4情報                                            */
#define	ID_TPL_INF05		0x15        /* ユーザー5情報                                            */
#define	ID_TPL_INF06		0x16        /* ユーザー6情報                                            */
#define	ID_TPL_INF07		0x17        /* ユーザー7情報                                            */
#define	ID_TPL_INF08		0x18        /* ユーザー8情報                                            */
#define	ID_TPL_INF09		0x19        /* ユーザー9情報                                            */
#define	ID_TPL_INF10		0x1A        /* ユーザー10情報                                           */
#define	ID_TPL_REC01		0x21        /* ユーザー1登録                                            */
#define	ID_TPL_REC02		0x22        /* ユーザー2登録                                            */
#define	ID_TPL_REC03		0x23        /* ユーザー3登録                                            */
#define	ID_TPL_REC04		0x24        /* ユーザー4登録                                            */
#define	ID_TPL_REC05		0x25        /* ユーザー5登録                                            */
#define	ID_TPL_REC06		0x26        /* ユーザー6登録                                            */
#define	ID_TPL_REC07		0x27        /* ユーザー7登録                                            */
#define	ID_TPL_REC08		0x28        /* ユーザー8登録                                            */
#define	ID_TPL_REC09		0x29        /* ユーザー9登録                                            */
#define	ID_TPL_REC10		0x2A        /* ユーザー10登録                                           */
#define	ID_TPL_CHG01		0x31        /* ユーザー1変更                                            */
#define	ID_TPL_CHG02		0x32        /* ユーザー2変更                                            */
#define	ID_TPL_CHG03		0x33        /* ユーザー3変更                                            */
#define	ID_TPL_CHG04		0x34        /* ユーザー4変更                                            */
#define	ID_TPL_CHG05		0x35        /* ユーザー5変更                                            */
#define	ID_TPL_CHG06		0x36        /* ユーザー6変更                                            */
#define	ID_TPL_CHG07		0x37        /* ユーザー7変更                                            */
#define	ID_TPL_CHG08		0x38        /* ユーザー8変更                                            */
#define	ID_TPL_CHG09		0x39        /* ユーザー9変更                                            */
#define	ID_TPL_CHG10		0x3A        /* ユーザー10変更                                           */
#define	ID_TPL_DEL01		0x41        /* ユーザー1削除                                            */
#define	ID_TPL_DEL02		0x42        /* ユーザー2削除                                            */
#define	ID_TPL_DEL03		0x43        /* ユーザー3削除                                            */
#define	ID_TPL_DEL04		0x44        /* ユーザー4削除                                            */
#define	ID_TPL_DEL05		0x45        /* ユーザー5削除                                            */
#define	ID_TPL_DEL06		0x46        /* ユーザー6削除                                            */
#define	ID_TPL_DEL07		0x47        /* ユーザー7削除                                            */
#define	ID_TPL_DEL08		0x48        /* ユーザー8削除                                            */
#define	ID_TPL_DEL09		0x49        /* ユーザー9削除                                            */
#define	ID_TPL_DEL10		0x4A        /* ユーザー10削除                                           */
#define	ID_TPL_FWPOL		0x50        /* FW状態（ポーリング）                                     */
#define	ID_TPL_PRTCL		0x51        /* プロトコル実行                                           */
#define	ID_TPL_RESTA		0x52        /* リスタート実行                                           */
#define	ID_TPL_PAUSE		0x53        /* 一時停止                                                 */
#define	ID_TPL_RESUM		0x54        /* 再開                                                     */
#define	ID_TPL_ABORT		0x55        /* 中断                                                     */
#define	ID_TPL_OPTCD		0x56        /* オプションバーコード実行                                 */
#define	ID_TPL_SMBAR		0x57        /* サンプルチューブQRコード                                 */
#define	ID_TPL_SMSGL		0x58        /* サンプルチューブ有無確認（単）                           */
#define	ID_TPL_SMALL		0x59        /* サンプルチューブ有無確認（全）                           */
#define	ID_TPL_CLQRC		0x5A        /* 回収チューブQRコード                                     */
#define	ID_TPL_CLSGL		0x5B        /* 回収チューブ有無（単）                                   */
#define	ID_TPL_CLALL		0x5C        /* 回収チューブ有無（全）                                   */
#define	ID_TPL_CLLNE		0x5D        /* 回収チューブレーンQRコード                               */
#define	ID_TPL_LDLMP		0x60        /* LED                                                      */
#define	ID_TPL_UVLMP		0x61        /* UV点灯                                                   */
#define	ID_TPL_PTLMP		0x62        /* パトランプ                                               */
#define	ID_TPL_POWOF		0x63        /* 電源OFF                                                  */
#define	ID_TPL_EXRUN		0x6B        /* 運転に歴要求                                             */
#define	ID_TPL_PRINF		0x6E        /* プロトコル情報要求                                       */

#endif


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// 割り込み系設定

//*************************************
// 割り込み優先順位
//*************************************
#define INT_PRI_SCI0        7           // SCI0
#define INT_PRI_SCI1        7           // SCI1
#define INT_PRI_SCI2        7           // SCI2
#define INT_PRI_SCI3        7           // SCI3
#define INT_PRI_IRQ4        6           // IRQ4

//*************************************
// 割り込み禁止マスク
//*************************************
#define CPU_INT_DISABLE     9

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// シリアル通信

//*************************************
// 通信ポート番号
//*************************************
#define SIO_PC              0          // ＰＣ通信ポートの番号
#define SIO_EX1             1          // 
#define SIO_EX2             2          // 

//*************************************
// ポートオープン／クローズ
//*************************************
#define SIO_CLOSE           0
#define SIO_OPEN            1

//*************************************
// 制御コード
//*************************************
#define ACK                 0x06
#define NAK                 0x15
#define STX                 0x02
#define ETX                 0x03
#define CRCHR               0x0d
#define LFCHR               0x0a

//*************************************
// コマンドデータ･バッファサイズ
//*************************************
#define CMD_BUFSIZ          60			// コマンド受信バッファサイズ
#define COM_BUFSIZ          128			// 通信データ受信バッファサイズ

//*************************************
// パラメータ最大数
//*************************************
//#define MAX_ARGS            36
#define MAX_ARGS            100

//*************************************
// 受信データタイプ
//*************************************
#define COM_MTYP_CHR        0          // 正常受信：キャラクタ
#define COM_MTYP_ORN        1          // エラー：オーバーランエラー
#define COM_MTYP_FER        2          // エラー：フレーミングエラー
#define COM_MTYP_PER        3          // エラー：パリティエラー
#define COM_MTYP_RTMO       4          // エラー：受信タイムアウト
#define COM_MTYP_STMO       5          // エラー：送信タイムアウト
#define COM_MTYP_ERR        6          // エラー：その他エラー
#define COM_MTYP_BDSTART    10          // 
#define COM_MTYP_BDEND      11          // 

//*************************************
// ＰＣ通信タイムアウト
//*************************************
#if DEBUG_SIM_MODE == 2 
	#define TMOUT_RCV       (3000/1) // 受信タイムアウト
	#define	TMOUT_ACK		(3000/1)
#else
	#define TMOUT_RCV       (3000/1) // 受信タイムアウト
	#define	TMOUT_ACK		(3000/1)
#endif

//*************************************
// 装置通信レスポンスタイムアウト
//*************************************
#define TMOUT_RSP       (3000/1) // 受信タイムアウト
#define TMOUT_BAR       (90000/1) // 受信タイムアウト


//*************************************
// レスポンス種別
//*************************************


//*****************************************
// ステータス
//*****************************************
#define STS00				0x00	// S00 初期化
#define STS01				0x01	// S01 
#define STS02				0x02	// S02 
#define STS03				0x03	// S03 
#define STS04				0x04	// S04 
#define STS05				0x05	// S05 
#define STS10				0x10	// S10 
#define STS20				0x20	// S20 
#define STS30				0x30	// S30 


//*************************************
// ＰＣ通信管理用
//*************************************
#define	RCVCON				0x01
#define	RCVCMD 				0x10	// コマンド受信
#define	RCVACK 				0x11	// ＡＣＫ受信
#define	RCVNAK 				0x12	// ＮＡＫ受信
#define	RCVEND				0x18
#define	BCCERR 				0x83	// エラー
#define	TMOERR 				0x84	// エラー
#define	RCVERR 				0x85	// エラー

#define RTY_OVER			3

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// コマンドタスク

// INTコマンド
#define	TRG_NM				0						// N    解除
#define	TRG_HD				1						// HD  ：High入力状態の間、CW/CCW両方向共に一時停止
#define	TRG_HP				2       				// H+  ：High入力状態の間、CW方向のみ一時停止
#define	TRG_HM				3       				// H-  ：High入力状態の間、CCW方向のみ一時停止
#define	TRG_LD				4       				// LD  ：Low入力状態の間、CW/CCW両方向共に一時停止
#define	TRG_LP				5       				// L+  ：Low入力状態の間、CW方向のみ一時停止
#define	TRG_LM				6       				// L-  ：Low入力状態の間、CCW方向のみ一時停止
#define	TRG_UD				7       				// UD  ：立ち上がりエッジ検出時、CW/CCW両方向共に一時停止
#define	TRG_UP				8       				// U+  ：立ち上がりエッジ検出時、CW方向のみ一時停止
#define	TRG_UM				9       				// U-  ：立ち上がりエッジ検出時、CCW方向のみ一時停止
#define	TRG_DD				10      				// DD  ：立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
#define	TRG_DP				11      				// D+  ：立ち下がりエッジ検出時、CW方向のみ一時停止
#define	TRG_DM				12      				// D-  ：立ち下がりエッジ検出時、CCW方向のみ一時停止
#define	TRG_ED				13      				// ED  ：立ち上がり／立ち下がりエッジ検出時、CW/CCW両方向共に一時停止
#define	TRG_EP				14      				// E+  ：立ち上がり／立ち下がりエッジ検出時、CW方向のみ一時停止
#define	TRG_EM				15      				// E-  ：立ち上がり／立ち下がりエッジ検出時、CCW方向のみ一時停止


//移動量MIN,MAX
#define MVMIN				-2147483648
#define MVMAX				2147483647

// PSET MIN,MAX
#define PSMIN				-32767
#define PSMAX				32767

// SPEED MIN,MAX
#define SPMIN				0
#define SPMAX				100000

#define BCCTRL_MAX			19


// LIMITコマンド
// 待機復帰種別
#define GENTEN_POS			0						// 原点復帰動作
#define NUKEDA_POS			1						// 原点抜け出し動作


// ABSMV,RELMV,LIMIT共通
#define REQ_STAUD			0
#define REQ_STAFH			1
#define REQ_STAFL			2


// 
#define DIS					0						// 受付禁止
#define ENA					1						// 受付許可
#define CHK					2						// モータ動作確認
#define BSY					3						// モータビジー
#define IGN					4						// 無視（規定外）



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// タイマ

#define	INTERVAL_1M			1						// インターバル1msec
#define	INTERVAL_10M		10						// インターバル10msec
#define	INTERVAL_100M		100						// インターバル100msec
#define	INTERVAL_1S			1000					// インターバル1sec
#define	DLY_1M				1						// 1msec
#define	DLY_4M				4						// 4msec
#define	DLY_10M				10						// 10msec
#define	DLY_50M				50						// 50msec
#define	DLY_100M			100						// 100msec
#define	WAITTIM				60						// 60μmsec
#define	TIM_AIR_VAl			200						// 空気弁監視2sec(10msec単位)



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// ＥＥＰＲＯＭ制御

// tskeep.h参照
#if ( EEP_RWSIZE_TYPE == 0 )
#define EEP_RWSIZE			1
#else
#define EEP_RWSIZE			120
#endif



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// モーションネット
#define SYS_RTY_OVER		10






//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// 実行処理

#define	MNET_CMD_RTY_MAX	120	
#define	READ_RTY_MAX		3
#define	WRITE_RTY_MAX		3




//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// エラー


// エラーコード
// 000
#define	E_SUCCESS		0x00000000		// 0000 正常終了
#define	E_IDLING		0x00000000		// 0000 アイドリング終了
#define	E_LIQSUCEND		0x00000001		// 0001 液吸引完了
#define	E_LIQDETECT		0x00000002		// 0002 液面検知成功
#define	E_PROTCOLSTOP	0x00000003		// 0003 プロトコル中断中
#define	E_PROTCOLPAUSE	0x00000004		// 0004 プロトコル一時停止中
#define	E_MIDDLEEND		0x00000080		// 0080 動作途中終了
//                     ～0x000000BF		// 00BF 動作途中終了
#define	E_CONTINUE		0x000000F1		// 00F1 処理中・処理継続中（※ファーム内部でのみ利用）
#define	E_IGNORE		0x000000F1		// 00F2 無視

#define	E_PRES_FAIL		0x00000020		// 加圧失敗
#define	E_PRES_LEAK		0x00000021		// 液抜け
#define	E_RECV_NONE		0x00000030		// 未受信
#define	E_SEND_FAIL		0x00000031		// 送信失敗
#define	E_RECV_FAIL		0x00000032		// 受信失敗
#define	E_TEMP_FAIL		0x000000FF		// 温度データ取得失敗





// 100
#define	E_BUSY			0x00000100		// ビジー先頭
#define E_BSY_HEAD_X1	0x00000101		// 0101 分注X軸ビジー
#define E_BSY_HEAD_Y1	0x00000102		// 0102 分注Y軸ビジー
#define E_BSY_HEAD_Z1	0x00000103		// 0103 分注Z1軸ビジー
#define E_BSY_HEAD_Z2	0x00000104		// 0104 分注Z2軸ビジー
#define E_BSY_HEAD_P1	0x00000105		// 0105 ポンプ1軸ビジー
#define E_BSY_HEAD_P2	0x00000106		// 0106 ポンプ2軸ビジー
#define E_BSY_HEAD_PT	0x00000107		// 0107 ピッチ軸ビジー
#define E_BSY_KAIS_Z1	0x00000108		// 0108 開閉栓Z軸ビジー
#define E_BSY_KAIS_R1	0x00000109		// 0109 開閉栓R1軸ビジー
#define E_BSY_KAIS_R2	0x0000010A		// 010A 開閉栓R2軸ビジー
#define E_BSY_EXT_BAR	0x0000010B		// 010B 突き出し棒軸ビジー
#define E_BSY_HANS_X1	0x0000010C		// 010C 搬送X軸ビジー
#define E_BSY_HANS_Y1	0x0000010D		// 010D 搬送Y軸ビジー
#define E_BSY_HANS_Z1	0x0000010E		// 010E 搬送Z軸ビジー
#define E_BSY_HANS_R1	0x0000010F		// 010F 搬送R軸ビジー
#define E_BSY_TURNTBL	0x00000110		// 0110 テーブル軸ビジー
#define E_BSY_BARMOV1	0x00000111		// 0111 バーコード1可動軸ビジー
#define E_BSY_BARMOV2	0x00000112		// 0112 バーコード2可動軸ビジー
										// 0113 予約
										// ～   予約
										// 013F 予約
#define	E_BSY_CMDDIS	0x00000140		// 0140 排他


// 200
#define	E_CUR_ERR		0x00000200		// 0200 励磁失敗
#define E_CUR_HEAD_X1	0x00000201		// 分注X軸励磁失敗
#define E_CUR_HEAD_Y1	0x00000202		// 分注Y軸励磁失敗
#define E_CUR_HEAD_Z1	0x00000203		// 分注Z1軸励磁失敗
#define E_CUR_HEAD_Z2	0x00000204		// 分注Z2軸励磁失敗
#define E_CUR_HEAD_P1	0x00000205		// ポンプ1軸励磁失敗
#define E_CUR_HEAD_P2	0x00000206		// ポンプ2軸励磁失敗
#define E_CUR_HEAD_PT	0x00000207		// ピッチ軸励磁失敗
#define E_CUR_KAIS_Z1	0x00000208		// 開閉栓Z軸励磁失敗
#define E_CUR_KAIS_R1	0x00000209		// 開閉栓R1軸励磁失敗
#define E_CUR_KAIS_R2	0x0000020A		// 開閉栓R2軸励磁失敗
#define E_CUR_EXT_BAR	0x0000020B		// 突き出し棒軸励磁失敗
#define E_CUR_HANS_X1	0x0000020C		// 搬送X軸励磁失敗
#define E_CUR_HANS_Y1	0x0000020D		// 搬送Y軸励磁失敗
#define E_CUR_HANS_Z1	0x0000020E		// 搬送Z軸励磁失敗
#define E_CUR_HANS_R1	0x0000020F		// 搬送R軸励磁失敗
#define E_CUR_TURNTBL	0x00000210		// テーブル軸励磁失敗
#define E_CUR_BARMOV1	0x00000211		// バーコード1可動軸励磁失敗
#define E_CUR_BARMOV2	0x00000212		// バーコード2可動軸励磁失敗
										// 0213 予約
										// ～   予約
										// 023F 予約

// 300
#define	E_DTY_ERR		0x00000300		// 0300 脱調
#define E_DTY_HEAD_X1	0x00000301		// 分注X軸脱調
#define E_DTY_HEAD_Y1	0x00000302		// 分注Y軸脱調
#define E_DTY_HEAD_Z1	0x00000303		// 分注Z1軸脱調
#define E_DTY_HEAD_Z2	0x00000304		// 分注Z2軸脱調
#define E_DTY_HEAD_P1	0x00000305		// ポンプ1軸脱調
#define E_DTY_HEAD_P2	0x00000306		// ポンプ2軸脱調
#define E_DTY_HEAD_PT	0x00000307		// ピッチ軸脱調
#define E_DTY_KAIS_Z1	0x00000308		// 開閉栓Z軸脱調
#define E_DTY_KAIS_R1	0x00000309		// 開閉栓R1軸脱調
#define E_DTY_KAIS_R2	0x0000030A		// 開閉栓R2軸脱調
#define E_DTY_EXT_BAR	0x0000030B		// 突き出し棒軸脱調
#define E_DTY_HANS_X1	0x0000030C		// 搬送X軸脱調
#define E_DTY_HANS_Y1	0x0000030D		// 搬送Y軸脱調
#define E_DTY_HANS_Z1	0x0000030E		// 搬送Z軸脱調
#define E_DTY_HANS_R1	0x0000030F		// 搬送R軸脱調
#define E_DTY_TURNTBL	0x00000310		// テーブル軸脱調
#define E_DTY_BARMOV1	0x00000311		// バーコード1可動軸脱調
#define E_DTY_BARMOV2	0x00000312		// バーコード2可動軸脱調


// 340
#define	E_LMT_ERR		0x00000340		// 0340 リミット
#define E_LMT_HEAD_X1	0x00000341		// 分注X軸脱調
#define E_LMT_HEAD_Y1	0x00000342		// 分注Y軸脱調
#define E_LMT_HEAD_Z1	0x00000343		// 分注Z1軸脱調
#define E_LMT_HEAD_Z2	0x00000344		// 分注Z2軸脱調
#define E_LMT_HEAD_P1	0x00000345		// ポンプ1軸脱調
#define E_LMT_HEAD_P2	0x00000346		// ポンプ2軸脱調
#define E_LMT_HEAD_PT	0x00000347		// ピッチ軸脱調
#define E_LMT_KAIS_Z1	0x00000348		// 開閉栓Z軸脱調
#define E_LMT_KAIS_R1	0x00000349		// 開閉栓R1軸脱調
#define E_LMT_KAIS_R2	0x0000034A		// 開閉栓R2軸脱調
#define E_LMT_EXT_BAR	0x0000034B		// 突き出し棒軸脱調
#define E_LMT_HANS_X1	0x0000034C		// 搬送X軸脱調
#define E_LMT_HANS_Y1	0x0000034D		// 搬送Y軸脱調
#define E_LMT_HANS_Z1	0x0000034E		// 搬送Z軸脱調
#define E_LMT_HANS_R1	0x0000034F		// 搬送R軸脱調
#define E_LMT_TURNTBL	0x00000350		// テーブル軸脱調
#define E_LMT_BARMOV1	0x00000351		// バーコード1可動軸脱調
#define E_LMT_BARMOV2	0x00000352		// バーコード2可動軸脱調


// 400
#define	E_CON_ERR		0x00000400		// 接続異常
#define E_CON_HEAD_Y0	0x00000401		// 分注Y軸接続異常
#define E_CON_HEAD_X1	0x00000401		// 分注X軸接続異常
#define E_CON_HEAD_Y1	0x00000402		// 分注Y軸接続異常
#define E_CON_HEAD_Z1	0x00000403		// 分注Z1軸接続異常
#define E_CON_HEAD_Z2	0x00000404		// 分注Z2軸接続異常
#define E_CON_HEAD_P1	0x00000405		// ポンプ1軸接続異常
#define E_CON_HEAD_P2	0x00000406		// ポンプ2軸接続異常
#define E_CON_HEAD_PT	0x00000407		// ピッチ軸接続異常
#define E_CON_KAIS_Z1	0x00000408		// 開閉栓Z軸接続異常
#define E_CON_KAIS_R1	0x00000409		// 開閉栓R1軸接続異常
#define E_CON_KAIS_R2	0x0000040A		// 開閉栓R2軸接続異常
#define E_CON_EXT_BAR	0x0000040B		// 突き出し棒軸接続異常
#define E_CON_HANS_X1	0x0000040C		// 搬送X軸接続異常
#define E_CON_HANS_Y1	0x0000040D		// 搬送Y軸接続異常
#define E_CON_HANS_Z1	0x0000040E		// 搬送Z軸接続異常
#define E_CON_HANS_R1	0x0000040F		// 搬送R軸接続異常
#define E_CON_TURNTBL	0x00000410		// テーブル軸接続異常
#define E_CON_BARMOV1	0x00000411		// バーコード1可動軸接続異常
#define E_CON_BARMOV2	0x00000412		// バーコード2可動軸接続異常
										// 0413 予約
										// ～   予約
										// 043F 予約
#define	E_CON_SYSCOM	0x00000440		// 0440 システム通信失敗
#define	E_CON_IO_COM	0x00000441		// 0441 IO通信失敗


// 500
#define	E_MNT_ACCESS	0x00000500		// 0500～05FF MNETレジスタアクセス異常

// 600
#define	E_COM_CTRLPC	0x00000600		// 0600 制御ＰＣ通信接続異常
#define	E_COM_BARCOD	0x00000602		// 0602 バーコード通信接続異常
#define	E_COM_P_OPEN	0x00000604		// 0604 ポートオープン済み
#define	E_COM_P_NONE	0x00000605		// 0605 ポート未オープン
#define	E_COM_BCCERR	0x00000606		// 0606 BCC不一致
#define	E_COM_CMDDEF	0x00000610		// 0610 コマンド未定義
#define	E_COM_SPITMO	0x00000621		// 0621 SPI通信タイムアウト
#define	E_COM_SPIOVR	0x00000622		// 0621 SPI通信オーバーラン
// 700
#define	E_PAR_SHORTG	0x00000700		// 07xx パラメータ過不足
// 800
#define	E_PAR_OUTRNG	0x00000800		// 08xx パラメータ範囲外
// 900
#define E_BAR_READNG 	0x00000900 		// 0900 バーコード読込失敗 読取り数不一致
#define E_BAR_RACKNG 	0x00000901 		// 0901 バーコード読込失敗 ラックＮＧ 
#define E_BAR_TIMOUT 	0x00000902 		// 0902 バーコード読込失敗 タイムアウト
#define E_BAR_POS1NG 	0x00000911 		// 0911 バーコード読込失敗 ポジション順序不正 

// A00
#define	E_TIP_NTHING	0x00000A00		// 0A00 チップ取得失敗
#define	E_TIP_EXISTS	0x00000A01		// 0A01 チップ有り
#define	E_TIP_DPFALL	0x00000A02		// 0A02 チップ脱落
#define	E_TIP_GETING	0x00000A03		// 0A03 チップ取得済み
//#define	E_TUB_PICARI	0x00000A08		// 0A08 (ピックアップ)チューブ有り
//#define	E_TUB_PICNAS	0x00000A09		// 0A09 (ピックアップ)チューブ無し
//#define	E_TUB_RLSNAS	0x00000A0A		// 0A0A (リリース)チューブ無し
//#define	E_TUB_RLSARI	0x00000A0B		// 0A0B (リリース)チューブ有り
//#define	E_TUB_DRPOUT	0x00000A0C		// 0A0C チューブ脱落
//#define	E_TUB_HNDSNS	0x00000A0D		// 0A0D チューブ搬送ハンドセンサ異常
//#define	E_CAP_TUBNAS	0x00000A10		// 0A10 開栓チューブ無し
//#define	E_CAP_TUBLUP	0x00000A11		// 0A11 開栓チューブ持ち上げ
//#define	E_CAP_SENSOR	0x00000A12		// 0A12 開栓センサ異常
//#define	E_CAP_DSPNON	0x00000A13		// 0A13 廃棄キャップ無し
//#define	E_CAP_FAILUR	0x00000A18		// 0A18 開栓失敗


#define	E_LIQ_FAILUR	0x00000A20		// 0A20 液面検知失敗（ 未検出 ）
#define	E_LIQ_JAMING	0x00000A21		// 0A21 詰まり検知
#define	E_LIQ_SHORTG	0x00000A22		// 0A22 液量不足( 泡噛み )
#define	E_NZL_COLLIS	0x00000A30		// 0A30 ノズル衝突
#define	E_DRP_OPENNG	0x00000A31		// 0A31 ドロップキャッチャOPENエラー
#define	E_DRP_CLSENG	0x00000A32		// 0A32 ドロップキャッチャCLOSEエラー
// B00
#define	E_AIR_EMPTY		0x00000B00		// 0B00 エアエンプティ
// B10
#define	E_EEP_ERR		0x00000B10		// 0B00 EEPROMエラー
#define	E_EEP_WRTERR	0x00000B10		// 0B00 EEPROM書込みエラー
#define	E_EEP_REDERR	0x00000B11		// 0B01 EEPROM読込みエラー
// F000
#define	E_SYS_MLTACT	0x0000F000		// F000 マルチタスクシステム動作異常
#define	E_SYS_TSKHDL	0x0000F001		// F001 タスク・ハンドラ生成異常
#define	E_SYS_MEMPOL	0x0000F002		// F002 メモリプール生成異常
#define	E_SYS_MAILQU	0x0000F003		// F003 メール・キュー生成異常
#define	E_SYS_SEMAPH	0x0000F004		// F004 セマフォ生成異常
#define	E_SYS_TSKSTA	0x0000F005		// F005 タスク起動異常
#define	E_SYS_CYCHDL	0x0000F006		// F006 周期ハンドラ起動異常
#define	E_SYS_DATERR	0x0000F007		// F007 データ整合異常
#define	E_SYS_MSGCOM	0x0000F008		// F008 メッセージ通知異常
#define	E_SYS_POLGET	0x0000F009		// F009 メモリプール取得失敗
#define	E_SYS_POLREF	0x0000F00A		// F00A メモリプール解放失敗
#define	E_SYS_FNCCAL	0x0000F00B		// F00B ファンクションコール異常


//--------------------------------------------------------------------------------------------------
// ＬＥＤ表示
#define	LED_ADDR		0x0C00F020			// LEDアドレス



//--------------------------------------------------------------------------------------------------
// ７ＳＥＧ表示

#define	SEG_CLR			0x00				/* ７ＳＥＧクリア */
#define	ALL_BLINK		0xFF				/* ７ＳＥＧ全消灯 */
#define	SEG7_ADDR		0x0C00F028			// 7SEGアドレス


//--------------------------------------------------------------------------------------------------
// 装置定数
#define MOTOR_CW        1
#define MOTOR_CCW       2

#define	MTMIN			0x00000000			// モーター割り当て最小値(モータ０)
#define	MTMAX			0xffffffff			// モーター割り当て最大値(モータ３１個分)


//--------------------------------------------------------------------------------------------------
// 周期










//--------------------------------------------------------------------------------------------------
// コマンド別




// [C-TPGET] --------------------------
// チップ有無
#define TP1_PRE			0x01
#define TP2_PRE			0x02
#define TPW_PRE			( TP1_PRE | TP2_PRE )

#define TP1_AFT			0x10
#define TP2_AFT			0x20
#define TPW_AFT			( TP1_AFT | TP2_AFT )

#define TPN_TARGET		0x00
#define TP1_TARGET		0x01
#define TP2_TARGET		0x02
#define TPW_TARGET		( TP1_TARGET | TP2_TARGET )


// [C-BCDSP] --------------------------
// 動作指定
#define BCD_QR			0		// 0:QR
#define BCD_39			1		// 1:code39
#define BCD_IT			2		// 2:ITF
#define BCD_12			3		// 3:code128
#define BCD_JN			4		// 4:JAN/EAN/UPC

// 種類
#define RACK_ID			0		// 0:ラックＩＤ
#define POSI_ID			1		// 1:ポジションＩＤ
#define TUBE_ID			2		// 2:チューブＩＤ

#define RACK_LEN		3		
#define POSI_LEN		4		
#define TUBE_LEN8		8		
#define TUBE_LEN9		9		
#define TUBE_LEN10		10		
#define TUBE_LEN11		11		
#define TUBE_LEN12		12		
#define TUBE_LEN13		13		
#define TUBE_LEN14		14		
#define TUBE_LEN15		15		
#define TUBE_LEN16		16		

// [C-BZCNT] --------------------------
// ﾊﾟﾄﾗｲﾄ点灯ﾊﾟﾀｰﾝ
#define PATOF			0x00	// 0:消灯
#define PATON			0x01	// 1:点灯
#define PATBL1			0x02	// 2:点滅(700msec)
#define PATBL2			0x03	// 3:点滅(500msec)
#define PATBL3			0x04	// 4:点滅(300msec)
#define PATCN			0x05	// 5:継続(処理継続)

// ﾌﾞｻﾞｰ鳴動ﾊﾟﾀｰﾝ
#define BUZOF			0x00	//  0:無音
#define BUZ01			0x01	//  1:ピー	
#define BUZ02			0x02	//  2:ピーッピーッ(ON:1000msec,OFF:1000msec)	
#define BUZ03			0x03	//  3:ピーッピーッ(ON:300 msec,OFF:700 msec)	
#define BUZ04			0x04	//  4:ピーッピーッ(ON:400 msec,OFF:600 msec)	
#define BUZ05			0x05	//  5:ピーッピーッ(ON:500 msec,OFF:500 msec)	
#define BUZ06			0x06	//  6:ピーッピーッ(ON:600 msec,OFF:400 msec)	
#define BUZ07			0x07	//  7:ピーッピーッ(ON:700 msec,OFF:300 msec)	
#define BUZ08			0x08	//  8:ピーッピーッ(ON:800 msec,OFF:200 msec)	
#define BUZ09			0x09	//  9:ピピッピピッ(ON:200 msec,OFF:300 msec)	
#define BUZ0A			0x0A	// 10:ピーッピピッ	
#define BUZCN			0x0B	// 11:継続(処理継続)


// ﾊﾟﾄﾗｲﾄ色ﾊﾟﾀｰﾝ
#define C_GRN			0		// 0:緑
#define C_YEL			1		// 1:黄
#define C_RED			2		// 2:赤
#define C_CLR			0xff	// 消灯


// [C-LEDON] --------------------------
// LED点灯ﾊﾟﾀｰﾝ
#define LEDOF			0x00	// 0:消灯
#define LEDON			0x01	// 1:点灯
#define LEDBL			0x02	// 2:点滅

// Z動作指示
#define ZERPOS			0		// 分注PZ軸撹拌初期位置移動
#define SUCPOS			1		// 分注PZ軸撹拌「吸引」位置移動     Zpos1 Z1
#define DOTPOS			2		// 分注PZ軸撹拌「吐出」位置移動     Zpos2 Z1
#define BLWPOS			3		// ブローアウトエア高さ移動         Zpos3 Z1
#define SUCSTA			4		// 分注PZ軸撹拌「吸引」開始高さ移動 Zpos4 Z1
#define SUCEND			5		// 分注PZ軸撹拌「吸引」終了高さ移動 Zpos5 Z2
#define DOTSTA			6		// 分注PZ軸撹拌「吐出」終了高さ移動 Zpos4 Z3
#define DOTEND			7		// 分注PZ軸撹拌「吐出」開始高さ移動 Zpos5 Z1
#define TIPPOS			8		// チップタッチ高さ移動             Touchz1 Z1
#define CUTPOS			9		// 液切れ吸引高さ移動               Cutz1 Z1






// タッチパネル通信 --------------------------

#define TP_ID_PROC_MAX	0x70	// 




// EEPROM ID --------------------------
#define EEPROM_SIZE_MAX		128


#define EEPROM_PAGE0_ADR	0x0000		// (   0 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE1_ADR	0x0080		// (   1 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE2_ADR	0x0100		// (   2 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE3_ADR	0x0180		// (   3 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE4_ADR	0x0200		// (   4 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE5_ADR	0x0280		// (   5 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE6_ADR	0x0300		// (   6 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE7_ADR	0x0380		// (   7 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE8_ADR	0x0400		// (   8 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE9_ADR	0x0480		// (   9 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE10_ADR	0x0500		// (  10 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE90_ADR	0x2D00		// (  90 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE91_ADR	0x2D80		// (  91 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE92_ADR	0x2E00		// (  92 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE93_ADR	0x2E80		// (  93 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE94_ADR	0x2F00		// (  94 * EEPROM_SIZE_MAX )

#define EEPROM_PAGE100_ADR	0x3200		// ( 100 * EEPROM_SIZE_MAX )// プロトコルアドレス開始ページアドレス
#define EEPROM_PAGE480_ADR	0xf000		// ( 480 * EEPROM_SIZE_MAX )// プロトコルアドレス最終ページアドレス

#define EEPROM_ALL_INFO		0x00		// Page0～Page1,Page2～Page5,Page6～Page7
#define EEPROM_USER_INFO	0x01		// Page0～Page1
#define EEPROM_SPED_PTRN	0x02		// Page2～Page6
//#define EEPROM_SPED_SETT	0x04		// Page7
#define EEPROM_POSI_INFO	0x04		// Page8～


#define EEPROM_PROT_INFO	0x10		// Page100～




// プロトコル --------------------------

#define	SRAM_START_ADDRESS			0x0C000000
#define PROTCOL_MAX		80
#define NOZZLE_MAX		12
#define PRESSMP_MAX		10
#define PRESSMP_WAIT	20
#define POMPAFT_WAIT	5
#define TEMP_CHK_WAIT	500
#define NEXTSTEP_WAIT	1


#define P_MSK		0x0F
#define S_MSK		0xD0
#define P_STA		0x00
#define P_NON		0x00
#define P_TGT		0x01
#define P_TDS		0x02
#define P_OPN		0x03
#define P_PRS		0x04
#define P_WAI		0x05
#define P_MIX		0x06
#define P_SUC		0x07
#define P_PUT		0x08
#define P_PR1		0x0A
#define P_PR2		0x0B
#define P_INC		0x0C
#define P_MOV		0x0D

#define STRIP_LY	0x00
#define STRIP_EB	0x01
#define STRIP_R1	0x02
#define STRIP_R2	0x03
#define STRIP_R3	0x04
#define STRIP_R4	0x05
#define STRIP_R5	0x06
#define STRIP_R6	0x07
#define STRIP_SP	0x08
#define STRIP_L1	0x09
#define STRIP_S1	0x0A
#define STRIP_S2	0x0B

#define ER_BUSY    		0x00000001
#define ER_MOTOR    	0x00000002
#define ER_TIPGETFAILE	0x00000004
#define ER_TIPDSPFAILE	0x00000008
#define ER_LIQERR		0x00000010
#define ER_LIQTUMARI	0x00000020
#define ER_BARFAILE		0x00000040

#define PROCESS_LYSI	0x01
#define PROCESS_BILD	0x02
#define PROCESS_WASH	0x03
#define PROCESS_ELUT	0x04

#endif  // _DEFEINE_H_
