/******************************************************************************
* マクロヘッダ                                                                *
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

// レンジチェック ----------------------------------------
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
		
// レンジチェック ----------------------------------------
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


// セマフォ( wai_sem ) -----------------------------------
// [戻り値]
// E_OK       0 : 正常終了
// E_ID     -18 : セマフォID が範囲外*
// E_NOEXS  -42 : セマフォが生成されていない
// E_CTX    -25 : 非タスクコンテキストで、または、ディスパッチ禁止状態で待ち実行*
// E_RLWAI  -49 : 待ち状態を強制解除された(待ちの間にrel_wai を受け付け)
// E_DLT    -51 : 待ちの間にセマフォが削除された
#define MAC_WAI_SEM( id ){                               \
	result = wai_sem(id);                                \
	if( result != E_OK )                                 \
	{                                                    \
		result *= -1;                                    \
		return( ( E_MNT_ACCESS + result ) );             \
	}                                                    \
}
//--------------------------------------------------------



// セマフォ( sig_sem ) -----------------------------------
// [戻り値]
// E_OK       0 : 正常終了
// E_ID     -18 : セマフォID が範囲外*
// E_NOEXS  -42 : セマフォが生成されていない
// E_CTX    -25 : 非タスクコンテキストで、または、ディスパッチ禁止状態で待ち実行*
// E_RLWAI  -49 : 待ち状態を強制解除された(待ちの間にrel_wai を受け付け)
// E_DLT    -51 : 待ちの間にセマフォが削除された
#define MAC_SIG_SEM( id ){                              \
	result = sig_sem(id);                               \
	if( result != E_OK )                                \
	{                                                   \
		result *= -1;                                   \
		return( ( E_MNT_ACCESS + result ) );             \
	}                                                   \
}
//-------------------------------------------------------


// 連続5回読み ------------------------------------------
// now:読込対象、rtn:5回読込確定値
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



// 連続5回読み -----------------------------------------
// now:読込対象、rtn:10回読込確定値
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



// 連続5回書込み ----------------------------------------
// now:書込み対象、rtn:書込み値
#define MAC_WRIT( trgt, rtn ){                          \
    INT Loop;                                           \
	trgt = rtn;                                         \
	for( Loop = 0; Loop < 5; Loop++ )                   \
	{                                                   \
		trgt = rtn;                                     \
	}                                                   \
}
//-------------------------------------------------------



// 速度設定 ---------------------------------------------
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


// 軸アイドリングWAit ----------------------------------
#define MAC_DATYO_CHECK( dv, nz1, nz2  ){               \
    result = GetDatyoDev( dv );                         \
    if( result != E_SUCCESS )                           \
    {                                                   \
        nz1 = result;                                   \
        nz2 = result;                                   \
        return( result );                               \
    }                                                   \
}

// リミット確認  --*****--------------------------------
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
