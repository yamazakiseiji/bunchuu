/******************************************************************************
* サブ関数                                                                    *
*                                                                             *
* File name : sub.c                                                           *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <machine.h>
#include "kernel.h"
#include "iodefine.h"
#include "sh7237.h"
#include "define.h"
#include "datastruct.h"
#include "sub.h"
#include "tblchr.h"
#include "mnet.h"
#include "automachine.h"


//*************************************
// 関数定義
//*************************************

//*************************************
// テーブル
//*************************************


//*************************************
// データ定義
//*************************************

static 	UB			InpChInf[INDAT_CNTRL_MAX];							// 入力チャネル
static volatile struct	StMotionnet	*pMotionnet;
//static 	INT			ErrorInfo;
static 	ST_DEVICE_INFO	dev;

/******************************************************************************
* 
* [概要]	データのコピーを行う
* [引数]	dst     コピー先
* 			disz    コピー先のサイズ
* 			src     コピー元
* 			siz     コピー長
* [戻値]	-
* 
******************************************************************************/
void MemCpy( B* dst, INT dsiz, B* src, INT siz )
{
    INT     i;

    if( dsiz < siz )
    {
        siz = dsiz;
    }

    for( i = 0; i < siz; i++ )
    {
        dst[i] = src[i];
    }
}

/******************************************************************************
* 
* [概要]	メモリプール取得
* [引数]	ID      メモリープールID
*			adr     メモリブロックへのポインタ
* [戻値]	-
* 
******************************************************************************/
ER api_get_mpf( B* fname, INT line, ID id, VP* adr )
{
    ER      err;

    err = tget_mpf( id, adr, 100/MSEC );

	if( E_OK != err )
    {
        return E_SYS_POLGET;
    }

    return err;
}

/******************************************************************************
* 
* [概要]	メモリプール解放
* [引数]	ID      メモリープールID
*			adr     メモリブロックへのポインタ
* [戻値]	-
* 
******************************************************************************/
ER api_rel_mpf( B* fname, INT line, ID id, VP adr )
{
    ER      err;

    err = rel_mpf( id, adr );

	if( E_OK != err )
    {
        return E_SYS_POLREF;
    }

    return err;
}


/******************************************************************************
* 
* [概要]
* [引数]	ID      データキューID
* 			data    送信するデータ
* [戻値]	エラーコード
* 
******************************************************************************/
ER api_snd_dtq( B* fname, INT line, ID id, VP_INT data )
{
    ER  err;

    // データキュー送信
    if( sns_ctx( ) )
    {   // 非コンテキスト
        err = ipsnd_dtq( id, data );
        if( E_OK != err )
        {
	        return E_SYS_MSGCOM;
        }
    }
    else
    {   // コンテキスト
        err = snd_dtq( id, data );
        if( E_OK != err )
        {
	        return E_SYS_MSGCOM;
        }
    }
    return err;
}

static ID tskid_wk;
static ID mpfid_wk;
static ID mbxid_wk;
static UB prid1_wk;
static UB bcid_wk ;
static UB msgid_wk;
static UB sts_wk  ;
static UB sts2_wk ;

