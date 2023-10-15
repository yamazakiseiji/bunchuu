/******************************************************************************
* ＳＨ７２３７ 内部Ｉ／Ｏ定義                                                 *
*                                                                             *
*  File name : SH7237.h                                                       *
*  All rights reserved.                                                       *
*                                                                             *
*                                                                             *
******************************************************************************/

#ifndef SH7237_H
#define SH7237_H

/* 内蔵周辺モジュールのベースアドレス */

#ifdef CPU_SH   /* SH用クロスコンパイラでコンパイルされている場合 */
#define IO_BASE ((volatile UB *)0x00000000L)

#endif

/* 内蔵周辺モジュール入出力マクロ */

UB sfr_in(UINT n);              /* バイトデータの入力 */
void sfr_out(UINT n, UB c);     /* バイトデータの出力 */
UH sfr_inw(UINT n);             /* ワードデータの入力 */
void sfr_outw(UINT n, UH c);    /* ワードデータの出力 */
UW sfr_inl(UINT n);             /* ロングワードデータの入力 */
void sfr_outl(UINT n, UW c);    /* ロングワードデータの出力 */
void sfr_set(UINT n, UB c);     /* バイトデータのビットセット */
void sfr_clr(UINT n, UB c);     /* バイトデータのビットクリア */
void sfr_setw(UINT n, UH c);    /* ワードデータのビットセット */
void sfr_clrw(UINT n, UH c);    /* ワードデータのビットクリア */
void sfr_setl(UINT n, UW c);    /* ロングワードデータのビットセット */
void sfr_clrl(UINT n, UW c);    /* ロングワードデータのビットクリア */

#ifdef IO_BASE
#define sfr_in(n)       (*(IO_BASE+(n)))
#define sfr_out(n,c)    (*(IO_BASE+(n))=(c))
#define sfr_inw(n)      (*(volatile UH *)(IO_BASE+(n)))
#define sfr_outw(n,c)   (*(volatile UH *)(IO_BASE+(n))=(c))
#define sfr_inl(n)      (*(volatile UW *)(IO_BASE+(n)))
#define sfr_outl(n,c)   (*(volatile UW *)(IO_BASE+(n))=(c))
#define sfr_set(n,c)    (*(IO_BASE+(n))|=(c))
#define sfr_clr(n,c)    (*(IO_BASE+(n))&=~(c))
#define sfr_setw(n,c)   (*(volatile UH *)(IO_BASE+(n))|=(c))
#define sfr_clrw(n,c)   (*(volatile UH *)(IO_BASE+(n))&=~(c))
#define sfr_setl(n,c)   (*(volatile UW *)(IO_BASE+(n))|=(c))
#define sfr_clrl(n,c)   (*(volatile UW *)(IO_BASE+(n))&=~(c))
#endif



