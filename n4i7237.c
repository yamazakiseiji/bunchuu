/*******************************************************************************
* 割込み管理機能/周期タイマ割込みハンドラ (SH7286)                             *
*                                                                              *
*  File name : n4i7286.c                                                       *
*  Copyright (c) 2010, MiSPO Co., Ltd.                                         *
*                                                                              *
*  Compile : shc -cp=sh2a -op=1 -def=CLK=?,CH=?,MSEC=?,INTVEC=?,INTVSZ=?,      *
*                 IP=? n4i7286.c                                   (Renesas C) *
*                                                                              *
* 2010-10-29 Created                                                        SZ *
*                                                                              *
* 2017/09～	ハンドラ追加                                                       *
* 			優先順位変更                                                       *
* 			タイマ値変更                                                       *
*                                                                              *
*******************************************************************************/

/*

最適化コンパイル

          割り込みハンドラのコンパイルには、必ず、最適化オプションを付け、
          ent_int 呼び出し前に、不要な命令が生成されないよう注意してください。
          ルネサスCでは、割込みハンドラを #pragma interrupt で定義してください。


割込み周期の指定

          割込み周期は、MSEC マクロで決まります。 デフォルト値は kernel.hで
          定義しており、10 msec です。
          割込み周期を変更する場合は、全てのファイルに対してMSECマクロを変更
          するようにコンパイルしてください。

          (例) shc <options> -def=MSEC=5 FILE … 5 msec
                                  ~~~~~~

割込みベクタテーブル

          def_inhシステムコールによる割込みベクタの設定を可能にするため、
          RAM先頭(0C000000H番地から1024バイト)に、割込みベクタテーブルがあると
          想定しています。
          割込みベクタテーブルの領域が、他の領域と重ならない様にリンクしてく
          ださい。(重なってもリンカで警告は出ません!)
          割込みベクタテーブルの先頭アドレスはINTVECマクロで変更できます。
          割込みベクタテーブルのサイズ(バイト数)は、INTVSZマクロで変更できます。

          (例) shc <op> -def=INTVEC=0,INTVSZ=408 n4iXXX.c … 0番地,408バイト
                             ~~~~~~~~ ~~~~~~~~~~
          割込みベクタテーブルがROM上にあり、def_inhに依らず直接ベクタを定義
          する場合、INTVEC=0, INTVSZ=0としてください。この場合、def_inhシステ
          ムコールは機能しません。


チャネル番号の指定

          周期タイマ割込みに使用している内蔵CMTのチャネルをCHマクロで指定で
          きます。(0～1, 無指定時は、CH=0)

          (例) shc <options> -def=CH=1 n4iXXX.c  .......... CMT CH1
                                  ~~~~

クロック周波数の指定

          本ソースのクロック(Pφ)のデフォルトは49.152 MHz です。変更する場合は
          CLKマクロを定義してコンパイルしてください。値は[Hz]単位の周波数です。

          (例) shc <options> -def=CLK=25000000 n4iXXX.c ……… 25 MHz
                                  ~~~~~~~~~~~~

dis_int, ena_int

          SHシリーズの割込みコントローラでは、汎用的なdis_int, ena_intシステ
          ムコールの実装が複雑となってしまいます。
          この２つのシステムコールは用意していませんので、個別の割込み禁止許
          可を行いたい場合は、各制御レジスタを直接アクセスしてください。


_kernel_inROM

          与えられたアドレスがROM領域か否かを返す関数です。ROM領域であれば
          TRUE(1), ROM領域でなければ FALSE(0) を返します。


割込み優先順位

          周期タイマ割込みハンドラのデフォルトの割込み優先順位は、7 です。
          これを変更する場合、IP マクロを定義してコンパイルしてください。
          ただし、カーネルより高優先にはできません。 (IP ≦ KNL_LEVEL)
          本ハンドラより高優先度の割込みハンドラが有る場合、多重割込みが
          起こります。

          (例) shc <options> -def=IP=2 n4iXXX.c ... 割込み優先順位 2
                                  ~~~~
*/

