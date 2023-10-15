/******************************************************************************
* �T�u�֐�                                                                    *
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
extern void		sio_CheckError( volatile struct st_sci* sci );						// sc�G���[�Z�b�g
extern void		scif_CheckError( );													// scif�G���[�Z�b�g
extern void		SetError(ER err);													// ���u�G���[��Ԑݒ�
extern void		ClrError(ER err);													// ���u�G���[��ԃN���A
extern ER		GetErrCode();														// �ʒm����G���[���擾
extern B		GetNetError(void);													// ���\�����u�G���[��Ԏ擾
extern void		SetInputDat(UB index, UB data);										// ���̓`���l����ݒ�
extern void		SetInputBit(UB index, UB data);
extern UB		GetInputDat(UB inputch);											// ���̓`���l�����擾
extern UB		HextoAsc( UB dat );													// �g�d�w�l(0�`9,A�`F)���`�r�b�U�R�[�h�ɕϊ�
extern UB		HextoAscLower( UB dat );											// �g�d�w�l(0�`9,A�`F)���`�r�b�U�R�[�h�ɕϊ��i�p���͏������j
extern UB		AsctoHex( UB dat );													// �`�r�b�U�R�[�h���g�d�w�l(0�`9,A�`F)�ɕϊ�
extern UB		AsctoDec( UB dat );													// �`�r�b�U�R�[�h���c�d�b�l(0�`9)�ɕϊ�
extern UB		DectoAscH( UB dat );
extern UB		DectoAscL( UB dat );
extern void		DectoHexASC( UB mot, UB *hdat, UB *ldat );							// �c�d�b�l���P�U�i������ɕϊ�
extern INT		BtoINT( UB *mot );													// BYTE�f�[�^��INT�f�[�^�ɕϊ�
extern INT		B2toINT( UB *mot );													// BYTE�f�[�^��INT�f�[�^�ɕϊ�
extern void		INTtoB2( INT mot, UB *sak );										// INT�f�[�^��2BYTE�f�[�^�ɕϊ�
extern void		SetByteArray( const char* str, UB* ary );							// 16�i���������BYTE�z��ɂ���
extern void		SetStrArray( UB* binary, UB* strary, INT siz);
extern INT		Strcmp( const char* s1, const char* s2 );							// ������̔�r
extern void		ErrBitSet(INT errcode);												// �G���[�Z�b�g
extern void		ErrBitClr(INT errcode);												// �G���[�N���A
extern BOOL		ErrBitGet(INT errcode);												//
extern void		ToUpper( char* str );												// �p����啶���ɂ���
extern void		ToUppern( char* str , INT len );									// 
extern UB		Split( char* src, const char** arg );								// �R�}���h��������؂�
extern UB		SplitNull( char* src, const char** arg );							// �R�}���h�������m�t�k�k�ŋ�؂�
extern UB		SplitSpace( char* src, const char** arg );
extern UB		SplitOnly( char* src, const char** arg );
extern char*	Strchr(const char* s, char c );										// �w�肵���������ŏ��Ɍ����ʒu����������
extern INT		Atoi( const char* str, INT rad, INT byte, INT sig );				// ������𐔒l�ɂ���
extern UB Rcvset_pinf( const char* str );
extern void Rcvset_prot( const char* str , UB pno , UB adr );
extern INT		Strlen( const char* str );											// ������̒���������߂�
extern void		Strcpy( char *s1, char *s2 );										// ��������R�s�[
extern void StrHexcpy( char *s1, char *s2 );
extern void StrHexcpyn( char *s1, char *s2, int siz );
extern UB		CalcBCC( UB* buff, INT size );										//  BCC���v�Z
extern void		WdgSet(void);														// �E�I�b�`�h�b�N�ݒ�
extern void		WdgStop(void);														// �E�I�b�`�h�b�N��~
extern void		WdgStart(void);														// �E�I�b�`�h�b�N�J�n
extern UB		GetCwInfo( char* arg );
extern void		BitSet_INT( UB index, INT *data );
extern void		BitClr_INT( UB index, INT *data );
extern INT		BitGet_INT( UB index, INT *data );
extern UB		GetFirstCh( INT *data );
extern void		TargetMotorStop( INT *motdata );									// �w�肵�����[�^���~
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

