/******************************************************************************
* �o�b�ʐM�n���h��(SIO0)                                                      *
*                                                                             *
* File name : hdrsio0.c                                                       *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "hdrsio0.h"
#include "datastruct.h"
#include "sub.h"

//static B dbgcnt;
//static B dbgbuf[32];

/*-----------------------------------------------------------------------------
  �@�\�F��M�G���[���荞�݃T�[�r�X���[�`��
 ----------------------------------------------------------------------------*/
void int_sio0_eri(void)
{
	T_COM_CHAR  dat;

	dat.byte.type = COM_MTYP_ERR;
	if( SCI0.SCSSR.BIT.ORER )			// �I�[�o�[�����G���[
	{
		dat.byte.type = COM_MTYP_ORN;
		SCI0.SCSSR.BIT.ORER = 0;
	}
	if( SCI0.SCSSR.BIT.FER )			// �t���[�~���O�G���[
	{
		dat.byte.type = COM_MTYP_FER;
		SCI0.SCSSR.BIT.FER = 0;
	}
	if( SCI0.SCSSR.BIT.PER )			// �p���e�B�[�G���[
	{
		dat.byte.type = COM_MTYP_PER;
		SCI0.SCSSR.BIT.PER = 0;
	}
	// �G���[���M
	dat.byte.data = SCI0.SCRDR;			// �f�[�^�͓ǂݎ̂�
	SndDtq( ID_DTQ_RXD, dat.data );

	SCI0.SCSSR.BIT.RDRF = 0;
}

/*-----------------------------------------------------------------------------
  �@�\�F��M�f�[�^�t�����荞�݃T�[�r�X���[�`��
 ----------------------------------------------------------------------------*/
void int_sio0_rxi(void)
{
    T_COM_CHAR  dat;

    dat.byte.type = COM_MTYP_CHR;
    dat.byte.data = SCI0.SCRDR;
	
//	if( dbgcnt > 32 )	dbgcnt = 0;
//	dbgbuf[dbgcnt] = dat.byte.data;
//	dbgcnt++;
    SndDtq( ID_DTQ_RXD, dat.data );

    SCI0.SCSSR.BIT.RDRF = 0;

}