#include "kernel.h"
#include "nosys4.h"
#include "sh7237.h"
#include "iodefine.h"
#include "hdrsio0.h"
#include "define.h"

/* 割込みベクタテーブル */

#ifndef INTVSZ
#define INTVSZ      1024        /* サイズ未定義の場合 */
#endif

/* 割込み優先順位の定義 */

#ifndef IP
#define IP      7               /* 割込み優先順位未定義の場合 */
#endif

/* クロックの定義 */

#ifndef CLK
#define CLK     40000000L     /* 周辺クロックPck0[Hz] */
#endif

/* チャネル番号の定義 */

#ifndef CH
#define CH      0               /* チャネル番号未定義の場合 */
#endif

/* CHに合わせたCMTレジスタの再定義 */

#if (CH==0)
#define CMT_CMCSR   CMT_CMCSR_0
#define CMT_CMCNT   CMT_CMCNT_0
#define CMT_CMCOR   CMT_CMCOR_0
#define INT_CMT     INT_CMI0
#elif (CH==1)
#define CMT_CMCSR   CMT_CMCSR_1
#define CMT_CMCNT   CMT_CMCNT_1
#define CMT_CMCOR   CMT_CMCOR_1
#define INT_CMT     INT_CMI1
#else
#error illegal CH number! (CH = 0 to 1)
#endif

/* 処理系の差異の調整 */

#if defined(__HITACHI__)        /* Renesas C */
#include <machine.h>
extern int *_C_BGN, *_C_END, *_P_BGN, *_P_END, *_VECT_BGN;
#if (INTVSZ==0)
#pragma interrupt(inthdr(resbank))
#pragma ifunc(inthdr)
#pragma interrupt(int_sio0_rx(resbank))
#pragma ifunc(int_sio0_rx)				
#pragma interrupt(int_sio0_er(resbank))
#pragma ifunc(int_sio0_er)				

#pragma interrupt(int_sio1_rx(resbank))
#pragma ifunc(int_sio1_rx)				
#pragma interrupt(int_sio1_er(resbank))
#pragma ifunc(int_sio1_er)				

#pragma interrupt(int_sio2_rx(resbank))
#pragma ifunc(int_sio2_rx)				
#pragma interrupt(int_sio2_er(resbank))
#pragma ifunc(int_sio2_er)				

// 5/11 #pragma interrupt(int_sio3_rx(resbank))
// 5/11 #pragma ifunc(int_sio3_rx)				
// 5/11 #pragma interrupt(int_sio3_er(resbank))
// 5/11 #pragma ifunc(int_sio3_er)				

#endif

#elif defined(__ghs)            /* Green Hills */
#include <machine.h>
extern char __ghsbegin_rodata[], __ghsend_rodata[];
extern char __ghsbegin_text[], __ghsend_text[];
extern char __ghsbegin_vect[];
#endif

/* Tick入力積算カウンタ */

#ifdef HOOK
UW TOTAL_TICKCK;
UW TICK_RATE;
#endif

/* 外部参照 */

extern void _kernel_interr(void);
void init_timer(void);

/* 割込みサービスルーチン生成情報 */

#if (INTVSZ!=0)
void isritm(VP_INT exinf);
const T_CISR disr_isritm = { TA_HLNG, NULL, INT_CMT, (FP)isritm, 0 };
#else
void isritm(void);
#endif

#if 1
#define	CPU_CLOCK		160000000				// CPUクロック	[Hz]
#define	PFAI_CLOCK		(CPU_CLOCK/4)			// Pφクロック	[Hz]
#define	CMT0_CLOCK			(PFAI_CLOCK/8)		// CMT0カウントクロック	[Hz]
#define	CMT0_COUNT_1MS		(CMT0_CYCLE_1000US*(CMT0_CLOCK/1000000))
												// CMT0の設定周期に対するカウント値
