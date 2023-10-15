/****************************************************************************
* メイン                                                                    *
*                                                                           *
* File name : main.c                                                        *
*                                                                           *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                     *
*                                                                           *
****************************************************************************/

#include "kernel.h"
#include "sh7237.h"
#include "define.h"
#include "datastruct.h"
#include "tskrx0.h"
#include "tsktx0.h"
#include "tskrxEx.h"
#include "tsktxEx.h"
#include "tskcmd.h"
#include "hdrcyc.h"
#include "spi.h"
#include "tskeep.h"
#include "seg.h"
#include "tskio.h"
#include "tskmotwait.h"
#include "tskmotact.h"
#include "tskmotend.h"
#include "tskint.h"
#include "tskled.h"
#include "tskpunit.h"
#include "tsktmpcnt.h"


//*************************************
// コンフィグレーション
//*************************************
#if 1
#define TSKID_MAX   TSK_NUM /* Maximum ID for task */
#define SEMID_MAX   SEM_NUM /* Maximum ID for semaphore */
#define FLGID_MAX   1       /* Maximum ID for event flag */
#define MBXID_MAX   MBX_NUM /* Maximum ID for mail box */
#define MBFID_MAX   1       /* Maximum ID for message buffer */
#define PORID_MAX   1       /* Maximum ID for rendevouz port */
#define MPLID_MAX   1       /* Maximum ID for valiable length memory pool */
#define MPFID_MAX   MPF_NUM /* Maximum ID for fixed length memory pool */
#define DTQID_MAX   DTQ_NUM /* Maximum ID for data queue */
#define MTXID_MAX   1       /* Maximum ID for mutex */
#define ISRID_MAX   1       /* Maximum ID for interrupt service routine */
#define SVCID_MAX   1       /* Maximum ID for superviser call */
#define CYCNO_MAX   HDR_NUM /* Maximum ID for cycric handler */
#define ALMNO_MAX   ALM_NUM /* Maximum ID for alarm handler */
//#define ISTKSZ      1024    /* Stack size for Interrupt Handlers */
#include "nocfg4.h"
#endif


//*************************************
// オブジェクト生成情報
//*************************************

//const T_CTSK ctsk_rx0    = { TA_HLNG, NULL, (FP)TskRx0,     3, 1280, NULL, "TskRx0"     };
//const T_CTSK ctsk_tx0    = { TA_HLNG, NULL, (FP)TskTx0,     2, 1280, NULL, "TskTx0"     };
//const T_CTSK ctsk_rxEx   = { TA_HLNG, NULL, (FP)TskRxEx,    3, 1280, NULL, "TskRxEx"    };
//const T_CTSK ctsk_txEx   = { TA_HLNG, NULL, (FP)TskTxEx,    3, 1280, NULL, "TskTxEx"    };
const T_CTSK ctsk_rx0    = { TA_HLNG, NULL, (FP)TskRx0,     3, 1024, NULL, "TskRx0"     };
const T_CTSK ctsk_tx0    = { TA_HLNG, NULL, (FP)TskTx0,     2, 1024, NULL, "TskTx0"     };
const T_CTSK ctsk_rxEx   = { TA_HLNG, NULL, (FP)TskRxEx,    3, 1024, NULL, "TskRxEx"    };
const T_CTSK ctsk_txEx   = { TA_HLNG, NULL, (FP)TskTxEx,    3, 1024, NULL, "TskTxEx"    };
const T_CTSK ctsk_cmd    = { TA_HLNG, NULL, (FP)TskCmd,     3, 2048, NULL, "TskCmd"     };// ※768以上にしてください
const T_CTSK ctsk_io     = { TA_HLNG, NULL, (FP)TskIo,      3, 512,  NULL, "TskIo"      };
const T_CTSK ctsk_motwait= { TA_HLNG, NULL, (FP)TskMotWait, 3, 1280, NULL, "TskMotWait" };
const T_CTSK ctsk_motact = { TA_HLNG, NULL, (FP)TskMotAct,  3, 1280, NULL, "TskMotAct"  };
const T_CTSK ctsk_motend = { TA_HLNG, NULL, (FP)TskMotEnd,  3, 1280, NULL, "TskMotEnd"  };
const T_CTSK ctsk_int    = { TA_HLNG, NULL, (FP)TskInt,     3, 1280, NULL, "TskInt"     };
const T_CTSK ctsk_led    = { TA_HLNG, NULL, (FP)TskLed,     5, 512,  NULL, "TskLed"     };
const T_CTSK ctsk_eep    = { TA_HLNG, NULL, (FP)TskEep,     3, 1024, NULL, "TskEep"     };
const T_CTSK ctsk_punit  = { TA_HLNG, NULL, (FP)TskPunit,   3, 1024, NULL, "TskPunit"   };
const T_CTSK ctsk_tmpcnt = { TA_HLNG, NULL, (FP)TskTmpCnt,  3, 1024, NULL, "TskTmpCnt"   };

