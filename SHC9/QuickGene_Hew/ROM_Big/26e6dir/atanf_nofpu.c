/********************************************************************************/
/* Copyright (C) 1992,2007 Renesas Technology Corp. and Renesas Solutions Corp. */
/* All rights reserved.                                                         */
/********************************************************************************/
/*! \file atanf.c
 *  \brief FFlib atanf�֐��t�@�C��
 */

#include "common.h"

/*! atanf�������W�� */
static const long coeffL[] = {
	0xFFCB8BA1,		/*	S31 * -0.001600787,	*/
	0x014832A3,		/*	S31 *  0.010015802,	*/
	0xFC3BE36D,		/*	S31 * -0.029422352,	*/
	0x072E21CD,		/*	S31 *  0.056095338,	*/
	0xF5646E59,		/*	S31 * -0.082872588,	*/
	0x0DF6B188,		/*	S31 *  0.109090988,	*/
	0xEDC0E0B3,		/*	S31 * -0.142551339,	*/
	0x1998CDB2,		/*	S31 *  0.199975693,	*/
	0xD5555B8E,		/*	S31 * -0.333332592,	*/
	0x6487ED5B,		/*	S31 *  (PI/2)*0.5	*/
};

/*! �������v�Z���̏�Z��` */
#define MUL(x,y) (dmuls_h(x,y)<<1)

/*--------------------------------------------------
 * �����֐��i�v���g�^�C�v�j
 *--------------------------------------------------*/
static long poly_atanL1(long d1, long d3, long e, long sign);			/* FFLIB B-011	*/
static long poly_atanL2(long d1, long d3, long e, long sign);			/* FFLIB B-011	*/
static unsigned long rev32(unsigned long m);							/* FFLIB B-011	*/

/*!
 *  \brief  atanf�W�� �������v�Z
 *  \param  d1   ���������_�̏�����
 *  \param  d3   ���������_�̉�����(Q32�Œ菬���_��)
 *  \param  e    �w��
 *  \param  sign ����
 *  \return  atanf�l�ilong�^�j
 */
static long poly_atanL1(long d1, long d3, long e, long sign)			/* FFLIB B-011	*/
{
	long ans, d2;
	const long *p;

	p = coeffL;
	d2 = dmulu_h(d1, d1) << 1;
	ans = *p++;

#ifdef OPT_TYPE_SIZE
	/* ROM�T�C�Y�D��I�v�V������I�������ꍇ�͑������v�Z�����[�v�ɂ��� */
	/* �������R���p�C���̍œK���I�v�V�����Ń��[�v�W�J��I�������ꍇ */
	/* �W�J�����\�������� */
	{
		unsigned short i;
		for( i = 8; i != 0; --i ){
			ans = *p++ + MUL(ans, d2);
		}
	}
#else
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
#endif
	ans =       MUL(ans, d2);
	ans = d3  + MUL(ans, d3);
	return __normalize(ans << 1, e, sign);								/* FFLIB B-011	*/
}

/*!
 *  \brief  atanf�W�� �������v�Z
 *  \param  d1   ���������_�̏�����
 *  \param  d3   ���������_�̉�����(Q32�Œ菬���_��)
 *  \param  e    �w��
 *  \param  sign ����
 *  \return  atanf�l�ilong�^�j
 */
static long poly_atanL2(long d1, long d3, long e, long sign) /* d3,e: for padding */	/* FFLIB B-011	*/
{
	long ans, d2;
	const long *p;

	p = coeffL;
	d2 = dmulu_h(d1 , d1) << 1;
	ans = *p++;

#ifdef OPT_TYPE_SIZE
	/* ROM�T�C�Y�D��I�v�V������I�������ꍇ�͑������v�Z�����[�v�ɂ��� */
	/* �������R���p�C���̍œK���I�v�V�����Ń��[�v�W�J��I�������ꍇ */
	/* �W�J�����\�������� */
	{
		unsigned short i;
		for( i = 8; i != 0; --i ){
			ans = *p++ + MUL(ans, d2);
		}
	}
#else
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
	ans = *p++ + MUL(ans, d2);
#endif
	ans =       MUL(ans, d2);
	ans = d1  + MUL(ans, d1);
	ans = *p - (ans >> 1);
	return __normalize(ans << 1, 1, sign);				/* FFLIB B-011	*/
}

static unsigned long rev32(unsigned long m)				/* FFLIB B-011	*/
{
	unsigned long t;
	unsigned long r;
	r = (unsigned long)0x80000000 / (m >> 16);
	r = (r - 2) << 16;
	t = -dmulu_h(r, m) << 1;
	r += dmulu_h(t, r);
	return r;
}

/*!
 *  \brief  atanf�֐��i���C�u�����j
 *  \param  d0  ���͒l
 * �i����̎�Ԃ��Ȃ����߁Afloat�^��unsigned long�^�Ƃ��Ď󂯎���Ă���j
 *  \return  atanf�l
 * �i����̎�Ԃ��Ȃ����߁Afloat�^��unsigned long�^�Ƃ��ĕԂ��Ă���j
 *
 *<hr>
 *  \par ���������F
 */
unsigned long atanf(unsigned long d0)	/* FFLIB B-012 B-016 */
{
	unsigned long m;			/* ���������_�̉�����(Q32�Œ菬���_��)	*/
	unsigned long fpart;		/* ���������_�̏�����					*/
	long e;						/* ���������_�̎w����					*/

	/*! ���������_�̎w�����Ɖ������̐؂�o��				*/
	/*! ���w�����̐؂�o�� frexp_e()						*/
	e = frexp_e(d0);			/* -126�`e�`129				*/
	/*! ���������̐؂�o�� frexp_m()						*/
	m = frexp_m(d0);			/* MSB=1��Q32�Œ菬���_��	*/

	/*! �w�����̃`�F�b�N */
	if( e <= 0 ){
		if( e < -11){
			/*! ���w������-11�����̏ꍇ�A�l�����̂܂ܕԂ�		*/
			return (d0);				/* FFLIB B-016	*/
		}
		/*! ���w������0�ȉ��̏ꍇ�͒��ڑ������v�Z������ poly_atanL1()	*/
		/* �������̂��������_�ȉ��̐��l�����߂�				*/
		m >>= 1;					/* 0mmmmmmmmmmmmm							*/
		fpart = m >> (-e);			/* 0.mmmmmmmmmmmm(e=0) 0.00mmmmmmmmmm(e=-2)	*/
		d0 = poly_atanL1(fpart, m, e, d0);
	}else{
		if( e > 31){
			/* NAN�Ή� */
			if( (e > 128) && ((m << 1) != 0) ){
				/*! ���}NAN�̎��́}NAN��Ԃ� */
				_errno = EDOM;		/* FFLIB B-015	*/
				if( (long)d0 < 0 ){
					/* -NAN */
					d0 = MNS_NAN;
				}else{
					/* +NAN */
					d0 = PLS_NAN;
				}
				/*! ���}���̎��͂��̂܂ܕԂ� */
				return (d0);			/* FFLIB B-016	*/
			}
			m = 0;
			fpart = 0;
		}else{
			e = 1 - e;
			m = rev32(m);
			if( m == 0 ){
				m--;					/* m = 0xFFFFFFFF; */
			}
			m >>= 1;
			fpart = m >> (-e);
		}
		/*! ���w������1�ȏ�̏ꍇ��poly_atanL2()���Ăяo�� */
		d0 = poly_atanL2(fpart, m, e, d0); 
	}
	return (d0);						/* FFLIB B-016	*/
}
