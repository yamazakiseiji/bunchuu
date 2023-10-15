/********************************************************************************/
/* Copyright (C) 1992 (1995 - 2011) Renesas Electronics Corporation and         */
/* Renesas Solutions Corp. All Rights Reserved.                                 */
/********************************************************************************/
/*! \file  common.h
 *  \brief FFlib�p�w�b�_�t�@�C��
 *  \note  �\�[�X����͂��̃w�b�_���C���N���[�h���A\n
 *         ���̃t�@�C���Ŋe��(SH,H8,M16C,M32R)�p�̃w�b�_�t�@�C�����C���N���[�h����
 */

#ifndef _FFLIB_H_
#define _FFLIB_H_


/*
**--------------------------------------------------
** ���C�u�����̃r���h�I�v�V����
** OPT_TYPE_SIZE�L���F�T�C�Y�œK���i�Ή��F�L��F���������[�v���j
** OPT_TYPE_SPEED�F�X�s�[�h�œK���i�Ή��F�L��F���ʃ��W���[���̃C�����C���W�J�j
**--------------------------------------------------
*/
#define OPT_TYPE_SIZE
#define OPT_TYPE_SPEED

/*
**--------------------------------------------------
** ���W���[�����ʒ萔��`
**--------------------------------------------------
*/
/*! �Œ菬���_�ϊ��p�萔(Q32�ϊ�) */
#define S32			(65536.0f*65536.0f)

/*! �Œ菬���_�ϊ��p�萔(Q31�ϊ�) */
#define S31			(65536.0f*32768.0f)

/*! �Œ菬���_�ϊ��p�萔(Q30�ϊ�) */
#define S30			(32768.0f*32768.0f)



/*
**--------------------------------------------------
**�}�C�R���ʐݒ�
**--------------------------------------------------
*/
/* �}�C�R���ʃC���N���[�h�t�@�C��			*/
/* �y�сA�R���p�C���ʃC�����C���W�J�̎w��	*/
/* ���ꂼ��̃}�N���̓R���p�C���̈Öِ錾	*/
#ifdef _SH
	/* SH�p(SHC) */
	#define INLINE static
	#pragma inline(integer, fraction, frexp_m, frexp_e)
	#pragma inline(in_sqrtf)
	#pragma inline(get_n, myfmod)
	#include "fflb_sh.h"
#else
#ifdef __H8__
	/* H8�p(H8C) */
	#define INLINE __inline static
	#include "fflb_h8.h"
#else
#ifdef __M32R__
	/* M32R�p(CC32R) */
	#define INLINE inline static
	#include "fflb_m32r.h"
	/* �A��CC32R�ŃC�����C���W�J��L���ɂ���ɂ�
	** �œK�����x�����S�ȏ�ɂ���K�v������[-O4]
	*/
#else
#ifdef M16C
	/* M16C�p(NC30) */
	#define INLINE inline
	#include "fflb_m16c.h"
#endif	/* M16C */
#endif	/* __M32R__ */
#endif	/* __H8__ */
#endif	/* _SH */

/*
**--------------------------------------------------
** FFlib�֐��i�O�����J�֐��j
**--------------------------------------------------
*/
unsigned long sinf( unsigned long );	/* FFLIB B-012 B-016	*/
unsigned long cosf( unsigned long );	/* FFLIB B-012 B-016	*/
unsigned long expf( unsigned long );	/* FFLIB B-012 B-016	*/
unsigned long logf( unsigned long );	/* FFLIB B-012 B-016	*/
#if defined(_SH2E)|defined(_SH3E)|defined(_SH4)|defined(_SH4A)|defined(_SH2AFPU)	/* BSH030 */
float sqrtf( float );
#else
unsigned long sqrtf( unsigned long );	/* FFLIB B-012 B-016	*/
#endif
unsigned long tanf( unsigned long );	/* FFLIB B-012 B-016	*/
unsigned long atanf( unsigned long );	/* FFLIB B-012 B-016	*/

/*
**--------------------------------------------------
** �����֐��i�v���g�^�C�v�j
**--------------------------------------------------
*/
/* sin/cos���C������ */
long __insinL(unsigned long phase, unsigned long m, long e, unsigned long d0);					/* FFLIB B-011	*/
/* sin�W�� �������v�Z */
long __poly_sinL(long d0, long d3, long e, long sign);											/* FFLIB B-011	*/
/* cos�W�� �������v�Z */
long __poly_cosL(long d1, long e, long sign);													/* FFLIB B-011	*/

/* ------ normalize and round ------------------------ */
long __myround(unsigned long m2, long e4, long sign); 		/* mant, exp, sign				*/	/* FFLIB B-011	*/
long __rtn_inf(unsigned long r5);							/* sign							*/	/* FFLIB B-011	*/
long __normalize(unsigned long m, long e, long sign);		/* mant, exp, sign				*/	/* FFLIB B-011	*/
long __normalize1(unsigned long r2, long r4, long r5);		/* mant, exp, sign				*/	/* FFLIB B-011	*/
long __denorm(unsigned long r2, long r4, long r5);			/* construct denormalized num.	*/	/* FFLIB B-011	*/

