/*******************************************************************************
* �����݊Ǘ��@�\/�����^�C�}�����݃n���h�� (SH7286)                             *
*                                                                              *
*  File name : n4i7286.c                                                       *
*  Copyright (c) 2010, MiSPO Co., Ltd.                                         *
*                                                                              *
*  Compile : shc -cp=sh2a -op=1 -def=CLK=?,CH=?,MSEC=?,INTVEC=?,INTVSZ=?,      *
*                 IP=? n4i7286.c                                   (Renesas C) *
*                                                                              *
* 2010-10-29 Created                                                        SZ *
*                                                                              *
* 2017/09�`	�n���h���ǉ�                                                       *
* 			�D�揇�ʕύX                                                       *
* 			�^�C�}�l�ύX                                                       *
*                                                                              *
*******************************************************************************/

/*

�œK���R���p�C��

          ���荞�݃n���h���̃R���p�C���ɂ́A�K���A�œK���I�v�V������t���A
          ent_int �Ăяo���O�ɁA�s�v�Ȗ��߂���������Ȃ��悤���ӂ��Ă��������B
          ���l�T�XC�ł́A�����݃n���h���� #pragma interrupt �Œ�`���Ă��������B


�����ݎ����̎w��

          �����ݎ����́AMSEC �}�N���Ō��܂�܂��B �f�t�H���g�l�� kernel.h��
          ��`���Ă���A10 msec �ł��B
          �����ݎ�����ύX����ꍇ�́A�S�Ẵt�@�C���ɑ΂���MSEC�}�N����ύX
          ����悤�ɃR���p�C�����Ă��������B

          (��) shc <options> -def=MSEC=5 FILE �c 5 msec
                                  ~~~~~~

�����݃x�N�^�e�[�u��

          def_inh�V�X�e���R�[���ɂ�銄���݃x�N�^�̐ݒ���\�ɂ��邽�߁A
          RAM�擪(0C000000H�Ԓn����1024�o�C�g)�ɁA�����݃x�N�^�e�[�u���������
          �z�肵�Ă��܂��B
          �����݃x�N�^�e�[�u���̗̈悪�A���̗̈�Əd�Ȃ�Ȃ��l�Ƀ����N���Ă�
          �������B(�d�Ȃ��Ă������J�Ōx���͏o�܂���!)
          �����݃x�N�^�e�[�u���̐擪�A�h���X��INTVEC�}�N���ŕύX�ł��܂��B
          �����݃x�N�^�e�[�u���̃T�C�Y(�o�C�g��)�́AINTVSZ�}�N���ŕύX�ł��܂��B

          (��) shc <op> -def=INTVEC=0,INTVSZ=408 n4iXXX.c �c 0�Ԓn,408�o�C�g
                             ~~~~~~~~ ~~~~~~~~~~
          �����݃x�N�^�e�[�u����ROM��ɂ���Adef_inh�Ɉ˂炸���ڃx�N�^���`
          ����ꍇ�AINTVEC=0, INTVSZ=0�Ƃ��Ă��������B���̏ꍇ�Adef_inh�V�X�e
          ���R�[���͋@�\���܂���B


�`���l���ԍ��̎w��

          �����^�C�}�����݂Ɏg�p���Ă������CMT�̃`���l����CH�}�N���Ŏw���
          ���܂��B(0�`1, ���w�莞�́ACH=0)

          (��) shc <options> -def=CH=1 n4iXXX.c  .......... CMT CH1
                                  ~~~~

�N���b�N���g���̎w��

          �{�\�[�X�̃N���b�N(P��)�̃f�t�H���g��49.152 MHz �ł��B�ύX����ꍇ��
          CLK�}�N�����`���ăR���p�C�����Ă��������B�l��[Hz]�P�ʂ̎��g���ł��B

          (��) shc <options> -def=CLK=25000000 n4iXXX.c �c�c�c 25 MHz
                                  ~~~~~~~~~~~~

dis_int, ena_int

          SH�V���[�Y�̊����݃R���g���[���ł́A�ėp�I��dis_int, ena_int�V�X�e
          ���R�[���̎��������G�ƂȂ��Ă��܂��܂��B
          ���̂Q�̃V�X�e���R�[���͗p�ӂ��Ă��܂���̂ŁA�ʂ̊����݋֎~��
          ���s�������ꍇ�́A�e���䃌�W�X�^�𒼐ڃA�N�Z�X���Ă��������B


_kernel_inROM

          �^����ꂽ�A�h���X��ROM�̈悩�ۂ���Ԃ��֐��ł��BROM�̈�ł����
          TRUE(1), ROM�̈�łȂ���� FALSE(0) ��Ԃ��܂��B


�����ݗD�揇��

          �����^�C�}�����݃n���h���̃f�t�H���g�̊����ݗD�揇�ʂ́A7 �ł��B
          �����ύX����ꍇ�AIP �}�N�����`���ăR���p�C�����Ă��������B
          �������A�J�[�l����荂�D��ɂ͂ł��܂���B (IP �� KNL_LEVEL)
          �{�n���h����荂�D��x�̊����݃n���h�����L��ꍇ�A���d�����݂�
          �N����܂��B

          (��) shc <options> -def=IP=2 n4iXXX.c ... �����ݗD�揇�� 2
                                  ~~~~
*/