#define	CMT0_CYCLE_1000US	1000				// CMT0の割り込み周期	[us]
#endif

/*****************************************************************************
* 割込み初期化
*
* 形式  ER intini(void);
*
* 戻値  常に E_OK
*
* 解説  カーネルの初期化ルーチン sysini から割込み禁止状態で呼び出されます。
******************************************************************************/


ER intini(void)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

  #if (INTVSZ!=0)
    int i;

    /* 割込みベクタテーブルの初期化 */

    for (i = INT_IRQ0; i < (INTVSZ) / 4; i++)
        intvec[i] = (UW)_kernel_interr;
  #endif

    /* ベクタベースレジスタを設定 */

    set_vbr((void **)intvec);

    /* 全ての割込み対してレジスタバンクを有効にする */

    sfr_outw(INTC_IBNR, 0x4000);

  #ifdef HOOK
    TOTAL_TICKCK = 0;
  #endif
    init_timer();

    return E_OK;
}

/*****************************************************************************
* 定数か否かをチェックする
*
* 形式  BOOL _kernel_inROM(const void *p);
*
*       p     チェックする領域へのポインタ
*
* 戻値  TRUE  コード領域または定数領域
*       FALSE 上記以外
*
* 解説  オブジェクト生成情報等が定数か変数かを判定するために、カーネルの各種
*       システムコールから呼び出されます。
******************************************************************************/

BOOL _kernel_inROM(const void *p)
{
#if defined(__HITACHI__)      /* Renesas C */
    return((((UW)p >= (UW)_P_BGN) && ((UW)p <= (UW)_P_END)) || (((UW)p >= (UW)_C_BGN) && ((UW)p <= (UW)_C_END)));

#elif defined(__ghs)            /* Green Hills */
    return((((UW)p >= (UW)__ghsbegin_text) && ((UW)p <= (UW)__ghsend_text)) || (((UW)p >= (UW)__ghsbegin_rodata) && ((UW)p <= (UW)__ghsend_rodata)));

#endif
}

/*****************************************************************************
* 割込みサービスルーチン生成情報のチェック
*
* 形式  ER _kernel_check_cisr(const T_CISR *pk_cisr);
*
*       pk_cisr  チェックする割込みサービスルーチン生成情報へのポインタ
*
* 戻値  E_PAR 割込み番号とマスクレベルのいずれかが範囲外
*       E_OK  割込み番号とマスクレベルは共に正常
*
* 解説  割込みサービスルーチン生成情報の割込み番号とマスクレベルの範囲を
*       チェックします。
*       cre_isr, acre_isr システムコールから呼び出されます。
******************************************************************************/

ER _kernel_check_cisr(const T_CISR *pk_cisr)
{
  #if (INTVSZ!=0)
    if (pk_cisr->intno >= (INTVSZ) / 4)
        return E_PAR;
    return E_OK;
  #else
    return E_PAR;
  #endif
}

/*****************************************************************************
* 割込みハンドラのアドレス設定
*
* 形式  void _kernel_set_inthdr(FP hdr, const T_CISR *pk_cisr);
*
*       hdr       割込みサービスルーチンを呼び出す割込みハンドラのアドレス
*       pk_cisr   チェックする割込みサービスルーチン生成情報へのポインタ
*
* 解説  割込みサービスルーチンを呼び出すために、内部で生成される割込みハンド
*       ラのアドレスを、割込みサービスルーチン生成情報の割込み番号より算出し
*       た割込みベクタへ格納します。
*       cre_isr, acre_isr システムコールから呼び出されます。
******************************************************************************/

void _kernel_set_inthdr(FP hdr, const T_CISR *pk_cisr)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    intvec[pk_cisr->intno] = (UW)hdr;
}