//*************************************
// メモリプール
//*************************************
//const T_CMPF cmpf_tx0     = { TA_TFIFO, 12, sizeof(T_CMD_MSG), NULL, "mpf_tx0"     };
const T_CMPF cmpf_tx0     = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_tx0"     };
const T_CMPF cmpf_txex    = { TA_TFIFO, 4 , sizeof(T_CMD_MSG), NULL, "mpf_txex"    };
const T_CMPF cmpf_cmd     = { TA_TFIFO, 12, sizeof(T_CMD_MSG), NULL, "mpf_cmd"     };
const T_CMPF cmpf_io      = { TA_TFIFO, 4 , sizeof(T_CMD_MSG), NULL, "mpf_io"      };
const T_CMPF cmpf_motwait = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_motwait" };
const T_CMPF cmpf_motact  = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_motact"  };
const T_CMPF cmpf_motend  = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_motend"  };
const T_CMPF cmpf_int     = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_int"     };
const T_CMPF cmpf_led     = { TA_TFIFO, 4 , sizeof(T_CMD_MSG), NULL, "mpf_led"     };
const T_CMPF cmpf_eep     = { TA_TFIFO, 4 , sizeof(T_CMD_MSG), NULL, "mpf_eep"     };
const T_CMPF cmpf_punit   = { TA_TFIFO, 8 , sizeof(T_CMD_MSG), NULL, "mpf_punit"   };
const T_CMPF cmpf_tmpcnt  = { TA_TFIFO, 4 , sizeof(T_CMD_MSG), NULL, "mpf_tmpcnt"   };

//*************************************
// メールボックス
//*************************************
const T_CMBX cmbx_rx0     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_rx0"  };
const T_CMBX cmbx_tx0     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_tx0"  };
const T_CMBX cmbx_txEx    = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_txEx" };
const T_CMBX cmbx_rxEx    = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_rxEx" };
const T_CMBX cmbx_cmd     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_cmd"  };
const T_CMBX cmbx_eep     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_eep"  };
const T_CMBX cmbx_io      = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_io"   };
const T_CMBX cmbx_motwait = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_motwait"  };
const T_CMBX cmbx_motact  = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_motact"  };
const T_CMBX cmbx_motend  = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_motend"  };
const T_CMBX cmbx_int     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_int"  };
const T_CMBX cmbx_led     = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_led"  };
const T_CMBX cmbx_punit   = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_punit"  };
const T_CMBX cmbx_tmpcnt  = { TA_TFIFO | TA_MFIFO, 0, NULL, "mbx_tmpcnt"  };

//*************************************
// アラームハンドラ
//*************************************
const T_CALM calm_r0dat   = { TA_HLNG, NULL, (FP)AlmComR0Dat };
const T_CALM calm_r0ack   = { TA_HLNG, NULL, (FP)AlmComR0Ack };
const T_CALM calm_r2dat   = { TA_HLNG, NULL, (FP)AlmComR2Dat };
const T_CALM calm_r2rsp   = { TA_HLNG, NULL, (FP)AlmCom2RspTmo };


//*************************************
// 周期ハンドラ
//*************************************
const T_CCYC chdr_ccyc = { TA_HLNG | TA_STA, NULL, (FP)HdrCyc, 1, 100 };

//*************************************
// データキュー生成情報
//*************************************
const T_CDTQ cdtq_rx0   = { TA_TFIFO, 1024, NULL, "dtq_rx0" };
const T_CDTQ cdtq_rxEx  = { TA_TFIFO, 768, NULL, "dtq_rxEx" };

