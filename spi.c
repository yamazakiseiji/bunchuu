/**************************************************************************************************
 SPI�ʐM���W���[��

 �X�V��     �X�V��      �X�V���e
 ---------- ----------- ---------------------------------------------------------------------------
 2014.06.23 K.Hidaka    �E�V�K�쐬


 ==================================================================================================


**************************************************************************************************/
#include <machine.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "iodefine.h"
#include "spi.h"
#include "tskeep.h"


/*=================================================================================================
  �}�N����`
=================================================================================================*/
#define WAIT_TIMEOUT                    100

#define SPI_SSL1		1
#define SPI_SSL2		2

/*=================================================================================================
  �\���̒�`
=================================================================================================*/


/*=================================================================================================
  ���[�J���ϐ���`
=================================================================================================*/


/*=================================================================================================
  ���[�J���֐��v���g�^�C�v�錾
=================================================================================================*/
static ER  select_ch( INT ch );
static ER  wait_send_busy(void);
static ER  wait_recv_busy(void);
static ER  send_bytes( const B* sbuff, INT slen );
static ER send_bytes_old( const B* sbuff, INT slen );
static ER  send_recv_bytes( const B* sbuff, INT slen, B* rbuff, INT rsize );
static ER send_recv_bytes_old( const B* sbuff, INT slen, B* rbuff, INT rsize );

