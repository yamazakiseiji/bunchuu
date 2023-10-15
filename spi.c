/**************************************************************************************************
 SPI通信モジュール

 更新日     更新者      更新内容
 ---------- ----------- ---------------------------------------------------------------------------
 2014.06.23 K.Hidaka    ・新規作成


 ==================================================================================================


**************************************************************************************************/
#include <machine.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "iodefine.h"
#include "spi.h"
#include "tskeep.h"


/*=================================================================================================
  マクロ定義
=================================================================================================*/
#define WAIT_TIMEOUT                    100

#define SPI_SSL1		1
#define SPI_SSL2		2

/*=================================================================================================
  構造体定義
=================================================================================================*/


/*=================================================================================================
  ローカル変数定義
=================================================================================================*/


/*=================================================================================================
  ローカル関数プロトタイプ宣言
=================================================================================================*/
static ER  select_ch( INT ch );
static ER  wait_send_busy(void);
static ER  wait_recv_busy(void);
static ER  send_bytes( const B* sbuff, INT slen );
static ER send_bytes_old( const B* sbuff, INT slen );
static ER  send_recv_bytes( const B* sbuff, INT slen, B* rbuff, INT rsize );
static ER send_recv_bytes_old( const B* sbuff, INT slen, B* rbuff, INT rsize );

/*=================================================================================================
  グローバル関数
=================================================================================================*/
/*-------------------------------------------------------------------------------------------------
  機能：SPI通信初期化
  引数：なし
  戻値：なし
-------------------------------------------------------------------------------------------------*/
void spi_init( void )
{
#if 1
	// 低消費電力モードを解除する
	STB.CR5.BIT._RSPI = 0;						// モジュールストップ50 : RSPIは動作
	
	// RSPIを初期化する
    RSPI.SPPCR.BYTE = 0x30;                     // MOSIアイドル固定値=1
    RSPI.SPSR.BYTE  = 0x22;                     // 
	RSPI.SPBR.BYTE = 3;							// ビットレート : 5Mbps
	RSPI.SPCMD0.WORD = 0xE7A3;					// RSPI次アクセス遅延イネーブル : SPNDの設定値
	
	PFC.PACRL3.BIT.PA8MD = 5;					// PA8モード : MISO入出力  (RSPI)
	PFC.PACRL2.BIT.PA7MD = 5;					// PA7モード : MOSI入出力  (RSPI)
	PFC.PACRL2.BIT.PA6MD = 5;					// PA6モード : RSPCK入出力 (RSPI)
	PFC.PECRL3.BIT.PE8MD = 5;					// PE8モード : SSL2出力	(RSPI)
	
	RSPI.SPCR.BIT.MSTR = 1;						// RSPIマスタ／スレーブモード選択 : マスタモード
	RSPI.SPCR.BIT.SPE = 1;						// RSPI機能イネーブル : RSPI機能を有効化する

#else
	// RSPI
    RSPI.SPCR.BIT.SPE                   = 0;                        // RSPI機能無効
    RSPI.SPCR.BIT.MSTR                  = 1;                        // マスターモード
    RSPI.SPPCR.BYTE                   = 0x30;                     // MOSIアイドル固定値=1
    RSPI.SPPCR.BYTE                    = 3;                        // ビットレート:5Mbpsに設定(PΦ=50MHz)
    RSPI.SPCMD0.WORD                    = 0xE783;                   // MSBファースト
                                                                    // データ長:8bit
                                                                    // 転送終了後もSSL信号レベルを保持する
                                                                    // ビットレート:分周なし
                                                                    // アイドル時のRSPCK:1
                                                                    // 奇数エッジでデータ変化、偶数エッジでデータサンプル
#endif
}