//*************************************
// セマフォ
// 装置データ
//*************************************
const T_CSEM csem_init0   = { TA_TFIFO, 1, 1, "sem_init0" };
const T_CSEM csem_pause   = { TA_TFIFO, 1, 1, "sem_pause" };
const T_CSEM csem_hdmov   = { TA_TFIFO, 1, 1, "sem_hdmov" };
const T_CSEM csem_tpget   = { TA_TFIFO, 1, 1, "sem_tpget" };
const T_CSEM csem_tprls   = { TA_TFIFO, 1, 1, "sem_tprls" };
const T_CSEM csem_dropn   = { TA_TFIFO, 1, 1, "sem_dropn" };
const T_CSEM csem_skset   = { TA_TFIFO, 1, 1, "sem_skset" };
const T_CSEM csem_suck0   = { TA_TFIFO, 1, 1, "sem_suck0" };
const T_CSEM csem_dtset   = { TA_TFIFO, 1, 1, "sem_dtset" };
const T_CSEM csem_dot00   = { TA_TFIFO, 1, 1, "sem_dot00" };
const T_CSEM csem_mxset   = { TA_TFIFO, 1, 1, "sem_mxset" };
const T_CSEM csem_mix00   = { TA_TFIFO, 1, 1, "sem_mix00" };
const T_CSEM csem_thmov   = { TA_TFIFO, 1, 1, "sem_thmov" };
const T_CSEM csem_cpopn   = { TA_TFIFO, 1, 1, "sem_cpopn" };
const T_CSEM csem_cpcls   = { TA_TFIFO, 1, 1, "sem_cpcls" };
const T_CSEM csem_bcdsp   = { TA_TFIFO, 1, 1, "sem_bcdsp" };
const T_CSEM csem_turn1   = { TA_TFIFO, 1, 1, "sem_turn1" };
const T_CSEM csem_bzcnt   = { TA_TFIFO, 1, 1, "sem_bzcnt" };
const T_CSEM csem_ledon   = { TA_TFIFO, 1, 1, "sem_ledon" };
const T_CSEM csem_memrw   = { TA_TFIFO, 1, 1, "sem_memrw" };


/******************************************************************************
* 
* [概要]	メイン
* 
* [引数]	－
* 
* [戻り値]	－
* 
******************************************************************************/
int main(void)
{
	ER err;

	SegInit();
	
	//*************************************
    // システム初期化
	//*************************************
    err = sysini();									if( err != E_OK )	SetError( E_SYS_MLTACT );

	//*************************************
    //タスク生成
	//*************************************
    err = cre_tsk( ID_TSK_RX0,     &ctsk_rx0);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_TX0,     &ctsk_tx0);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_RXEX,    &ctsk_rxEx);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_TXEX,    &ctsk_txEx);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_CMD,     &ctsk_cmd);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_EEP,     &ctsk_eep);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_IO,      &ctsk_io);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_MOTWAIT, &ctsk_motwait);	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_MOTACT,  &ctsk_motact);	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_MOTEND,  &ctsk_motend);	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_INT,     &ctsk_int);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_LED,     &ctsk_led);		if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_PUNIT,   &ctsk_punit);	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_tsk( ID_TSK_TMPCNT,  &ctsk_tmpcnt);	if( err != E_OK )	SetError( E_SYS_TSKHDL );

	//*************************************
    // アラームハンドラ生成
	//*************************************
    err = cre_alm( ID_ALM_R0DAT,   &calm_r0dat );	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_alm( ID_ALM_R0ACK,   &calm_r0ack );	if( err != E_OK )	SetError( E_SYS_TSKHDL );
//    err = cre_alm( ID_ALM_R2DAT,   &calm_r2dat );	if( err != E_OK )	SetError( E_SYS_TSKHDL );
    err = cre_alm( ID_ALM_R2RSP,   &calm_r2rsp );	if( err != E_OK )	SetError( E_SYS_TSKHDL );

	//*************************************
    // メモリプール生成
	//*************************************