#include "kernel.h"
#include "nosys4.h"
#include "sh7237.h"
#include "iodefine.h"
#include "hdrsio0.h"
#include "define.h"

/* �����݃x�N�^�e�[�u�� */

#ifndef INTVSZ
#define INTVSZ      1024        /* �T�C�Y����`�̏ꍇ */
#endif

/* �����ݗD�揇�ʂ̒�` */

#ifndef IP
#define IP      7               /* �����ݗD�揇�ʖ���`�̏ꍇ */
#endif

/* �N���b�N�̒�` */

#ifndef CLK
#define CLK     40000000L     /* ���ӃN���b�NPck0[Hz] */
#endif

/* �`���l���ԍ��̒�` */

#ifndef CH
#define CH      0               /* �`���l���ԍ�����`�̏ꍇ */
#endif

/* CH�ɍ��킹��CMT���W�X�^�̍Ē�` */

#if (CH==0)
#define CMT_CMCSR   CMT_CMCSR_0
#define CMT_CMCNT   CMT_CMCNT_0
#define CMT_CMCOR   CMT_CMCOR_0
#define INT_CMT     INT_CMI0
#elif (CH==1)
#define CMT_CMCSR   CMT_CMCSR_1
#define CMT_CMCNT   CMT_CMCNT_1
#define CMT_CMCOR   CMT_CMCOR_1
#define INT_CMT     INT_CMI1
#else
#error illegal CH number! (CH = 0 to 1)
#endif

/* �����n�̍��ق̒��� */

#if defined(__HITACHI__)        /* Renesas C */
#include <machine.h>
extern int *_C_BGN, *_C_END, *_P_BGN, *_P_END, *_VECT_BGN;
#if (INTVSZ==0)
#pragma interrupt(inthdr(resbank))
#pragma ifunc(inthdr)
#pragma interrupt(int_sio0_rx(resbank))
#pragma ifunc(int_sio0_rx)				
#pragma interrupt(int_sio0_er(resbank))
#pragma ifunc(int_sio0_er)				

#pragma interrupt(int_sio1_rx(resbank))
#pragma ifunc(int_sio1_rx)				
#pragma interrupt(int_sio1_er(resbank))
#pragma ifunc(int_sio1_er)				

#pragma interrupt(int_sio2_rx(resbank))
#pragma ifunc(int_sio2_rx)				
#pragma interrupt(int_sio2_er(resbank))
#pragma ifunc(int_sio2_er)				

// 5/11 #pragma interrupt(int_sio3_rx(resbank))
// 5/11 #pragma ifunc(int_sio3_rx)				
// 5/11 #pragma interrupt(int_sio3_er(resbank))
// 5/11 #pragma ifunc(int_sio3_er)				

#endif

#elif defined(__ghs)            /* Green Hills */
#include <machine.h>
extern char __ghsbegin_rodata[], __ghsend_rodata[];
extern char __ghsbegin_text[], __ghsend_text[];
extern char __ghsbegin_vect[];
#endif

/* Tick���͐ώZ�J�E���^ */

#ifdef HOOK
UW TOTAL_TICKCK;
UW TICK_RATE;
#endif

/* �O���Q�� */

extern void _kernel_interr(void);
void init_timer(void);

/* �����݃T�[�r�X���[�`��������� */

#if (INTVSZ!=0)
void isritm(VP_INT exinf);
const T_CISR disr_isritm = { TA_HLNG, NULL, INT_CMT, (FP)isritm, 0 };
#else
void isritm(void);
#endif

#if 1
#define	CPU_CLOCK		160000000				// CPU�N���b�N	[Hz]
#define	PFAI_CLOCK		(CPU_CLOCK/4)			// P�ӃN���b�N	[Hz]
#define	CMT0_CLOCK			(PFAI_CLOCK/8)		// CMT0�J�E���g�N���b�N	[Hz]
#define	CMT0_COUNT_1MS		(CMT0_CYCLE_1000US*(CMT0_CLOCK/1000000))
												// CMT0�̐ݒ�����ɑ΂���J�E���g�l