/******************************************************************************
* Mailbox Send
* 
* [概要]
* 		書き方 pool_send(ﾀｽｸID,ﾒｯｾｰｼﾞBOXID,ﾒｯｾｰｼﾞID,状態,DATA);
* 		（※通信データサイズ付き）
* [引数]	-
* 
* [戻値]	-
* 
******************************************************************************/
ER mbx_send( ID tskid, ID mpfid, ID mbxid, UB prid1, UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz)
{
	ER		err ;
	
	
	tskid_wk=tskid;
	mpfid_wk=mpfid;
	mbxid_wk=mbxid;
	prid1_wk=prid1;
	bcid_wk =bcid;
	msgid_wk=msgid;
	sts_wk  =sts;
	sts2_wk =sts2;
	
	
	
	for(;;)
	{
		err = sndproc(tskid, mpfid, mbxid, prid1, bcid, msgid, sts, sts2, data,siz);

		// タイムアウト以外のエラーの時は、無限ループとなる可能性があるので注意すること！！
		if(err == E_TMOUT)
		{
			dly_tsk(DLY_10M);			/* dly 10ms */
		}								/* タイムアウト時は再トライ */
		else
		{
			break;
		}
	}
	return(err);
}
/******************************************************************************
* Mailbox Send
* 
* [概要]
* 
* [引数]	-
* 
* [戻値]	-
* 
******************************************************************************/
ER sndproc( ID tskid, ID mpfid, ID mbxid, UB prid1,UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz )
{
	T_CMD_MSG*    msg;
	ER		err ;

	if(mbxid <= 0)
	{
		return(E_SYS_DATERR);							// データ整合性エラー
	}

	err = GetMpf( mpfid, (VP)&msg );					// メモリプール取得
	if(err == E_OK)
	{
		msg->msgdata.tskid  = tskid;					// 送信元のタスクＩＤ
		msg->msgdata.prid1  = prid1;					// 
		msg->msgdata.bcid   = bcid;						// ブロックコマンドＩＤ
		msg->msgdata.msgid  = msgid;					// メッセージＩＤ
		msg->msgdata.inf    = sts;						// 情報
		msg->msgdata.inf2   = sts2;						// 情報
		msg->msgdata.siz    = siz;						// サイズ
		memcpy( &msg->msgdata.data[0], data, CMD_BUFSIZ );

		err         = snd_mbx(mbxid, msg);				// メール送信

		if (err != E_OK)
		{
			RelMpf( mpfid, (VP)msg );					// メモリ解放
			err = E_SYS_MSGCOM;
		}
	}
	return(err);
}
/******************************************************************************
* シリアルポートのエラーのチェック
* 
* [概要]
* 
* [引数]	sci:シリアルポートのレジスタへのポインタ
* 
* [戻値]	-
* 
******************************************************************************/
void sio_CheckError( volatile struct st_sci* sci )
{
	if( sci->SCSSR.BIT.ORER )         // オーバーランエラー
	{
		sci->SCSSR.BIT.ORER = 0;
	}
	if( sci->SCSSR.BIT.FER )          // フレーミングエラー
	{
		sci->SCSSR.BIT.FER = 0;
	}
	if( sci->SCSSR.BIT.PER )          // パリティーエラー
	{
		sci->SCSSR.BIT.PER = 0;
	}
}
/******************************************************************************
* シリアルポートのエラーのチェック
* 
* [概要]
* 
* [引数]	-
* 
* [戻値]	-
* 
******************************************************************************/
void scif_CheckError()
{
	if( SCIF3.SCFSR.BIT.ER )         	// エラー
	{
		SCIF3.SCFSR.BIT.ER = 0;
	}
	if( SCIF3.SCFSR.BIT.FER )          // フレーミングエラー
	{
		SCIF3.SCFSR.BIT.FER = 0;
	}
	if( SCIF3.SCFSR.BIT.PER )          // パリティーエラー
	{
		SCIF3.SCFSR.BIT.PER = 0;
	}
}
/******************************************************************************
* 
* [概要]	装置エラー状態設定
* [引数]	
* [戻値]	-
* 
******************************************************************************/
void SetError(ER err)
{
}
/******************************************************************************
* 
* [概要]	装置エラー状態クリア
* [引数]	
* [戻値]	-
* 
******************************************************************************/
void ClrError(ER err)
{
}
/******************************************************************************
* 
* [概要]	エラーを取得する
* [引数]	
* [戻値]	-
* 
******************************************************************************/
ER GetErrCode()
{
}

/******************************************************************************
* 
* [概要]	エラーセット
* [引数]	errcode: エラーコード
* [戻値]	－
* 
******************************************************************************/
void ErrBitSet(INT errcode)
{
}
/******************************************************************************
* 
* [概要]	エラークリア
* [引数]	errcode: エラーコード
* [戻値]	－
* 
******************************************************************************/
void ErrBitClr(INT errcode)
{
}
/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
BOOL ErrBitChk(INT errcode)
{
}

/******************************************************************************
* 
* [概要]	入力チャネル情報を内部共有データに設定
* [引数]	入力チャネルデータ
* [戻値]	-
* 
******************************************************************************/
void SetInputDat(UB index, UB data)
{
   int save_i;
    // 割込み禁止処理
    save_i = get_imask();
 	set_imask(0x0f);

//5/24	InpChInfOld[index] = InpChInf[index] ;
	InpChInf[index] = data;

	set_imask(save_i);
}
/******************************************************************************
* 
* [概要]	入力チャネル情報を内部共有データに設定
* [引数]	入力チャネルデータ
* [戻値]	-
* 
******************************************************************************/
void SetInputBit(UB index, UB data)
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;

	syou    = ( index / 8 );			// 余り取得
	amari   = ( index % 8 );			// 余り取得
	sftdat  = ( sftbit << amari );		// 余りをシフト

//
//	タイマ割込みのみで呼ばれるため不要
//
//	// 割込み禁止処理
//    save_i = get_imask();
 //	set_imask(0x0f);
	