//    err = cre_mpf( ID_MPF_RX0,     &cmpf_rx0 );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> RX0
    err = cre_mpf( ID_MPF_TX0,     &cmpf_tx0 );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> TX0
    err = cre_mpf( ID_MPF_TXEX,    &cmpf_txex);		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> TX2
    err = cre_mpf( ID_MPF_CMD,     &cmpf_cmd );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> CMD
    err = cre_mpf( ID_MPF_EEP,     &cmpf_eep );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// 
    err = cre_mpf( ID_MPF_IO,      &cmpf_io );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> IO
    err = cre_mpf( ID_MPF_MOTWAIT, &cmpf_motwait);	if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> MOTWAIT
    err = cre_mpf( ID_MPF_MOTACT,  &cmpf_motact );	if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> MOTACT
    err = cre_mpf( ID_MPF_MOTEND,  &cmpf_motend );	if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> MOTEND
    err = cre_mpf( ID_MPF_INT,     &cmpf_int );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> INT
    err = cre_mpf( ID_MPF_LED,     &cmpf_led );		if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> LED
    err = cre_mpf( ID_MPF_PUNIT,   &cmpf_punit );	if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> LED
    err = cre_mpf( ID_MPF_TMPCNT,  &cmpf_tmpcnt );	if( err != E_OK )	SetError( E_SYS_MEMPOL );	// -> LED

	//*************************************
    // メールBOXの作成
	//*************************************
    err = cre_mbx( ID_MBX_RX0 ,    &cmbx_rx0  );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_TX0 ,    &cmbx_tx0  );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
	err = cre_mbx( ID_MBX_RXEX,    &cmbx_rxEx );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
	err = cre_mbx( ID_MBX_TXEX,    &cmbx_txEx );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_CMD ,    &cmbx_cmd  );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_EEP ,    &cmbx_eep  );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_IO ,     &cmbx_io   );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_MOTWAIT, &cmbx_motwait);	if( err != E_OK )	SetError( E_SYS_MEMPOL );
	err = cre_mbx( ID_MBX_MOTACT,  &cmbx_motact);	if( err != E_OK )	SetError( E_SYS_MEMPOL );
	err = cre_mbx( ID_MBX_MOTEND,  &cmbx_motend);	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_INT,     &cmbx_int   );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_LED,     &cmbx_led   );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_PUNIT,   &cmbx_punit );	if( err != E_OK )	SetError( E_SYS_MEMPOL );
    err = cre_mbx( ID_MBX_TMPCNT,  &cmbx_tmpcnt );	if( err != E_OK )	SetError( E_SYS_MEMPOL );

	//*************************************
	// データキューの作成
	//*************************************
	err = cre_dtq( ID_DTQ_RXD,  &cdtq_rx0 );		if( err != E_OK )	SetError( E_SYS_MAILQU );
	err = cre_dtq( ID_DTQ_RXEX,&cdtq_rxEx );		if( err != E_OK )	SetError( E_SYS_MAILQU );

	//*************************************
    // セマフォ
	//*************************************
	err = cre_sem( ID_SEM_INIT0 , &csem_init0 );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_PAUSE , &csem_pause );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_HDMOV , &csem_hdmov );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_TPGET , &csem_tpget );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_TPRLS , &csem_tprls );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_DROPN , &csem_dropn );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_SKSET , &csem_skset );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_SUCK0 , &csem_suck0 );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_DTSET , &csem_dtset );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_DOT00 , &csem_dot00 );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_MXSET , &csem_mxset );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_MIX00 , &csem_mix00 );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_THMOV , &csem_thmov );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_CPOPN , &csem_cpopn );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_CPCLS , &csem_cpcls );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_BCDSP , &csem_bcdsp );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
    err = cre_sem( ID_SEM_TURN1 , &csem_turn1 );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_BZCNT , &csem_bzcnt );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_LEDON , &csem_ledon );	if( err != E_OK )	SetError( E_SYS_SEMAPH );
	err = cre_sem( ID_SEM_MEMRW , &csem_memrw );	if( err != E_OK )	SetError( E_SYS_SEMAPH );

	//*************************************
    // タスク起動
	//*************************************
    err = sta_tsk(ID_TSK_RX0, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_TX0, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_TXEX, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_RXEX, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_CMD, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_EEP, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_IO, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_MOTWAIT, 0);				if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_MOTACT, 0);				if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_MOTEND, 0);				if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_INT, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_LED, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_PUNIT, 0);					if( err != E_OK )	SetError( E_SYS_TSKSTA );
    err = sta_tsk(ID_TSK_TMPCNT, 0);				if( err != E_OK )	SetError( E_SYS_TSKSTA );

	//*************************************
    // 周期ハンドラ起動
	//*************************************
	CycInit();
    err = cre_cyc( ID_HDR_CYC, &chdr_ccyc );		if( err != E_OK )	SetError( E_SYS_CYCHDL );

	//*************************************
    // 周期タイマ割込み起動
	//*************************************
    err = intsta();									if( err != E_OK )	SetError( E_SYS_CYCHDL );

	//*************************************
	// マルチタスクへ移行
	//*************************************
    err = syssta();									if( err != E_OK )	SetError( E_SYS_MLTACT );


    return 0;
}

/* end */
