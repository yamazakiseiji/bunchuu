/********************************************************************************/
/* Copyright (C) 1992,2007 Renesas Technology Corp. and Renesas Solutions Corp. */
/* All rights reserved.                                                         */
/********************************************************************************/
/*! \file cosf.c
 *  \brief FFlib cosf�֐��t�@�C��
 */

#include "common.h"


/*!
 *  \brief  cosf�֐��i���C�u�����j
 *  \param  d0  ���͒l
 * �i����̎�Ԃ��Ȃ����߁Afloat�^��unsigned long�^�Ƃ��Ď󂯎���Ă���j
 *  \return  cosf�l
 * �i����̎�Ԃ��Ȃ����߁Afloat�^��unsigned long�^�Ƃ��ĕԂ��Ă���j
 *
 *<hr>
 *  \par ���������F
 */
unsigned long cosf(unsigned long d0)	/* FFLIB B-012 B-016 */
{	
	unsigned long m;			/* ���������_�̉�����(Q32�Œ菬���_��)	*/
	unsigned long fpart;		/* ���������_�̏�����					*/
	long e;						/* ���������_�̎w����					*/

	/*! ���������_�̎w�����Ɖ������̐؂�o��				*/
	/*! ���w�����̐؂�o�� frexp_e()						*/
	e = frexp_e( d0 );			/* -126�`e�`129				*/
	/*! ���������̐؂�o�� frexp_m()						*/
	m = frexp_m( d0 );			/* MSB=1��Q32�Œ菬���_��	*/

	/*! �w�����̃`�F�b�N */
	if( e <= 0 ){
		if( e < -16 ){
			/*! ���w������-16�����̏ꍇ�A�P��Ԃ��ucos(x)=1 �}x��0�v*/
			/* return 1.0f; */
			d0 = 0x3F800000;
		}else{
			/*! ���w�������O�ȉ��̏ꍇ�͒��ڑ������v�Z������ __poly_cosL() */
			/* �������̂��������_�ȉ��̐��l�����߂� */
			m >>= 1;					/* 0mmmmmmmmmmmmm							*/
			fpart = m >> (-e);			/* 0.mmmmmmmmmmmm(e=0) 0.00mmmmmmmmmm(e=-2)	*/
			/* 0�́ucos(x)=+X �}x��0�vcos(x)�������ɂȂ邽��						*/
			d0 = 0;
			d0 = __poly_cosL((long)fpart, e, (long)d0);	/* FFLIB B-011	*/
		}
	}else{
		/*! ���w�������P�ȏ�̏ꍇ�����֐����Ă� __insinL() */
		/*! �@(�ُ�l�`�F�b�N�͓����֐��ōs�Ȃ��Ă���)      */

		/* cos(x)�� n = 2 */
		d0 = __insinL( 2, m, e, d0 );								/* FFLIB B-011	*/
	}
	return (d0);						/* FFLIB B-016	*/
}