//5/24	InpChInfOld[syou] = InpChInf[syou] ;
	
	if( data )
	{
		InpChInf[syou] |= sftdat;
	}
	else
	{
		InpChInf[syou] &= (~sftdat);
	}
//	set_imask(save_i);
}
/******************************************************************************
* 
* [概要]	入力チャネルを取得
* [引数]	入力チャネル取得位置
* [戻値]	入力チャネルデータ
* 
******************************************************************************/
UB GetInputDat(UB inputch)
{
	UB sts = 0;
	UB	devno;
	UB	portno;
	UB	mskbit;

	pMotionnet = (struct StMotionnet *)MNET_BASE_ADDRESS;

	devno  = INPUTCH_INFTBL[inputch][0];

	if( devno != NONE )
	{
		portno = INPUTCH_INFTBL[inputch][1];
		mskbit = INPUTCH_INFTBL[inputch][2];
		sts = (pMotionnet->portData[devno][portno] & mskbit) ;
	}

	return(( sts > 0 ? SET : CLR ));
}

/******************************************************************************
* 
* [概要]	ＨＥＸ値(0～9,A～F)をＡＳＣⅡコードに変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB	HextoAsc( UB dat )
{
	UB result;

	dat &= 0x0f;
	result = ASC_CHG[dat];

	return(result);
}
/******************************************************************************
* 
* [概要]	ＨＥＸ値(0～9,A～F)をＡＳＣⅡコードに変換（英字は小文字）
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB	HextoAscLower( UB dat )
{
	UB result;

	dat &= 0x0f;
	result = ASC_CHG_LOWER[dat];

	return(result);
}
/******************************************************************************
* 
* [概要]	ＡＳＣⅡコードをＨＥＸ値(0～9,A～F)に変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB	AsctoHex( UB dat )
{
	UB result = 0;

	if( ( dat >= '0') && ( dat <= '9'))
	{
		result = (dat - 0x30) & 0x0f;
	}
	if( ( dat >= 'A') && ( dat <= 'F'))
	{
		result = (dat - 0x37) & 0x0f;
	}
	if( ( dat >= 'a') && ( dat <= 'f'))
	{
		result = (dat - 0x57) & 0x0f;
	}
	
	return(result);
}
/******************************************************************************
* 
* [概要]	ＡＳＣⅡコードをＤＥＣ値(0～9)に変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB	AsctoDec( UB dat )
{
	UB result = 0;

	if( ( dat >= '0') && ( dat <= '9'))
	{
		result = (dat - 0x30) ;
	}
	
	return(result);
}

/******************************************************************************
* 
* [概要]	ＤＥＣ値をＡＳＣⅡコードに変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB		DectoAscH( UB dat )
{
	UB hdat = 0x00;

	hdat = ( dat / 10 );
	hdat = (UB)ASC_CHG[hdat];

	return( hdat );
}
/******************************************************************************
* 
* [概要]	ＤＥＣ値をＡＳＣⅡコードに変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
******************************************************************************/
UB		DectoAscL( UB dat )
{
	UB ldat = 0x00;

	ldat = ( dat % 10 );
	ldat = (UB)ASC_CHG[ldat];

	return( ldat );
}