#define	CMT0_CYCLE_1000US	1000				// CMT0�̊��荞�ݎ���	[us]
#endif

/*****************************************************************************
* �����ݏ�����
*
* �`��  ER intini(void);
*
* �ߒl  ��� E_OK
*
* ���  �J�[�l���̏��������[�`�� sysini ���犄���݋֎~��ԂŌĂяo����܂��B
******************************************************************************/


ER intini(void)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

  #if (INTVSZ!=0)
    int i;

    /* �����݃x�N�^�e�[�u���̏����� */

    for (i = INT_IRQ0; i < (INTVSZ) / 4; i++)
        intvec[i] = (UW)_kernel_interr;
  #endif

    /* �x�N�^�x�[�X���W�X�^��ݒ� */

    set_vbr((void **)intvec);

    /* �S�Ă̊����ݑ΂��ă��W�X�^�o���N��L���ɂ��� */

    sfr_outw(INTC_IBNR, 0x4000);

  #ifdef HOOK
    TOTAL_TICKCK = 0;
  #endif
    init_timer();

    return E_OK;
}

/*****************************************************************************
* �萔���ۂ����`�F�b�N����
*
* �`��  BOOL _kernel_inROM(const void *p);
*
*       p     �`�F�b�N����̈�ւ̃|�C���^
*
* �ߒl  TRUE  �R�[�h�̈�܂��͒萔�̈�
*       FALSE ��L�ȊO
*
* ���  �I�u�W�F�N�g������񓙂��萔���ϐ����𔻒肷�邽�߂ɁA�J�[�l���̊e��
*       �V�X�e���R�[������Ăяo����܂��B
******************************************************************************/

BOOL _kernel_inROM(const void *p)
{
#if defined(__HITACHI__)      /* Renesas C */
    return((((UW)p >= (UW)_P_BGN) && ((UW)p <= (UW)_P_END)) || (((UW)p >= (UW)_C_BGN) && ((UW)p <= (UW)_C_END)));

#elif defined(__ghs)            /* Green Hills */
    return((((UW)p >= (UW)__ghsbegin_text) && ((UW)p <= (UW)__ghsend_text)) || (((UW)p >= (UW)__ghsbegin_rodata) && ((UW)p <= (UW)__ghsend_rodata)));

#endif
}

/*****************************************************************************
* �����݃T�[�r�X���[�`���������̃`�F�b�N
*
* �`��  ER _kernel_check_cisr(const T_CISR *pk_cisr);
*
*       pk_cisr  �`�F�b�N���銄���݃T�[�r�X���[�`���������ւ̃|�C���^
*
* �ߒl  E_PAR �����ݔԍ��ƃ}�X�N���x���̂����ꂩ���͈͊O
*       E_OK  �����ݔԍ��ƃ}�X�N���x���͋��ɐ���
*
* ���  �����݃T�[�r�X���[�`���������̊����ݔԍ��ƃ}�X�N���x���͈̔͂�
*       �`�F�b�N���܂��B
*       cre_isr, acre_isr �V�X�e���R�[������Ăяo����܂��B
******************************************************************************/

ER _kernel_check_cisr(const T_CISR *pk_cisr)
{
  #if (INTVSZ!=0)
    if (pk_cisr->intno >= (INTVSZ) / 4)
        return E_PAR;
    return E_OK;
  #else
    return E_PAR;
  #endif
}

/*****************************************************************************
* �����݃n���h���̃A�h���X�ݒ�
*
* �`��  void _kernel_set_inthdr(FP hdr, const T_CISR *pk_cisr);
*
*       hdr       �����݃T�[�r�X���[�`�����Ăяo�������݃n���h���̃A�h���X
*       pk_cisr   �`�F�b�N���銄���݃T�[�r�X���[�`���������ւ̃|�C���^
*
* ���  �����݃T�[�r�X���[�`�����Ăяo�����߂ɁA�����Ő�������銄���݃n���h
*       ���̃A�h���X���A�����݃T�[�r�X���[�`���������̊����ݔԍ����Z�o��
*       �������݃x�N�^�֊i�[���܂��B
*       cre_isr, acre_isr �V�X�e���R�[������Ăяo����܂��B
******************************************************************************/

void _kernel_set_inthdr(FP hdr, const T_CISR *pk_cisr)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    intvec[pk_cisr->intno] = (UW)hdr;
}