#define  CPG_FRQCR          0xFFFE0010  /* 周波数制御レジスタ */
#define  CPG_MCLKCR         0xFFFE0410  /* MTUクロック周波数制御レジスタ */
#define  CPG_ACLKCR         0xFFFE0414  /* ADクロック周波数制御レジスタ */
#define  CPG_OSCCR          0xFFFE001C  /* 発振停止検出制御レジスタ */
#define  INTC_ICR0          0xFFFE0800  /* 割り込みコントロールレジスタ0 */
#define  INTC_ICR1          0xFFFE0802  /* 割り込みコントロールレジスタ1 */
#define  INTC_IRQRR         0xFFFE0806  /* IRQ割り込み要求レジスタ */
#define  INTC_IBCR          0xFFFE080C  /* バンクコントロールレジスタ */
#define  INTC_IBNR          0xFFFE080E  /* バンク番号レジスタ */
#define  INTC_IPR01         0xFFFE0818  /* 割り込み優先レベル設定レジスタ01 */
#define  INTC_IPR02         0xFFFE081A  /* 割り込み優先レベル設定レジスタ02 */
#define  INTC_IPR05         0xFFFE0820  /* 割り込み優先レベル設定レジスタ05 */
#define  INTC_IPR06         0xFFFE0C00  /* 割り込み優先レベル設定レジスタ06 */
#define  INTC_IPR07         0xFFFE0C02  /* 割り込み優先レベル設定レジスタ07 */
#define  INTC_IPR08         0xFFFE0C04  /* 割り込み優先レベル設定レジスタ08 */
#define  INTC_IPR09         0xFFFE0C06  /* 割り込み優先レベル設定レジスタ09 */
#define  INTC_IPR10         0xFFFE0C08  /* 割り込み優先レベル設定レジスタ10 */
#define  INTC_IPR11         0xFFFE0C0A  /* 割り込み優先レベル設定レジスタ11 */
#define  INTC_IPR12         0xFFFE0C0C  /* 割り込み優先レベル設定レジスタ12 */
#define  INTC_IPR13         0xFFFE0C0E  /* 割り込み優先レベル設定レジスタ13 */
#define  INTC_IPR14         0xFFFE0C10  /* 割り込み優先レベル設定レジスタ14 */
#define  INTC_IPR15         0xFFFE0C12  /* 割り込み優先レベル設定レジスタ15 */
#define  INTC_IPR16         0xFFFE0C14  /* 割り込み優先レベル設定レジスタ16 */
#define  INTC_IPR17         0xFFFE0C16  /* 割り込み優先レベル設定レジスタ17 */
#define  INTC_IPR18         0xFFFE0C18  /* 割り込み優先レベル設定レジスタ18 */
#define  UBC_BAR_0          0xFFFC0400  /* ブレークアドレスレジスタ_0 */
#define  UBC_BAMR_0         0xFFFC0404  /* ブレークアドレスマスクレジスタ_0 */
#define  UBC_BBR_0          0xFFFC04A0  /* ブレークバスサイクルレジスタ_0 */
#define  UBC_BAR_1          0xFFFC0410  /* ブレークアドレスレジスタ_1 */
#define  UBC_BAMR_1         0xFFFC0414  /* ブレークアドレスマスクレジスタ_1 */
#define  UBC_BBR_1          0xFFFC04B0  /* ブレークバスサイクルレジスタ_1 */
#define  UBC_BAR_2          0xFFFC0420  /* ブレークアドレスレジスタ_2 */
#define  UBC_BAMR_2         0xFFFC0424  /* ブレークアドレスマスクレジスタ_2 */
#define  UBC_BBR_2          0xFFFC04A4  /* ブレークバスサイクルレジスタ_2 */
#define  UBC_BAR_3          0xFFFC0430  /* ブレークアドレスレジスタ_3 */
#define  UBC_BAMR_3         0xFFFC0434  /* ブレークアドレスマスクレジスタ_3 */
#define  UBC_BBR_3          0xFFFC04B4  /* ブレークバスサイクルレジスタ_3 */
#define  UBC_BRCR           0xFFFC04C0  /* ブレークコントロールレジスタ */
#define  DTC_DTCERA         0xFFFE6000  /* DTCイネーブルレジスタA */
#define  DTC_DTCERB         0xFFFE6002  /* DTCイネーブルレジスタB */
#define  DTC_DTCERC         0xFFFE6004  /* DTCイネーブルレジスタC */
#define  DTC_DTCERD         0xFFFE6006  /* DTCイネーブルレジスタD */
#define  DTC_DTCERE         0xFFFE6008  /* DTCイネーブルレジスタE */
#define  DTC_DTCCR          0xFFFE6010  /* DTCコントロールレジスタ */
#define  DTC_DTCVBR         0xFFFE6014  /* DTCベクタベースレジスタ */
#define  BSC_CMNCR          0xFFFC0000  /* 共通コントロールレジスタ */
#define  BSC_CS0BCR         0xFFFC0004  /* CS0空間バスコントロールレジスタ */
#define  BSC_CS1BCR         0xFFFC0008  /* CS1空間バスコントロールレジスタ */
#define  BSC_CS3BCR         0xFFFC0010  /* CS3空間バスコントロールレジスタ */
#define  BSC_CS4BCR         0xFFFC0014  /* CS4空間バスコントロールレジスタ */
#define  BSC_CS5BCR         0xFFFC0018  /* CS5空間バスコントロールレジスタ */
#define  BSC_CS6BCR         0xFFFC001C  /* CS6空間バスコントロールレジスタ */
#define  BSC_CS0WCR         0xFFFC0028  /* CS0空間ウェイトコントロールレジスタ */
#define  BSC_CS1WCR         0xFFFC002C  /* CS1空間ウェイトコントロールレジスタ */
#define  BSC_CS3WCR         0xFFFC0034  /* CS3空間ウェイトコントロールレジスタ */
#define  BSC_CS4WCR         0xFFFC0038  /* CS4空間ウェイトコントロールレジスタ */
#define  BSC_CS5WCR         0xFFFC003C  /* CS5空間ウェイトコントロールレジスタ */
#define  BSC_CS6WCR         0xFFFC0040  /* CS6空間ウェイトコントロールレジスタ */
#define  BSC_BSCEHR         0xFFFE3C1A  /* バス機能拡張レジスタ */
#define  DMAC_SAR_0         0xFFFE1000  /* DMAソースアドレスレジスタ_0 */
#define  DMAC_DAR_0         0xFFFE1004  /* DMAデスティネーションアドレスレジスタ_0 */
#define  DMAC_DMATCR_0      0xFFFE1008  /* DMAトランスファカウントレジスタ_0 */
#define  DMAC_CHCR_0        0xFFFE100C  /* DMAチャネルコントロールレジスタ_0 */
#define  DMAC_RSAR_0        0xFFFE1100  /* DMAリロードソースアドレスレジスタ_0 */
#define  DMAC_RDAR_0        0xFFFE1104  /* DMAリロードデスティネーションアドレスレジスタ_0 */
#define  DMAC_RDMATCR_0     0xFFFE1108  /* DMAリロードトランスファカウントレジスタ_0 */
#define  DMAC_SAR_1         0xFFFE1010  /* DMAソースアドレスレジスタ_1 */
#define  DMAC_DAR_1         0xFFFE1014  /* DMAデスティネーションアドレスレジスタ_1 */
#define  DMAC_DMATCR_1      0xFFFE1018  /* DMAトランスファカウントレジスタ_1 */
#define  DMAC_CHCR_1        0xFFFE101C  /* DMAチャネルコントロールレジスタ_1 */
#define  DMAC_RSAR_1        0xFFFE1110  /* DMAリロードソースアドレスレジスタ_1 */
#define  DMAC_RDAR_1        0xFFFE1114  /* DMAリロードデスティネーションアドレスレジスタ_1 */
#define  DMAC_RDMATCR_1     0xFFFE1118  /* DMAリロードトランスファカウントレジスタ_1 */
#define  DMAC_SAR_2         0xFFFE1020  /* DMAソースアドレスレジスタ_2 */
#define  DMAC_DAR_2         0xFFFE1024  /* DMAデスティネーションアドレスレジスタ_2 */
#define  DMAC_DMATCR_2      0xFFFE1028  /* DMAトランスファカウントレジスタ_2 */
#define  DMAC_CHCR_2        0xFFFE102C  /* DMAチャネルコントロールレジスタ_2 */
#define  DMAC_RSAR_2        0xFFFE1120  /* DMAリロードソースアドレスレジスタ_2 */
#define  DMAC_RDAR_2        0xFFFE1124  /* DMAリロードデスティネーションアドレスレジスタ_2 */
#define  DMAC_RDMATCR_2     0xFFFE1128  /* DMAリロードトランスファカウントレジスタ_2 */
#define  DMAC_SAR_3         0xFFFE1030  /* DMAソースアドレスレジスタ_3 */
#define  DMAC_DAR_3         0xFFFE1034  /* DMAデスティネーションアドレスレジスタ_3 */
#define  DMAC_DMATCR_3      0xFFFE1038  /* DMAトランスファカウントレジスタ_3 */
#define  DMAC_CHCR_3        0xFFFE103C  /* DMAチャネルコントロールレジスタ_3 */
#define  DMAC_RSAR_3        0xFFFE1130  /* DMAリロードソースアドレスレジスタ_3 */
#define  DMAC_RDAR_3        0xFFFE1134  /* DMAリロードデスティネーションアドレスレジスタ_3 */
#define  DMAC_RDMATCR_3     0xFFFE1138  /* DMAリロードトランスファカウントレジスタ_3 */
#define  DMAC_SAR_4         0xFFFE1040  /* DMAソースアドレスレジスタ_4 */
#define  DMAC_DAR_4         0xFFFE1044  /* DMAデスティネーションアドレスレジスタ_4 */
#define  DMAC_DMATCR_4      0xFFFE1048  /* DMAトランスファカウントレジスタ_4 */
#define  DMAC_CHCR_4        0xFFFE104C  /* DMAチャネルコントロールレジスタ_4 */
#define  DMAC_RSAR_4        0xFFFE1140  /* DMAリロードソースアドレスレジスタ_4 */
#define  DMAC_RDAR_4        0xFFFE1144  /* DMAリロードデスティネーションアドレスレジスタ_4 */
#define  DMAC_RDMATCR_4     0xFFFE1148  /* DMAリロードトランスファカウントレジスタ_4 */
#define  DMAC_SAR_5         0xFFFE1050  /* DMAソースアドレスレジスタ_5 */
#define  DMAC_DAR_5         0xFFFE1054  /* DMAデスティネーションアドレスレジスタ_5 */
#define  DMAC_DMATCR_5      0xFFFE1058  /* DMAトランスファカウントレジスタ_5 */
#define  DMAC_CHCR_5        0xFFFE105C  /* DMAチャネルコントロールレジスタ_5 */
#define  DMAC_RSAR_5        0xFFFE1150  /* DMAリロードソースアドレスレジスタ_5 */
#define  DMAC_RDAR_5        0xFFFE1154  /* DMAリロードデスティネーションアドレスレジスタ_5 */
#define  DMAC_RDMATCR_5     0xFFFE1158  /* DMAリロードトランスファカウントレジスタ_5 */
#define  DMAC_SAR_6         0xFFFE1060  /* DMAソースアドレスレジスタ_6 */
#define  DMAC_DAR_6         0xFFFE1064  /* DMAデスティネーションアドレスレジスタ_6 */
#define  DMAC_DMATCR_6      0xFFFE1068  /* DMAトランスファカウントレジスタ_6 */
#define  DMAC_CHCR_6        0xFFFE106C  /* DMAチャネルコントロールレジスタ_6 */
#define  DMAC_RSAR_6        0xFFFE1160  /* DMAリロードソースアドレスレジスタ_6 */
#define  DMAC_RDAR_6        0xFFFE1164  /* DMAリロードデスティネーションアドレスレジスタ_6 */
#define  DMAC_RDMATCR_6     0xFFFE1168  /* DMAリロードトランスファカウントレジスタ_6 */
#define  DMAC_SAR_7         0xFFFE1070  /* DMAソースアドレスレジスタ_7 */
#define  DMAC_DAR_7         0xFFFE1074  /* DMAデスティネーションアドレスレジスタ_7 */
#define  DMAC_DMATCR_7      0xFFFE1078  /* DMAトランスファカウントレジスタ_7 */
#define  DMAC_CHCR_7        0xFFFE107C  /* DMAチャネルコントロールレジスタ_7 */
#define  DMAC_RSAR_7        0xFFFE1170  /* DMAリロードソースアドレスレジスタ_7 */
#define  DMAC_RDAR_7        0xFFFE1174  /* DMAリロードデスティネーションアドレスレジスタ_7 */
#define  DMAC_RDMATCR_7     0xFFFE1178  /* DMAリロードトランスファカウントレジスタ_7 */
#define  DMAC_DMAOR         0xFFFE1200  /* DMAオペレーションレジスタ */
#define  DMAC_DMARS0        0xFFFE1300  /* DMA拡張リソースセレクタ0 */
#define  DMAC_DMARS1        0xFFFE1304  /* DMA拡張リソースセレクタ1 */
#define  DMAC_DMARS2        0xFFFE1308  /* DMA拡張リソースセレクタ2 */
#define  DMAC_DMARS3        0xFFFE130C  /* DMA拡張リソースセレクタ3 */
#define  MTU2_TCR_0         0xFFFE4300  /* タイマコントロールレジスタ_0 */
#define  MTU2_TMDR_0        0xFFFE4301  /* タイマモードレジスタ_0 */
#define  MTU2_TIORH_0       0xFFFE4302  /* タイマI/OコントロールレジスタH_0 */
#define  MTU2_TIORL_0       0xFFFE4303  /* タイマI/OコントロールレジスタL_0 */
#define  MTU2_TIER_0        0xFFFE4304  /* タイマインタラプトイネーブルレジスタ_0 */
#define  MTU2_TSR_0         0xFFFE4305  /* タイマステータスレジスタ_0 */
#define  MTU2_TCNT_0        0xFFFE4306  /* タイマカウンタ_0 */
#define  MTU2_TGRA_0        0xFFFE4308  /* タイマジェネラルレジスタA_0 */
#define  MTU2_TGRB_0        0xFFFE430A  /* タイマジェネラルレジスタB_0 */
#define  MTU2_TGRC_0        0xFFFE430C  /* タイマジェネラルレジスタC_0 */
#define  MTU2_TGRD_0        0xFFFE430E  /* タイマジェネラルレジスタD_0 */
#define  MTU2_TGRE_0        0xFFFE4320  /* タイマジェネラルレジスタE_0 */
#define  MTU2_TGRF_0        0xFFFE4322  /* タイマジェネラルレジスタF_0 */
#define  MTU2_TIER2_0       0xFFFE4324  /* タイマインタラプトイネーブルレジスタ2_0 */
#define  MTU2_TSR2_0        0xFFFE4325  /* タイマステータスレジスタ2_0 */
#define  MTU2_TBTM_0        0xFFFE4326  /* タイマバッファ動作転送モードレジスタ_0 */
#define  MTU2_TCR_1         0xFFFE4380  /* タイマコントロールレジスタ_1 */
#define  MTU2_TMDR_1        0xFFFE4381  /* タイマモードレジスタ_1 */
#define  MTU2_TIOR_1        0xFFFE4382  /* タイマI/Oコントロールレジスタ_1 */
#define  MTU2_TIER_1        0xFFFE4384  /* タイマインタラプトイネーブルレジスタ_1 */
#define  MTU2_TSR_1         0xFFFE4385  /* タイマステータスレジスタ_1 */
#define  MTU2_TCNT_1        0xFFFE4386  /* タイマカウンタ_1 */
#define  MTU2_TGRA_1        0xFFFE4388  /* タイマジェネラルレジスタA_1 */
#define  MTU2_TGRB_1        0xFFFE438A  /* タイマジェネラルレジスタB_1 */
#define  MTU2_TICCR         0xFFFE4390  /* タイマインプットキャプチャコントロールレジスタ */
#define  MTU2_TCR_2         0xFFFE4000  /* タイマコントロールレジスタ_2 */
#define  MTU2_TMDR_2        0xFFFE4001  /* タイマモードレジスタ_2 */
#define  MTU2_TIOR_2        0xFFFE4002  /* タイマI/Oコントロールレジスタ_2 */
#define  MTU2_TIER_2        0xFFFE4004  /* タイマインタラプトイネーブルレジスタ_2 */
#define  MTU2_TSR_2         0xFFFE4005  /* タイマステータスレジスタ_2 */
#define  MTU2_TCNT_2        0xFFFE4006  /* タイマカウンタ_2 */
#define  MTU2_TGRA_2        0xFFFE4008  /* タイマジェネラルレジスタA_2 */
#define  MTU2_TGRB_2        0xFFFE400A  /* タイマジェネラルレジスタB_2 */
#define  MTU2_TCR_3         0xFFFE4200  /* タイマコントロールレジスタ_3 */
#define  MTU2_TMDR_3        0xFFFE4202  /* タイマモードレジスタ_3 */
#define  MTU2_TIORH_3       0xFFFE4204  /* タイマI/OコントロールレジスタH_3 */
#define  MTU2_TIORL_3       0xFFFE4205  /* タイマI/OコントロールレジスタL_3 */
#define  MTU2_TIER_3        0xFFFE4208  /* タイマインタラプトイネーブルレジスタ_3 */
#define  MTU2_TSR_3         0xFFFE422C  /* タイマステータスレジスタ_3 */
#define  MTU2_TCNT_3        0xFFFE4210  /* タイマカウンタ_3 */
#define  MTU2_TGRA_3        0xFFFE4218  /* タイマジェネラルレジスタA_3 */
#define  MTU2_TGRB_3        0xFFFE421A  /* タイマジェネラルレジスタB_3 */
#define  MTU2_TGRC_3        0xFFFE4224  /* タイマジェネラルレジスタC_3 */
#define  MTU2_TGRD_3        0xFFFE4226  /* タイマジェネラルレジスタD_3 */
#define  MTU2_TBTM_3        0xFFFE4238  /* タイマバッファ動作転送モードレジスタ_3 */
#define  MTU2_TCR_4         0xFFFE4201  /* タイマコントロールレジスタ_4 */
#define  MTU2_TMDR_4        0xFFFE4203  /* タイマモードレジスタ_4 */
#define  MTU2_TIORH_4       0xFFFE4206  /* タイマI/OコントロールレジスタH_4 */
#define  MTU2_TIORL_4       0xFFFE4207  /* タイマI/OコントロールレジスタL_4 */
#define  MTU2_TIER_4        0xFFFE4209  /* タイマインタラプトイネーブルレジスタ_4 */
#define  MTU2_TSR_4         0xFFFE422D  /* タイマステータスレジスタ_4 */
#define  MTU2_TCNT_4        0xFFFE4212  /* タイマカウンタ_4 */
#define  MTU2_TGRA_4        0xFFFE421C  /* タイマジェネラルレジスタA_4 */
#define  MTU2_TGRB_4        0xFFFE421E  /* タイマジェネラルレジスタB_4 */
#define  MTU2_TGRC_4        0xFFFE4228  /* タイマジェネラルレジスタC_4 */
#define  MTU2_TGRD_4        0xFFFE422A  /* タイマジェネラルレジスタD_4 */
#define  MTU2_TBTM_4        0xFFFE4239  /* タイマバッファ動作転送モードレジスタ_4 */
#define  MTU2_TADCR         0xFFFE4240  /* タイマA/D変換開始要求コントロールレジスタ */
#define  MTU2_TADCORA_4     0xFFFE4244  /* タイマA/D変換開始要求周期設定レジスタA_4 */
#define  MTU2_TADCORB_4     0xFFFE4246  /* タイマA/D変換開始要求周期設定レジスタB_4 */
#define  MTU2_TADCOBRA_4    0xFFFE4248  /* タイマA/D変換開始要求周期設定バッファレジスタA_4 */
#define  MTU2_TADCOBRB_4    0xFFFE424A  /* タイマA/D変換開始要求周期設定バッファレジスタB_4 */
#define  MTU2_TCRU_5        0xFFFE4084  /* タイマコントロールレジスタU_5 */
#define  MTU2_TCRV_5        0xFFFE4094  /* タイマコントロールレジスタV_5 */
#define  MTU2_TCRW_5        0xFFFE40A4  /* タイマコントロールレジスタW_5 */
#define  MTU2_TIORU_5       0xFFFE4086  /* タイマI/OコントロールレジスタU_5 */
#define  MTU2_TIORV_5       0xFFFE4096  /* タイマI/OコントロールレジスタV_5 */
#define  MTU2_TIORW_5       0xFFFE40A6  /* タイマI/OコントロールレジスタW_5 */
#define  MTU2_TIER_5        0xFFFE40B2  /* タイマインタラプトイネーブルレジスタ_5 */
#define  MTU2_TSR_5         0xFFFE40B0  /* タイマステータスレジスタ_5 */
#define  MTU2_TSTR_5        0xFFFE40B4  /* タイマスタートレジスタ_5 */
#define  MTU2_TCNTU_5       0xFFFE4080  /* タイマカウンタU_5 */
#define  MTU2_TCNTV_5       0xFFFE4090  /* タイマカウンタV_5 */
#define  MTU2_TCNTW_5       0xFFFE40A0  /* タイマカウンタW_5 */
#define  MTU2_TGRU_5        0xFFFE4082  /* タイマジェネラルレジスタU_5 */
#define  MTU2_TGRV_5        0xFFFE4092  /* タイマジェネラルレジスタV_5 */
#define  MTU2_TGRW_5        0xFFFE40A2  /* タイマジェネラルレジスタW_5 */
#define  MTU2_TCNTCMPCLR    0xFFFE40B6  /* タイマコンペアマッチクリアレジスタ */
#define  MTU2_TSTR          0xFFFE4280  /* タイマスタートレジスタ */
#define  MTU2_TSYR          0xFFFE4281  /* タイマシンクロレジスタ */
#define  MTU2_TCSYSTR       0xFFFE4282  /* タイマカウンタシンクロスタートレジスタ */
#define  MTU2_TRWER         0xFFFE4284  /* タイマリードライトイネーブルレジスタ */
#define  MTU2_TOER          0xFFFE420A  /* タイマアウトプットマスタイネーブルレジスタ */
#define  MTU2_TOCR1         0xFFFE420E  /* タイマアウトプットコントロールレジスタ1 */
#define  MTU2_TOCR2         0xFFFE420F  /* タイマアウトプットコントロールレジスタ2 */
#define  MTU2_TGCR          0xFFFE420D  /* タイマゲートコントロールレジスタ */
#define  MTU2_TCDR          0xFFFE4214  /* タイマ周期データレジスタ */
#define  MTU2_TDDR          0xFFFE4216  /* タイマデッドタイムデータレジスタ */
#define  MTU2_TCNTS         0xFFFE4220  /* タイマサブカウンタ */
#define  MTU2_TCBR          0xFFFE4222  /* タイマ周期バッファレジスタ */
#define  MTU2_TITCR         0xFFFE4230  /* タイマ割り込み間引き設定レジスタ */
#define  MTU2_TITCNT        0xFFFE4231  /* タイマ割り込み間引き回数カウンタ */
#define  MTU2_TBTER         0xFFFE4232  /* タイマバッファ転送設定レジスタ */
#define  MTU2_TDER          0xFFFE4234  /* タイマデッドタイムイネーブルレジスタ */
#define  MTU2_TWCR          0xFFFE4260  /* タイマ波形コントロールレジスタ */
#define  MTU2_TOLBR         0xFFFE4236  /* タイマアウトプットレベルバッファレジスタ */
#define  MTU2S_TCR_3S       0xFFFE4A00  /* タイマコントロールレジスタ_3S */
#define  MTU2S_TMDR_3S      0xFFFE4A02  /* タイマモードレジスタ_3S */
#define  MTU2S_TIORH_3S     0xFFFE4A04  /* タイマI/OコントロールレジスタH_3S */
#define  MTU2S_TIORL_3S     0xFFFE4A05  /* タイマI/OコントロールレジスタL_3S */
#define  MTU2S_TIER_3S      0xFFFE4A08  /* タイマインタラプトイネーブルレジスタ_3S */
#define  MTU2S_TSR_3S       0xFFFE4A2C  /* タイマステータスレジスタ_3S */
#define  MTU2S_TCNT_3S      0xFFFE4A10  /* タイマカウンタ_3S */
#define  MTU2S_TGRA_3S      0xFFFE4A18  /* タイマジェネラルレジスタA_3S */
#define  MTU2S_TGRB_3S      0xFFFE4A1A  /* タイマジェネラルレジスタB_3S */
#define  MTU2S_TGRC_3S      0xFFFE4A24  /* タイマジェネラルレジスタC_3S */
#define  MTU2S_TGRD_3S      0xFFFE4A26  /* タイマジェネラルレジスタD_3S */
#define  MTU2S_TBTM_3S      0xFFFE4A38  /* タイマバッファ動作転送モードレジスタ_3S */
#define  MTU2S_TCR_4S       0xFFFE4A01  /* タイマコントロールレジスタ_4S */
#define  MTU2S_TMDR_4S      0xFFFE4A03  /* タイマモードレジスタ_4S */
#define  MTU2S_TIORH_4S     0xFFFE4A06  /* タイマI/OコントロールレジスタH_4S */
#define  MTU2S_TIORL_4S     0xFFFE4A07  /* タイマI/OコントロールレジスタL_4S */
#define  MTU2S_TIER_4S      0xFFFE4A09  /* タイマインタラプトイネーブルレジスタ_4S */
#define  MTU2S_TSR_4S       0xFFFE4A2D  /* タイマステータスレジスタ_4S */
#define  MTU2S_TCNT_4S      0xFFFE4A12  /* タイマカウンタ_4S */
#define  MTU2S_TGRA_4S      0xFFFE4A1C  /* タイマジェネラルレジスタA_4S */
#define  MTU2S_TGRB_4S      0xFFFE4A1E  /* タイマジェネラルレジスタB_4S */
#define  MTU2S_TGRC_4S      0xFFFE4A28  /* タイマジェネラルレジスタC_4S */
#define  MTU2S_TGRD_4S      0xFFFE4A2A  /* タイマジェネラルレジスタD_4S */
#define  MTU2S_TBTM_4S      0xFFFE4A39  /* タイマバッファ動作転送モードレジスタ_4S */
#define  MTU2S_TADCRS       0xFFFE4A40  /* タイマA/D変換開始要求コントロールレジスタS */
#define  MTU2S_TADCORA_4S   0xFFFE4A44  /* タイマA/D変換開始要求周期設定レジスタA_4S */
#define  MTU2S_TADCORB_4S   0xFFFE4A46  /* タイマA/D変換開始要求周期設定レジスタB_4S */
#define  MTU2S_TADCOBRA_4S  0xFFFE4A48  /* タイマA/D変換開始要求周期設定バッファレジスタA_4S */
#define  MTU2S_TADCOBRB_4S  0xFFFE4A4A  /* タイマA/D変換開始要求周期設定バッファレジスタB_4S */
#define  MTU2S_TCRU_5S      0xFFFE4884  /* タイマコントロールレジスタU_5S */
#define  MTU2S_TCRV_5S      0xFFFE4894  /* タイマコントロールレジスタV_5S */
#define  MTU2S_TCRW_5S      0xFFFE48A4  /* タイマコントロールレジスタW_5S */
#define  MTU2S_TIORU_5S     0xFFFE4886  /* タイマI/OコントロールレジスタU_5S */
#define  MTU2S_TIORV_5S     0xFFFE4896  /* タイマI/OコントロールレジスタV_5S */
#define  MTU2S_TIORW_5S     0xFFFE48A6  /* タイマI/OコントロールレジスタW_5S */
#define  MTU2S_TIER_5S      0xFFFE48B2  /* タイマインタラプトイネーブルレジスタ_5S */
#define  MTU2S_TSR_5S       0xFFFE48B0  /* タイマステータスレジスタ_5S */
#define  MTU2S_TSTR_5S      0xFFFE48B4  /* タイマスタートレジスタ_5S */
#define  MTU2S_TCNTU_5S     0xFFFE4880  /* タイマカウンタU_5S */
#define  MTU2S_TCNTV_5S     0xFFFE4890  /* タイマカウンタV_5S */
#define  MTU2S_TCNTW_5S     0xFFFE48A0  /* タイマカウンタW_5S */
#define  MTU2S_TGRU_5S      0xFFFE4882  /* タイマジェネラルレジスタU_5S */
#define  MTU2S_TGRV_5S      0xFFFE4892  /* タイマジェネラルレジスタV_5S */
#define  MTU2S_TGRW_5S      0xFFFE48A2  /* タイマジェネラルレジスタW_5S */
#define  MTU2S_TCNTCMPCLRS  0xFFFE48B6  /* タイマコンペアマッチクリアレジスタS */
#define  MTU2S_TSTRS        0xFFFE4A80  /* タイマスタートレジスタS */
#define  MTU2S_TSYRS        0xFFFE4A81  /* タイマシンクロレジスタS */
#define  MTU2S_TRWERS       0xFFFE4A84  /* タイマリードライトイネーブルレジスタS */
#define  MTU2S_TOERS        0xFFFE4A0A  /* タイマアウトプットマスタイネーブルレジスタS */
#define  MTU2S_TOCR1S       0xFFFE4A0E  /* タイマアウトプットコントロールレジスタ1S */
#define  MTU2S_TOCR2S       0xFFFE4A0F  /* タイマアウトプットコントロールレジスタ2S */
#define  MTU2S_TGCRS        0xFFFE4A0D  /* タイマゲートコントロールレジスタS */
#define  MTU2S_TCDRS        0xFFFE4A14  /* タイマ周期データレジスタS */
#define  MTU2S_TDDRS        0xFFFE4A16  /* タイマデッドタイムデータレジスタS */
#define  MTU2S_TCNTSS       0xFFFE4A20  /* タイマサブカウンタS */
#define  MTU2S_TCBRS        0xFFFE4A22  /* タイマ周期バッファレジスタS */
#define  MTU2S_TITCRS       0xFFFE4A30  /* タイマ割り込み間引き設定レジスタS */
#define  MTU2S_TITCNTS      0xFFFE4A31  /* タイマ割り込み間引き回数カウンタS */
#define  MTU2S_TBTERS       0xFFFE4A32  /* タイマバッファ転送設定レジスタS */
#define  MTU2S_TDERS        0xFFFE4A34  /* タイマデッドタイムイネーブルレジスタS */
#define  MTU2S_TSYCRS       0xFFFE4A50  /* タイマシンクロクリアレジスタS */
#define  MTU2S_TWCRS        0xFFFE4A60  /* タイマ波形コントロールレジスタS */
#define  MTU2S_TOLBRS       0xFFFE4A36  /* タイマアウトプットレベルバッファレジスタS */
#define  POE2_ICSR1         0xFFFE5000  /* 入力レベルコントロール／ステータスレジスタ1 */
#define  POE2_OCSR1         0xFFFE5002  /* 出力レベルコントロール／ステータスレジスタ1 */
#define  POE2_ICSR2         0xFFFE5004  /* 入力レベルコントロール／ステータスレジスタ2 */
#define  POE2_OCSR2         0xFFFE5006  /* 出力レベルコントロール／ステータスレジスタ2 */
#define  POE2_ICSR3         0xFFFE5008  /* 入力レベルコントロール／ステータスレジスタ3 */
#define  POE2_SPOER         0xFFFE500A  /* ソフトウェアポートアウトプットイネーブルレジスタ */
#define  POE2_POECR1        0xFFFE500B  /* ポートアウトプットイネーブルコントロールレジスタ1 */
#define  POE2_POECR2        0xFFFE500C  /* ポートアウトプットイネーブルコントロールレジスタ2 */
#define  POE2_POECR3        0xFFFE500E  /* ポートアウトプットイネーブルコントロールレジスタ3 */
#define  CMT_CMSTR          0xFFFEC000  /* コンペアマッチタイマスタートレジスタ */
#define  CMT_CMCSR_0        0xFFFEC002  /* コンペアマッチタイマコントロール／ステータスレジスタ_0 */
#define  CMT_CMCNT_0        0xFFFEC004  /* コンペアマッチカウンタ_0 */
#define  CMT_CMCOR_0        0xFFFEC006  /* コンペアマッチコンスタントレジスタ_0 */
#define  CMT_CMCSR_1        0xFFFEC008  /* コンペアマッチタイマコントロール／ステータスレジスタ_1 */
#define  CMT_CMCNT_1        0xFFFEC00A  /* コンペアマッチカウンタ_1 */
#define  CMT_CMCOR_1        0xFFFEC00C  /* コンペアマッチコンスタントレジスタ_1 */
#define  WDT_WTCSR          0xFFFE0000  /* ウォッチドッグタイマコントロール／ステータスレジスタ */
#define  WDT_WTCNT          0xFFFE0002  /* ウォッチドッグタイマカウンタ */
#define  WDT_WRCSR          0xFFFE0004  /* ウォッチドッグリセットコントロール／ステータスレジスタ */
#define  SCI_SCSMR_0        0xFFFF8000  /* シリアルモードレジスタ_0 */
#define  SCI_SCBRR_0        0xFFFF8002  /* ビットレートレジスタ_0 */
#define  SCI_SCSCR_0        0xFFFF8004  /* シリアルコントロールレジスタ_0 */
#define  SCI_SCTDR_0        0xFFFF8006  /* トランスミットデータレジスタ_0 */
#define  SCI_SCSSR_0        0xFFFF8008  /* シリアルステータスレジスタ_0 */
#define  SCI_SCRDR_0        0xFFFF800A  /* レシーブデータレジスタ_0 */
#define  SCI_SCSDCR_0       0xFFFF800C  /* シリアルディレクションコントロールレジスタ_0 */
#define  SCI_SCSPTR_0       0xFFFF800E  /* シリアルポートレジスタ_0 */
#define  SCI_SPMR_0         0xFFFF8014  /* サンプリングモードレジスタ_0 */
#define  SCI_SCSMR_1        0xFFFF8800  /* シリアルモードレジスタ_1 */
#define  SCI_SCBRR_1        0xFFFF8802  /* ビットレートレジスタ_1 */
#define  SCI_SCSCR_1        0xFFFF8804  /* シリアルコントロールレジスタ_1 */
#define  SCI_SCTDR_1        0xFFFF8806  /* トランスミットデータレジスタ_1 */
#define  SCI_SCSSR_1        0xFFFF8808  /* シリアルステータスレジスタ_1 */
#define  SCI_SCRDR_1        0xFFFF880A  /* レシーブデータレジスタ_1 */
#define  SCI_SCSDCR_1       0xFFFF880C  /* シリアルディレクションコントロールレジスタ_1 */
#define  SCI_SCSPTR_1       0xFFFF880E  /* シリアルポートレジスタ_1 */
#define  SCI_SPMR_1         0xFFFF8814  /* サンプリングモードレジスタ_1 */
#define  SCI_SCSMR_2        0xFFFF9000  /* シリアルモードレジスタ_2 */
#define  SCI_SCBRR_2        0xFFFF9002  /* ビットレートレジスタ_2 */
#define  SCI_SCSCR_2        0xFFFF9004  /* シリアルコントロールレジスタ_2 */
#define  SCI_SCTDR_2        0xFFFF9006  /* トランスミットデータレジスタ_2 */
#define  SCI_SCSSR_2        0xFFFF9008  /* シリアルステータスレジスタ_2 */
#define  SCI_SCRDR_2        0xFFFF900A  /* レシーブデータレジスタ_2 */
#define  SCI_SCSDCR_2       0xFFFF900C  /* シリアルディレクションコントロールレジスタ_2 */
#define  SCI_SCSPTR_2       0xFFFF900E  /* シリアルポートレジスタ_2 */
#define  SCI_SPMR_2         0xFFFF9014  /* サンプリングモードレジスタ_2 */
#define  SCIF_SCSMR_3       0xFFFE9800  /* シリアルモードレジスタ_3 */
#define  SCIF_SCBRR_3       0xFFFE9804  /* ビットレートレジスタ_3 */
#define  SCIF_SCSCR_3       0xFFFE9808  /* シリアルコントロールレジスタ_3 */
#define  SCIF_SCFTDR_3      0xFFFE980C  /* トランスミットFIFOデータレジスタ_3 */
#define  SCIF_SCFSR_3       0xFFFE9810  /* シリアルステータスレジスタ_3 */
#define  SCIF_SCFRDR_3      0xFFFE9814  /* レシーブFIFOデータレジスタ_3 */
#define  SCIF_SCFCR_3       0xFFFE9818  /* FIFOコントロールレジスタ_3 */
#define  SCIF_SCFDR_3       0xFFFE981C  /* FIFOデータカウントセットレジスタ_3 */
#define  SCIF_SCSPTR_3      0xFFFE9820  /* シリアルポートレジスタ_3 */
#define  SCIF_SCLSR_3       0xFFFE9824  /* ラインステータスレジスタ_3 */
#define  SCIF_SCSEMR_3      0xFFFE9900  /* シリアル拡張モードレジスタ */
#define  RSPI_SPCR          0xFFFFB000  /* RSPI制御レジスタ */
#define  RSPI_SSLP          0xFFFFB001  /* RSPIスレーブセレクト極性レジスタ */
#define  RSPI_SPPCR         0xFFFFB002  /* RSPI端子制御レジスタ */
#define  RSPI_SPSR          0xFFFFB003  /* RSPIステータスレジスタ */
#define  RSPI_SPDR          0xFFFFB004  /* RSPIデータレジスタ */
#define  RSPI_SPSCR         0xFFFFB008  /* RSPIシーケンス制御レジスタ */
#define  RSPI_SPSSR         0xFFFFB009  /* RSPIシーケンスステータスレジスタ */
#define  RSPI_SPBR          0xFFFFB00A  /* RSPIビットレートレジスタ */
#define  RSPI_SPDCR         0xFFFFB00B  /* RSPIデータコントロールレジスタ */
#define  RSPI_SPCKD         0xFFFFB00C  /* RSPIクロック遅延レジスタ */
#define  RSPI_SSLND         0xFFFFB00D  /* RSPIスレーブセレクトネゲート遅延レジスタ */
#define  RSPI_SPND          0xFFFFB00E  /* RSPI次アクセス遅延レジスタ */
#define  RSPI_SPCMD0        0xFFFFB010  /* RSPIコマンドレジスタ0 */
#define  RSPI_SPCMD1        0xFFFFB012  /* RSPIコマンドレジスタ1 */
#define  RSPI_SPCMD2        0xFFFFB014  /* RSPIコマンドレジスタ2 */
#define  RSPI_SPCMD3        0xFFFFB016  /* RSPIコマンドレジスタ3 */
#define  ADC_ADCR_0         0xFFFFE800  /* A/Dコントロールレジスタ_0 */
#define  ADC_ADSR_0         0xFFFFE802  /* A/Dステータスレジスタ_0 */
#define  ADC_ADSTRGR_0      0xFFFFE81C  /* A/D開始トリガ選択レジスタ_0 */
#define  ADC_ADANSR_0       0xFFFFE820  /* A/Dアナログ入力チャネル選択レジスタ_0 */
#define  ADC_ADBYPSCR_0     0xFFFFE830  /* A/Dバイパスコントロールレジスタ_0 */
#define  ADC_ADDR0          0xFFFFE840  /* A/Dデータレジスタ0 */
#define  ADC_ADDR1          0xFFFFE842  /* A/Dデータレジスタ1 */
#define  ADC_ADDR2          0xFFFFE844  /* A/Dデータレジスタ2 */
#define  ADC_ADDR3          0xFFFFE846  /* A/Dデータレジスタ3 */
#define  ADC_ADCR_1         0xFFFFEC00  /* A/Dコントロールレジスタ_1 */
#define  ADC_ADSR_1         0xFFFFEC02  /* A/Dステータスレジスタ_1 */
#define  ADC_ADSTRGR_1      0xFFFFEC1C  /* A/D開始トリガ選択レジスタ_1 */
#define  ADC_ADANSR_1       0xFFFFEC20  /* A/Dアナログ入力チャネル選択レジスタ_1 */
#define  ADC_ADBYPSCR_1     0xFFFFEC30  /* A/Dバイパスコントロールレジスタ_1 */
#define  ADC_ADDR4          0xFFFFEC40  /* A/Dデータレジスタ4 */
#define  ADC_ADDR5          0xFFFFEC42  /* A/Dデータレジスタ5 */
#define  ADC_ADDR6          0xFFFFEC44  /* A/Dデータレジスタ6 */
#define  ADC_ADDR7          0xFFFFEC46  /* A/Dデータレジスタ7 */
#define  ADC_ADCR_2         0xFFFFEE00  /* A/Dコントロールレジスタ_2 */
#define  ADC_ADSR_2         0xFFFFEE02  /* A/Dステータスレジスタ_2 */
#define  ADC_ADSTRGR_2      0xFFFFEE1C  /* A/D開始トリガ選択レジスタ_2 */
#define  ADC_ADANSR_2       0xFFFFEE20  /* A/Dアナログ入力チャネル選択レジスタ_2 */
#define  ADC_ADBYPSCR_2     0xFFFFEE30  /* A/Dバイパスコントロールレジスタ_2 */
#define  ADC_ADTSR_0        0xFFFFE930  /* A/Dトリガセレクトレジスタ_0 */
#define  ADC_ADTSR_1        0xFFFFED30  /* A/Dトリガセレクトレジスタ_1 */
#define  ADC_ADTSR_2        0xFFFFEF30  /* A/Dトリガセレクトレジスタ_2 */
#define  ADC_ADDR8          0xFFFFEE40  /* A/Dデータレジスタ8 */
#define  ADC_ADDR9          0xFFFFEE42  /* A/Dデータレジスタ9 */
#define  ADC_ADDR10         0xFFFFEE44  /* A/Dデータレジスタ10 */
#define  ADC_ADDR11         0xFFFFEE46  /* A/Dデータレジスタ11 */
#define  ADC_ADDR12         0xFFFFEE48  /* A/Dデータレジスタ12 */
#define  ADC_ADDR13         0xFFFFEE4A  /* A/Dデータレジスタ13 */
#define  ADC_ADDR14         0xFFFFEE4C  /* A/Dデータレジスタ14 */
#define  ADC_ADDR15         0xFFFFEE4E  /* A/Dデータレジスタ15 */
#define  ADC_ADDR0GR0_0     0xFFFFE932  /* A/Dグループ0データ0レジスタ_0 */
#define  ADC_ADDR0GR0_1     0xFFFFED32  /* A/Dグループ0データ0レジスタ_1 */
#define  ADC_ADDR0GR0_2     0xFFFFEF32  /* A/Dグループ0データ0レジスタ_2 */
#define  ADC_ADDR2GR1_0     0xFFFFE934  /* A/Dグループ1データ2レジスタ_0 */
#define  ADC_ADDR2GR1_1     0xFFFFED34  /* A/Dグループ1データ2レジスタ_1 */
#define  ADC_ADDR2GR1_2     0xFFFFEF34  /* A/Dグループ1データ2レジスタ_2ADDR2GR1_2 */
#define  PFC_PAIORH         0xFFFE3804  /* ポートA・IOレジスタH */
#define  PFC_PAIORL         0xFFFE3806  /* ポートA・IOレジスタL */
#define  PFC_PACRH1         0xFFFE380E  /* ポートAコントロールレジスタH1 */
#define  PFC_PACRL4         0xFFFE3810  /* ポートAコントロールレジスタL4 */
#define  PFC_PACRL3         0xFFFE3812  /* ポートAコントロールレジスタL3 */
#define  PFC_PACRL2         0xFFFE3814  /* ポートAコントロールレジスタL2 */
#define  PFC_PACRL1         0xFFFE3816  /* ポートAコントロールレジスタL1 */
#define  PFC_PAPCRH         0xFFFE3828  /* ポートAプルアップMOSコントロールレジスタH */
#define  PFC_PAPCRL         0xFFFE382A  /* ポートAプルアップMOSコントロールレジスタL */
#define  PFC_PBIORH         0xFFFE3884  /* ポートB・IOレジスタH */
#define  PFC_PBIORL         0xFFFE3886  /* ポートB・IOレジスタL */
#define  PFC_PBCRH2         0xFFFE388C  /* ポートBコントロールレジスタH2 */
#define  PFC_PBCRH1         0xFFFE388E  /* ポートBコントロールレジスタH1 */
#define  PFC_PBCRL2         0xFFFE3894  /* ポートBコントロールレジスタL2 */
#define  PFC_PBCRL1         0xFFFE3896  /* ポートBコントロールレジスタL1 */
#define  PFC_PBPCRH         0xFFFE38A8  /* ポートBプルアップMOSコントロールレジスタH */
#define  PFC_PBPCRL         0xFFFE38AA  /* ポートBプルアップMOSコントロールレジスタL */
#define  PFC_PCIORL         0xFFFE3906  /* ポートC・IOレジスタL */
#define  PFC_PCCRL4         0xFFFE3910  /* ポートCコントロールレジスタL4 */
#define  PFC_PCCRL3         0xFFFE3912  /* ポートCコントロールレジスタL3 */
#define  PFC_PCCRL2         0xFFFE3914  /* ポートCコントロールレジスタL2 */
#define  PFC_PCCRL1         0xFFFE3916  /* ポートCコントロールレジスタL1 */
#define  PFC_PCPCRL         0xFFFE392A  /* ポートCプルアップMOSコントロールレジスタL */
#define  PFC_PDIORL         0xFFFE3986  /* ポートD・IOレジスタL */
#define  PFC_PDCRL4         0xFFFE3990  /* ポートDコントロールレジスタL4 */
#define  PFC_PDCRL3         0xFFFE3992  /* ポートDコントロールレジスタL3 */
#define  PFC_PDCRL2         0xFFFE3994  /* ポートDコントロールレジスタL2 */
#define  PFC_PDCRL1         0xFFFE3996  /* ポートDコントロールレジスタL1 */
#define  PFC_PDPCRL         0xFFFE39AA  /* ポートDプルアップMOSコントロールレジスタL */
#define  PFC_PEIORL         0xFFFE3A06  /* ポートE・IOレジスタL */
#define  PFC_PECRL4         0xFFFE3A10  /* ポートEコントロールレジスタL4 */
#define  PFC_PECRL3         0xFFFE3A12  /* ポートEコントロールレジスタL3 */
#define  PFC_PECRL2         0xFFFE3A14  /* ポートEコントロールレジスタL2 */
#define  PFC_PECRL1         0xFFFE3A16  /* ポートEコントロールレジスタL1 */
#define  PFC_HCPCR          0xFFFE3A20  /* 大電流ポートコントロールレジスタ */
#define  PFC_PEPCRL         0xFFFE3A2A  /* ポートEプルアップMOSコントロールレジスタL */
#define  PFC_PDACKCR        0xFFFE3A2C  /* DACK出力タイミングコントロールレジスタ */