/******************************************************************************
* 
* [概要]	ＤＥＣ値を１６進文字列に変換
* [引数]	dat:変換元
* [戻値]	変換後
* 
* 
* 
******************************************************************************/
void	DectoHexASC( UB mot, UB *hdat, UB *ldat )
{
	UB dat = 0;

	dat = ( mot / 16 );
	*hdat = HextoAsc( dat );
	
	dat = ( mot % 16 );
	*ldat = HextoAsc( dat );
}
/******************************************************************************
* 
* [概要]	BYTEデータをINTデータに変換
* [引数]	mot:変換元データ先頭ポインタ ※変換元データは必ず4byte構成であること。
* [戻値]	－
* 
******************************************************************************/
INT BtoINT( UB *mot )
{
 	INT int_wk    = 0;
 	INT int_calwk = 0;

	int_wk = ( mot[0] & 0xff );
	int_calwk  = (int_wk << 24);

	int_wk = ( mot[1] & 0xff );
	int_calwk |= (int_wk << 16);

	int_wk = ( mot[2] & 0xff );
	int_calwk |= (int_wk <<  8);

	int_wk = ( mot[3] & 0xff );
	int_calwk |= int_wk;
	
	return(int_calwk);
}
/******************************************************************************
* 
* [概要]	BYTEデータ(2BYTE)をINTデータに変換
* [引数]	mot:変換元データ先頭ポインタ
* [戻値]	－
* 
******************************************************************************/
INT B2toINT( UB *mot )
{
 	INT int_wk    = 0;
 	INT int_calwk = 0;

	int_wk = ( mot[0] & 0xff );
	int_calwk |= (int_wk <<  8);

	int_wk = ( mot[1] & 0xff );
	int_calwk |= int_wk;
	
	return(int_calwk);
}
/******************************************************************************
* 
* [概要]	INTデータを2BYTEデータに変換
* [引数]	mot:変換元データ ※変換元データは必ず2byte構成であること。
* [戻値]	－
* 
******************************************************************************/
void INTtoB2( INT mot, UB *sak )
{
	B calwk;
	
	calwk = (mot / 0x100);
	*sak  = calwk;
	sak++;
	calwk = (mot % 0x100);
	*sak  = calwk;
}
/******************************************************************************
* 
* [概要]	16進数文字列をBYTE配列にする
* [引数]	str:16進数文字列, ary:格納先BYTE配列
* [戻値]	－
* 
******************************************************************************/
void SetByteArray( const char* str, UB* ary )
{
    INT i;
    UB  val;
    INT idx;

    for( i = Strlen( str ) - 1,idx = 0 ; i>=0; i-=2,idx++ )
    {
        if( ( i - 1 ) < 0 )
        {
            val = AsctoHex( str[i] );
        }
        else
        {
            val = AsctoHex( str[i - 1] );
            val <<= 4;
            val |= AsctoHex( str[i] );
        }
        ary[idx] = val;
    }
}

/******************************************************************************
* 
* [概要]	16進数値を文字列配列にする
* [引数]	binary:データ元(16進数値), strary:格納先文字列,siz:データ元サイズ
* [戻値]	－
* 
******************************************************************************/
void SetStrArray( UB* binary, UB* strary, INT siz)
{
    INT i;
	UB  ldat;
	UB  hdat;
	
	
	for( i = 0; i < siz; i=i+2 )
	{
		ldat = ( *binary )      & 0x0f;
		hdat = ( *binary >> 4 ) & 0x0f;
		strary[(i+0)] = HextoAsc( ldat );
		strary[(i+1)] = HextoAsc( hdat );
	}
}

/******************************************************************************
* 
* [概要]	：文字列の比較
* [引数]	：cmpcmd:比較ターゲット、cmpchr:比較文字列
* [戻値]	：0:s1==s2、1:s1>s2, -1:s1<s2
* 
*******************************************************************************/
INT Strncmp( const char* cmpcmd, const char* cmpchr, INT len )
{
	INT Loop;
	INT Result = -1;

	for( Loop = 0; Loop < len; Loop++,cmpcmd++,cmpchr++ )
	{
		if( *cmpcmd != *cmpchr )	return( Result );
	}

	
	return( 0 );
}
/******************************************************************************
* 
* [概要]	：英字を大文字にする
* [引数]	：str:文字列
* [戻値]	：
* 
*******************************************************************************/
void ToUpper( char* str )
{
    for( ; *str; str++ )
    {
        if( 'a' <= *str && *str <= 'z' )
        {
            *str = *str - 'a' + 'A';
        }
    }
}
/******************************************************************************
* 
* [概要]	：英字を大文字にする
* [引数]	：str:文字列
* [戻値]	：
* 
*******************************************************************************/
void ToUppern( char* str , INT len )
{
	INT Loop;

	for( Loop = 0; Loop < len; Loop++,str++ )
	{
        if( 'a' <= *str && *str <= 'z' )
        {
            *str = *str - 'a' + 'A';
        }
	}
}
/******************************************************************************
* 
* [概要]	：コマンド引数を区切る
* [引数]	：str:文字列、arg:引数のポインタ
* [戻値]	：区切り数
* 
*******************************************************************************/
UB Split( char* src, const char** arg )
{
    char*  p;
    char*  s;
    UB idx = 0;

    p = src;
    for( ; ; )
    {
        s = (char*)(Strchr( p, ',' ));
        if( NULL == s )
        {
            arg[idx] = p;
            idx++;
            break;
        }
        arg[idx] = p;
        idx++;
        *s = '\0';
        p = s + 1;
    }
    return idx;
}
/******************************************************************************
* 
* [概要]	：コマンド引数をＮＵＬＬで区切る
* [引数]	：str:文字列、arg:引数のポインタ
* [戻値]	：区切り数
* 
*******************************************************************************/
UB SplitNull( char* src, const char** arg )
{
    char*  p;
    char*  s;
    UB idx = 0;

    p = src;
    for( ; ; )
    {
        s = (char*)(Strchr( p, 0x00 ));
        if( NULL == s )
        {
            arg[idx] = p;
            idx++;
            break;
        }
        arg[idx] = p;
        idx++;
        *s = '\0';
        p = s + 1;
    }
    return idx;
}
/******************************************************************************
* 
* [概要]	：コマンド引数をＮＵＬＬで区切る
* [引数]	：str:文字列、arg:引数のポインタ
* [戻値]	：区切り数
* 
*******************************************************************************/
UB SplitSpace( char* src, const char** arg )
{
    char*  p;
    char*  s;
    UB idx = 0;

    p = src;
    for( ; ; )
    {
        s = (char*)(Strchr( p, 0x20 ));
        if( NULL == s )
        {
            arg[idx] = p;
            idx++;
            break;
        }
        arg[idx] = p;
        idx++;
        *s = '\0';
        p = s + 1;
    }
    return idx;
}
/******************************************************************************
* 
* [概要]	：コマンド引数を区切る
* [引数]	：str:文字列、arg:引数のポインタ
* [戻値]	：区切り数
* 
*******************************************************************************/
UB SplitOnly( char* src, const char** arg )
{
    char*  p;
    char*  s;
    UB idx = 0;

    p = src;
    for( ; ; )
    {
        s = (char*)(Strchr( p, ',' ));
        if( NULL == s )
        {
            arg[idx] = p;
            idx++;
            break;
        }
        arg[idx] = p;
        idx++;
//        *s = '\0';
        p = s + 1;
    }
    return idx;
}
/******************************************************************************
* 
* [概要]	：指定した文字が最初に現れる位置を検索する
* [引数]	：str:文字列、arg:検索する文字
* [戻値]	：見つかった位置のポインタ、見つからなかった場合はNULL
* 
*******************************************************************************/
char* Strchr(const char* s, char c )
{
    for( ; *s ; s++ )
    {
        if( *s == c )
        {
            return (const char*)(s);
        }
    }
    return NULL;
}
/*-----------------------------------------------------------------------------
  機能：文字列を数値にする
  引数：str     数字文字
        rad     進数( 10 or 16 )
        byte    バイト数(1/2/4)
        sig     符号(0:符号なし、1:符号あり)
  戻値：数値
 ----------------------------------------------------------------------------*/
