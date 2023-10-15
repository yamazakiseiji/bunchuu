/******************************************************************************
* �}�N���w�b�_                                                                *
*                                                                             *
* File name : macro.h                                                         *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _MACRO_H_
#define _MACRO_H_

#define sfr_outw(n,c)   (*(volatile UH *)(IO_BASE+(n))=(c))
#define sfr_inw(n)      (*(volatile UH *)(IO_BASE+(n)))

// �����W�`�F�b�N ----------------------------------------
#define RANGECHK( pnum, dat, min, max){                  \
      if(dat < min)                                      \
      {                                                  \
            return ( E_PAR_OUTRNG + pnum );              \
      }                                                  \
      if(dat > max)                                      \
      {                                                  \
            return ( E_PAR_OUTRNG + pnum );              \
      }                                                  \
}
//--------------------------------------------------------
		
// �����W�`�F�b�N ----------------------------------------
#define RANGECHK_RESULTSET( pnum, dat, min, max){      \
    if(result == E_SUCCESS)                            \
    {                                                  \
        if(dat < min)                                  \
        {                                              \
           result = E_PAR_OUTRNG + pnum ;              \
        }                                              \
        if(dat > max)                                  \
        {                                              \
           result = E_PAR_OUTRNG + pnum ;              \
        }                                              \
    }                                                  \
}
//--------------------------------------------------------


// �Z�}�t�H( wai_sem ) -----------------------------------
// [�߂�l]
// E_OK       0 : ����I��
// E_ID     -18 : �Z�}�t�HID ���͈͊O*
// E_NOEXS  -42 : �Z�}�t�H����������Ă��Ȃ�
// E_CTX    -25 : ��^�X�N�R���e�L�X�g�ŁA�܂��́A�f�B�X�p�b�`�֎~��Ԃő҂����s*
// E_RLWAI  -49 : �҂���Ԃ������������ꂽ(�҂��̊Ԃ�rel_wai ���󂯕t��)
// E_DLT    -51 : �҂��̊ԂɃZ�}�t�H���폜���ꂽ
#define MAC_WAI_SEM( id ){                               \
	result = wai_sem(id);                                \
	if( result != E_OK )                                 \
	{                                                    \
		result *= -1;                                    \
		return( ( E_MNT_ACCESS + result ) );             \
	}                                                    \
}
//--------------------------------------------------------



// �Z�}�t�H( sig_sem ) -----------------------------------
// [�߂�l]
// E_OK       0 : ����I��
// E_ID     -18 : �Z�}�t�HID ���͈͊O*
// E_NOEXS  -42 : �Z�}�t�H����������Ă��Ȃ�
// E_CTX    -25 : ��^�X�N�R���e�L�X�g�ŁA�܂��́A�f�B�X�p�b�`�֎~��Ԃő҂����s*
// E_RLWAI  -49 : �҂���Ԃ������������ꂽ(�҂��̊Ԃ�rel_wai ���󂯕t��)
// E_DLT    -51 : �҂��̊ԂɃZ�}�t�H���폜���ꂽ
#define MAC_SIG_SEM( id ){                              \
	result = sig_sem(id);                               \
	if( result != E_OK )                                \
	{                                                   \
		result *= -1;                                   \
		return( ( E_MNT_ACCESS + result ) );             \
	}                                                   \
}
//-------------------------------------------------------


// �A��5��ǂ� ------------------------------------------
// now:�Ǎ��ΏہArtn:5��Ǎ��m��l
#define MAC_READ( trgt, rtn ){                          \
    INT Loop;                                           \
    UB  now;                                            \
	rtn = trgt;                                         \
	for( Loop = 0; Loop < 5; Loop++ )                   \
	{                                                   \
		now = trgt;                                     \
		if( now != rtn )                                \
		{                                               \
			Loop = 0;                                   \
			rtn = now;                                  \
		}                                               \
	}                                                   \
}
//-------------------------------------------------------



// �A��5��ǂ� -----------------------------------------
// now:�Ǎ��ΏہArtn:10��Ǎ��m��l
#define MAC_READ_10( trgt, rtn ){                       \
    INT Loop;                                           \
    UB  now;                                            \
	rtn = trgt;                                         \
	for( Loop = 0; Loop < 10; Loop++ )                  \
	{                                                   \
		now = trgt;                                     \
		if( now != rtn )                                \
		{                                               \
			Loop = 0;                                   \
			rtn = now;                                  \
		}                                               \
	}                                                   \
}
//-------------------------------------------------------



// �A��5�񏑍��� ----------------------------------------
// now:�����ݑΏہArtn:�����ݒl
#define MAC_WRIT( trgt, rtn ){                          \
    INT Loop;                                           \
	trgt = rtn;                                         \
	for( Loop = 0; Loop < 5; Loop++ )                   \
	{                                                   \
		trgt = rtn;                                     \
	}                                                   \
}
//-------------------------------------------------------



// ���x�ݒ� ---------------------------------------------
#define MAC_SPEED_SET( dv, acl, zfl, zfh, zgl ){        \
    ST_MOTOR_PARAM  mp;                                 \
    mp.accel_type = acl;                                \
    mp.factor     = GetFactor(3);                       \
    mp.FL         = zfl;                                \
    mp.FH         = zfh;                                \
    mp.utime      = zgl;                                \
    mp.dtime      = zgl;                                \
    mp.us         = 0;                                  \
    mp.ds         = 0;                                  \
    motor_SET( dv, mp );                                \
}


// ���A�C�h�����OWAit ----------------------------------
#define MAC_DATYO_CHECK( dv, nz1, nz2  ){               \
    result = GetDatyoDev( dv );                         \
    if( result != E_SUCCESS )                           \
    {                                                   \
        nz1 = result;                                   \
        nz2 = result;                                   \
        return( result );                               \
    }                                                   \
}

// ���~�b�g�m�F  --*****--------------------------------
#define MAC_HEADLIMIT_CHECK( dv, dat1, dat2  ){         \
    if( motor_IsLimitW( dv ) == TRUE )                  \
    {                                                   \
        result = E_CON_ERR | dv ;                       \
    	dat1   = (INT)result;                           \
    	dat2   = (INT)result;                           \
        return( result );                               \
    }                                                   \
}










#endif  // _MACRO_H_