/*=================================================================================================
  �O���[�o���֐�
=================================================================================================*/
/*-------------------------------------------------------------------------------------------------
  �@�\�FSPI�ʐM������
  �����F�Ȃ�
  �ߒl�F�Ȃ�
-------------------------------------------------------------------------------------------------*/
void spi_init( void )
{
#if 1
	// �����d�̓��[�h����������
	STB.CR5.BIT._RSPI = 0;						// ���W���[���X�g�b�v50 : RSPI�͓���
	
	// RSPI������������
    RSPI.SPPCR.BYTE = 0x30;                     // MOSI�A�C�h���Œ�l=1
    RSPI.SPSR.BYTE  = 0x22;                     // 
	RSPI.SPBR.BYTE = 3;							// �r�b�g���[�g : 5Mbps
	RSPI.SPCMD0.WORD = 0xE7A3;					// RSPI���A�N�Z�X�x���C�l�[�u�� : SPND�̐ݒ�l
	
	PFC.PACRL3.BIT.PA8MD = 5;					// PA8���[�h : MISO���o��  (RSPI)
	PFC.PACRL2.BIT.PA7MD = 5;					// PA7���[�h : MOSI���o��  (RSPI)
	PFC.PACRL2.BIT.PA6MD = 5;					// PA6���[�h : RSPCK���o�� (RSPI)
	PFC.PECRL3.BIT.PE8MD = 5;					// PE8���[�h : SSL2�o��	(RSPI)
	
	RSPI.SPCR.BIT.MSTR = 1;						// RSPI�}�X�^�^�X���[�u���[�h�I�� : �}�X�^���[�h
	RSPI.SPCR.BIT.SPE = 1;						// RSPI�@�\�C�l�[�u�� : RSPI�@�\��L��������

#else
	// RSPI
    RSPI.SPCR.BIT.SPE                   = 0;                        // RSPI�@�\����
    RSPI.SPCR.BIT.MSTR                  = 1;                        // �}�X�^�[���[�h
    RSPI.SPPCR.BYTE                   = 0x30;                     // MOSI�A�C�h���Œ�l=1
    RSPI.SPPCR.BYTE                    = 3;                        // �r�b�g���[�g:5Mbps�ɐݒ�(P��=50MHz)
    RSPI.SPCMD0.WORD                    = 0xE783;                   // MSB�t�@�[�X�g
                                                                    // �f�[�^��:8bit
                                                                    // �]���I�����SSL�M�����x����ێ�����
                                                                    // �r�b�g���[�g:�����Ȃ�
                                                                    // �A�C�h������RSPCK:1
                                                                    // ��G�b�W�Ńf�[�^�ω��A�����G�b�W�Ńf�[�^�T���v��
#endif
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^���M
  �����Fch          SPI�`���l���ԍ�
        sbuff       ���M�f�[�^
        slen        ���M�f�[�^�T�C�Y
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
ER spi_send( INT ch, B* sbuff, INT slen )
{
    ER     err;
    INT     imask;

    // ���荞�݋֎~
    etc_dis_int( CPU_INT_DISABLE, &imask );

    // SSL�A�T�[�g
    RSPI.SPCR.BIT.SPE = 1;

    RSPI.SPCMD0.BIT.BRDV    = 0;                // �r�b�g���[�g�F5Mbps
    RSPI.SPCMD0.BIT.SPB     = 7;                // �f�[�^���F8bit
    RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2�A�T�[�g

    // �f�[�^���M
    err = send_bytes( sbuff, slen );
    if( err ) goto lbl_ret;

    // ���M�o�b�t�@�G���v�e�B�҂�
    err = wait_send_busy();
    if( err ) goto lbl_ret;

lbl_ret:
    // SSL�l�Q�[�g
    RSPI.SPCR.BIT.SPE = 0;

    // ���荞�݋���
    etc_ena_int( imask );

    if( err )
    {
        return err + ch;
    }
    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^���M�Ǝ�M
  �����Fch          SPI�`���l���ԍ�
        sbuff       ���M�f�[�^
        slen        ���M�f�[�^�T�C�Y
        rbuff       ��M�f�[�^�o�b�t�@
        rsize       ��M�f�[�^�o�b�t�@�̃T�C�Y
        rlen        [out]��M�f�[�^��
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
ER spi_send_recv( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     imask;

    // ���荞�݋֎~
    etc_dis_int( CPU_INT_DISABLE, &imask );

    // SSL�A�T�[�g
    RSPI.SPCR.BIT.SPE = 1;

    RSPI.SPCMD0.BIT.BRDV    = 0;                // �r�b�g���[�g�F5Mbps
    RSPI.SPCMD0.BIT.SPB     = 7;                // �f�[�^���F8bit
    RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2�A�T�[�g
    err = send_recv_bytes( sbuff, slen, rbuff, rsize);
    if( err ) goto lbl_ret;

lbl_ret:
    // SSL�l�Q�[�g
    RSPI.SPCR.BIT.SPE = 0;

    // ���荞�݋���
    etc_ena_int( imask );

    if( err )
    {
        return err + SPI_SSL2;
    }
    return E_SUC;
}


/*=================================================================================================
  ���[�J���֐�
=================================================================================================*/
/*-------------------------------------------------------------------------------------------------
  �@�\�FSPI�`���l���̑I�����s��
  �����Fch          SPI�`���l���ԍ�
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER select_ch( INT ch )
{
	switch( ch )
    {
      case SPI_SSL1:
        RSPI.SPCMD0.BIT.BRDV    = 0;                // �r�b�g���[�g�F4Mbps
        RSPI.SPCMD0.BIT.SPB     = 7;                // �f�[�^���F8bit
#if(CPU==7237)
        RSPI.SPCMD0.BIT.SSLA    = 1;                // SSL1�A�T�[�g
#elif(CPU==7216)
        RSPI.SPCMD0.BIT.SSLA    = 0;                // SSL0�A�T�[�g
#endif
        break;
      case SPI_SSL2:
        RSPI.SPCMD0.BIT.BRDV    = 0;                // �r�b�g���[�g�F5Mbps
        RSPI.SPCMD0.BIT.SPB     = 7;                // �f�[�^���F8bit
        RSPI.SPCMD0.BIT.SSLA    = 2;                // SSL2�A�T�[�g
        break;
      default:
        break;
    }
    return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F���M�\��Ԃ܂ő҂�
  �����Ftmout   �^�C���A�E�g�l
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER wait_send_busy()
{
	INT	rty = 0;

	while( 0 == RSPI.SPSR.BIT.SPTEF )
	{
		rty++;
		if( rty > SPI_TIMOUT)
		{
			return E_EEP_ERR;
		}
		dly_tsk(DLY_10M);
	}

	return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F��M�\��Ԃ܂ő҂�
  �����Ftmout   �^�C���A�E�g�l
  �ߒl�F�G���[�R�[�h
-------------------------------------------------------------------------------------------------*/
static ER wait_recv_busy()
{
	INT	rty = 0;

	while( ( RSPI.SPSR.BYTE & 0x81 ) == 0x00 )
	{
		rty++;
		if( rty > SPI_TIMOUT)
		{
			return E_EEP_ERR;
		}
		dly_tsk(DLY_10M);
	}

	return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^���M
  �����Fsbuff       ���M�f�[�^
        slen        ���M�f�[�^�T�C�Y
  �ߒl�F�G���[�R�[�h
  ���ӁF���O��SSL�̃A�T�[�g�������o���Ă��邱�ƁB
-------------------------------------------------------------------------------------------------*/
static ER send_bytes( const B* sbuff, INT slen )
{
    INT     i;
    ER     err;
    UH    work;

    for( i = 0; i < slen; i++ )
    {
        // ���M�o�b�t�@�G���v�e�B�҂�
        err = wait_send_busy();
        if( err ) return err;
		
        // �f�[�^���M
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // �f�[�^��M(�ǂݎ̂�)
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }
    // ���M�o�b�t�@�G���v�e�B�҂�
    err = wait_send_busy();
    return err;
}
static ER send_bytes_old( const B* sbuff, INT slen )
{
    INT     i;
    ER     err;
    UH    work;

    for( i = 0; i < slen; i++ )
    {
        // ���M�o�b�t�@�G���v�e�B�҂�
        err = wait_send_busy();
        if( err ) return err;
		
        // �f�[�^���M
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // �f�[�^��M(�ǂݎ̂�)
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }
    // ���M�o�b�t�@�G���v�e�B�҂�
    err = wait_send_busy();
    return err;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^���M�Ǝ�M
  �����Fsbuff       ���M�f�[�^
        slen        ���M�f�[�^�T�C�Y
        rbuff       ��M�f�[�^�o�b�t�@
        rsize       ��M�f�[�^�o�b�t�@�̃T�C�Y
        rlen        [out]��M�f�[�^��
  �ߒl�F�G���[�R�[�h
  ���ӁF���O��SSL�̃A�T�[�g�������o���Ă��邱�ƁB
-------------------------------------------------------------------------------------------------*/
static ER send_recv_bytes( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     i;
    UH    work;

    // �f�[�^���M
    for( i = 0; i < slen; i++ )
    {
        // ���M�o�b�t�@�G���v�e�B�҂�
        err = wait_send_busy();
        if( err ) return err;

        // �f�[�^���M
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // ��M�f�[�^�ǂݎ̂�
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    // �f�[�^��M
    for( i = 0; i < rsize; i++ )
    {
        err = wait_send_busy();
        if( err ) return err;

        // �_�~�[�f�[�^���M
        RSPI.SPDR.WORD      = 0xff;
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // �I�[�o�����G���[�����H
        if( 1 == RSPI.SPSR.BIT.OVRF )
        {
            RSPI.SPSR.BIT.OVRF = 0;
            return E_EEP_ERR;
        }
        // ��M�f�[�^���[�h
        rbuff[i] = (B)RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    return E_SUC;
}
/*-------------------------------------------------------------------------------------------------
  �@�\�F�f�[�^���M�Ǝ�M
  �����Fsbuff       ���M�f�[�^
        slen        ���M�f�[�^�T�C�Y
        rbuff       ��M�f�[�^�o�b�t�@
        rsize       ��M�f�[�^�o�b�t�@�̃T�C�Y
        rlen        [out]��M�f�[�^��
  �ߒl�F�G���[�R�[�h
  ���ӁF���O��SSL�̃A�T�[�g�������o���Ă��邱�ƁB
-------------------------------------------------------------------------------------------------*/
static ER send_recv_bytes_old( const B* sbuff, INT slen, B* rbuff, INT rsize )
{
    ER     err;
    INT     i;
    UH    work;

    // �f�[�^���M
    for( i = 0; i < slen; i++ )
    {
        // ���M�o�b�t�@�G���v�e�B�҂�
        err = wait_send_busy();
        if( err ) return err;

        // �f�[�^���M
        RSPI.SPDR.WORD = (UH)(0x00ff & sbuff[i]);
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // ��M�f�[�^�ǂݎ̂�
        work = RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    // �f�[�^��M
    for( i = 0; i < rsize; i++ )
    {
        err = wait_send_busy();
        if( err ) return err;

        // �_�~�[�f�[�^���M
        RSPI.SPDR.WORD      = 0xff;
        RSPI.SPSR.BIT.SPTEF = 0;

        // ��M�����҂�
        err = wait_recv_busy( );
        if( err ) return err;

        // �I�[�o�����G���[�����H
        if( 1 == RSPI.SPSR.BIT.OVRF )
        {
            RSPI.SPSR.BIT.OVRF = 0;
            return E_EEP_ERR;
        }
        // ��M�f�[�^���[�h
        rbuff[i] = (B)RSPI.SPDR.WORD;
        RSPI.SPSR.BIT.SPRF = 0; 
    }

    return E_SUC;
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F���荞�݋֎~
  �����Fmask        ���荞�݋֎~�}�X�N
        imask       [out]���荞�݋֎~�O�̊��荞�݃}�X�N�l
  �ߒl�F�Ȃ�
-------------------------------------------------------------------------------------------------*/
void etc_dis_int( INT mask, INT* imask )
{
//    *imask = get_imask( );
//    set_imask( mask );
}

/*-------------------------------------------------------------------------------------------------
  �@�\�F���荞�݋���
  �����Fimask   ���荞�݃}�X�N�l(�O�F���荞�݋���)
  �ߒl�F�Ȃ�
-------------------------------------------------------------------------------------------------*/
void etc_ena_int( INT imask )
{
//    set_imask( imask );
}
