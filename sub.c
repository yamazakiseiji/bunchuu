/******************************************************************************
* �T�u�֐�                                                                    *
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
// �֐���`
//*************************************

//*************************************
// �e�[�u��
//*************************************


//*************************************
// �f�[�^��`
//*************************************

static 	UB			InpChInf[INDAT_CNTRL_MAX];							// ���̓`���l��
static volatile struct	StMotionnet	*pMotionnet;
//static 	INT			ErrorInfo;
static 	ST_DEVICE_INFO	dev;

/******************************************************************************
* 
* [�T�v]	�f�[�^�̃R�s�[���s��
* [����]	dst     �R�s�[��
* 			disz    �R�s�[��̃T�C�Y
* 			src     �R�s�[��
* 			siz     �R�s�[��
* [�ߒl]	-
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
* [�T�v]	�������v�[���擾
* [����]	ID      �������[�v�[��ID
*			adr     �������u���b�N�ւ̃|�C���^
* [�ߒl]	-
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
* [�T�v]	�������v�[�����
* [����]	ID      �������[�v�[��ID
*			adr     �������u���b�N�ւ̃|�C���^
* [�ߒl]	-
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
* [�T�v]
* [����]	ID      �f�[�^�L���[ID
* 			data    ���M����f�[�^
* [�ߒl]	�G���[�R�[�h
* 
******************************************************************************/
ER api_snd_dtq( B* fname, INT line, ID id, VP_INT data )
{
    ER  err;

    // �f�[�^�L���[���M
    if( sns_ctx( ) )
    {   // ��R���e�L�X�g
        err = ipsnd_dtq( id, data );
        if( E_OK != err )
        {
	        return E_SYS_MSGCOM;
        }
    }
    else
    {   // �R���e�L�X�g
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
* [�T�v]
* 		������ pool_send(���ID,ү����BOXID,ү����ID,���,DATA);
* 		�i���ʐM�f�[�^�T�C�Y�t���j
* [����]	-
* 
* [�ߒl]	-
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

		// �^�C���A�E�g�ȊO�̃G���[�̎��́A�������[�v�ƂȂ�\��������̂Œ��ӂ��邱�ƁI�I
		if(err == E_TMOUT)
		{
			dly_tsk(DLY_10M);			/* dly 10ms */
		}								/* �^�C���A�E�g���͍ăg���C */
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
* [�T�v]
* 
* [����]	-
* 
* [�ߒl]	-
* 
******************************************************************************/
ER sndproc( ID tskid, ID mpfid, ID mbxid, UB prid1,UB bcid, UB msgid, UB sts, UB sts2, UB* data, INT siz )
{
	T_CMD_MSG*    msg;
	ER		err ;

	if(mbxid <= 0)
	{
		return(E_SYS_DATERR);							// �f�[�^�������G���[
	}

	err = GetMpf( mpfid, (VP)&msg );					// �������v�[���擾
	if(err == E_OK)
	{
		msg->msgdata.tskid  = tskid;					// ���M���̃^�X�N�h�c
		msg->msgdata.prid1  = prid1;					// 
		msg->msgdata.bcid   = bcid;						// �u���b�N�R�}���h�h�c
		msg->msgdata.msgid  = msgid;					// ���b�Z�[�W�h�c
		msg->msgdata.inf    = sts;						// ���
		msg->msgdata.inf2   = sts2;						// ���
		msg->msgdata.siz    = siz;						// �T�C�Y
		memcpy( &msg->msgdata.data[0], data, CMD_BUFSIZ );

		err         = snd_mbx(mbxid, msg);				// ���[�����M

		if (err != E_OK)
		{
			RelMpf( mpfid, (VP)msg );					// ���������
			err = E_SYS_MSGCOM;
		}
	}
	return(err);
}
/******************************************************************************
* �V���A���|�[�g�̃G���[�̃`�F�b�N
* 
* [�T�v]
* 
* [����]	sci:�V���A���|�[�g�̃��W�X�^�ւ̃|�C���^
* 
* [�ߒl]	-
* 
******************************************************************************/
void sio_CheckError( volatile struct st_sci* sci )
{
	if( sci->SCSSR.BIT.ORER )         // �I�[�o�[�����G���[
	{
		sci->SCSSR.BIT.ORER = 0;
	}
	if( sci->SCSSR.BIT.FER )          // �t���[�~���O�G���[
	{
		sci->SCSSR.BIT.FER = 0;
	}
	if( sci->SCSSR.BIT.PER )          // �p���e�B�[�G���[
	{
		sci->SCSSR.BIT.PER = 0;
	}
}
/******************************************************************************
* �V���A���|�[�g�̃G���[�̃`�F�b�N
* 
* [�T�v]
* 
* [����]	-
* 
* [�ߒl]	-
* 
******************************************************************************/
void scif_CheckError()
{
	if( SCIF3.SCFSR.BIT.ER )         	// �G���[
	{
		SCIF3.SCFSR.BIT.ER = 0;
	}
	if( SCIF3.SCFSR.BIT.FER )          // �t���[�~���O�G���[
	{
		SCIF3.SCFSR.BIT.FER = 0;
	}
	if( SCIF3.SCFSR.BIT.PER )          // �p���e�B�[�G���[
	{
		SCIF3.SCFSR.BIT.PER = 0;
	}
}
/******************************************************************************
* 
* [�T�v]	���u�G���[��Ԑݒ�
* [����]	
* [�ߒl]	-
* 
******************************************************************************/
void SetError(ER err)
{
}
/******************************************************************************
* 
* [�T�v]	���u�G���[��ԃN���A
* [����]	
* [�ߒl]	-
* 
******************************************************************************/
void ClrError(ER err)
{
}
/******************************************************************************
* 
* [�T�v]	�G���[���擾����
* [����]	
* [�ߒl]	-
* 
******************************************************************************/
ER GetErrCode()
{
}

/******************************************************************************
* 
* [�T�v]	�G���[�Z�b�g
* [����]	errcode: �G���[�R�[�h
* [�ߒl]	�|
* 
******************************************************************************/
void ErrBitSet(INT errcode)
{
}
/******************************************************************************
* 
* [�T�v]	�G���[�N���A
* [����]	errcode: �G���[�R�[�h
* [�ߒl]	�|
* 
******************************************************************************/
void ErrBitClr(INT errcode)
{
}
/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
BOOL ErrBitChk(INT errcode)
{
}

/******************************************************************************
* 
* [�T�v]	���̓`���l������������L�f�[�^�ɐݒ�
* [����]	���̓`���l���f�[�^
* [�ߒl]	-
* 
******************************************************************************/
void SetInputDat(UB index, UB data)
{
   int save_i;
    // �����݋֎~����
    save_i = get_imask();
 	set_imask(0x0f);

//5/24	InpChInfOld[index] = InpChInf[index] ;
	InpChInf[index] = data;

	set_imask(save_i);
}
/******************************************************************************
* 
* [�T�v]	���̓`���l������������L�f�[�^�ɐݒ�
* [����]	���̓`���l���f�[�^
* [�ߒl]	-
* 
******************************************************************************/
void SetInputBit(UB index, UB data)
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;

	syou    = ( index / 8 );			// �]��擾
	amari   = ( index % 8 );			// �]��擾
	sftdat  = ( sftbit << amari );		// �]����V�t�g

//
//	�^�C�}�����݂݂̂ŌĂ΂�邽�ߕs�v
//
//	// �����݋֎~����
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
* [�T�v]	���̓`���l�����擾
* [����]	���̓`���l���擾�ʒu
* [�ߒl]	���̓`���l���f�[�^
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
* [�T�v]	�g�d�w�l(0�`9,A�`F)���`�r�b�U�R�[�h�ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�g�d�w�l(0�`9,A�`F)���`�r�b�U�R�[�h�ɕϊ��i�p���͏������j
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�`�r�b�U�R�[�h���g�d�w�l(0�`9,A�`F)�ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�`�r�b�U�R�[�h���c�d�b�l(0�`9)�ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�c�d�b�l���`�r�b�U�R�[�h�ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�c�d�b�l���`�r�b�U�R�[�h�ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	�c�d�b�l���P�U�i������ɕϊ�
* [����]	dat:�ϊ���
* [�ߒl]	�ϊ���
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
* [�T�v]	BYTE�f�[�^��INT�f�[�^�ɕϊ�
* [����]	mot:�ϊ����f�[�^�擪�|�C���^ ���ϊ����f�[�^�͕K��4byte�\���ł��邱�ƁB
* [�ߒl]	�|
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
* [�T�v]	BYTE�f�[�^(2BYTE)��INT�f�[�^�ɕϊ�
* [����]	mot:�ϊ����f�[�^�擪�|�C���^
* [�ߒl]	�|
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
* [�T�v]	INT�f�[�^��2BYTE�f�[�^�ɕϊ�
* [����]	mot:�ϊ����f�[�^ ���ϊ����f�[�^�͕K��2byte�\���ł��邱�ƁB
* [�ߒl]	�|
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
* [�T�v]	16�i���������BYTE�z��ɂ���
* [����]	str:16�i��������, ary:�i�[��BYTE�z��
* [�ߒl]	�|
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
* [�T�v]	16�i���l�𕶎���z��ɂ���
* [����]	binary:�f�[�^��(16�i���l), strary:�i�[�敶����,siz:�f�[�^���T�C�Y
* [�ߒl]	�|
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
* [�T�v]	�F������̔�r
* [����]	�Fcmpcmd:��r�^�[�Q�b�g�Acmpchr:��r������
* [�ߒl]	�F0:s1==s2�A1:s1>s2, -1:s1<s2
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
* [�T�v]	�F�p����啶���ɂ���
* [����]	�Fstr:������
* [�ߒl]	�F
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
* [�T�v]	�F�p����啶���ɂ���
* [����]	�Fstr:������
* [�ߒl]	�F
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
* [�T�v]	�F�R�}���h��������؂�
* [����]	�Fstr:������Aarg:�����̃|�C���^
* [�ߒl]	�F��؂萔
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
* [�T�v]	�F�R�}���h�������m�t�k�k�ŋ�؂�
* [����]	�Fstr:������Aarg:�����̃|�C���^
* [�ߒl]	�F��؂萔
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
* [�T�v]	�F�R�}���h�������m�t�k�k�ŋ�؂�
* [����]	�Fstr:������Aarg:�����̃|�C���^
* [�ߒl]	�F��؂萔
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
* [�T�v]	�F�R�}���h��������؂�
* [����]	�Fstr:������Aarg:�����̃|�C���^
* [�ߒl]	�F��؂萔
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
* [�T�v]	�F�w�肵���������ŏ��Ɍ����ʒu����������
* [����]	�Fstr:������Aarg:�������镶��
* [�ߒl]	�F���������ʒu�̃|�C���^�A������Ȃ������ꍇ��NULL
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
  �@�\�F������𐔒l�ɂ���
  �����Fstr     ��������
        rad     �i��( 10 or 16 )
        byte    �o�C�g��(1/2/4)
        sig     ����(0:�����Ȃ��A1:��������)
  �ߒl�F���l
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

    if( N_SIG == sig )    // ��������
    {
        if     ( ( 1 == byte ) && ( val & 0x00000080 ) ) val |= 0xffffff00;
        else if( ( 2 == byte ) && ( val & 0x00008000 ) ) val |= 0xffff0000;
    }

    return val;
}

/*-----------------------------------------------------------------------------
  �@�\�F������̒��������߂�
  �����Fstr     ������
  �ߒl�F������
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
  �@�\�F��������R�s�[
  �����Fs1:�R�s�[��As2:�R�s�[��
  �ߒl�F�|
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
  �@�\�F��������R�s�[
  �����Fs1:�R�s�[��As2:�R�s�[��
  �ߒl�F�|
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
  �@�\�F��������R�s�[
  �����Fs1:�R�s�[��As2:�R�s�[��
  �ߒl�F�|
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
* [�T�v]	BCC�̌v�Z
* [����]	buff:�v�Z���f�[�^,size:�v�Z���f�[�^��
* [�ߒl]	BCC
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
* [�T�v]	�E�I�b�`�h�b�N�ݒ�
* [����]	�|
* [�ߒl]	�|
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
* [�T�v]	�E�I�b�`�h�b�N��~
* [����]	�|
* [�ߒl]	�|
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
* [�T�v]	�E�I�b�`�h�b�N�J�n
* [����]	�|
* [�ߒl]	�|
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
* [�T�v]CW/CCW���擾
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
* [�T�v]	�w�肵��index�l�̃r�b�g���Z�b�g
* [����]	index = 1�`�Adata = �i�[��f�[�^
* [�ߒl]	�r�b�g�Z�b�g�l
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
		syou    = ( indexwk / 8 );			// �]��擾
		amari   = ( indexwk % 8 );			// �]��擾
		sftdat  = ( sftbit << amari );		// �]����V�t�g
		*data  |= sftdat;
	}
}
/******************************************************************************
* 
* [�T�v]	�w�肵��index�l�̃r�b�g���N���A
* [����]	index = 1�`�Adata = �i�[��f�[�^
* [�ߒl]	�r�b�g�N���A�l
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
		syou    = ( indexwk / 8 );			// �]��擾
		amari   = ( indexwk % 8 );			// �]��擾
		sftdat  = ( sftbit << amari ) ;		// �]����V�t�g
		*data  &= (~sftdat);
	}
}

/******************************************************************************
* 
* [�T�v]	�w�肵��index�l�̃r�b�g���Z�b�g(INT�^�z��p)
*           ��jdata = 0x0000000, index = 31  ��  �߂�l 0x80000000
* [����]	data = �w��f�[�^�Aindex = 0�`31
* [�ߒl]	�r�b�g�Z�b�g�l
* 
******************************************************************************/
void BitSet_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	UB indexwk;										// 2020/2/12 �P�͂��܂�Ή�

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 �P�͂��܂�Ή�
	{
		indexwk = index - 1;						// 2020/2/12 �P�͂��܂�Ή�
		syou    = ( indexwk / 32 );			// �]��擾
		amari   = ( indexwk % 32 );			// �]��擾
		sftdat  = ( sftbit << amari );		// �]����V�t�g
		data[syou] |= sftdat;
	}
}

/******************************************************************************
* 
* [�T�v]	�w�肵��index�l�̃r�b�g���N���A(INT�^�z��p)
*           ��jdata = 0x8000000, index = 31  ��  �߂�l 0x00000000
* [����]	data = �w��f�[�^�Aindex = 0�`31
* [�ߒl]	�r�b�g�N���A�l
* 
******************************************************************************/
void BitClr_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	UB indexwk;										// 2020/2/12 �P�͂��܂�Ή�

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 �P�͂��܂�Ή�
	{
		indexwk = index - 1;						// 2020/2/12 �P�͂��܂�Ή�
		syou    = ( indexwk / 32 );			// �]��擾
		amari   = ( indexwk % 32 );			// �]��擾
		sftdat  = ( sftbit << amari ) ;		// �]����V�t�g
		data[syou] &= (~sftdat);
	}
}

/******************************************************************************
* 
* [�T�v]	�w�肵��index�l�̃r�b�g��Ԃ��擾(INT�^�z��p)
* [����]	index = 0�`31
* [�ߒl]	���O ( index�Ŏw�肵���ʒu�̃r�b�g���P )
* 
******************************************************************************/
INT BitGet_INT( UB index, INT *data )
{
	INT sftbit = 0x00000001;
	INT amari;
	INT syou;
	INT sftdat;
	INT rltdat;
	UB indexwk;										// 2020/2/12 �P�͂��܂�Ή�

	rltdat = *data;

	if(( index >= MIN_CH )&&( index <= MAX_CH ))	// 2020/2/12 �P�͂��܂�Ή�
	{
		indexwk = index - 1;						// 2020/2/12 �P�͂��܂�Ή�
		syou    = ( indexwk / 32 );			// �]��擾
		amari   = ( indexwk % 32 );			// �]��擾
		sftdat  = ( sftbit << amari ) ;		// �]����V�t�g
		rltdat  = ( data[syou] & sftdat );
	}

	return( rltdat );
}

/******************************************************************************
* 
* [�T�v]	�ŏ��Ɍ�����bit(=1)�̃��[�v�l��Ԃ�
*           ���O��Ƃ��ĕK���A�ǂ�����bit�������Ă���O��Ŗ{�֐��͌Ă΂��
* [����]	�h�m�s�f�[�^�A�h���X
* [�ߒl]	���[�v�l
* 
******************************************************************************/
UB GetFirstCh( INT *data )
{
	INT sftbit = 0x00000001;
	INT sftdat;
	INT rltdat;
	UB  loop;


	// bit0�`bit31
	sftbit = 0x00000001;
	for( loop = 0; loop < 32; loop++ )
	{
		sftdat  = ( sftbit << loop ) ;
		rltdat  = ( *data & sftdat );

		if( rltdat )	return( (loop+1) );
	}

	return( NO_DATA );						// �Ȃ�
}
/******************************************************************************
* 
* [�T�v]	�w�肵�����[�^���~
* [����]	�w�胂�[�^�ibit=1:��~���[�^�j
* [�ߒl]	�|
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
* [�T�v]    Ch�ԍ���BIT����UB�z��̃f�[�^�ɃZ�b�g����B
*           �p�r�Ƃ���INT�R�}���h���ɗ��p   ���z��[0]���珬����Ch�ԍ�
* 
*           ��jCh.63�̏ꍇ�ɔz��data�ɂ́A�ȉ��̂悤�ɃZ�b�g
* 
*           ������Ch    data[0] = 0x00
*             ��        data[1] = 0x00
*             �b        data[2] = 0x00
*             �b        data[3] = 0x00
*             �b        data[4] = 0x00
*             �b        data[5] = 0x00
*             ��        data[6] = 0x00
*           �傫��Ch    data[7] = 0x80   �� Ch.63
* 
* [����]    Ch = CH�ԍ��Aarray = �z��f�[�^�擪�A�h���X
* [�ߒl]    �|
* 
******************************************************************************/
void SetDatToArray( UB ch, UB *array )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB chwk;								// 2020/2/12 �P�͂��܂�Ή�

	if(( ch >= MIN_CH )&&( ch <= MAX_CH ))	// 2020/2/12 �P�͂��܂�Ή�
	{
		chwk  = ch - 1;						// 2020/2/12 �P�͂��܂�Ή�
		syou  = ( chwk / 8 );
		amari = ( chwk % 8 );
		sftdat = ( sftbit << amari );
		array[syou] |= sftdat;
	}
}

/******************************************************************************
* 
* [�T�v]	Ch�ԍ���BIT����UB�z��̃f�[�^�̃N���A�B
* [����]	Ch = CH�ԍ��Aarray = �z��f�[�^�擪�A�h���X
* [�ߒl]	�|
* 
******************************************************************************/
void ClrDatToArray( UB ch, UB *array )
{
	UB sftbit = 0x01;
	UB amari;
	UB syou;
	UB sftdat;
	UB chwk;								// 2020/2/12 �P�͂��܂�Ή�

	if(( ch >= MIN_CH )&&( ch <= MAX_CH ))	// 2020/2/12 �P�͂��܂�Ή�
	{
		chwk  = ch - 1;						// 2020/2/12 �P�͂��܂�Ή�
		syou  = ( chwk / 8 );
		amari = ( chwk % 8 );
		sftdat = ( sftbit << amari );
		array[syou] &= (~sftdat);
	}
}

/******************************************************************************
* 
* [�T�v]	
* [����]	���u��ԁA�v���g�R���ԍ��A�X�e�b�v�A�R�}���h
* [�ߒl]	
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
	
	// ���u��ԍ쐬
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
	
	if( flg == SET )				// �f�[�^�ɕω��L��
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
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
void DeviceInfoClr()
{
	DeviceInfoSet(CLR,CLR,CLR,CLR);
}
/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
INT DeviceStatusGet()
{
	return( dev.sts );
}
/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
UB DeviceProtcolNoGet()
{
	return( dev.pno );
}
/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
UB DeviceStepGet()
{
	return( dev.step );
}
/******************************************************************************
* 
* [�T�v]	
* [����]	
* [�ߒl]	
* 
******************************************************************************/
UB DeviceCmdGet()
{
	return( (dev.cmd & 0x0f) );
}

/******************************************************************************
* 
* [�T�v]	CNT�����A�E����bit���Z�b�g
* [����]	
* [�ߒl]	
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