#define  PADRH              0xFFFE3800  /* ポートAデータレジスタH */
#define  PADRL              0xFFFE3802  /* ポートAデータレジスタL */
#define  PAPRH              0xFFFE381C  /* ポートAポートレジスタH */
#define  PAPRL              0xFFFE381E  /* ポートAポートレジスタL */
#define  PBDRH              0xFFFE3880  /* ポートBデータレジスタH */
#define  PBDRL              0xFFFE3882  /* ポートBデータレジスタL */
#define  PBPRH              0xFFFE389C  /* ポートBポートレジスタH */
#define  PBPRL              0xFFFE389E  /* ポートBポートレジスタL */
#define  PCDRL              0xFFFE3902  /* ポートCデータレジスタL */
#define  PCPRL              0xFFFE391E  /* ポートCポートレジスタL */
#define  PDDRL              0xFFFE3982  /* ポートDデータレジスタL */
#define  PDPRL              0xFFFE399E  /* ポートDポートレジスタL */
#define  PEDRL              0xFFFE3A02  /* ポートEデータレジスタL */
#define  PEPRL              0xFFFE3A1E  /* ポートEポートレジスタL */
#define  PFDRL              0xFFFE3A82  /* ポートFデータレジスタL */
#define  FPMON_              0xFFFFA800  /* フラッシュ端子モニタレジスタ */
#define  FMODR_              0xFFFFA802  /* フラッシュモードレジスタ */
#define  FASTAT_             0xFFFFA810  /* フラッシュアクセスステータスレジスタ */
#define  FAEINT_             0xFFFFA811  /* フラッシュアクセスエラー割り込み許可レジスタ */
#define  ROMMAT_             0xFFFFA820  /* ROMマット選択レジスタ */
#define  FCURAME_            0xFFFFA854  /* FCURAMイネーブルレジスタ */
#define  FSTATR0_            0xFFFFA900  /* フラッシュステータスレジスタ0 */
#define  FSTATR1_            0xFFFFA901  /* フラッシュステータスレジスタ1 */
#define  FENTRYR_            0xFFFFA902  /* フラッシュP/Eモードエントリレジスタ */
#define  FPROTR_             0xFFFFA904  /* フラッシュプロテクトレジスタ */
#define  FRESETR_            0xFFFFA906  /* フラッシュリセットレジスタ */
#define  FCMDR_              0xFFFFA90A  /* FCUコマンドレジスタ */
#define  FCPSR_              0xFFFFA918  /* FCU処理切り替えレジスタ */
#define  EEPBCCNT_           0xFFFFA91A  /* FLDブランクチェック制御レジスタ */
#define  FPESTAT_            0xFFFFA91C  /* フラッシュP/Eステータスレジスタ */
#define  EEPBCSTAT_          0xFFFFA91E  /* FLDブランクチェックステータスレジスタ */
#define  PCKAR_              0xFFFFA938  /* 周辺クロック通知レジスタ */
#define  EEPRE0_             0xFFFFA840  /* FLD読み出し許可レジスタ0 */
#define  EEPRE1_             0xFFFFA842  /* FLD読み出し許可レジスタ1 */
#define  EEPWE0_             0xFFFFA850  /* FLD書き込み／消去許可レジスタ0 */
#define  EEPWE1_             0xFFFFA852  /* FLD書き込み／消去許可レジスタ1 */
#define  RCCR_               0xFFFC1400  /* ROMキャッシュ制御レジスタ */
#define  STBCR              0xFFFE0014  /* スタンバイコントロールレジスタ */
#define  STBCR2             0xFFFE0018  /* スタンバイコントロールレジスタ2 */
#define  SYSCR1             0xFFFE0402  /* システムコントロールレジスタ1 */
#define  SYSCR2             0xFFFE0404  /* システムコントロールレジスタ2 */
#define  STBCR3             0xFFFE0408  /* スタンバイコントロールレジスタ3 */
#define  STBCR4             0xFFFE040C  /* スタンバイコントロールレジスタ4 */
#define  STBCR5             0xFFFE0418  /* スタンバイコントロールレジスタ5 */
#define  STBCR6             0xFFFE041C  /* スタンバイコントロールレジスタ6 */
#define  SDIR_               0xFFFE2000  /* イントラクションレジスタ */