/*****************************************************************************
* 割込みハンドラのアドレス取得
*
* 形式  FP _kernel_get_inthdr(const T_CISR *pk_cisr);
*
*       pk_cisr  チェックする割込みサービスルーチン生成情報へのポインタ
*
* 戻値  当該割込み番号の割込みハンドラアドレス
*
* 解説  割込みサービスルーチン生成情報の割込み番号より算出した割込みベクタか
*       ら、割込みハンドラアドレスを読み出します。
*       cre_isr, acre_isr システムコールから呼び出されます。
******************************************************************************/

FP _kernel_get_inthdr(const T_CISR *pk_cisr)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    return (FP)intvec[pk_cisr->intno];
}

/*****************************************************************************
* 割込みハンドラを定義する
*
* 形式  ER def_inh(UINT inhno, const T_DINH *pk_dinh);
*
*       inhno    割込みベクタ番号
*       pk_dinh  割込みハンドラ定義情報
*
* 戻値  エラーコード
*
* 解説  inhno に対応する割込みベクタテーブルに、inthdr を設定します。
******************************************************************************/

ER v4_def_inh(INHNO inhno, const V4_T_DINH *pk_dinh)
{
  #if (INTVSZ==0)

    return E_NOSPT;

  #else

    UINT psw;
    FP fp;
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    /* 割込みベクタテーブルへのポインタ算出 */

    if (inhno >= (INTVSZ) / 4)
        return E_PAR;

    /* 定義解除の場合、未定義割込みハンドラを設定 */

    if (pk_dinh == (T_DINH *)NADR)
        fp = (FP)_kernel_interr;
    else
        fp = pk_dinh->inthdr;

    /* CPU割込み禁止 */

    psw = vdis_psw();

    /* 新しい割込みベクタ設定 */

    intvec[inhno] = (UW)fp;

    /* CPU割込み禁止戻す */

    vset_psw(psw);
    return E_OK;

  #endif
}

/*****************************************************************************
* 周期タイマ割込み起動 (内蔵CMT)
*
* 形式  ER intsta(void);
*
* 戻値  エラーコード
*
* 補足  割込み禁止状態で実行してください。
******************************************************************************/

static UW tc;
static UH cks;

void init_timer(void)
{
    UW prescale[] = { 8, 32, 128, 512 };
    /* 時定数を計算 */

    for (cks = 0; cks <= 3; cks++) {
        tc = (((UW)(CLK) * MSEC)/((UW)prescale[cks] * 1000)) - 1;
        if (tc <= 0xffffL)
            break;
    }

  #ifdef HOOK
    TICK_RATE = 0;
  #endif
    if (cks > 3 || tc == 0L)
        return; /* エラー */

    /* タイマユニット初期化 */

    sfr_clr(STBCR4, 0x4);           /* CMTへのクロック供給 */
    sfr_clrw(CMT_CMSTR, 0x01 << CH);/* カウント一旦停止 */
    sfr_outw(CMT_CMCOR, (UH)(-1));  /* 比較値 */
    sfr_outw(CMT_CMCSR, cks);       /* クロック選択 */
    sfr_outw(CMT_CMCNT, 0x00);      /* カウンタクリア */
}

ER intsta(void)
{
    if (cks > 3 || tc == 0L)
        return E_PAR; /* エラー */

    /* 割込み定義 */

  #if (INTVSZ!=0) /* ROMに定義する場合は不要 */
    acre_isr(&disr_isritm);
  #endif

    /* 割込み優先レベル設定 */

	/* ↓下記、コメントにすると _RESET繰り返しは起きない。 */
    sfr_setw(INTC_IPR08, (IP) << (12 - CH * 4));

	
    /* タイマ初期化 */

  #ifdef HOOK
    TICK_RATE = (UW)tc;
    TOTAL_TICKCK = ((sfr_inw(CMT_CMCNT)*MSEC*1000)/TICK_RATE);
  #endif

    sfr_clrw(CMT_CMSTR, 0x01 << CH);/* カウント一旦停止 */
    sfr_outw(CMT_CMCOR, (H)(tc));   /* 比較値 */
    sfr_outw(CMT_CMCNT, (H)(0));    /* カウンタクリア */
    sfr_outw(CMT_CMCSR, 0x40|cks);  /* コンペアマッチ割り込みを許可 */
                                    /* クロック選択 */
	INTC.IPR08.BIT._CMT0 = 8;					// CMT0割り込みレベル = 4
    sfr_setw(CMT_CMSTR, 0x01 << CH);/* カウント動作再スタート */

    return E_OK;
}

