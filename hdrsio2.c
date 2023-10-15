/******************************************************************************
* �V���A���ʐM�g���p�n���h��                                                  *
*                                                                             *
* File name : hdrsioEx.c                                                      *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#include <kernel.h>
#include "iodefine.h"
#include "define.h"
#include "hdrsioEx.h"
#include "datastruct.h"
#include "sub.h"


/*-----------------------------------------------------------------------------
  �@�\�F��M�G���[���荞�݃T�[�r�X���[�`��
 ----------------------------------------------------------------------------*/
void int_sio2_eri(INT portno)
{
	T_COM_CHAR  dat;

	dat.byte.type = COM_MTYP_ERR;

	if( portno == SIO_EX2 )
	{
		if( SCI2.SCSSR.BIT.ORER )			// �I�[�o�[�����G���[
		{
			dat.byte.type = COM_MTYP_ORN;
			SCI2.SCSSR.BIT.ORER = 0;
		}
		if( SCI2.SCSSR.BIT.FER )			// �t���[�~���O�G���[
		{
			dat.byte.type = COM_MTYP_FER;
			SCI2.SCSSR.BIT.FER = 0;
		}
		if( SCI2.SCSSR.BIT.PER )			// �p���e�B�[�G���[
		{
			dat.byte.type = COM_MTYP_PER;
			SCI2.SCSSR.BIT.PER = 0;
		}
		dat.byte.portno = portno;
		dat.byte.data = SCI2.SCRDR;
		SndDtq( ID_DTQ_RXEX, dat.data );

		SCI2.SCSSR.BIT.RDRF = 0;
	}
}

/*-----------------------------------------------------------------------------
  �@�\�F��M�f�[�^�t�����荞�݃T�[�r�X���[�`��
 ----------------------------------------------------------------------------*/
void int_sio2_rxi(INT portno)
{
	T_COM_CHAR  dat;

	if( portno == SIO_EX2 )
	{
		dat.byte.portno = portno;
		dat.byte.type = COM_MTYP_CHR;
		dat.byte.data = SCI2.SCRDR;
		SndDtq( ID_DTQ_RXEX, dat.data );
		SCI2.SCSSR.BIT.RDRF = 0;
	}
}