INT Atoi( const char* str, INT rad, INT byte, INT sig )
{
	INT     val = 0;
    INT     len;
    INT     i;
    INT     top;

	if( N_DEC == rad )
    {
        if( '-' == str[0] ) top = 1;
        else                top = 0;
    }
    else
    {
        top = 0;
    }

    len = Strlen( str );
    for( i = top; i < len; i++ )
    {
        val *= rad;

        if( N_DEC == rad )
        {
            if( '0' <= str[i] && str[i] <= '9' )
                val += str[i] - '0';
            else
                return -1;
        }
        else
        {
            if( '0' <= str[i] && str[i] <= '9' )
                val += str[i] - '0';
            else if( 'A' <= str[i] && str[i] <= 'F' )
                val += str[i] - 'A' + 10;
            else if( 'a' <= str[i] && str[i] <= 'f' )
                val += str[i] - 'a' + 10;
            else
                return -1;
        }
    }
    if( 1 == top )
    {
        val *= -1;
    }

    if( N_SIG == sig )    // 符号あり
    {
        if     ( ( 1 == byte ) && ( val & 0x00000080 ) ) val |= 0xffffff00;
        else if( ( 2 == byte ) && ( val & 0x00008000 ) ) val |= 0xffff0000;
    }

    return val;
}

/*-----------------------------------------------------------------------------
  機能：文字列の長さを求める
  引数：str     文字列
  戻値：文字列長
 ----------------------------------------------------------------------------*/
INT Strlen( const char* str )
{
    INT     len;

    for( len = 0; *str; str++, len++ )
    {
        ;
    }
    return len;
}

/*-----------------------------------------------------------------------------
  機能：文字列をコピー
  引数：s1:コピー先、s2:コピー先
  戻値：－
 ----------------------------------------------------------------------------*/
void Strcpy( char *s1, char *s2 )
{
    for( ; *s2; s1++, s2++ )
    {
        *s1 = *s2;
    }
    *s1 = '\0';
}




/*-----------------------------------------------------------------------------
  機能：文字列をコピー
  引数：s1:コピー先、s2:コピー元
  戻値：－
 ----------------------------------------------------------------------------*/