/*****************************************************************************
* �����݃n���h���̃A�h���X�擾
*
* �`��  FP _kernel_get_inthdr(const T_CISR *pk_cisr);
*
*       pk_cisr  �`�F�b�N���銄���݃T�[�r�X���[�`���������ւ̃|�C���^
*
* �ߒl  ���Y�����ݔԍ��̊����݃n���h���A�h���X
*
* ���  �����݃T�[�r�X���[�`���������̊����ݔԍ����Z�o���������݃x�N�^��
*       ��A�����݃n���h���A�h���X��ǂݏo���܂��B
*       cre_isr, acre_isr �V�X�e���R�[������Ăяo����܂��B
******************************************************************************/

FP _kernel_get_inthdr(const T_CISR *pk_cisr)
{
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    return (FP)intvec[pk_cisr->intno];
}

/*****************************************************************************
* �����݃n���h�����`����
*
* �`��  ER def_inh(UINT inhno, const T_DINH *pk_dinh);
*
*       inhno    �����݃x�N�^�ԍ�
*       pk_dinh  �����݃n���h����`���
*
* �ߒl  �G���[�R�[�h
*
* ���  inhno �ɑΉ����銄���݃x�N�^�e�[�u���ɁAinthdr ��ݒ肵�܂��B
******************************************************************************/

ER v4_def_inh(INHNO inhno, const V4_T_DINH *pk_dinh)
{
  #if (INTVSZ==0)

    return E_NOSPT;

  #else

    UINT psw;
    FP fp;
  #ifndef INTVEC
   #if defined(__ghs)               /* Green Hills */
    UW *intvec = (UW *)__ghsbegin_vect;
   #elif defined(__HITACHI__)       /* Renesas C */
    UW *intvec = (UW *)_VECT_BGN;
   #endif
  #else
    UW *intvec = (UW *)INTVEC;
  #endif

    /* �����݃x�N�^�e�[�u���ւ̃|�C���^�Z�o */

    if (inhno >= (INTVSZ) / 4)
        return E_PAR;

    /* ��`�����̏ꍇ�A����`�����݃n���h����ݒ� */

    if (pk_dinh == (T_DINH *)NADR)
        fp = (FP)_kernel_interr;
    else
        fp = pk_dinh->inthdr;

    /* CPU�����݋֎~ */

    psw = vdis_psw();

    /* �V���������݃x�N�^�ݒ� */

    intvec[inhno] = (UW)fp;

    /* CPU�����݋֎~�߂� */

    vset_psw(psw);
    return E_OK;

  #endif
}

/*****************************************************************************
* �����^�C�}�����݋N�� (����CMT)
*
* �`��  ER intsta(void);
*
* �ߒl  �G���[�R�[�h
*
* �⑫  �����݋֎~��ԂŎ��s���Ă��������B
******************************************************************************/

static UW tc;
static UH cks;

void init_timer(void)
{
    UW prescale[] = { 8, 32, 128, 512 };
    /* ���萔���v�Z */

    for (cks = 0; cks <= 3; cks++) {
        tc = (((UW)(CLK) * MSEC)/((UW)prescale[cks] * 1000)) - 1;
        if (tc <= 0xffffL)
            break;
    }

  #ifdef HOOK
    TICK_RATE = 0;
  #endif
    if (cks > 3 || tc == 0L)
        return; /* �G���[ */

    /* �^�C�}���j�b�g������ */

    sfr_clr(STBCR4, 0x4);           /* CMT�ւ̃N���b�N���� */
    sfr_clrw(CMT_CMSTR, 0x01 << CH);/* �J�E���g��U��~ */
    sfr_outw(CMT_CMCOR, (UH)(-1));  /* ��r�l */
    sfr_outw(CMT_CMCSR, cks);       /* �N���b�N�I�� */
    sfr_outw(CMT_CMCNT, 0x00);      /* �J�E���^�N���A */
}

