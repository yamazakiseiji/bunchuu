/****************************************************************************
* NORTi ボード依存 サンプルサブルーチン (AP-SH2A-2A)                        *
*                                                                           *
*  Copyright (c) 2010, MiSPO Co., Ltd.                                      *
*  All rights reserved.                                                     *
*                                                                           *
* 2010-11-04 Created                                                     SZ *
****************************************************************************/

#include <machine.h>
#include "kernel.h"
#include "sh7237.h"
#include "iodefine.h"
#include "sub.h"

#ifdef __HITACHI__
#pragma noregsave(ini_cpu)
#endif

/*****************************************************************************
* Initialize port (★ Please customize !!)
*
******************************************************************************/

//#define sdram_mode  (*(volatile unsigned short *)(0xFFFC5040))

void ini_cpu(void)
{
    /* ==== CPG setting ==== */
    CPG.FRQCR.WORD  = 0x0303;		/* Clock-in =  10MHz */
                                    /* I-clock  = 160MHz */
                                    /* B-clock  =  40MHz */
                                    /* P-clock  =  40MHz */
    

    /* ---- Inserting 32 NOP instructions ---- */
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
    
    CPG.MCLKCR.BYTE = 0x41;			/* MTU2S = 80MHz */
    CPG.ACLKCR.BYTE = 0x43;			/* AD    = 40MHz */


    /* ==== Enable module clock ==== */
    STB.CR3.BYTE = 0x1A;            /* Module Standby Clear */
                                    /* HIZ,MTU2S,MTU2,Reserve(1),Reserve(1),ADC0,Reserve(1),FLASH */

    STB.CR4.BYTE = 0xE3;            /* Module Standby Clear */
                                    /* Reserve(1),Reserve(1),Reserve(1),SCIF3,Reserve(0),CMT,Reserve(1),Reserve(1) */

    STB.CR5.BYTE = 0x18;            /* Module Standby Clear */
                                    /* SCI0,SCI1,SCI2,Reserve(1),Reserve(1),ADC1,ADC2,RSPI */
    
    STB.CR6.BYTE = 0xCF;            /* USB: Using USBXTAL/USBEXTAL for USBCLK. */
                                    /* RCAN-ET */                               



	/* ==== Init SRAM ==== */
	/* Configure Pin Function Controller (PFC) */
	BSC.CS3BCR.BIT.BSZ = 1;						// BSZ = 1		: CS3空間 = 8ビット
	BSC.CS3WCR.BIT.WR  = 0x04;					// 4cycle
//	BSC.CS3WCR.BIT.WM  = 1;					// 

	PFC.PACRH1.BIT.PA17MD = 1;					// Set RD#
	PFC.PACRH1.BIT.PA16MD = 1;					// Set WRL#
	PFC.PACRL4.BIT.PA15MD = 1;					// Set WRH#
	PFC.PACRL3.BIT.PA9MD  = 1;					// Set CS3#

	PFC.PBCRL1.WORD = 0x0011;					// Set A17:A16
	PFC.PCCRL4.WORD = 0x1111;					// Set A15:A12
	PFC.PCCRL3.WORD = 0x1111;					// Set A11:A8
	PFC.PCCRL2.WORD = 0x1111;					// Set A7:A4
	PFC.PCCRL1.WORD = 0x1111;					// Set A3:A0

	PFC.PDCRL2.WORD = 0x1111;					// Set D7:D4
	PFC.PDCRL1.WORD = 0x1111;					// Set D3:D0

//	BSC.CS3BCR.LONG = 0x02400200;				// 8bitアクセス
//	BSC.CS3WCR.LONG	= 0x00000940;

	// USART設定
	PFC.PACRL1.BIT.PA1MD	= 0x6;	/* Set TXD0	*/
	PFC.PACRL1.BIT.PA0MD	= 0x6;	/* Set RXD0	*/
	
	// SIO2
	PFC.PECRL3.BIT.PE10MD = 6;					// PE10モード : TXD2出力	(SCI)
	PFC.PECRL2.BIT.PE7MD  = 6;					// PE7 モード : RXD2入力	(SCI)

	// EEPROM設定
	PFC.PACRL2.BIT.PA6MD	= 0x05;	/* Set RSPCK*/
	PFC.PACRL2.BIT.PA7MD	= 0x05;	/* Set MOSI	*/
	PFC.PACRL3.BIT.PA8MD	= 0x05;	/* Set MISO	*/
	PFC.PECRL3.BIT.PE8MD	= 0x05;	/* Set SSL0	*/

	
	
	// MTU2
	
	
	
	
	
	
	
	
	
	// WDT設定
	PE.DR.BIT.B1 = 0;							// PE1 : 出力初期値 = L
	PE.DR.BIT.B0 = 0;							// PE0 : 出力初期値 = L
	PFC.PEIORL.BIT.B1 = 1;						// PE1 : 汎用出力
	PFC.PEIORL.BIT.B0 = 1;						// PE0 : 汎用出力
	
	// WDT設定
	// WdgStop、WdgStartにて対応
	
	
    /* ==== RAM access ==== */
	
	SYS.CR1.BIT.RAME5 = 1;
	SYS.CR1.BIT.RAME4 = 1;
	SYS.CR2.BIT.RAMWE5 = 1;
	SYS.CR2.BIT.RAMWE4 = 1;
}

/* end */