void StrHexcpy( char *s1, char *s2 )
{
	UB h_dat;
	UB l_dat;
	UB hl_dat;
	
    for( ; *s2; s1++ )
    {
		h_dat = *s2;
    	s2++;
		l_dat = *s2;
    	s2++;
    	*s1 = AsctoHex(h_dat) * 0x10 + AsctoHex(l_dat);
    }
    *s1 = '\0';
}
/*-----------------------------------------------------------------------------
  機能：文字列をコピー
  引数：s1:コピー先、s2:コピー元
  戻値：－
 ----------------------------------------------------------------------------*/
void StrHexcpyn( char *s1, char *s2,  int siz)
{
	UB h_dat;
	UB l_dat;
	UB hl_dat;
	int i;
	
    for( i = 0; i < siz; s1++ )
    {
		h_dat = *s2;
    	s2++;
		l_dat = *s2;
    	s2++;
    	*s1 = AsctoHex(h_dat) * 0x10 + AsctoHex(l_dat);
    }
    *s1 = '\0';
}

/***********************************************************************
* 
* [概要]	BCCの計算
* [引数]	buff:計算元データ,size:計算元データ長
* [戻値]	BCC
* 
***********************************************************************/
UB CalcBCC( UB* buff, INT size )
{
    INT     i;
    UB       bcc;
    bcc = 0;

    for( i = 0; i < size; i++ )
    {
        bcc ^= buff[i];
    }
    return bcc;
}

/***********************************************************************
* 
* [概要]	ウオッチドック設定
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void WdgSet(void)
{
	sfr_outw(WDT_WTCSR,0xA548);
	sfr_inw(WDT_WRCSR);
	sfr_outw(WDT_WRCSR,0x5A5F);//b6:rste=1,b5:rsts=0
	sfr_outw(WDT_WTCNT,0x5A00);//
}
/***********************************************************************
* 
* [概要]	ウオッチドック停止
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void WdgStop(void)
{
	// [WTCSR]
	// b7    b6     b5    b4   b3   b2   b1   b0
	// IOVF  WT/IT  TME   -    -    CKS  CKS  CKS
	// 0     1      0     1    1    1    1    1
	sfr_outw(WDT_WTCSR,0xA55F);

	// b7    b6     b5    b4   b3   b2   b1   b0
	// WOVF  RSTE   RSTS  -    -    -    -    - 
	// *     1      0     1    1    1    1    1
	sfr_outw(WDT_WRCSR,0x5A5F);
	
	// b7:WOVF=0 Write
	sfr_outw(WDT_WRCSR,0xA500);
	
	// WTCNT CLEAR
	sfr_outw(WDT_WTCNT,0x5A00);//
}
/***********************************************************************
* 
* [概要]	ウオッチドック開始
* [引数]	－
* [戻値]	－
* 
***********************************************************************/
void WdgStart(void)
{
	// WTCNT CLEAR
	sfr_outw(WDT_WTCNT,0x5A00);//

	// [WTCSR]
	// b7    b6     b5    b4   b3   b2   b1   b0
	// IOVF  WT/IT  TME   -    -    CKS  CKS  CKS
	// 0     1      1     1    1    1    1    1
	sfr_outw(WDT_WTCSR,0xA57F);
}

/***********************************************************************
* 
* [概要]CW/CCW情報取得
* 
* 
* 
***********************************************************************/
UB GetCwInfo( char* arg )
{
	UB result;

	ToUppern( arg, 2 );

	if( !Strncmp( arg, &STRTBL_CW[0],2 ) )
	{
		result = CW;
	}
	else
	{
		result = CCW;
	}

	return( result );
}
/******************************************************************************
* 
* [概要]	指定したindex値のビットをセット
* [引数]	index = 1～、data = 格納先データ
* [戻値]	ビットセット値
* 
******************************************************************************/
void BitSet( UB index, UB *data )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB indexwk;

	if(( index >= DVMIN )&&( index <= DVMAX ))
	{
		indexwk = index - 1;
		syou    = ( indexwk / 8 );			// 余り取得
		amari   = ( indexwk % 8 );			// 余り取得
		sftdat  = ( sftbit << amari );		// 余りをシフト
		*data  |= sftdat;
	}
}
/******************************************************************************
* 
* [概要]	指定したindex値のビットをクリア
* [引数]	index = 1～、data = 格納先データ
* [戻値]	ビットクリア値
* 
******************************************************************************/
void BitClr( UB index, UB *data )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB indexwk;

	if(( index >= MIN_CH )&&( index <= MAX_CH ))
	{
		indexwk = index - 1;
		syou    = ( indexwk / 8 );			// 余り取得
		amari   = ( indexwk % 8 );			// 余り取得
		sftdat  = ( sftbit << amari ) ;		// 余りをシフト
		*data  &= (~sftdat);
	}
}

