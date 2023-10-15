/******************************************************************************
* 構造体定義                                                                  *
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
// コマンドメッセージ
//*************************************
typedef struct {
    INT     tskid;                     	// タスクＩＤ          4
    UB      prid1;                      // 情報                1
    UB      prid2;                      // 情報                1
    UB      bcid;                       // ブロックコマンドＩＤ1
    UB      msgid;                      // メッセージ          1
    UB      inf;                        // 情報                1
    UB      inf2;                       // 情報                1
    UB      inf3;                       // 情報                1
    UB      inf4;                       // 情報                1
    INT     siz;                        //                     4
    UB      data[CMD_BUFSIZ];           // データ(stx.....bcc) 60
}T_MSG_DAT;

// コマンド･コマンド応答バッファ
typedef struct {
    T_MSG       msghead;                // メッセージヘッダ
	T_MSG_DAT   msgdata;
} T_CMD_MSG;

//*************************************
// 通信データ
//*************************************
										
typedef union {
    struct {
        B   type;                       // 受信データタイプ
        B   data;                       // 受信データ
        B   portno;                     // 通信番号
        B   reserved1;                  // 予備１
    } byte;
    VP_INT  data;
} T_COM_CHAR;

//*************************************
// SIO通信制御
//*************************************
typedef struct {
    INT         rstatus;                // 受信状態
    INT         sstatus;                // 送信状態
    INT         type;                   // コマンドタイプ
    UB          rbuff[3600];      // 受信データ
    UB          bcc[2];                 // 受信BCC
    INT         wpnt;                   // 受信データサイズ
    INT         sizmax;                 // 受信データサイズ
} ST_SEQ;
typedef struct {
    UB          rstatus;                // 受信状態
    UB          bcc[2];                 // 受信BCC
    UB          dummy;                  // ダミー
    INT         type;                   // コマンドタイプ
    INT         wpnt;                   // 受信データサイズ
} ST_SEQ2;

// *************************************
//  制御PC通信データ
// *************************************
typedef struct {
	UB		msgid;						// コマンドインデックス
	INT		siz;                        // データサイズ
	INT		rty;                        // リトライカウンタ（応答時のリトライで使用）
	UB      buf[CMD_BUFSIZ];            // データ（コマンドタスク→送信タスク）
	UB      **arg[16];
	UB      argnum;
} ST_COMMDAT;

typedef struct {
	UB				bcid;				// 
	ST_COMMDAT		req;			    //コマンドリクエスト（→実行タスク）
	ST_COMMDAT		rsp;			    //コマンドレスポンス（→送信タスク）
} ST_CNTLDATA;

//// *************************************
////  バーコード通信データ
//// *************************************
//typedef struct {
//	UB		msgid;						// コマンドインデックス
//	INT		siz;                        // データサイズ
//	INT		rty;                        // リトライカウンタ（応答時のリトライで使用）
//	UB      buf[BAR_BUFSIZ];            // バーコード読み取りデータ
//	UB      **arg[16];
//	UB      argnum;
//} ST_COMMDAT_BAR;
//typedef struct {
//	UB				bcid;				// BCID
//	ST_COMMDAT_BAR	req;			    // 
//	ST_COMMDAT_BAR	rsp;			    // コマンドレスポンス（→送信タスク）
//} ST_CNTLDATA_BAR;
//
// *************************************
//  モータ制御用データ
// *************************************
typedef struct {
	UB	mm;
	UB	st;
	UB	dmy[2];
	INT	pppp;
} ST_ABSMV;

typedef struct {
	UB	mm;																// デバイス番号（モーター番号）
	UB	dr;																// リミット方向
	UB	md;																// リミット種別
	UB	dmy;															// リザーブ
	INT	sp;																// 定速度
} ST_LIMIT;

//typedef struct {
//	UB mm;			// モータ番号	0-15 : 対象のモータ番号
//	UB dr;			// 移動方向		CW : 正転方向, CCW : 逆転方向
//	UB rsv[2];		// reserve
//	INT spd;		// 駆動速度		1 - : モータの駆動速度					( 単位 : Hz    )
//	INT orgpls;		// 原点抜け出しパルス数		0 - : 						【pulse】
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
	UB			req;													// 要求＝１カウント監視開始
	UB			tskid;													// 
	UB			prid1;													// 
	UB			bcid;													// 
	UB			msgid;													// 
	UB			motpare;												// 
	UB			inf;													// 
	UB			inf2;													// 
	UB			msgdat[CMD_BUFSIZ];										// 
	INT			tim;													// タイマ値
	INT			cnt;													// 残時間
}ST_MOTWAIT;

typedef struct {
	UB		nn;															// INTコマンド     監視入力チャネル
	UB		trg;														// INTコマンド     入力チャネルトリガチャネル
	UB		dmy[2];														// INTコマンド     ダミー
	// [0]   bit7 ・・・・・・bit0
	// [1]   bit15・・・・・・bit8
	//              ・
	//              ・
	//              ・
	// [7]   bit63・・・・・・bit56
	UB		mmm[8];														// INTコマンド     一時停止するモータ
	UB		dat[8];														// INTコマンド     出力チャネルへ出力する値
	UB		msk[8];														// INTコマンド     割り込み発生時に出力するチャネル指定
	INT		EndZ;														// (予備機能)液面検知時Ｚ限界位置
}ST_INTINF;

typedef struct {
	UB			tskid;													// 
	UB			prid1;													// 
	UB			bcid;													// 
	UB			msgid;													// 
	ST_INTINF	intinf;
}ST_INTCNTL;


// SETコマンド
typedef struct {
	UB		mm;															// モータ番号
	UB		dr;															// 駆動タイプ
	UB		fc;															// 速度番号
	UB		dmy;														// dummy
	INT		fl;															// 初速[Hz](0～32767) 
	INT		fh;															// 最高速[Hz](0～32767)
	INT		gl;															// 加減速時間[ms](１～5000)
	INT		sc;															// Ｓ字変速領域[Hz](１～16383) 
} ST_SET;


// BOUTコマンド
typedef struct {
	UB		nn;		// 
	UB		trg;	// 
	UB		dmy[2];														// ダミー
} ST_BOUT;

// BIN
typedef struct {
	UB		nn;		// 
	UB		dmy[3];														// ダミー
} ST_BIN;

// CINIT
typedef struct {
	UB		nn;		// 
	UB		f;		// 
	UB		dmy[2];														// ダミー
} ST_CINIT;

// COUNT
typedef struct {
	UB	nn;
	UB	dmy[3];
} ST_COUNT;

// ABORT
typedef struct {
	UB		mm;		// 
	UB		dmy[3];														// ダミー
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
	UB		dmy[3];														// ダミー
} ST_STATUS;

// COMOPN
typedef struct {
	UB		ch;		// 
	UB		dd;		// 
	UB		dmy[2];														// ダミー
} ST_COMOPN;

// COMSET
typedef struct {
	UB		ch;		// 
	UB		ll;		// 
	UB		ss;		// 
	UB		pp;		// 
	UB		tt;		// 
	UB		dmy[3];														// ダミー
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
	UB mm;			// モータ番号	0-15 : 対象のモータ番号
	UB dr;			// 移動方向		CW : 正転方向, CCW : 逆転方向
	UB rsv[2];		// reserve
	INT spd;		// 駆動速度		1 - : モータの駆動速度					( 単位 : Hz    )
	INT orgpls;		// 原点抜け出しパルス数		0 - : 						【pulse】
} ST_INIT0;


// C-HDMOV
typedef struct {
	INT Xpos;		// X座標												【pulse】
	INT Ypos;		// Y座標												【pulse】
	INT Xfl;		// X初速
	INT Xfh;		// X最高速
	INT Xgl;		// X加減速時間
	INT Yfl;		// Y初速
	INT Yfh;		// Y最高速
	INT Ygl;		// Y加減速時間
	INT Zfl;		// Z初速
	INT Zfh;		// Z最高速
	INT Zgl;		// Z加減速時間
} ST_HDMOV;


// C-TPGET
typedef struct {
	INT Xpos1;		// X座標( ノズル取得位置[ 1回目移動 ]  )				【pulse】
	INT Ypos1;		// Y座標( ノズル取得位置[ 1回目移動 ]  )				【pulse】
	INT Z1pos1;		// Z1座標( チップ取得前 )								【pulse】
	INT Z1pos2;		// Z1座標( チップ取得 )									【pulse】
	INT Z2pos1;		// Z2座標( チップ取得前 )								【pulse】
	INT Z2pos2;		// Z2座標( チップ取得 )									【pulse】
	INT PTpos1;		// ピッチ可変座標( チップ有無確認 )						【pulse】
	INT PTpos2;		// ピッチ可変座標( チップ取得位置 )						【pulse】
	INT Xfl;		// X初速
	INT Xfh;		// X最高速
	INT Xgl;		// X加減速時間
	INT Yfl;		// Y初速
	INT Yfh;		// Y最高速
	INT Ygl;		// Y加減速時間
	INT Z1fl;		// Z初速
	INT Z1fh;		// Z最高速
	INT Z1gl;		// Z加減速時間
	INT Z2fl;		// Z初速( チップ取得 )
	INT Z2fh;		// Z最高速( チップ取得 )
	INT Z2gl;		// Z加減速時間( チップ取得 )
	INT PTfl;		// ピッチ可変初速
	INT PTfh;		// ピッチ可変最高速
	INT PTgl;		// ピッチ可変加減速時間
	INT Pfl;		// ポンプ初速
	INT Pfh;		// ポンプ最高速
	INT Pgl;		// ポンプ加減速時間
	INT nn;			// ノズル1/2取得指定 ノズル指定を参照【0:しない, 1:する】
} ST_TPGET;


// C-TPRLS
typedef struct {
	INT Xpos1;		// X座標( エジェクト位置 )								【pulse】
	INT Xpos2;		// X座標( スライド位置 )								【pulse】
	INT Ypos;		// Y座標( エジェクト位置 )								【pulse】
	INT Z1pos1;		// Z1座標( 下降位置 )									【pulse】
	INT Z1pos2;		// Z1座標( 1段上昇位置 )								【pulse】
	INT Z2pos1;		// Z2座標( 下降位置 )									【pulse】
	INT Z2pos2;		// Z2座標( 1段上昇位置 )								【pulse】
	INT PTpos1;		// ピッチ可変座標( チップ有無確認 )						【pulse】
	INT PTpos2;		// ピッチ可変座標( チップ廃棄位置 )						【pulse】
	INT Xfl;		// X初速
	INT Xfh;		// X最高速
	INT Xgl;		// X加減速時間
	INT Yfl;		// Y初速
	INT Yfh;		// Y最高速
	INT Ygl;		// Y加減速時間
	INT Z1fl;		// Z初速 
	INT Z1fh;		// Z最高速
	INT Z1gl;		// Z加減速時間
	INT Z2fl;		// Z初速   ( 1段上昇速度 )	
	INT Z2fh;		// Z最高速( 1段上昇速度 )
	INT Z2gl;		// Z加減速時間( 1段上昇速度 )
	INT PTfl;		// ピッチ可変初速
	INT PTfh;		// ピッチ可変最高速
	INT PTgl;		// ピッチ可変加減速時間
	INT nn;			// ノズル1/2廃棄指定       ノズル指定を参照【0:しない, 1:する】
} ST_TPRLS;
typedef struct {
	INT n1;
	INT n2;
} ST_TPRLS_RET;


// C-DROPN
typedef struct {
	UB nn;			// 開閉指定(0:閉じる、1:開く)
	UB rsv[3];		// reserve
} ST_DROPN;
typedef struct {
	UB req;
	UB rty;
} ST_DROPC_CTRL;

// C-SKSET
typedef struct {
	INT Xpos    ;	// X座標                          [Pulse]
	INT Ypos    ;	// Y座標                          [Pulse]
	INT Ptpos   ;	// ﾋﾟｯﾁ可変座標(ｻﾝﾌﾟﾙﾗｯｸ幅)       [Pulse]
	INT vol1    ;	// ﾉｽﾞﾙ1吸引量                    [Pulse]
	INT rsv1    ;	// ﾉｽﾞﾙ1余剰液量                  [Pulse]
	INT pd1     ;	// ﾉｽﾞﾙ1余剰戻し液量              [Pulse]
	INT blow1   ;	// ﾉｽﾞﾙ1ﾌﾞﾛｰｱｳﾄｴｱ量               [Pulse]
	INT trans1  ;	// ﾉｽﾞﾙ1ﾄﾗﾝｽﾎﾟｰﾄｴｱ量              [Pulse]
	INT sufvol1 ;	// ﾉｽﾞﾙ1液面検知時の吸引ｴｱ量      [Pulse]
	INT StartZ1 ;	// ﾉｽﾞﾙ1吸引開始高さ              [Pulse]
	INT Endz1   ;	// ﾉｽﾞﾙ1吸引底位置高さ            [Pulse]
	INT liqinv1 ;	// ﾉｽﾞﾙ1液面検知無効高さ          [Pulse]
	INT prvofs1 ;	// ﾉｽﾞﾙ1液面からの脱出量          [Pulse]
	INT fwdofs1 ;	// ﾉｽﾞﾙ1液面からの突っ込み量      [Pulse]
	INT escZ1   ;	// ﾉｽﾞﾙ1退避高さ                  [Pulse]
	INT vol2    ;	// ﾉｽﾞﾙ2吸引量                    [Pulse]
	INT rsv2    ;	// ﾉｽﾞﾙ2余剰液量                  [Pulse]
	INT pd2     ;	// ﾉｽﾞﾙ2余剰戻し液量              [Pulse]
	INT blow2   ;	// ﾉｽﾞﾙ2ﾌﾞﾛｰｱｳﾄｴｱ量               [Pulse]
	INT trans2  ;	// ﾉｽﾞﾙ2ﾄﾗﾝｽﾎﾟｰﾄｴｱ量              [Pulse]
	INT sufvol2 ;	// ﾉｽﾞﾙ2液面検知時の吸引ｴｱ量      [Pulse]
	INT StartZ2 ;	// ﾉｽﾞﾙ2吸引開始高さ              [Pulse]
	INT Endz2   ;	// ﾉｽﾞﾙ2吸引底位置高さ            [Pulse]
	INT liqinv2 ;	// ﾉｽﾞﾙ2液面検知無効高さ          [Pulse]
	INT prvofs2 ;	// ﾉｽﾞﾙ2液面からの脱出量          [Pulse]
	INT fwdofs2 ;	// ﾉｽﾞﾙ2液面からの突っ込み量      [Pulse]
	INT escZ2   ;	// ﾉｽﾞﾙ2退避高さ                  [Pulse]
	INT Xfl     ;	// X初速                          [Hz]
	INT Xfh     ;	// X最高速                        [Hz]
	INT Xgl     ;	// X加減速時間                    [ms]
	INT Yfl     ;	// Y初速                          [Hz]
	INT Yfh     ;	// Y最高速                        [Hz]
	INT Ygl     ;	// Y加減速時間                    [ms]
	INT P1fl    ;	// ﾎﾟﾝﾌﾟ初速(吸引)                [Hz]
	INT P1fh    ;	// ﾎﾟﾝﾌﾟ最高速(吸引)              [Hz]
	INT P1gl    ;	// ﾎﾟﾝﾌﾟ加減速時間(吸引)          [ms]
	INT P2fl    ;	// ﾎﾟﾝﾌﾟ初速(液面検出)            [Hz]
	INT P2fh    ;	// ﾎﾟﾝﾌﾟ最高速(液面検出)          [Hz]
	INT P2gl    ;	// ﾎﾟﾝﾌﾟ加減速時間(液面検出)      [ms]
	INT P3fl    ;	// ﾎﾟﾝﾌﾟ初速(吐出)                [Hz]
	INT P3fh    ;	// ﾎﾟﾝﾌﾟ最高速(吐出)              [Hz]
	INT P3gl    ;	// ﾎﾟﾝﾌﾟ加減速時間(吐出)          [ms]
	INT Z1fl    ;	// Z初速(下降/上昇)               [Hz]
	INT Z1fh    ;	// Z最高速(下降/上昇)             [Hz]
	INT Z1gl    ;	// Z加減速時間(下降/上昇)         [ms]
	INT Z2fl    ;	// Z初速(吸引下降/吐出上昇)       [Hz]
	INT Z2fh    ;	// Z最高速(吸引下降/吐出上昇)     [Hz]
	INT Z2gl    ;	// Z加減速時間(吸引下降/吐出上昇) [ms]
	INT Z3fl    ;	// Z初速(液面検出)                [Hz]
	INT Z3fh    ;	// Z最高速(液面検出)              [Hz]
	INT Z3gl    ;	// Z加減速時間(液面検出)          [ms]
	INT Z4fl    ;	// Z初速(液吸引後上昇)            [Hz]
	INT Z4fh    ;	// Z最高速(液吸引後上昇)          [Hz]
	INT Z4gl    ;	// Z加減速時間(液吸引後上昇)      [ms]
	INT PTfl    ;	// ﾋﾟｯﾁ可変初速                   [Hz]
	INT PTfh    ;	// ﾋﾟｯﾁ可変最高速                 [Hz]
	INT PTgl    ;	// ﾋﾟｯﾁ可変加速度                 [ms]
	INT suf     ;	// 液面検知設定                   [-]
	INT suckwt  ;	// 吸引後ｳｪｲﾄ時間                 [ms]
	INT dotwt   ;	// 吐出後ｳｪｲﾄ時間                 [ms]
	INT El      ;	// 吸引ｴﾗｰ検出設定                [-]
} ST_SKSET;


// C-SUCK0
typedef struct {
	UB nn;			// ノズル1/2吸引指定 ノズル指定を参照【0:しない, 1:する】
	UB rsv[3];		// reserve
} ST_SUCK0;
typedef struct {
	ER n1;			// ノズル1結果
	INT z1_liq;		// ノズル1液面検知高さ
	INT z1_suc;		// ノズル1吸引高さ
	ER n2;			// ノズル2結果
	INT z2_liq;		// ノズル2液面検知高さ
	INT z2_suc;		// ノズル2吸引高さ
	UB inf;			// BIT0 = 1:ノズル1液面検知済み
					// BIT1 = 1:ノズル2液面検知済み
					// BIT2 = 1:
					// BIT3 = 1:
					// BIT4 = 1:
					// BIT5 = 1:
	UB rsv[3];		// reserve
} ST_SUCK0_RET;


// C-DTSET
typedef struct {
	INT Xpos    ;	// X座標                          [Pulse]
	INT Ypos    ;	// Y座標                          [Pulse]
	INT Ptpos   ;	// ﾋﾟｯﾁ可変座標(ﾃｰﾌﾞﾙ穴ﾋﾟｯﾁ)      [Pulse]
	INT vol1    ;	// ﾉｽﾞﾙ1吐出量                    [Pulse]
	INT cut1    ;	// ﾉｽﾞﾙ1液切れ吸引ｴｱ量            [Pulse]
	INT Dotz1   ;	// ﾉｽﾞﾙ1吐出高さ                  [Pulse]
	INT cutz1   ;	// ﾉｽﾞﾙ1液切れ吸引高さ            [Pulse]
	INT Touchz1 ;	// ﾉｽﾞﾙ1ﾁｯﾌﾟﾀｯﾁ高さ               [Pulse]
	INT vol2    ;	// ﾉｽﾞﾙ2吐出量                    [Pulse]
	INT cut2    ;	// ﾉｽﾞﾙ2液切れ吸引ｴｱ量            [Pulse]
	INT Dotz2   ;	// ﾉｽﾞﾙ2吐出高さ                  [Pulse]
	INT cutz2   ;	// ﾉｽﾞﾙ2液切れ吸引高さ            [Pulse]
	INT Touchz2 ;	// ﾉｽﾞﾙ2ﾁｯﾌﾟﾀｯﾁ高さ               [Pulse]
	INT Xfl     ;	// X初速                          [Hz]
	INT Xfh     ;	// X最高速                        [Hz]
	INT Xgl     ;	// X加減速時間                    [ms]
	INT Yfl     ;	// Y初速                          [Hz]
	INT Yfh     ;	// Y最高速                        [Hz]
	INT Ygl     ;	// Y加減速時間                    [ms]
	INT P1fl    ;	// ﾎﾟﾝﾌﾟ初速(吐出)                [Hz]
	INT P1fh    ;	// ﾎﾟﾝﾌﾟ最高速(吐出)              [Hz]
	INT P1gl    ;	// ﾎﾟﾝﾌﾟ加減速時間(吐出)          [ms]
	INT P2fl    ;	// ﾎﾟﾝﾌﾟ初速(吸引)                [Hz]
	INT P2fh    ;	// ﾎﾟﾝﾌﾟ最高速(吸引)              [Hz]
	INT P2gl    ;	// ﾎﾟﾝﾌﾟ加減速時間(吸引)          [ms]
	INT Z1fl    ;	// Z初速(下降/上昇)               [Hz]
	INT Z1fh    ;	// Z最高速(下降/上昇)             [Hz]
	INT Z1gl    ;	// Z加減速時間(下降/上昇)         [ms]
	INT Z2fl    ;	// Z初速(吐出上昇)                [Hz]
	INT Z2fh    ;	// Z最高速(吐出上昇)              [Hz]
	INT Z2gl    ;	// Z加減速時間(吐出上昇)          [ms]
	INT Z3fl    ;	// Z初速(ﾁｯﾌﾟﾀｯﾁ)                 [Hz]
	INT Z3fh    ;	// Z最高速(ﾁｯﾌﾟﾀｯﾁ)               [Hz]
	INT Z3gl    ;	// Z加減速時間(ﾁｯﾌﾟﾀｯﾁ)           [ms]
	INT PTfl    ;	// ﾋﾟｯﾁ可変初速                   [Hz]
	INT PTfh    ;	// ﾋﾟｯﾁ可変最高速                 [Hz]
	INT PTgl    ;	// ﾋﾟｯﾁ可変加減速時間             [ms]
	INT dotwt   ;	// 吐出後ｳｪｲﾄ時間                 [ms]
	INT suckwt  ;	// 吸引後ｳｪｲﾄ時間                 [ms]
	INT Add1st  ;	// 1回目吐出補正量                [Pulse]
	INT El      ;	// 吐出ｴﾗｰ検出設定                [-]
} ST_DTSET;


// C-DOT00
typedef struct {
	UB nn;			// ノズル1/2吸引指定 ノズル指定を参照【0:しない, 1:する】
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
	INT Xpos   ;	// X座標                      【Pulse】
	INT Ypos   ;	// Y座標                      【Pulse】
	INT Ptpos  ;	// ﾋﾟｯﾁ可変座標(ﾃｰﾌﾞﾙ穴ﾋﾟｯﾁ)  【Pulse】
	INT Z1pos1 ;	// ﾉｽﾞﾙ1Z座標(吸引高さ)       【Pulse】
	INT Z1pos2 ;	// ﾉｽﾞﾙ1Z座標(吐出高さ)       【Pulse】
	INT Z1pos3 ;	// ﾉｽﾞﾙ1Z座標(ﾌﾞﾛｰｱｳﾄ高さ)    【Pulse】
	INT Z1pos4 ;	// ﾉｽﾞﾙ1Z座標(吸引開始高さ)   【Pulse】
	INT Z1pos5 ;	// ﾉｽﾞﾙ1Z座標(吸引終了高さ)   【Pulse】
	INT Touchz1;	// ﾉｽﾞﾙ1ﾁｯﾌﾟﾀｯﾁ高さ           【Pulse】
	INT Cutz1  ;	// ﾉｽﾞﾙ1液切れ吸引高さ        【Pulse】
	INT vol1   ;	// ﾉｽﾞﾙ1吸引吐出量            【Pulse】
	INT cut1   ;	// ﾉｽﾞﾙ1液切れ吸引ｴｱ量        【Pulse】
	INT blow1  ;	// ﾉｽﾞﾙ1ﾌﾞﾛｰｱｳﾄｴｱ量           【Pulse】
	INT advol1 ;	// ﾉｽﾞﾙ1撹拌余剰量            【Pulse】
	INT Z2pos1 ;	// ﾉｽﾞﾙ2Z座標(吸引高さ)       【Pulse】
	INT Z2pos2 ;	// ﾉｽﾞﾙ2Z座標(吐出高さ)       【Pulse】
	INT Z2pos3 ;	// ﾉｽﾞﾙ2Z座標(ﾌﾞﾛｰｱｳﾄ高さ)    【Pulse】
	INT Z2pos4 ;	// ﾉｽﾞﾙ4Z座標(吸引開始高さ)   【Pulse】
	INT Z2pos5 ;	// ﾉｽﾞﾙ5Z座標(吸引終了高さ)   【Pulse】
	INT Touchz2;	// ﾉｽﾞﾙ2ﾁｯﾌﾟﾀｯﾁ高さ           【Pulse】
	INT Cutz2  ;	// ﾉｽﾞﾙ2液切れ吸引高さ        【Pulse】
	INT vol2   ;	// ﾉｽﾞﾙ2吸引吐出量            【Pulse】
	INT cut2   ;	// ﾉｽﾞﾙ2液切れ吸引ｴｱ量        【Pulse】
	INT blow2  ;	// ﾉｽﾞﾙ2ﾌﾞﾛｰｱｳﾄｴｱ量           【Pulse】
	INT advol2 ;	// ﾉｽﾞﾙ2撹拌余剰量            【Pulse】
	INT Num    ;	// 撹拌回数                   【回】
	INT Xfl    ;	// X初速                      【Hz】
	INT Xfh    ;	// X最高速                    【Hz】
	INT Xgl    ;	// X加減速時間                【ms】
	INT Yfl    ;	// Y初速                      【Hz】
	INT Yfh    ;	// Y最高速                    【Hz】
	INT Ygl    ;	// Y加減速時間                【ms】
	INT P1fl   ;	// ﾎﾟﾝﾌﾟ初速(吸引)            【Hz】
	INT P1fh   ;	// ﾎﾟﾝﾌﾟ最高速(吸引)          【Hz】
	INT P1gl   ;	// ﾎﾟﾝﾌﾟ加減速時間(吸引)      【ms】
	INT P2fl   ;	// ﾎﾟﾝﾌﾟ初速(吐出)            【Hz】
	INT P2fh   ;	// ﾎﾟﾝﾌﾟ最高速(吐出)          【Hz】
	INT P2gl   ;	// ﾎﾟﾝﾌﾟ加減速時間(吐出)      【ms】
	INT Z1fl   ;	// Z初速(下降/上昇)           【Hz】
	INT Z1fh   ;	// Z最高速(下降/上昇)         【Hz】
	INT Z1gl   ;	// Z加減速時間(下降/上昇)     【ms】
	INT Z2fl   ;	// Z初速(吸引下降)            【Hz】
	INT Z2fh   ;	// Z最高速(吸引下降)          【Hz】
	INT Z2gl   ;	// Z加減速時間(吸引下降)      【ms】
	INT Z3fl   ;	// Z初速(吐出上昇)            【Hz】
	INT Z3fh   ;	// Z最高速(吐出上昇)          【Hz】
	INT Z3gl   ;	// Z加減速時間(吐出上昇)      【ms】
	INT PTfl   ;	// ﾋﾟｯﾁ可変初速               【Hz】
	INT PTfh   ;	// ﾋﾟｯﾁ可変最高速             【Hz】
	INT PTgl   ;	// ﾋﾟｯﾁ可変加減速時間         【ms】
	INT suckwt ;	// 吸引後ｳｪｲﾄ時間             【ms】
	INT dotwt  ;	// 吐出後ｳｪｲﾄ時間             【ms】
	INT El     ;	// 吸引ｴﾗｰ検出設定            【- 】
} ST_MXSET;


// C-MIX00
typedef struct {
	UB nn;			// ノズル1/2吸引指定 ノズル指定を参照【0:しない, 1:する】
	UB rsv[3];		// reserve
} ST_MIX00;
typedef struct {
	ER n1;
	ER n2;
	UB cnt;			// 撹拌回数
	UB inf;			// BIT0 = 1:
					// BIT1 = 1:
					// BIT2 = 1:
					// BIT3 = 1:
} ST_MIX00_RET;


// C-THMOV
typedef struct {
	INT Act;	// 動作指定
	INT Xpos;	// X座標( ピックアップ／リリース／位置移動 )
	INT Ypos;	// Y座標( ピックアップ／リリース／位置移動 )
	INT Zpos1;	// Z座標( ピックアップ／リリース／位置移動 )
	INT Zpos2;	// Z座標( 移動高さ )
	INT Zpos3;	// Z座標( リリース後高さ )
	INT Rpos;	// R座標( ピックアップ／リリース／位置移動 )
	INT Xfl;	// X初速
	INT Xfh;	// X最高速
	INT Xgl;	// X加減速時間
	INT Yfl;	// Y初速
	INT Yfh;	// Y最高速
	INT Ygl;	// Y加減速時間
	INT Z1fl;	// Z1初速
	INT Z1fh;	// Z1最高速
	INT Z1gl;	// Z1加減速時間
	INT Z2fl;	// Z2初速
	INT Z2fh;	// Z2最高速
	INT Z2gl;	// Z2加減速時間
	INT Rfl;	// R初速
	INT Rfh;	// R最高速
	INT Rgl;	// R加減速時間
	INT Delay;	// 搬送ハンド動作後ディレイ時間[ms]
	INT Rout;	// XY順路( 0:X->Y , 1:Y->X )
} ST_THMOV;


// C-CPOPN/C-CPCLS
typedef struct {
	INT Act    ; // 開閉指定(0:開閉栓なし,1:CAP1,2:CAP2,3:BOTH)
	INT ZposS  ; // 開閉栓Z軸 開閉栓開始高さ           [Pls]
	INT ZposE  ; // 開閉栓Z軸 開閉栓終了高さ           [Pls]
	INT R1posS ; // 開閉栓R1軸 開閉栓開始位置          [Pls]
	INT R1posE ; // 開閉栓R1軸 開閉栓終了位置          [Pls]
	INT R2posS ; // 開閉栓R1軸 開閉栓開始位置          [Pls]
	INT R2posE ; // 開閉栓R1軸 開閉栓終了位置          [Pls]
	INT Zcap   ; // 開閉栓Z軸 キャップ有無高           [Pls]
	INT Tpend  ; // 突っ張り棒終了位置                 [Pls]
	INT Z2posS ; // 開閉栓Z軸 開栓時2段高さ            [Pls]
	INT Patn   ; // 0:棒→把持,1=把持→棒,2=棒→把持無 [Pls]
	INT Znmfl  ; // 開閉栓Z軸 通常速度 初速             [Hz]
	INT Znmfh  ; // 開閉栓Z軸 通常速度 最高速           [Hz]
	INT Znmgl  ; // 開閉栓Z軸 通常速度 加減速時間       [ms]
	INT Zopfl  ; // 開閉栓Z軸 開閉栓時速度 初速         [Hz]
	INT Zopfh  ; // 開閉栓Z軸 開閉栓時速度 最高速       [Hz]
	INT Zopgl  ; // 開閉栓Z軸 開閉栓時速度 加減速時間   [ms]
	INT R1fl   ; // 開閉栓R1軸 開閉栓時速度 初速        [Hz]
	INT R1fh   ; // 開閉栓R1軸 開閉栓時速度 最高速      [Hz]
	INT R1gl   ; // 開閉栓R1軸 開閉栓時速度 加減速時間  [ms]
	INT R2fl   ; // 開閉栓R2軸 開閉栓時速度 初速        [Hz]
	INT R2fh   ; // 開閉栓R2軸 開閉栓時速度 最高速      [Hz]
	INT R2gl   ; // 開閉栓R2軸 開閉栓時速度 加減速時間  [ms]
	INT Tpfl   ; // 突っ張り棒軸 速度 初速              [Hz]
	INT Tpfh   ; // 突っ張り棒軸 速度 最高速            [Hz]
	INT Tpgl   ; // 突っ張り棒軸 速度 加減速時間        [Hz]
	INT Z2fl   ; // 開閉栓Z軸 開閉栓時2段速度 初速      [Hz]
	INT Z2fh   ; // 開閉栓Z軸 開閉栓時2段速度 最高速    [Hz]
	INT Z2gl   ; // 開閉栓Z軸 開閉栓時2段速度 加減速時間[ms]
} ST_CPOPN;

typedef struct {
	UB nn;
	UB inf;			// BIT0 = 1: 開栓R1
					// BIT1 = 1: 開栓R2
					// BIT2 = 1: 開栓Z
					// BIT3 = 1: 突き出し棒
	UB base;
	UB rsv[1];		// reserve
} ST_CAP_RET;


// C-BCDSP
typedef struct {
	INT Rno   ;	// ラック位置(1～4)
	INT Bpos  ;	// バーコード軸 移動量              [Pls]
	INT Bfl   ;	// バーコード軸 動作速度 初速       [Hz]
	INT Bfh   ;	// バーコード軸 動作速度 最高速     [Hz]
	INT Bgl   ;	// バーコード軸 動作速度 加減速時間 [ms]
	INT Tim   ;	// バーコード読取りタイムアウト     [ms]
} ST_BCDSP;

// バーコード読み取りデータ
typedef struct {
	INT  Pos;				// ポジションID
	char Id[TUBE_LEN16];	// チューブID
} ST_BCDTB;

typedef struct {
	char RackId[8];	
	ST_BCDTB Tub[12];
} ST_BCDRD;



// C-TURN1
typedef struct {
	INT Act   ; 	// 動作指定
	INT Rsrv1 ; 	// 予備(0固定)
	INT Tpos1 ; 	// ターンテーブル軸 移動量              [Pls]
	INT Tfl   ; 	// ターンテーブル軸 動作速度 初速       [Hz]
	INT Tfh   ; 	// ターンテーブル軸 動作速度 最高速     [Hz]
	INT Tgl   ; 	// ターンテーブル軸 動作速度 加減速時間 [ms]
} ST_TURN1;


// C-BZCNT
typedef struct {
	UB Act;			// パトライト動作   ( 0:消灯, 1:点灯,   2:点滅 )
	UB Colptn;      // パトライトカラー ( 0:緑    1:黄,     2:赤   )
	UB Buzptn;      // ブザー           ( 0:OFF,  1:ブザー         )
	UB rsv[1];		// reserve
} ST_BZCNT;


// C-LEDON
typedef struct {
	UB nn;			// 出力ポート番号		1 -
	UB act;			// 出力モード			(0:消灯, 1:点灯, 2:点滅)
	UB rsv[2];		// reserve
} ST_LEDON;



//
// === cyc handler =====================================================
//
typedef struct {
	UB			devno;						// デバイス番号
	UB			portno;						// ポート番号
}ST_CH;

typedef struct {
		UB	cnt;
		UB	dat;
}IOCH;


//
// === tskio ===========================================================
//
// カウンタ管理
typedef struct {
	UB			sts;			// 1:カウント中、0:待機中
	UB			old;			// 
	UB			now;			// 
	UB			rsv;			// 予約
	UINT		cnt;			// 
}ST_IOCOUNT;


//
// === tskcmd ==========================================================
//
// データチェック用レンジ
typedef struct {
	INT	min;
	INT	max;
}ST_RANGE;

// データチェック管理
typedef struct {
	UB			argnum;
	ST_RANGE	range[MAX_ARGS];
}ST_DATACHK;


//typedef struct {
//	INT Xpos;	// [X軸]移動位置（パルス）
//	INT Ypos;	// [Y軸]移動位置（パルス）
//	INT Zpos1;	// [Z1軸]移動位置（パルス）
//	INT Zpos2;	// [Z2軸]移動位置（パルス）
//}ST_HDMOV;

// 動作制御データ
typedef struct {
	UB	msgid;
	UB	devno;
	UB	actstp;
	UB	prid1;	
	UB	inf;		// 汎用情報
	UB	inf2;		// 汎用情報
	UB	inf3;		// 汎用情報
	UB	inf4;		// 汎用情報
}ST_BCCTRL;


//
// === tskmotact =======================================================
//
typedef struct {
	UB		cmdsts;		// コマンド進行状態 0xff:IDLE,0x01:ACTTION,0x02:END
	UB		mot;		// モーター番号（MAP対応のため２モータ分）
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
	INT  sts; // 状態
	UB no;  // プロトコル番号
	UB step;// 処理ステップ
	UB proc;// 工程
	UB resv;// 予備
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
	INT  Y0_Offset;        // [Y0] 仮想原点オフセット値                  ポジション番号１  
	INT  Z0_Offset;        // [Z0] 仮想原点オフセット値                  ポジション番号２  
	INT  P0_Offset;        // [P0] 仮想原点オフセット値                  ポジション番号３  
	INT  F0_Offset;        // [F0] 仮想原点オフセット値                  ポジション番号４  
	INT  Y1_Offset;        // [Y1] 仮想原点オフセット値                  ポジション番号５  
	INT  Y0_Taihi;         // [Y0] 退避位置                              ポジション番号６  
	INT  Z0_Taihi;         // [Z0] 退避位置                              ポジション番号７  
	INT  P0_Taihi;         // [P0] 退避位置                              ポジション番号８  
	INT  F0_Taihi;         // [Y1] 退避位置                              ポジション番号９  
	INT  Y1_Taihi;         // [F0] 退避位置                              ポジション番号１０
	INT  StripTp1;         // [Y0] 試薬ストリップ-ショートチップ１       ポジション番号１１
	INT  StripTp2;         // [Y0] 試薬ストリップ-ショートチップ２       ポジション番号１２
	INT  StripTpL;         // [Y0] 試薬ストリップ-ロングチップ           ポジション番号１３
	INT  StripTub;         // [Y0] 試薬ストリップ-サンプルチューブ       ポジション番号１４
	INT  StripSm6;         // [Y0] 試薬ストリップ-抽出用試薬6            ポジション番号１５
	INT  StripSm5;         // [Y0] 試薬ストリップ-抽出用試薬5            ポジション番号１６
	INT  StripSm4;         // [Y0] 試薬ストリップ-抽出用試薬4            ポジション番号１７
	INT  StripSm3;         // [Y0] 試薬ストリップ-抽出用試薬3            ポジション番号１８
	INT  StripSm2;         // [Y0] 試薬ストリップ-抽出用試薬2            ポジション番号１９
	INT  StripSm1;         // [Y0] 試薬ストリップ-抽出用試薬1            ポジション番号２０
	INT  StripCat;         // [Y0] 試薬ストリップ-カートリッジ           ポジション番号２１
	INT  StripCol;         // [Y0] 試薬ストリップ-核酸回収液             ポジション番号２２
	INT  Z0_StripOpnE;     // [Z0] 試薬ストリップ-シール開け最終位置     ポジション番号２３
	INT  Z0_StripOpn1;     // [Z0] 試薬ストリップ-シール開け１段位置     ポジション番号２４
	INT  Z0_StripPres;     // [Z0] 試薬ストリップ-加圧位置               ポジション番号２５
	INT  Z0_TipGetE;       // [Z0] チップ取得最終位置                    ポジション番号２６
	INT  Z0_TipGet1;       // [Z0] チップ取得１段位置                    ポジション番号２７
	INT  Z0_TipDelE;       // [Z0] チップ廃棄最終位置                    ポジション番号２８
	INT  Z0_TipDel1;       // [Z0] チップ廃棄１段位置                    ポジション番号２９
	INT  Z0_LiqPut;        // [Z0] 廃液吐出位置                          ポジション番号３０
	INT  Y1_ColTube;       // [Y1] コレクション回収チューブ位置          ポジション番号３１
	INT  Y1_ColDspTube;    // [Y1] 廃液チューブ位置                      ポジション番号３２
} ST_PAGE7;

// --- Page 6 ---
typedef struct {
	INT  Z0_SucStaHei1;    // [Z0] 吸引開始高さ1                         ポジション番号３３
	INT  Z0_SucStaHei2;    // [Z0] 吸引開始高さ2                         ポジション番号３４
	INT  Z0_SucStaHei3;    // [Z0] 吸引開始高さ3                         ポジション番号３５
	INT  Z0_SucStaHei4;    // [Z0] 吸引開始高さ4                         ポジション番号３６
	INT  Z0_SucStaHei5;    // [Z0] 吸引開始高さ5                         ポジション番号３７
	INT  Z0_SucEndHei1;    // [Z0] 吸引終了高さ1                         ポジション番号３８
	INT  Z0_SucEndHei2;    // [Z0] 吸引終了高さ2                         ポジション番号３９
	INT  Z0_SucEndHei3;    // [Z0] 吸引終了高さ3                         ポジション番号４０
	INT  Z0_SucEndHei4;    // [Z0] 吸引終了高さ4                         ポジション番号４１
	INT  Z0_SucEndHei5;    // [Z0] 吸引終了高さ5                         ポジション番号４２
	INT  Z0_PutStaHei1;    // [Z0] 吐出開始高さ1                         ポジション番号４３
	INT  Z0_PutStaHei2;    // [Z0] 吐出開始高さ2                         ポジション番号４４
	INT  Z0_PutStaHei3;    // [Z0] 吐出開始高さ3                         ポジション番号４５
	INT  Z0_PutStaHei4;    // [Z0] 吐出開始高さ4                         ポジション番号４６
	INT  Z0_PutStaHei5;    // [Z0] 吐出開始高さ5                         ポジション番号４７
	INT  Z0_PutEndHei1;    // [Z0] 吐出終了高さ1                         ポジション番号４８
	INT  Z0_PutEndHei2;    // [Z0] 吐出終了高さ2                         ポジション番号４９
	INT  Z0_PutEndHei3;    // [Z0] 吐出終了高さ3                         ポジション番号５０
	INT  Z0_PutEndHei4;    // [Z0] 吐出終了高さ4                         ポジション番号５１
	INT  Z0_PutEndHei5;    // [Z0] 吐出終了高さ5                         ポジション番号５２
	INT  Z0_MixStaSuc;     // [Z0] 攪拌吸引開始高さ                      ポジション番号５３
	INT  Z0_MixEndSuc;     // [Z0] 攪拌吸引終了高さ                      ポジション番号５４
	INT  P0_TpDispose;     // [P0]廃棄動作                               ポジション番号５５
	INT  XX_Reserve56;     // 0固定                                      ポジション番号５６
	INT  XX_Reserve57;     // 0固定                                      ポジション番号５７
	INT  XX_Reserve58;     // 0固定                                      ポジション番号５８
	INT  XX_Reserve59;     // 0固定                                      ポジション番号５９
	INT  XX_Reserve60;     // 0固定                                      ポジション番号６０
	INT  XX_Reserve61;     // 0固定                                      ポジション番号６１
	INT  XX_Reserve62;     // 0固定                                      ポジション番号６２
	INT  XX_Reserve63;     // 0固定                                      ポジション番号６３
	INT  XX_Reserve64;     // 0固定                                      ポジション番号６４
} ST_PAGE8;

// --- Page 7 ---
typedef struct {
	INT  HS_VL1;         // 補正パターン１　容量10%                    ポジション番号６５  ７１  ７７
	INT  HS_VL2;         //   容量50%                                  ポジション番号６６  ７２  ７８
	INT  HS_VL3;         //   容量100%                                 ポジション番号６７  ７３  ７９
	INT  HS_Out;         //   ブローアウトエア量                       ポジション番号６８  ７４  ８０
	INT  HS_Air;         //   エアギャップ量                           ポジション番号６９  ７５  ８１
	INT  HS_Tra;         //   余剰量                                   ポジション番号７０  ７６  ８２
} ST_HOSEI_PTN;

// --- Page 8 ---
typedef struct {
	ST_HOSEI_PTN Hosei[3];
// --- Page 9 ---
// 	INT  H1_C1Vo2;         //   容量50%                                  ポジション番号６６
// 	INT  H1_C1Vo3;         //   容量100%                                 ポジション番号６７
// 	INT  H1_C1Out;         //   ブローアウトエア量                       ポジション番号６８
// 	INT  H1_C1Air;         //   エアギャップ量                           ポジション番号６９
// 	INT  H1_C1Tra;         //   余剰量                                   ポジション番号７０
// 	INT  H1_C2Vo1;         // 補正パターン2　容量10%                     ポジション番号７１
// 	INT  H1_C2Vo2;         //   容量50%                                  ポジション番号７２
// 	INT  H1_C2Vo3;         //   容量100%                                 ポジション番号７３
// 	INT  H1_C2Out;         //   ブローアウトエア量                       ポジション番号７４
// 	INT  H1_C2Air;         //   エアギャップ量                           ポジション番号７５
// 	INT  H1_C2Tra;         //   余剰量                                   ポジション番号７６
// 	INT  H1_C3Vo1;         // 補正パターン3　容量10%                     ポジション番号７７
// 	INT  H1_C3Vo2;         //   容量50%                                  ポジション番号７８
// 	INT  H1_C3Vo3;         //   容量100%                                 ポジション番号７９
// 	INT  H1_C3Out;         //   ブローアウトエア量                       ポジション番号８０
// 	INT  H1_C3Air;         //   エアギャップ量                           ポジション番号８１
// 	INT  H1_C3Tra;         //   余剰量                                   ポジション番号８２
	INT  XX_Reserve83;     // 0固定                                      ポジション番号８３
	INT  XX_Reserve84;     // 0固定                                      ポジション番号８４
	INT  XX_Reserve85;     // 0固定                                      ポジション番号８５
	INT  XX_Reserve86;     // 0固定                                      ポジション番号８６
	INT  XX_Reserve87;     // 0固定                                      ポジション番号８７
	INT  XX_Reserve88;     // 0固定                                      ポジション番号８８
	INT  XX_Reserve89;     // 0固定                                      ポジション番号８９
	INT  XX_Reserve90;     // 0固定                                      ポジション番号９０
	INT  XX_Reserve91;     // 0固定                                      ポジション番号９１
	INT  XX_Reserve92;     // 0固定                                      ポジション番号９２
	INT  XX_Reserve93;     // 0固定                                      ポジション番号９３
	INT  XX_Reserve94;     // 0固定                                      ポジション番号９４
	INT  XX_Reserve95;     // 0固定                                      ポジション番号９５
	INT  XX_Reserve96;     // 0固定                                      ポジション番号９６
} ST_PAGE9;

// --- Page 10 ---
typedef struct {
	UB Login;            // 
	UB Reseve[127];      // 
} ST_PAGE10;

// --- Page 90 ---
typedef struct {
//    UB Pno;	// プロトコル番号
    UB Cnt;	// トータルステップ数（ 1 ～ 80 ）
    UB Prc;	// 工程有無
    UB Stp;	// 処理ステップ
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
	INT  sts;		// 装置状態
	UB pno;		// プロトコル番号
	UB step;		// プロトコル動作中の詳細ステップ
	UB cmd;		// プロトコル動作中の処理状態(LYSIS,BAIDING,WASH,ELUTE)
	UB dat1;		// 予備
} ST_DEVICE_INFO;

typedef struct	StPulseMotor{
	// 0x0～0x3
	union	{
		struct	{
			UB	COMB0;						// [W] 制御コマンド
			UB	COMB1;						// [W] 軸指定（制御コマンドの実行軸の指定）
			UB	OTPB;						// [W] 汎用出力ポートの状態変更
		} stWrite;
		struct	{
			UB	MSTSB0;						// [R] メインステータス（ビット 7～ 0）
			UB	MSTSB1;						// [R] メインステータス（ビット15～ 8）
			UB	IOPB;						// [R] 汎用入出力ポートの読み出し
			UB	SSTSB;						// [R] サブステータスの読み出し
		} stRead;
	} unCmdSts;
	// 0x4～0x7
	UB	BUFB0;								// 入出力バッファ（ビット 7～ 0）
	UB	BUFB1;								// 入出力バッファ（ビット15～ 8）
	UB	BUFB2;								// 入出力バッファ（ビット23～16）
	UB	BUFB3;								// 入出力バッファ（ビット31～24）
};
// 検査制御ブロック
typedef struct	StCheckControlBlock {
	struct	StCmd {
		long	chkRun;							// 検査実行
		long	chkCnt;							// 検査回数
		long	chkPmt;							// 検査パラメータ
		long	rsv3;							// （予約）
	} stCmd;
	char	cmd[0x40 - sizeof(struct StCmd)];
	
	struct	StSts {
		long	result;							// 検査結果
		long	status;							// 検査状態
		long	chkCnt;							// 検査回数
		long	errCnt;							// エラー回数
		long	errInfo[8];						// エラー情報[]
		long	curInfo[4];						// 現在情報[]
	} stSts;
	char	sts[0xC0 - sizeof(struct StSts)];
};


// パトライト
typedef struct {
	UB buz;            // ブザー指示  ：0 = なし、1 = あり
	UB blk;            // 点灯方法指示：0 = 消灯、1 = 点灯、2 = 点滅
	UB col;            // 点灯色指示  ：0 = 赤色、1 = 黄色、2 = 緑色
	UB tim;            // 時間（10msec単位）
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
	
	unsigned short	pre[NOZZLE_MAX+1];//加圧前
	unsigned short	now[NOZZLE_MAX+1];//現在
	unsigned short	pek[NOZZLE_MAX+1];//Peek
	unsigned short	dat[NOZZLE_MAX+1][PRESSMP_MAX];//履歴
} ST_PSNS;

typedef struct {
	unsigned char	dat[24];
} ST_PSNSCHAR;

typedef struct {
	UB	pno;// プロトコル番号
	UB	smp;// 検査数
	UB	val;// 容量
	UB	dmy;// 空き
} ST_PROTCOL_SEL;

typedef struct {
	UB	tmp;   // 指定温度
	UB	smp[3];// 
	INT	tim;   // 指定時間
	INT	cnt;   // カウント用
	INT	tmo;   // タイムアウト用
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
// 構造体の定義
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