#ifdef OPT_TYPE_SPEED
/*
**--------------------------------------------------
** �����֐��i�C�����C���W�J or static�j
**--------------------------------------------------
*/
/* �O�u��INLINE�͊��ɂ��inline��static�ɒu������ */
/*!
 *  \brief  �[��double�l�̐������擾�֐�
 *  \param  u  Q32�Œ菬���_��
 *  \param  e  �w��
 *  \return    �����l
 *  \note  ������u*2^e�Ƃ��āA���̂����̐������������߂Ė߂�l�Ƃ���\n
 *  ����������0�̏ꍇ��0��Ԃ�  e��33�ȏ�̏ꍇ�͕s��\n
 */
INLINE unsigned long integer(unsigned long u, long e)
{
	if(e <= 0){
		return (0);
	}else{
		return (u >> (32 - e));
	}
}
/*!
 *  \brief  �[��double�l�̏������擾�֐�
 *  \param  u  Q32�Œ菬���_��
 *  \param  e  �w����
 *  \return    Q32�Œ菬���_��
 *
 *  ������u*2^n�Ƃ��āA���̂����̏������������߂�\n
 *  �Œ菬���_���Ƃ��Ė߂�l�Ƃ���\n
 *  e��32�ȏ�̏ꍇ�͕s��\n
 *  e��-32�ȏ�̏ꍇ�͕s��\n
 */
INLINE unsigned long fraction(unsigned long u, long e)
{
	if(e < 0){
		return (u >> (-e));
	}else{
		return (u << e);
	}
}
/*!
 *  \brief  float�^�̃f�[�^���牼������؂�o��
 *  \param  f  ���̃f�[�^�iI/F��unsigned long�������g��float�j
 *  \return  �؂�o����������
 *  \note   ��������MSB�͕K���P\n
 *  - ������(return)\n
 *  MSB  .  .  ... b8  b7  b6  ...  .  LSB\n
 *   1   m  m  ...  m   0   0  ...  0   0
 */
INLINE unsigned long frexp_m(unsigned long f)
{
	return (0x80000000 | (f << 8));
}
/*!
 *  \brief  float�^�̃f�[�^����w������؂�o��
 *  \param  f  ���̃f�[�^�iI/F��unsigned long�������g��float�j
 *  \return  �؂�o�����w����
 *  \note   �߂�l�̒l���-126�`129\n
 *  - �w����\n
 *  MSB  ...  b1 LSB\n
 *   s   ...  n   n
 */
INLINE long frexp_e(unsigned long f)
{
	return ((long)((f << 1) >> 24) - 126);
}

#ifdef __H8__
/*!
 *  \brief  32b.+.32b+.32b����
 *  \param  *i     Q0����
 *  \param  *fpart Q32�Œ菬���_��
 *  \param  adder  Q32�Œ菬���_��
 *  \note   ���Z��̏�������fpart�ɓ���
 */
INLINE void __addc3( long *i, unsigned long *fpart, unsigned long adder)
{
/*   input  &iiii.       */
/*   input       .&fpart */
/* + input       .adder  */
/* --------------------- */
/*             +c        */
/*   output &iiii.       */
/*   return      .&fpart */

	if( *fpart + adder < *fpart ){
		*i = *i + 1;
	}
	*fpart = *fpart + adder;
}

/*!
 *  \brief  �Œ菬���_�p���K���֐�
 *  \param  m  �񐳋K���Œ菬���_���|�C���^
 *  \param  e  �w���l���i�[����|�C���^
 *
 * ����m��MSB��1�ƂȂ�悤�ɐ��K������\n
 * ����e�ɂ͐��K�������ۂ̎w�����Z�b�g����
 */
INLINE void __fixed_normalize( unsigned long *m, short *e )
{
	while( (long)*m > 0 ){		/* MSB = 0�Ȃ�J��Ԃ�		*/
		*e = *e - 1;			/* �w����-1����				*/
		*m = *m << 1;			/* �Œ菬���_�������V�t�g	*/
	}
}

/*!
 *  \brief  64b(32b.32b)�̘_�����P�r�b�g�V�t�g����
 *  \param  *i     Q0����
 *  \param  *fpart Q32�Œ菬���_��
 *  \note   fpart��MSB��i��LSB�ɓ���
 */
INLINE void __shll64( long *i, unsigned long *fpart)
{
/*   input  &iiii.       */
/*   input       .&fpart */
/* <<1                   */
/* --------------------- */
/*                       */
/*  output &iiiic.       */
/*  output       .&fpart */

	*i = (*i << 1) | (*fpart >> 31);
	*fpart <<= 1;
}
#endif		/* __H8__ */

#else	/* OPT_TYPE_SPEED */

#ifdef __H8__
/* 32b.+.32b+.32b���� */
void __addc3( long *i, unsigned long *fpart, unsigned long adder);

/* �Œ菬���_�p���K���֐� */
void __fixed_normalize( unsigned long *m, short *e );

/* 64b(32b.32b)�̘_�����P�r�b�g�V�t�g���� */
void __shll64( long *i, unsigned long *fpart);
#endif	/* __H8__ */

unsigned long __integer(short e, unsigned long u);
unsigned long __fraction(short e, unsigned long u);
unsigned long __frexp_m(unsigned long f);
short __frexp_e(unsigned long f);
#endif		/* OPT_TYPE_SPEED */

#ifdef __H8__
/* no inline */
/* 64b-96b */
unsigned long __sub96( unsigned long *dw_1i, unsigned long *dw_1f, unsigned long dw_2i, unsigned long dw_2f, unsigned long dw_2ff);
#endif	/* __H8__ */

#endif		/* _FFLIB_H_ */