/******************************************************************************
* 
* [概要]	指定したindex値のビットをセット(INT型配列用)
*           例）data = 0x0000000, index = 31  →  戻り値 0x80000000
* [引数]	data = 指定データ、index = 0～31
* [戻値]	ビットセット値
* 
******************************************************************************/
void BitSet_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	UB indexwk;										// 2020/2/12 １はじまり対応

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 １はじまり対応
	{
		indexwk = index - 1;						// 2020/2/12 １はじまり対応
		syou    = ( indexwk / 32 );			// 余り取得
		amari   = ( indexwk % 32 );			// 余り取得
		sftdat  = ( sftbit << amari );		// 余りをシフト
		data[syou] |= sftdat;
	}
}

/******************************************************************************
* 
* [概要]	指定したindex値のビットをクリア(INT型配列用)
*           例）data = 0x8000000, index = 31  →  戻り値 0x00000000
* [引数]	data = 指定データ、index = 0～31
* [戻値]	ビットクリア値
* 
******************************************************************************/
void BitClr_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	UB indexwk;										// 2020/2/12 １はじまり対応

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 １はじまり対応
	{
		indexwk = index - 1;						// 2020/2/12 １はじまり対応
		syou    = ( indexwk / 32 );			// 余り取得
		amari   = ( indexwk % 32 );			// 余り取得
		sftdat  = ( sftbit << amari ) ;		// 余りをシフト
		data[syou] &= (~sftdat);
	}
}

/******************************************************************************
* 
* [概要]	指定したindex値のビット状態を取得(INT型配列用)
* [引数]	index = 0～31
* [戻値]	≠０ ( indexで指定した位置のビットが１ )
* 
******************************************************************************/
INT BitGet_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	INT rltdat;
	UB indexwk;										// 2020/2/12 １はじまり対応

	rltdat = *data;

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 １はじまり対応
	{
		indexwk = index - 1;						// 2020/2/12 １はじまり対応
		syou    = ( indexwk / 32 );			// 余り取得
		amari   = ( indexwk % 32 );			// 余り取得
		sftdat  = ( sftbit << amari ) ;		// 余りをシフト
		rltdat  = ( data[syou] & sftdat );
	}

	return( rltdat );
}

/******************************************************************************
* 
* [概要]	最初に見つけたbit(=1)のループ値を返す
*           ※前提として必ず、どこかのbitが立っている前提で本関数は呼ばれる
* [引数]	ＩＮＴデータアドレス
* [戻値]	ループ値
* 
******************************************************************************/
UB GetFirstCh( INT *data )
{
	INT sftbit = 0x00000001;
	INT sftdat;
	INT rltdat;
	UB  loop;


	// bit0～bit31
	sftbit = 0x00000001;
	for( loop = 0; loop < 32; loop++ )
	{
		sftdat  = ( sftbit << loop ) ;
		rltdat  = ( *data & sftdat );

		if( rltdat )	return( (loop+1) );
	}

	return( NO_DATA );						// なし
}
/******************************************************************************
* 
* [概要]	指定したモータを停止
* [引数]	指定モータ（bit=1:停止モータ）
* [戻値]	－
* 
******************************************************************************/
void TargetMotorStop( INT *motdata )
{
	UB  stopmot;

	while(1)
	{
		stopmot = GetFirstCh( motdata );
		if( stopmot == NO_DATA )	break;
		motor_ABORT(stopmot);
		
		BitClr_INT(stopmot, motdata );
	}
}

/******************************************************************************
* 
* [概要]    Ch番号をBIT化しUB配列のデータにセットする。
*           用途としてINTコマンド等に利用   ※配列[0]から小さいCh番号
* 
*           例）Ch.63の場合に配列dataには、以下のようにセット
* 
*           小さいCh    data[0] = 0x00
*             ↑        data[1] = 0x00
*             ｜        data[2] = 0x00
*             ｜        data[3] = 0x00
*             ｜        data[4] = 0x00
*             ｜        data[5] = 0x00
*             ↓        data[6] = 0x00
*           大きいCh    data[7] = 0x80   ← Ch.63
* 
* [引数]    Ch = CH番号、array = 配列データ先頭アドレス
* [戻値]    －
* 
******************************************************************************/
void SetDatToArray( UB ch, UB *array )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB chwk;								// 2020/2/12 １はじまり対応

	if(( ch >= MIN_CH )&&( ch <= MAX_CH ))	// 2020/2/12 １はじまり対応
	{
		chwk  = ch - 1;						// 2020/2/12 １はじまり対応
		syou  = ( chwk / 8 );
		amari = ( chwk % 8 );
		sftdat = ( sftbit << amari );
		array[syou] |= sftdat;
	}
}