/*-------------------------------------------------------------------------------------------------
  機能：データ送信
  引数：ch          SPIチャネル番号
        sbuff       送信データ
        slen        送信データサイズ
  戻値：エラーコード
-------------------------------------------------------------------------------------------------*/
ER spi_send( INT ch, B* sbuff, INT slen )
{
    ER     err;
    INT     imask;

    // 割り込み禁止
    etc_dis_int( CPU_INT_DISABLE, &imask );

    // SSLアサート
    RSPI.SPCR.BIT.SPE = 1;

    RSPI.SPCMD0.BIT.BRDV    = 0;                // ビットレート：5Mbps
    RSPI.SPCMD0.BIT.SPB     = 7;                // データ長：8bit
    RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2アサート

    // データ送信
    err = send_bytes( sbuff, slen );
    if( err ) goto lbl_ret;

    // 送信バッファエンプティ待ち
    err = wait_send_busy();
    if( err ) goto lbl_ret;

lbl_ret:
    // SSLネゲート
    RSPI.SPCR.BIT.SPE = 0;

    // 割り込み許可
    etc_ena_int( imask );

    if( err )
    {
        return err + ch;
    }
    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  機能：データ送信と受信
  引数：ch          SPIチャネル番号
        sbuff       送信データ
        slen        送信データサイズ
        rbuff       受信データバッファ
        rsize       受信データバッファのサイズ
        rlen        [out]受信データ長
  戻値：エラーコード
-------------------------------------------------------------------------------------------------*/
ER spi_send_recv( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     imask;

    // 割り込み禁止
    etc_dis_int( CPU_INT_DISABLE, &imask );

    // SSLアサート
    RSPI.SPCR.BIT.SPE = 1;

    RSPI.SPCMD0.BIT.BRDV    = 0;                // ビットレート：5Mbps
    RSPI.SPCMD0.BIT.SPB     = 7;                // データ長：8bit
    RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2アサート
    err = send_recv_bytes( sbuff, slen, rbuff, rsize);
    if( err ) goto lbl_ret;

lbl_ret:
    // SSLネゲート
    RSPI.SPCR.BIT.SPE = 0;

    // 割り込み許可
    etc_ena_int( imask );

    if( err )
    {
        return err + SPI_SSL2;
    }
    return E_SUC;
}


/*=================================================================================================
  ローカル関数
=================================================================================================*/
/*-------------------------------------------------------------------------------------------------
  機能：SPIチャネルの選択を行う
  引数：ch          SPIチャネル番号
  戻値：エラーコード
-------------------------------------------------------------------------------------------------*/
static ER select_ch( INT ch )
{
	switch( ch )
    {
      case SPI_SSL1:
        RSPI.SPCMD0.BIT.BRDV    = 0;                // ビットレート：4Mbps
        RSPI.SPCMD0.BIT.SPB     = 7;                // データ長：8bit
#if(CPU==7237)
        RSPI.SPCMD0.BIT.SSLA    = 1;                // SSL1アサート
#elif(CPU==7216)
        RSPI.SPCMD0.BIT.SSLA    = 0;                // SSL0アサート
#endif
        break;
      case SPI_SSL2:
        RSPI.SPCMD0.BIT.BRDV    = 0;                // ビットレート：5Mbps
        RSPI.SPCMD0.BIT.SPB     = 7;                // データ長：8bit
        RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2アサート
        break;
      default:
        break;
    }
    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  機能：送信可能状態まで待つ
  引数：tmout   タイムアウト値
  戻値：エラーコード
-------------------------------------------------------------------------------------------------*/
static ER wait_send_busy()
{
	INT	rty = 0;

	while( 0 == RSPI.SPSR.BIT.SPTEF )
	{
		rty++;
		if( rty > SPI_TIMOUT)
		{
			return E_EEP_ERR;
		}
		dly_tsk(DLY_10M);
	}

	return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  機能：受信可能状態まで待つ
  引数：tmout   タイムアウト値
  戻値：エラーコード
-------------------------------------------------------------------------------------------------*/
static ER wait_recv_busy()
{
	INT	rty = 0;

	while( ( RSPI.SPSR.BYTE & 0x81 ) == 0x00 )
	{
		rty++;
		if( rty > SPI_TIMOUT)
		{
			return E_EEP_ERR;
		}
		dly_tsk(DLY_10M);
	}

	return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  機能：データ送信
  引数：sbuff       送信データ
        slen        送信データサイズ
  戻値：エラーコード
  注意：事前にSSLのアサートが完了出来ていること。
-------------------------------------------------------------------------------------------------*/
static ER send_bytes( const B* sbuff, INT slen )
{
    INT     i;
    ER     err;
    UH    work;

    for( i = 0; i < slen; i++ )
    {
        // 送信バッファエンプティ待ち
        err = wait_send_busy();
        if( err ) return err;
		
        // データ送信
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // データ受信(読み捨て)
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }
    // 送信バッファエンプティ待ち
    err = wait_send_busy();
    return err;
}
static ER send_bytes_old( const B* sbuff, INT slen )
{
    INT     i;
    ER     err;
    UH    work;

    for( i = 0; i < slen; i++ )
    {
        // 送信バッファエンプティ待ち
        err = wait_send_busy();
        if( err ) return err;
		
        // データ送信
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // データ受信(読み捨て)
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }
    // 送信バッファエンプティ待ち
    err = wait_send_busy();
    return err;
}

/*-------------------------------------------------------------------------------------------------
  機能：データ送信と受信
  引数：sbuff       送信データ
        slen        送信データサイズ
        rbuff       受信データバッファ
        rsize       受信データバッファのサイズ
        rlen        [out]受信データ長
  戻値：エラーコード
  注意：事前にSSLのアサートが完了出来ていること。
-------------------------------------------------------------------------------------------------*/
static ER send_recv_bytes( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     i;
    UH    work;

    // データ送信
    for( i = 0; i < slen; i++ )
    {
        // 送信バッファエンプティ待ち
        err = wait_send_busy();
        if( err ) return err;

        // データ送信
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // 受信データ読み捨て
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    // データ受信
    for( i = 0; i < rsize; i++ )
    {
        err = wait_send_busy();
        if( err ) return err;

        // ダミーデータ送信
        RSPI.SPDR.WORD      = 0xff;
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // オーバランエラー発生？
        if( 1 == RSPI.SPSR.BIT.OVRF )
        {
            RSPI.SPSR.BIT.OVRF = 0;
            return E_EEP_ERR;
        }
        // 受信データリード
        rbuff[i] = (B)RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    return E_SUC;
}
/*-------------------------------------------------------------------------------------------------
  機能：データ送信と受信
  引数：sbuff       送信データ
        slen        送信データサイズ
        rbuff       受信データバッファ
        rsize       受信データバッファのサイズ
        rlen        [out]受信データ長
  戻値：エラーコード
  注意：事前にSSLのアサートが完了出来ていること。
-------------------------------------------------------------------------------------------------*/
static ER send_recv_bytes_old( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     i;
    UH    work;

    // データ送信
    for( i = 0; i < slen; i++ )
    {
        // 送信バッファエンプティ待ち
        err = wait_send_busy();
        if( err ) return err;

        // データ送信
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // 受信データ読み捨て
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    // データ受信
    for( i = 0; i < rsize; i++ )
    {
        err = wait_send_busy();
        if( err ) return err;

        // ダミーデータ送信
        RSPI.SPDR.WORD      = 0xff;
        RSPI.SPSR.BIT.SPTEF = 0;

        // 受信完了待ち
        err = wait_recv_busy( );
        if( err ) return err;

        // オーバランエラー発生？
        if( 1 == RSPI.SPSR.BIT.OVRF )
        {
            RSPI.SPSR.BIT.OVRF = 0;
            return E_EEP_ERR;
        }
        // 受信データリード
        rbuff[i] = (B)RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  機能：割り込み禁止
  引数：mask        割り込み禁止マスク
        imask       [out]割り込み禁止前の割り込みマスク値
  戻値：なし
-------------------------------------------------------------------------------------------------*/
void etc_dis_int( INT mask, INT* imask )
{
//    *imask = get_imask( );
//    set_imask( mask );
}

/*-------------------------------------------------------------------------------------------------
  機能：割り込み許可
  引数：imask   割り込みマスク値(０：割り込み許可)
  戻値：なし
-------------------------------------------------------------------------------------------------*/
void etc_ena_int( INT imask )
{
//    set_imask( imask );
}