/*****************************************************************************
* 周期タイマ割込サービスルーチン (内蔵 CMT)
*
******************************************************************************/

#if (INTVSZ==0) /* ROMに定義する場合は割込みハンドラ */
void isritm(void)
#else
void isritm(VP_INT exinf)
#endif
{
  #ifdef HOOK
    UINT psw;

    psw = vdis_psw();
    TOTAL_TICKCK += MSEC*1000;
  #endif
    sfr_clrw(CMT_CMCSR, 0x80);      /* CMF コンペアマッチフラグクリア */
  #ifdef HOOK
    vset_psw(psw);
  #endif
    isig_tim();                     /* システムクロック処理 */
}

/*****************************************************************************
* 周期タイマ割込みハンドラ (内蔵 CMT)
*
******************************************************************************/

#if (INTVSZ==0)
INTHDR inthdr(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    isritm();                       /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
INTHDR int_sio0_rx(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio0_rxi();                 /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
INTHDR int_sio0_er(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio0_eri();                 /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}

INTHDR int_sio1_rx(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio_rxEx(SIO_EX1);          /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
INTHDR int_sio1_er(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio_erEx(SIO_EX1);          /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
INTHDR int_sio2_rx(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio_rxEx(SIO_EX2);          /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
INTHDR int_sio2_er(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    int_sio_erEx(SIO_EX2);          /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
// 5/11INTHDR int_sio3_rx(void)
// 5/11{
// 5/11    ent_int();                      /* 割込みハンドラの開始 */
// 5/11    int_sio_rxEx(SIO_EX3);          /* 割込サービスルーチンを呼ぶ */
// 5/11    ret_int();                      /* 割込みハンドラから復帰する */
// 5/11}
// 5/11INTHDR int_sio3_er(void)
// 5/11{
// 5/11    ent_int();                      /* 割込みハンドラの開始 */
// 5/11    int_sio_erEx(SIO_EX3);          /* 割込サービスルーチンを呼ぶ */
// 5/11    ret_int();                      /* 割込みハンドラから復帰する */
// 5/11}

INTHDR int_irq4(void)
{
    ent_int();                      /* 割込みハンドラの開始 */
    IRQ4_ISR();                     /* 割込サービスルーチンを呼ぶ */
    ret_int();                      /* 割込みハンドラから復帰する */
}
#endif

/*****************************************************************************
* Tick入力積算値を算出 (内蔵CMT)
*
******************************************************************************/

#ifdef HOOK
#if (HOOK==1)
UW tickck_read(void)
#else
UW ctex_timer_read(void)
#endif
{
    UW ret, tmp;
    UH tmp_flg;

    if (TICK_RATE) {
        /* 周期タイマー起動後 */

        do {
            ret = *((volatile UW*)&TOTAL_TICKCK);
            tmp_flg = sfr_inw(CMT_CMCSR);
            tmp = sfr_inw(CMT_CMCNT);
        } while((tmp == TICK_RATE) || (ret != *((volatile UW*)&TOTAL_TICKCK)));
        /* コンペアマッチを起こしていると想定される場合は再度計算しなおす。*/

        if (tmp_flg & 0x80) {
            tmp += TICK_RATE;
        }

        return (ret + ((tmp*MSEC*1000)/TICK_RATE));

    } else {
        /* 周期タイマー起動前 */
        return ((sfr_inw(CMT_CMCNT)*MSEC*1000)/tc);
    }
}

#else

UW tickck_read(void)
{
    return 0L;
}
#endif

/* end */