/******************************************************************************
* 
* [概要]	Ch番号をBIT化しUB配列のデータのクリア。
* [引数]	Ch = CH番号、array = 配列データ先頭アドレス
* [戻値]	－
* 
******************************************************************************/
void ClrDatToArray( UB ch, UB *array )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB chwk;								// 2020/2/12 １はじまり対応

	if(( ch >= MIN_CH )&&( ch <= MAX_CH ))	// 2020/2/12 １はじまり対応
	{
		chwk  = ch - 1;						// 2020/2/12 １はじまり対応
		syou  = ( chwk / 8 );
		amari = ( chwk % 8 );
		sftdat = ( sftbit << amari );
		array[syou] &= (~sftdat);
	}
}

/******************************************************************************
* 
* [概要]	
* [引数]	装置状態、プロトコル番号、ステップ、コマンド
* [戻値]	
* 
******************************************************************************/
//void DeviceInfoSet( INT sts, UB pno, UB step, UB cmd )
void DeviceInfoSet( ER result, UB pno, UB step, UB sts )
{
	UB flg = CLR;
	UB bufwk[CMD_BUFSIZ];
	UB cmd = CLR;
	UB status = CLR;
	UB devsts = CLR;

	cmd    =  ( sts & P_MSK );
	status = (( sts >> 5 ) & 0x07);
	
	// 装置状態作成
	if( status == PROCESS_LYSI )	devsts |= BIT0;
	if( status == PROCESS_BILD )	devsts |= BIT1;
	if( status == PROCESS_WASH )	devsts |= BIT2;
	if( status == PROCESS_ELUT )	devsts |= BIT3;
	if( pio_BIN(I_DOORSW1) )		devsts |= BIT4;
	if( result == ER_LIQERR )		devsts |= BIT5;
	if( result == ER_LIQTUMARI )	devsts |= BIT6;

	
	if( DeviceStatusGet() != devsts )	flg = SET;
	if( DeviceProtcolNoGet() != pno )	flg = SET;
	if( DeviceStepGet() != step )		flg = SET;
	if( DeviceCmdGet() != cmd )			flg = SET;
	
	if( flg == SET )				// データに変化有り
	{
		dev.sts  = devsts;
		dev.pno  = pno;
		dev.step = step;
		dev.cmd  = cmd;

		memset( &bufwk[0], CLR,  CMD_BUFSIZ );
		memcpy( &bufwk[0], &dev.sts, sizeof(ST_DEVICE_INFO) );

		mbx_send	(
			ID_TSK_CMD,             // tskid    <---->   msg->msgdata.tskid
			ID_MPF_EEP,             // memory   <---->   To EEPTSK
			ID_MBX_EEP,             // mailbox  <---->   To EEPTSK
			NO_DATA,                // procid   <---->   msg->msgdata.procid
			NO_DATA,                // bcid     <---->   msg->msgdata.bcid
			ID_MSG_EEPWRIT_STS,     // msg      <---->   msg->msgdata.msgid
			DEFDAT,                 // sts      <---->   msg->msgdata.inf
			DEFDAT,                 // sts2     <---->   msg->msgdata.inf2
			&bufwk[0],              // data     <---->   msg->msgdata.data[]
			CMD_BUFSIZ              // data siz <---->   msg->msgdata.siz
		);
	}
}

/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
void DeviceInfoClr()
{
	DeviceInfoSet(CLR,CLR,CLR,CLR);
}
/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
INT DeviceStatusGet()
{
	return( dev.sts );
}
/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
UB DeviceProtcolNoGet()
{
	return( dev.pno );
}
/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
UB DeviceStepGet()
{
	return( dev.step );
}
/******************************************************************************
* 
* [概要]	
* [引数]	
* [戻値]	
* 
******************************************************************************/
UB DeviceCmdGet()
{
	return( (dev.cmd & 0x0f) );
}

/******************************************************************************
* 
* [概要]	CNT数分、右からbitをセット
* [引数]	
* [戻値]	
* 
******************************************************************************/
unsigned short IdleBitSet(unsigned char cnt)
{
	unsigned char  loop;
	unsigned short data   = 0x0000;
	unsigned short sftbit = 0x0001;
	
	for( loop = 0; loop < cnt; loop++ )
	{
		data |= sftbit;
		
		sftbit = ( sftbit << 1 );
	}
	
//	data = ((data ^ 0x0fff) & 0x0fff);
	
	return( data );
}