/* 割込みベクタ番号 */
#define  INT_NMI                  11             /* NMI */
#define  INT_UBC                  12             /* ユーザブレーク */
#define  INT_H-UDI                14             /* H-UDI ブレーク */
#define  INT_IRQ0                 64             /* 外部端子 IRQ0  */
#define  INT_IRQ1                 65             /* 外部端子 IRQ1  */
#define  INT_IRQ2                 66             /* 外部端子 IRQ2  */
#define  INT_IRQ3                 67             /* 外部端子 IRQ3  */
#define  INT_IRQ4                 68             /* 外部端子 IRQ4  */
#define  INT_IRQ5                 69             /* 外部端子 IRQ5  */
#define  INT_IRQ6                 70             /* 外部端子 IRQ6  */
#define  INT_FIFE                 91             /* 外部端子 IRQ7  */
#define  INT_ADC_ADI0             92
#define  INT_ADC_ADI1             96
#define  INT_ADC_ADI2             100
#define  INT_RCAN_ERS_0           104
#define  INT_RCAN_OVR_0           105
#define  INT_RCAN_RM              106
#define  INT_RCAN_SLE_0           107
#define  INT_DMAC_DEI0            108
#define  INT_DMAC_HEI0            109
#define  INT_DMAC_DEI1            112
#define  INT_DMAC_HEI1            113
#define  INT_DMAC_DEI2            116
#define  INT_DMAC_HEI2            117
#define  INT_DMAC_DEI3            120
#define  INT_DMAC_HEI3            121
#define  INT_DMAC_DEI4            124
#define  INT_DMAC_HEI4            125
#define  INT_DMAC_DEI5            128
#define  INT_DMAC_HEI5            129
#define  INT_DMAC_DEI6            132
#define  INT_DMAC_HEI6            133
#define  INT_DMAC_DEI7            136
#define  INT_DMAC_HEI7            137
#define  INT_CMI0                 140
#define  INT_CMI1                 144
#define  INT_WDT_ITI              152
#define  INT_MTU2_TGIA_0          156
#define  INT_MTU2_TGIB_0          157
#define  INT_MTU2_TGIC_0          158
#define  INT_MTU2_TGID_0          159
#define  INT_MTU2_TCIV_0          160
#define  INT_MTU2_TGIE_0          161
#define  INT_MTU2_TGIF_0          162
#define  INT_MTU2_TGIA_1          164
#define  INT_MTU2_TGIB_1          165
#define  INT_MTU2_TCIV_1          168
#define  INT_MTU2_TCIU_1          169
#define  INT_MTU2_TGIA_2          172
#define  INT_MTU2_TGIB_2          173
#define  INT_MTU2_TCIV_2          176
#define  INT_MTU2_TCIU_2          177
#define  INT_MTU2_TGIA_3          180
#define  INT_MTU2_TGIB_3          181
#define  INT_MTU2_TGIC_3          182
#define  INT_MTU2_TGID_3          183
#define  INT_MTU2_TCIV_3          184
#define  INT_MTU2_TGIA_4          188
#define  INT_MTU2_TGIB_4          189
#define  INT_MTU2_TGIC_4          190
#define  INT_MTU2_TGID_4          191
#define  INT_MTU2_TCIV_4          192
#define  INT_MTU2_TGIU_5          196
#define  INT_MTU2_TGIV_5          197
#define  INT_MTU2_TGIW_5          198
#define  INT_POE2_OEI1            200
#define  INT_POE2_OEI2            201
#define  INT_MTU2S_TGIA_3S        204
#define  INT_MTU2S_TGIB_3S        205
#define  INT_MTU2S_TGIC_3S        206
#define  INT_MTU2S_TGID_3S        207
#define  INT_MTU2S_TCIV_3S        208
#define  INT_MTU2S_TGIA_4S        212
#define  INT_MTU2S_TGIB_4S        213
#define  INT_MTU2S_TGIC_4S        214
#define  INT_MTU2S_TGID_4S        215
#define  INT_MTU2S_TCIV_4S        216
#define  INT_MTU2S_TGIU_5S        220
#define  INT_MTU2S_TGIV_5S        221
#define  INT_MTU2S_TGIW_5S        222
#define  INT_POE2_OEI3            224
#define  INT_RSPI_SPEI            233
#define  INT_RSPI_SPRI            234
#define  INT_RSPI_SPTI            235
#define  INT_ERI0                 240
#define  INT_RXI0                 241
#define  INT_TXI0                 242
#define  INT_TEI0                 243
#define  INT_ERI1                 244
#define  INT_RXI1                 245
#define  INT_TXI1                 246
#define  INT_TEI1                 247
#define  INT_ERI2                 248
#define  INT_RXI2                 249
#define  INT_TXI2                 250
#define  INT_TEI2                 251
#define  INT_BRI3                 252
#define  INT_ERI3                 253
#define  INT_RXI3                 254
#define  INT_TXI3                 255

#endif /* SH7237_H */