ER intsta(void)
{
    if (cks > 3 || tc == 0L)
        return E_PAR; /* �G���[ */

    /* �����ݒ�` */

  #if (INTVSZ!=0) /* ROM�ɒ�`����ꍇ�͕s�v */
    acre_isr(&disr_isritm);
  #endif

    /* �����ݗD�惌�x���ݒ� */

	/* �����L�A�R�����g�ɂ���� _RESET�J��Ԃ��͋N���Ȃ��B */
    sfr_setw(INTC_IPR08, (IP) << (12 - CH * 4));

	
    /* �^�C�}������ */

  #ifdef HOOK
    TICK_RATE = (UW)tc;
    TOTAL_TICKCK = ((sfr_inw(CMT_CMCNT)*MSEC*1000)/TICK_RATE);
  #endif

    sfr_clrw(CMT_CMSTR, 0x01 << CH);/* �J�E���g��U��~ */
    sfr_outw(CMT_CMCOR, (H)(tc));   /* ��r�l */
    sfr_outw(CMT_CMCNT, (H)(0));    /* �J�E���^�N���A */
    sfr_outw(CMT_CMCSR, 0x40|cks);  /* �R���y�A�}�b�`���荞�݂����� */
                                    /* �N���b�N�I�� */
	INTC.IPR08.BIT._CMT0 = 8;					// CMT0���荞�݃��x�� = 4
    sfr_setw(CMT_CMSTR, 0x01 << CH);/* �J�E���g����ăX�^�[�g */

    return E_OK;
}

/*****************************************************************************
* �����^�C�}�����T�[�r�X���[�`�� (���� CMT)
*
******************************************************************************/

#if (INTVSZ==0) /* ROM�ɒ�`����ꍇ�͊����݃n���h�� */
void isritm(void)
#else
void isritm(VP_INT exinf)
#endif
{
  #ifdef HOOK
    UINT psw;

    psw = vdis_psw();
    TOTAL_TICKCK += MSEC*1000;
  #endif
    sfr_clrw(CMT_CMCSR, 0x80);      /* CMF �R���y�A�}�b�`�t���O�N���A */
  #ifdef HOOK
    vset_psw(psw);
  #endif
    isig_tim();                     /* �V�X�e���N���b�N���� */
}

/*****************************************************************************
* �����^�C�}�����݃n���h�� (���� CMT)
*
******************************************************************************/

#if (INTVSZ==0)
INTHDR inthdr(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    isritm();                       /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
INTHDR int_sio0_rx(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio0_rxi();                 /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
INTHDR int_sio0_er(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio0_eri();                 /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}

INTHDR int_sio1_rx(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio_rxEx(SIO_EX1);          /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
INTHDR int_sio1_er(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio_erEx(SIO_EX1);          /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
INTHDR int_sio2_rx(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio_rxEx(SIO_EX2);          /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
INTHDR int_sio2_er(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    int_sio_erEx(SIO_EX2);          /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
// 5/11INTHDR int_sio3_rx(void)
// 5/11{
// 5/11    ent_int();                      /* �����݃n���h���̊J�n */
// 5/11    int_sio_rxEx(SIO_EX3);          /* �����T�[�r�X���[�`�����Ă� */
// 5/11    ret_int();                      /* �����݃n���h�����畜�A���� */
// 5/11}
// 5/11INTHDR int_sio3_er(void)
// 5/11{
// 5/11    ent_int();                      /* �����݃n���h���̊J�n */
// 5/11    int_sio_erEx(SIO_EX3);          /* �����T�[�r�X���[�`�����Ă� */
// 5/11    ret_int();                      /* �����݃n���h�����畜�A���� */
// 5/11}

INTHDR int_irq4(void)
{
    ent_int();                      /* �����݃n���h���̊J�n */
    IRQ4_ISR();                     /* �����T�[�r�X���[�`�����Ă� */
    ret_int();                      /* �����݃n���h�����畜�A���� */
}
#endif

/*****************************************************************************
* Tick���͐ώZ�l���Z�o (����CMT)
*
******************************************************************************/

#ifdef HOOK
#if (HOOK==1)
UW tickck_read(void)
#else
UW ctex_timer_read(void)
#endif
{
    UW ret, tmp;
    UH tmp_flg;

    if (TICK_RATE) {
        /* �����^�C�}�[�N���� */

        do {
            ret = *((volatile UW*)&TOTAL_TICKCK);
            tmp_flg = sfr_inw(CMT_CMCSR);
            tmp = sfr_inw(CMT_CMCNT);
        } while((tmp == TICK_RATE) || (ret != *((volatile UW*)&TOTAL_TICKCK)));
        /* �R���y�A�}�b�`���N�����Ă���Ƒz�肳���ꍇ�͍ēx�v�Z���Ȃ����B*/

        if (tmp_flg & 0x80) {
            tmp += TICK_RATE;
        }

        return (ret + ((tmp*MSEC*1000)/TICK_RATE));

    } else {
        /* �����^�C�}�[�N���O */
        return ((sfr_inw(CMT_CMCNT)*MSEC*1000)/tc);
    }
}

#else

UW tickck_read(void)
{
    return 0L;
}
#endif

/* end */
