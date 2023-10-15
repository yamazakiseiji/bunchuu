/******************************************************************************
* サブ関数                                                                    *
*                                                                             *
* File name : sub.h                                                           *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _SUB_H_
#define _SUB_H_

extern void		MemCpy( B* dst, INT dsiz, B* src, INT siz );						// 
extern ER		api_get_mpf( B* fname, INT line, ID id, VP* adr );					// 
extern ER		api_rel_mpf( B* fname, INT line, ID id, VP adr );					// 
extern ER		api_snd_dtq( B* fname, INT line, ID id, VP_INT data );				// 
//extern ER		mbx_send ( ID tskid, ID mpfid, ID mbxid, UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz);
extern ER		mbx_send( ID tskid, ID mpfid, ID mbxid, UB prid1, UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz);
//extern ER		sndproc  ( ID tskid, ID mpfid, ID mbxid, UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz );
extern ER		sndproc( ID tskid, ID mpfid, ID mbxid, UB prid1,UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz );
extern void		sio_CheckError( volatile struct st_sci* sci );						// scエラーセット
extern void		scif_CheckError( );													// scifエラーセット
extern void		SetError(ER err);													// 装置エラー状態設定
extern void		ClrError(ER err);													// 装置エラー状態クリア
extern ER		GetErrCode();														// 通知するエラーを取得
extern B		GetNetError(void);													// 次表示装置エラー状態取得
extern void		SetInputDat(UB index, UB data);										// 入力チャネルを設定
extern void		SetInputBit(UB index, UB data);
extern UB		GetInputDat(UB inputch);											// 入力チャネルを取得
extern UB		HextoAsc( UB dat );													// ＨＥＸ値(0～9,A～F)をＡＳＣⅡコードに変換
extern UB		HextoAscLower( UB dat );											// ＨＥＸ値(0～9,A～F)をＡＳＣⅡコードに変換（英字は小文字）
extern UB		AsctoHex( UB dat );													// ＡＳＣⅡコードをＨＥＸ値(0～9,A～F)に変換
extern UB		AsctoDec( UB dat );													// ＡＳＣⅡコードをＤＥＣ値(0～9)に変換
extern UB		DectoAscH( UB dat );
extern UB		DectoAscL( UB dat );
extern void		DectoHexASC( UB mot, UB *hdat, UB *ldat );							// ＤＥＣ値を１６進文字列に変換
extern INT		BtoINT( UB *mot );													// BYTEデータをINTデータに変換
extern INT		B2toINT( UB *mot );													// BYTEデータをINTデータに変換
extern void		INTtoB2( INT mot, UB *sak );										// INTデータを2BYTEデータに変換
extern void		SetByteArray( const char* str, UB* ary );							// 16進数文字列をBYTE配列にする
extern void		SetStrArray( UB* binary, UB* strary, INT siz);
extern INT		Strcmp( const char* s1, const char* s2 );							// 文字列の比較
extern void		ErrBitSet(INT errcode);												// エラーセット
extern void		ErrBitClr(INT errcode);												// エラークリア
extern BOOL		ErrBitGet(INT errcode);												//
extern void		ToUpper( char* str );												// 英字を大文字にする
extern void		ToUppern( char* str , INT len );									// 
extern UB		Split( char* src, const char** arg );								// コマンド引数を区切る
extern UB		SplitNull( char* src, const char** arg );							// コマンド引数をＮＵＬＬで区切る
extern UB		SplitSpace( char* src, const char** arg );
extern UB		SplitOnly( char* src, const char** arg );
extern char*	Strchr(const char* s, char c );										// 指定した文字が最初に現れる位置を検索する
extern INT		Atoi( const char* str, INT rad, INT byte, INT sig );				// 文字列を数値にする
extern UB Rcvset_pinf( const char* str );
extern void Rcvset_prot( const char* str , UB pno , UB adr );
extern INT		Strlen( const char* str );											// 文字列の長さをもろめる
extern void		Strcpy( char *s1, char *s2 );										// 文字列をコピー
extern void StrHexcpy( char *s1, char *s2 );
extern void StrHexcpyn( char *s1, char *s2, int siz );
extern UB		CalcBCC( UB* buff, INT size );										//  BCCを計算
extern void		WdgSet(void);														// ウオッチドック設定
extern void		WdgStop(void);														// ウオッチドック停止
extern void		WdgStart(void);														// ウオッチドック開始
extern UB		GetCwInfo( char* arg );
extern void		BitSet_INT( UB index, INT *data );
extern void		BitClr_INT( UB index, INT *data );
extern INT		BitGet_INT( UB index, INT *data );
extern UB		GetFirstCh( INT *data );
extern void		TargetMotorStop( INT *motdata );									// 指定したモータを停止
extern void		SetDatToArray( UB ch, UB *array );
extern void		ClrDatToArray( UB ch, UB *array );
extern UB		DectoAscH( UB dat );
extern UB		DectoAscL( UB dat );

//extern void DeviceInfoSet( INT sts, UB pno, UB step, UB cmd );
extern void DeviceInfoSet( ER result, UB pno, UB step, UB sts );
extern void		DeviceInfoClr();
extern INT		DeviceStatusGet();
extern UB		DeviceProtcolNoGet();
extern UB		DeviceStepGet();
extern UB		DeviceCmdGet();
extern unsigned short IdleBitSet(unsigned char cnt);

#define GetMpf(a, b)    api_get_mpf( __FILE__, __LINE__, a, b )				// 
#define RelMpf(a, b)    api_rel_mpf( __FILE__, __LINE__, a, b )				// 
#define SndDtq(a, b)    api_snd_dtq( __FILE__, __LINE__, a, b )				// 


#endif  // _SUB_H_

