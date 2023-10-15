/******************************************************************************
* �r�g�V�Q�R�V �����h�^�n��`                                                 *
*                                                                             *
*  File name : SH7237.h                                                       *
*  All rights reserved.                                                       *
*                                                                             *
*                                                                             *
******************************************************************************/

#ifndef SH7237_H
#define SH7237_H

/* �������Ӄ��W���[���̃x�[�X�A�h���X */

#ifdef CPU_SH   /* SH�p�N���X�R���p�C���ŃR���p�C������Ă���ꍇ */
#define IO_BASE ((volatile UB *)0x00000000L)

#endif

/* �������Ӄ��W���[�����o�̓}�N�� */

UB sfr_in(UINT n);              /* �o�C�g�f�[�^�̓��� */
void sfr_out(UINT n, UB c);     /* �o�C�g�f�[�^�̏o�� */
UH sfr_inw(UINT n);             /* ���[�h�f�[�^�̓��� */
void sfr_outw(UINT n, UH c);    /* ���[�h�f�[�^�̏o�� */
UW sfr_inl(UINT n);             /* �����O���[�h�f�[�^�̓��� */
void sfr_outl(UINT n, UW c);    /* �����O���[�h�f�[�^�̏o�� */
void sfr_set(UINT n, UB c);     /* �o�C�g�f�[�^�̃r�b�g�Z�b�g */
void sfr_clr(UINT n, UB c);     /* �o�C�g�f�[�^�̃r�b�g�N���A */
void sfr_setw(UINT n, UH c);    /* ���[�h�f�[�^�̃r�b�g�Z�b�g */
void sfr_clrw(UINT n, UH c);    /* ���[�h�f�[�^�̃r�b�g�N���A */
void sfr_setl(UINT n, UW c);    /* �����O���[�h�f�[�^�̃r�b�g�Z�b�g */
void sfr_clrl(UINT n, UW c);    /* �����O���[�h�f�[�^�̃r�b�g�N���A */

#ifdef IO_BASE
#define sfr_in(n)       (*(IO_BASE+(n)))
#define sfr_out(n,c)    (*(IO_BASE+(n))=(c))
#define sfr_inw(n)      (*(volatile UH *)(IO_BASE+(n)))
#define sfr_outw(n,c)   (*(volatile UH *)(IO_BASE+(n))=(c))
#define sfr_inl(n)      (*(volatile UW *)(IO_BASE+(n)))
#define sfr_outl(n,c)   (*(volatile UW *)(IO_BASE+(n))=(c))
#define sfr_set(n,c)    (*(IO_BASE+(n))|=(c))
#define sfr_clr(n,c)    (*(IO_BASE+(n))&=~(c))
#define sfr_setw(n,c)   (*(volatile UH *)(IO_BASE+(n))|=(c))
#define sfr_clrw(n,c)   (*(volatile UH *)(IO_BASE+(n))&=~(c))
#define sfr_setl(n,c)   (*(volatile UW *)(IO_BASE+(n))|=(c))
#define sfr_clrl(n,c)   (*(volatile UW *)(IO_BASE+(n))&=~(c))
#endif



#define  CPG_FRQCR          0xFFFE0010  /* ���g�����䃌�W�X�^ */
#define  CPG_MCLKCR         0xFFFE0410  /* MTU�N���b�N���g�����䃌�W�X�^ */
#define  CPG_ACLKCR         0xFFFE0414  /* AD�N���b�N���g�����䃌�W�X�^ */
#define  CPG_OSCCR          0xFFFE001C  /* ���U��~���o���䃌�W�X�^ */
#define  INTC_ICR0          0xFFFE0800  /* ���荞�݃R���g���[�����W�X�^0 */
#define  INTC_ICR1          0xFFFE0802  /* ���荞�݃R���g���[�����W�X�^1 */
#define  INTC_IRQRR         0xFFFE0806  /* IRQ���荞�ݗv�����W�X�^ */
#define  INTC_IBCR          0xFFFE080C  /* �o���N�R���g���[�����W�X�^ */
#define  INTC_IBNR          0xFFFE080E  /* �o���N�ԍ����W�X�^ */
#define  INTC_IPR01         0xFFFE0818  /* ���荞�ݗD�惌�x���ݒ背�W�X�^01 */
#define  INTC_IPR02         0xFFFE081A  /* ���荞�ݗD�惌�x���ݒ背�W�X�^02 */
#define  INTC_IPR05         0xFFFE0820  /* ���荞�ݗD�惌�x���ݒ背�W�X�^05 */
#define  INTC_IPR06         0xFFFE0C00  /* ���荞�ݗD�惌�x���ݒ背�W�X�^06 */
#define  INTC_IPR07         0xFFFE0C02  /* ���荞�ݗD�惌�x���ݒ背�W�X�^07 */
#define  INTC_IPR08         0xFFFE0C04  /* ���荞�ݗD�惌�x���ݒ背�W�X�^08 */
#define  INTC_IPR09         0xFFFE0C06  /* ���荞�ݗD�惌�x���ݒ背�W�X�^09 */
#define  INTC_IPR10         0xFFFE0C08  /* ���荞�ݗD�惌�x���ݒ背�W�X�^10 */
#define  INTC_IPR11         0xFFFE0C0A  /* ���荞�ݗD�惌�x���ݒ背�W�X�^11 */
#define  INTC_IPR12         0xFFFE0C0C  /* ���荞�ݗD�惌�x���ݒ背�W�X�^12 */
#define  INTC_IPR13         0xFFFE0C0E  /* ���荞�ݗD�惌�x���ݒ背�W�X�^13 */
#define  INTC_IPR14         0xFFFE0C10  /* ���荞�ݗD�惌�x���ݒ背�W�X�^14 */
#define  INTC_IPR15         0xFFFE0C12  /* ���荞�ݗD�惌�x���ݒ背�W�X�^15 */
#define  INTC_IPR16         0xFFFE0C14  /* ���荞�ݗD�惌�x���ݒ背�W�X�^16 */
#define  INTC_IPR17         0xFFFE0C16  /* ���荞�ݗD�惌�x���ݒ背�W�X�^17 */
#define  INTC_IPR18         0xFFFE0C18  /* ���荞�ݗD�惌�x���ݒ背�W�X�^18 */
#define  UBC_BAR_0          0xFFFC0400  /* �u���[�N�A�h���X���W�X�^_0 */
#define  UBC_BAMR_0         0xFFFC0404  /* �u���[�N�A�h���X�}�X�N���W�X�^_0 */
#define  UBC_BBR_0          0xFFFC04A0  /* �u���[�N�o�X�T�C�N�����W�X�^_0 */
#define  UBC_BAR_1          0xFFFC0410  /* �u���[�N�A�h���X���W�X�^_1 */
#define  UBC_BAMR_1         0xFFFC0414  /* �u���[�N�A�h���X�}�X�N���W�X�^_1 */
#define  UBC_BBR_1          0xFFFC04B0  /* �u���[�N�o�X�T�C�N�����W�X�^_1 */
#define  UBC_BAR_2          0xFFFC0420  /* �u���[�N�A�h���X���W�X�^_2 */
#define  UBC_BAMR_2         0xFFFC0424  /* �u���[�N�A�h���X�}�X�N���W�X�^_2 */
#define  UBC_BBR_2          0xFFFC04A4  /* �u���[�N�o�X�T�C�N�����W�X�^_2 */
#define  UBC_BAR_3          0xFFFC0430  /* �u���[�N�A�h���X���W�X�^_3 */
#define  UBC_BAMR_3         0xFFFC0434  /* �u���[�N�A�h���X�}�X�N���W�X�^_3 */
#define  UBC_BBR_3          0xFFFC04B4  /* �u���[�N�o�X�T�C�N�����W�X�^_3 */
#define  UBC_BRCR           0xFFFC04C0  /* �u���[�N�R���g���[�����W�X�^ */
#define  DTC_DTCERA         0xFFFE6000  /* DTC�C�l�[�u�����W�X�^A */
#define  DTC_DTCERB         0xFFFE6002  /* DTC�C�l�[�u�����W�X�^B */
#define  DTC_DTCERC         0xFFFE6004  /* DTC�C�l�[�u�����W�X�^C */
#define  DTC_DTCERD         0xFFFE6006  /* DTC�C�l�[�u�����W�X�^D */
#define  DTC_DTCERE         0xFFFE6008  /* DTC�C�l�[�u�����W�X�^E */
#define  DTC_DTCCR          0xFFFE6010  /* DTC�R���g���[�����W�X�^ */
#define  DTC_DTCVBR         0xFFFE6014  /* DTC�x�N�^�x�[�X���W�X�^ */
#define  BSC_CMNCR          0xFFFC0000  /* ���ʃR���g���[�����W�X�^ */
#define  BSC_CS0BCR         0xFFFC0004  /* CS0��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS1BCR         0xFFFC0008  /* CS1��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS3BCR         0xFFFC0010  /* CS3��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS4BCR         0xFFFC0014  /* CS4��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS5BCR         0xFFFC0018  /* CS5��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS6BCR         0xFFFC001C  /* CS6��ԃo�X�R���g���[�����W�X�^ */
#define  BSC_CS0WCR         0xFFFC0028  /* CS0��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_CS1WCR         0xFFFC002C  /* CS1��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_CS3WCR         0xFFFC0034  /* CS3��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_CS4WCR         0xFFFC0038  /* CS4��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_CS5WCR         0xFFFC003C  /* CS5��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_CS6WCR         0xFFFC0040  /* CS6��ԃE�F�C�g�R���g���[�����W�X�^ */
#define  BSC_BSCEHR         0xFFFE3C1A  /* �o�X�@�\�g�����W�X�^ */
#define  DMAC_SAR_0         0xFFFE1000  /* DMA�\�[�X�A�h���X���W�X�^_0 */
#define  DMAC_DAR_0         0xFFFE1004  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_0 */
#define  DMAC_DMATCR_0      0xFFFE1008  /* DMA�g�����X�t�@�J�E���g���W�X�^_0 */
#define  DMAC_CHCR_0        0xFFFE100C  /* DMA�`���l���R���g���[�����W�X�^_0 */
#define  DMAC_RSAR_0        0xFFFE1100  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_0 */
#define  DMAC_RDAR_0        0xFFFE1104  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_0 */
#define  DMAC_RDMATCR_0     0xFFFE1108  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_0 */
#define  DMAC_SAR_1         0xFFFE1010  /* DMA�\�[�X�A�h���X���W�X�^_1 */
#define  DMAC_DAR_1         0xFFFE1014  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_1 */
#define  DMAC_DMATCR_1      0xFFFE1018  /* DMA�g�����X�t�@�J�E���g���W�X�^_1 */
#define  DMAC_CHCR_1        0xFFFE101C  /* DMA�`���l���R���g���[�����W�X�^_1 */
#define  DMAC_RSAR_1        0xFFFE1110  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_1 */
#define  DMAC_RDAR_1        0xFFFE1114  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_1 */
#define  DMAC_RDMATCR_1     0xFFFE1118  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_1 */
#define  DMAC_SAR_2         0xFFFE1020  /* DMA�\�[�X�A�h���X���W�X�^_2 */
#define  DMAC_DAR_2         0xFFFE1024  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_2 */
#define  DMAC_DMATCR_2      0xFFFE1028  /* DMA�g�����X�t�@�J�E���g���W�X�^_2 */
#define  DMAC_CHCR_2        0xFFFE102C  /* DMA�`���l���R���g���[�����W�X�^_2 */
#define  DMAC_RSAR_2        0xFFFE1120  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_2 */
#define  DMAC_RDAR_2        0xFFFE1124  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_2 */
#define  DMAC_RDMATCR_2     0xFFFE1128  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_2 */
#define  DMAC_SAR_3         0xFFFE1030  /* DMA�\�[�X�A�h���X���W�X�^_3 */
#define  DMAC_DAR_3         0xFFFE1034  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_3 */
#define  DMAC_DMATCR_3      0xFFFE1038  /* DMA�g�����X�t�@�J�E���g���W�X�^_3 */
#define  DMAC_CHCR_3        0xFFFE103C  /* DMA�`���l���R���g���[�����W�X�^_3 */
#define  DMAC_RSAR_3        0xFFFE1130  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_3 */
#define  DMAC_RDAR_3        0xFFFE1134  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_3 */
#define  DMAC_RDMATCR_3     0xFFFE1138  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_3 */
#define  DMAC_SAR_4         0xFFFE1040  /* DMA�\�[�X�A�h���X���W�X�^_4 */
#define  DMAC_DAR_4         0xFFFE1044  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_4 */
#define  DMAC_DMATCR_4      0xFFFE1048  /* DMA�g�����X�t�@�J�E���g���W�X�^_4 */
#define  DMAC_CHCR_4        0xFFFE104C  /* DMA�`���l���R���g���[�����W�X�^_4 */
#define  DMAC_RSAR_4        0xFFFE1140  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_4 */
#define  DMAC_RDAR_4        0xFFFE1144  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_4 */
#define  DMAC_RDMATCR_4     0xFFFE1148  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_4 */
#define  DMAC_SAR_5         0xFFFE1050  /* DMA�\�[�X�A�h���X���W�X�^_5 */
#define  DMAC_DAR_5         0xFFFE1054  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_5 */
#define  DMAC_DMATCR_5      0xFFFE1058  /* DMA�g�����X�t�@�J�E���g���W�X�^_5 */
#define  DMAC_CHCR_5        0xFFFE105C  /* DMA�`���l���R���g���[�����W�X�^_5 */
#define  DMAC_RSAR_5        0xFFFE1150  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_5 */
#define  DMAC_RDAR_5        0xFFFE1154  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_5 */
#define  DMAC_RDMATCR_5     0xFFFE1158  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_5 */
#define  DMAC_SAR_6         0xFFFE1060  /* DMA�\�[�X�A�h���X���W�X�^_6 */
#define  DMAC_DAR_6         0xFFFE1064  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_6 */
#define  DMAC_DMATCR_6      0xFFFE1068  /* DMA�g�����X�t�@�J�E���g���W�X�^_6 */
#define  DMAC_CHCR_6        0xFFFE106C  /* DMA�`���l���R���g���[�����W�X�^_6 */
#define  DMAC_RSAR_6        0xFFFE1160  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_6 */
#define  DMAC_RDAR_6        0xFFFE1164  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_6 */
#define  DMAC_RDMATCR_6     0xFFFE1168  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_6 */
#define  DMAC_SAR_7         0xFFFE1070  /* DMA�\�[�X�A�h���X���W�X�^_7 */
#define  DMAC_DAR_7         0xFFFE1074  /* DMA�f�X�e�B�l�[�V�����A�h���X���W�X�^_7 */
#define  DMAC_DMATCR_7      0xFFFE1078  /* DMA�g�����X�t�@�J�E���g���W�X�^_7 */
#define  DMAC_CHCR_7        0xFFFE107C  /* DMA�`���l���R���g���[�����W�X�^_7 */
#define  DMAC_RSAR_7        0xFFFE1170  /* DMA�����[�h�\�[�X�A�h���X���W�X�^_7 */
#define  DMAC_RDAR_7        0xFFFE1174  /* DMA�����[�h�f�X�e�B�l�[�V�����A�h���X���W�X�^_7 */
#define  DMAC_RDMATCR_7     0xFFFE1178  /* DMA�����[�h�g�����X�t�@�J�E���g���W�X�^_7 */
#define  DMAC_DMAOR         0xFFFE1200  /* DMA�I�y���[�V�������W�X�^ */
#define  DMAC_DMARS0        0xFFFE1300  /* DMA�g�����\�[�X�Z���N�^0 */
#define  DMAC_DMARS1        0xFFFE1304  /* DMA�g�����\�[�X�Z���N�^1 */
#define  DMAC_DMARS2        0xFFFE1308  /* DMA�g�����\�[�X�Z���N�^2 */
#define  DMAC_DMARS3        0xFFFE130C  /* DMA�g�����\�[�X�Z���N�^3 */
#define  MTU2_TCR_0         0xFFFE4300  /* �^�C�}�R���g���[�����W�X�^_0 */
#define  MTU2_TMDR_0        0xFFFE4301  /* �^�C�}���[�h���W�X�^_0 */
#define  MTU2_TIORH_0       0xFFFE4302  /* �^�C�}I/O�R���g���[�����W�X�^H_0 */
#define  MTU2_TIORL_0       0xFFFE4303  /* �^�C�}I/O�R���g���[�����W�X�^L_0 */
#define  MTU2_TIER_0        0xFFFE4304  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_0 */
#define  MTU2_TSR_0         0xFFFE4305  /* �^�C�}�X�e�[�^�X���W�X�^_0 */
#define  MTU2_TCNT_0        0xFFFE4306  /* �^�C�}�J�E���^_0 */
#define  MTU2_TGRA_0        0xFFFE4308  /* �^�C�}�W�F�l�������W�X�^A_0 */
#define  MTU2_TGRB_0        0xFFFE430A  /* �^�C�}�W�F�l�������W�X�^B_0 */
#define  MTU2_TGRC_0        0xFFFE430C  /* �^�C�}�W�F�l�������W�X�^C_0 */
#define  MTU2_TGRD_0        0xFFFE430E  /* �^�C�}�W�F�l�������W�X�^D_0 */
#define  MTU2_TGRE_0        0xFFFE4320  /* �^�C�}�W�F�l�������W�X�^E_0 */
#define  MTU2_TGRF_0        0xFFFE4322  /* �^�C�}�W�F�l�������W�X�^F_0 */
#define  MTU2_TIER2_0       0xFFFE4324  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^2_0 */
#define  MTU2_TSR2_0        0xFFFE4325  /* �^�C�}�X�e�[�^�X���W�X�^2_0 */
#define  MTU2_TBTM_0        0xFFFE4326  /* �^�C�}�o�b�t�@����]�����[�h���W�X�^_0 */
#define  MTU2_TCR_1         0xFFFE4380  /* �^�C�}�R���g���[�����W�X�^_1 */
#define  MTU2_TMDR_1        0xFFFE4381  /* �^�C�}���[�h���W�X�^_1 */
#define  MTU2_TIOR_1        0xFFFE4382  /* �^�C�}I/O�R���g���[�����W�X�^_1 */
#define  MTU2_TIER_1        0xFFFE4384  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_1 */
#define  MTU2_TSR_1         0xFFFE4385  /* �^�C�}�X�e�[�^�X���W�X�^_1 */
#define  MTU2_TCNT_1        0xFFFE4386  /* �^�C�}�J�E���^_1 */
#define  MTU2_TGRA_1        0xFFFE4388  /* �^�C�}�W�F�l�������W�X�^A_1 */
#define  MTU2_TGRB_1        0xFFFE438A  /* �^�C�}�W�F�l�������W�X�^B_1 */
#define  MTU2_TICCR         0xFFFE4390  /* �^�C�}�C���v�b�g�L���v�`���R���g���[�����W�X�^ */
#define  MTU2_TCR_2         0xFFFE4000  /* �^�C�}�R���g���[�����W�X�^_2 */
#define  MTU2_TMDR_2        0xFFFE4001  /* �^�C�}���[�h���W�X�^_2 */
#define  MTU2_TIOR_2        0xFFFE4002  /* �^�C�}I/O�R���g���[�����W�X�^_2 */
#define  MTU2_TIER_2        0xFFFE4004  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_2 */
#define  MTU2_TSR_2         0xFFFE4005  /* �^�C�}�X�e�[�^�X���W�X�^_2 */
#define  MTU2_TCNT_2        0xFFFE4006  /* �^�C�}�J�E���^_2 */
#define  MTU2_TGRA_2        0xFFFE4008  /* �^�C�}�W�F�l�������W�X�^A_2 */
#define  MTU2_TGRB_2        0xFFFE400A  /* �^�C�}�W�F�l�������W�X�^B_2 */
#define  MTU2_TCR_3         0xFFFE4200  /* �^�C�}�R���g���[�����W�X�^_3 */
#define  MTU2_TMDR_3        0xFFFE4202  /* �^�C�}���[�h���W�X�^_3 */
#define  MTU2_TIORH_3       0xFFFE4204  /* �^�C�}I/O�R���g���[�����W�X�^H_3 */
#define  MTU2_TIORL_3       0xFFFE4205  /* �^�C�}I/O�R���g���[�����W�X�^L_3 */
#define  MTU2_TIER_3        0xFFFE4208  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_3 */
#define  MTU2_TSR_3         0xFFFE422C  /* �^�C�}�X�e�[�^�X���W�X�^_3 */
#define  MTU2_TCNT_3        0xFFFE4210  /* �^�C�}�J�E���^_3 */
#define  MTU2_TGRA_3        0xFFFE4218  /* �^�C�}�W�F�l�������W�X�^A_3 */
#define  MTU2_TGRB_3        0xFFFE421A  /* �^�C�}�W�F�l�������W�X�^B_3 */
#define  MTU2_TGRC_3        0xFFFE4224  /* �^�C�}�W�F�l�������W�X�^C_3 */
#define  MTU2_TGRD_3        0xFFFE4226  /* �^�C�}�W�F�l�������W�X�^D_3 */
#define  MTU2_TBTM_3        0xFFFE4238  /* �^�C�}�o�b�t�@����]�����[�h���W�X�^_3 */
#define  MTU2_TCR_4         0xFFFE4201  /* �^�C�}�R���g���[�����W�X�^_4 */
#define  MTU2_TMDR_4        0xFFFE4203  /* �^�C�}���[�h���W�X�^_4 */
#define  MTU2_TIORH_4       0xFFFE4206  /* �^�C�}I/O�R���g���[�����W�X�^H_4 */
#define  MTU2_TIORL_4       0xFFFE4207  /* �^�C�}I/O�R���g���[�����W�X�^L_4 */
#define  MTU2_TIER_4        0xFFFE4209  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_4 */
#define  MTU2_TSR_4         0xFFFE422D  /* �^�C�}�X�e�[�^�X���W�X�^_4 */
#define  MTU2_TCNT_4        0xFFFE4212  /* �^�C�}�J�E���^_4 */
#define  MTU2_TGRA_4        0xFFFE421C  /* �^�C�}�W�F�l�������W�X�^A_4 */
#define  MTU2_TGRB_4        0xFFFE421E  /* �^�C�}�W�F�l�������W�X�^B_4 */
#define  MTU2_TGRC_4        0xFFFE4228  /* �^�C�}�W�F�l�������W�X�^C_4 */
#define  MTU2_TGRD_4        0xFFFE422A  /* �^�C�}�W�F�l�������W�X�^D_4 */
#define  MTU2_TBTM_4        0xFFFE4239  /* �^�C�}�o�b�t�@����]�����[�h���W�X�^_4 */
#define  MTU2_TADCR         0xFFFE4240  /* �^�C�}A/D�ϊ��J�n�v���R���g���[�����W�X�^ */
#define  MTU2_TADCORA_4     0xFFFE4244  /* �^�C�}A/D�ϊ��J�n�v�������ݒ背�W�X�^A_4 */
#define  MTU2_TADCORB_4     0xFFFE4246  /* �^�C�}A/D�ϊ��J�n�v�������ݒ背�W�X�^B_4 */
#define  MTU2_TADCOBRA_4    0xFFFE4248  /* �^�C�}A/D�ϊ��J�n�v�������ݒ�o�b�t�@���W�X�^A_4 */
#define  MTU2_TADCOBRB_4    0xFFFE424A  /* �^�C�}A/D�ϊ��J�n�v�������ݒ�o�b�t�@���W�X�^B_4 */
#define  MTU2_TCRU_5        0xFFFE4084  /* �^�C�}�R���g���[�����W�X�^U_5 */
#define  MTU2_TCRV_5        0xFFFE4094  /* �^�C�}�R���g���[�����W�X�^V_5 */
#define  MTU2_TCRW_5        0xFFFE40A4  /* �^�C�}�R���g���[�����W�X�^W_5 */
#define  MTU2_TIORU_5       0xFFFE4086  /* �^�C�}I/O�R���g���[�����W�X�^U_5 */
#define  MTU2_TIORV_5       0xFFFE4096  /* �^�C�}I/O�R���g���[�����W�X�^V_5 */
#define  MTU2_TIORW_5       0xFFFE40A6  /* �^�C�}I/O�R���g���[�����W�X�^W_5 */
#define  MTU2_TIER_5        0xFFFE40B2  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_5 */
#define  MTU2_TSR_5         0xFFFE40B0  /* �^�C�}�X�e�[�^�X���W�X�^_5 */
#define  MTU2_TSTR_5        0xFFFE40B4  /* �^�C�}�X�^�[�g���W�X�^_5 */
#define  MTU2_TCNTU_5       0xFFFE4080  /* �^�C�}�J�E���^U_5 */
#define  MTU2_TCNTV_5       0xFFFE4090  /* �^�C�}�J�E���^V_5 */
#define  MTU2_TCNTW_5       0xFFFE40A0  /* �^�C�}�J�E���^W_5 */
#define  MTU2_TGRU_5        0xFFFE4082  /* �^�C�}�W�F�l�������W�X�^U_5 */
#define  MTU2_TGRV_5        0xFFFE4092  /* �^�C�}�W�F�l�������W�X�^V_5 */
#define  MTU2_TGRW_5        0xFFFE40A2  /* �^�C�}�W�F�l�������W�X�^W_5 */
#define  MTU2_TCNTCMPCLR    0xFFFE40B6  /* �^�C�}�R���y�A�}�b�`�N���A���W�X�^ */
#define  MTU2_TSTR          0xFFFE4280  /* �^�C�}�X�^�[�g���W�X�^ */
#define  MTU2_TSYR          0xFFFE4281  /* �^�C�}�V���N�����W�X�^ */
#define  MTU2_TCSYSTR       0xFFFE4282  /* �^�C�}�J�E���^�V���N���X�^�[�g���W�X�^ */
#define  MTU2_TRWER         0xFFFE4284  /* �^�C�}���[�h���C�g�C�l�[�u�����W�X�^ */
#define  MTU2_TOER          0xFFFE420A  /* �^�C�}�A�E�g�v�b�g�}�X�^�C�l�[�u�����W�X�^ */
#define  MTU2_TOCR1         0xFFFE420E  /* �^�C�}�A�E�g�v�b�g�R���g���[�����W�X�^1 */
#define  MTU2_TOCR2         0xFFFE420F  /* �^�C�}�A�E�g�v�b�g�R���g���[�����W�X�^2 */
#define  MTU2_TGCR          0xFFFE420D  /* �^�C�}�Q�[�g�R���g���[�����W�X�^ */
#define  MTU2_TCDR          0xFFFE4214  /* �^�C�}�����f�[�^���W�X�^ */
#define  MTU2_TDDR          0xFFFE4216  /* �^�C�}�f�b�h�^�C���f�[�^���W�X�^ */
#define  MTU2_TCNTS         0xFFFE4220  /* �^�C�}�T�u�J�E���^ */
#define  MTU2_TCBR          0xFFFE4222  /* �^�C�}�����o�b�t�@���W�X�^ */
#define  MTU2_TITCR         0xFFFE4230  /* �^�C�}���荞�݊Ԉ����ݒ背�W�X�^ */
#define  MTU2_TITCNT        0xFFFE4231  /* �^�C�}���荞�݊Ԉ����񐔃J�E���^ */
#define  MTU2_TBTER         0xFFFE4232  /* �^�C�}�o�b�t�@�]���ݒ背�W�X�^ */
#define  MTU2_TDER          0xFFFE4234  /* �^�C�}�f�b�h�^�C���C�l�[�u�����W�X�^ */
#define  MTU2_TWCR          0xFFFE4260  /* �^�C�}�g�`�R���g���[�����W�X�^ */
#define  MTU2_TOLBR         0xFFFE4236  /* �^�C�}�A�E�g�v�b�g���x���o�b�t�@���W�X�^ */
#define  MTU2S_TCR_3S       0xFFFE4A00  /* �^�C�}�R���g���[�����W�X�^_3S */
#define  MTU2S_TMDR_3S      0xFFFE4A02  /* �^�C�}���[�h���W�X�^_3S */
#define  MTU2S_TIORH_3S     0xFFFE4A04  /* �^�C�}I/O�R���g���[�����W�X�^H_3S */
#define  MTU2S_TIORL_3S     0xFFFE4A05  /* �^�C�}I/O�R���g���[�����W�X�^L_3S */
#define  MTU2S_TIER_3S      0xFFFE4A08  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_3S */
#define  MTU2S_TSR_3S       0xFFFE4A2C  /* �^�C�}�X�e�[�^�X���W�X�^_3S */
#define  MTU2S_TCNT_3S      0xFFFE4A10  /* �^�C�}�J�E���^_3S */
#define  MTU2S_TGRA_3S      0xFFFE4A18  /* �^�C�}�W�F�l�������W�X�^A_3S */
#define  MTU2S_TGRB_3S      0xFFFE4A1A  /* �^�C�}�W�F�l�������W�X�^B_3S */
#define  MTU2S_TGRC_3S      0xFFFE4A24  /* �^�C�}�W�F�l�������W�X�^C_3S */
#define  MTU2S_TGRD_3S      0xFFFE4A26  /* �^�C�}�W�F�l�������W�X�^D_3S */
#define  MTU2S_TBTM_3S      0xFFFE4A38  /* �^�C�}�o�b�t�@����]�����[�h���W�X�^_3S */
#define  MTU2S_TCR_4S       0xFFFE4A01  /* �^�C�}�R���g���[�����W�X�^_4S */
#define  MTU2S_TMDR_4S      0xFFFE4A03  /* �^�C�}���[�h���W�X�^_4S */
#define  MTU2S_TIORH_4S     0xFFFE4A06  /* �^�C�}I/O�R���g���[�����W�X�^H_4S */
#define  MTU2S_TIORL_4S     0xFFFE4A07  /* �^�C�}I/O�R���g���[�����W�X�^L_4S */
#define  MTU2S_TIER_4S      0xFFFE4A09  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_4S */
#define  MTU2S_TSR_4S       0xFFFE4A2D  /* �^�C�}�X�e�[�^�X���W�X�^_4S */
#define  MTU2S_TCNT_4S      0xFFFE4A12  /* �^�C�}�J�E���^_4S */
#define  MTU2S_TGRA_4S      0xFFFE4A1C  /* �^�C�}�W�F�l�������W�X�^A_4S */
#define  MTU2S_TGRB_4S      0xFFFE4A1E  /* �^�C�}�W�F�l�������W�X�^B_4S */
#define  MTU2S_TGRC_4S      0xFFFE4A28  /* �^�C�}�W�F�l�������W�X�^C_4S */
#define  MTU2S_TGRD_4S      0xFFFE4A2A  /* �^�C�}�W�F�l�������W�X�^D_4S */
#define  MTU2S_TBTM_4S      0xFFFE4A39  /* �^�C�}�o�b�t�@����]�����[�h���W�X�^_4S */
#define  MTU2S_TADCRS       0xFFFE4A40  /* �^�C�}A/D�ϊ��J�n�v���R���g���[�����W�X�^S */
#define  MTU2S_TADCORA_4S   0xFFFE4A44  /* �^�C�}A/D�ϊ��J�n�v�������ݒ背�W�X�^A_4S */
#define  MTU2S_TADCORB_4S   0xFFFE4A46  /* �^�C�}A/D�ϊ��J�n�v�������ݒ背�W�X�^B_4S */
#define  MTU2S_TADCOBRA_4S  0xFFFE4A48  /* �^�C�}A/D�ϊ��J�n�v�������ݒ�o�b�t�@���W�X�^A_4S */
#define  MTU2S_TADCOBRB_4S  0xFFFE4A4A  /* �^�C�}A/D�ϊ��J�n�v�������ݒ�o�b�t�@���W�X�^B_4S */
#define  MTU2S_TCRU_5S      0xFFFE4884  /* �^�C�}�R���g���[�����W�X�^U_5S */
#define  MTU2S_TCRV_5S      0xFFFE4894  /* �^�C�}�R���g���[�����W�X�^V_5S */
#define  MTU2S_TCRW_5S      0xFFFE48A4  /* �^�C�}�R���g���[�����W�X�^W_5S */
#define  MTU2S_TIORU_5S     0xFFFE4886  /* �^�C�}I/O�R���g���[�����W�X�^U_5S */
#define  MTU2S_TIORV_5S     0xFFFE4896  /* �^�C�}I/O�R���g���[�����W�X�^V_5S */
#define  MTU2S_TIORW_5S     0xFFFE48A6  /* �^�C�}I/O�R���g���[�����W�X�^W_5S */
#define  MTU2S_TIER_5S      0xFFFE48B2  /* �^�C�}�C���^���v�g�C�l�[�u�����W�X�^_5S */
#define  MTU2S_TSR_5S       0xFFFE48B0  /* �^�C�}�X�e�[�^�X���W�X�^_5S */
#define  MTU2S_TSTR_5S      0xFFFE48B4  /* �^�C�}�X�^�[�g���W�X�^_5S */
#define  MTU2S_TCNTU_5S     0xFFFE4880  /* �^�C�}�J�E���^U_5S */
#define  MTU2S_TCNTV_5S     0xFFFE4890  /* �^�C�}�J�E���^V_5S */
#define  MTU2S_TCNTW_5S     0xFFFE48A0  /* �^�C�}�J�E���^W_5S */
#define  MTU2S_TGRU_5S      0xFFFE4882  /* �^�C�}�W�F�l�������W�X�^U_5S */
#define  MTU2S_TGRV_5S      0xFFFE4892  /* �^�C�}�W�F�l�������W�X�^V_5S */
#define  MTU2S_TGRW_5S      0xFFFE48A2  /* �^�C�}�W�F�l�������W�X�^W_5S */
#define  MTU2S_TCNTCMPCLRS  0xFFFE48B6  /* �^�C�}�R���y�A�}�b�`�N���A���W�X�^S */
#define  MTU2S_TSTRS        0xFFFE4A80  /* �^�C�}�X�^�[�g���W�X�^S */
#define  MTU2S_TSYRS        0xFFFE4A81  /* �^�C�}�V���N�����W�X�^S */
#define  MTU2S_TRWERS       0xFFFE4A84  /* �^�C�}���[�h���C�g�C�l�[�u�����W�X�^S */
#define  MTU2S_TOERS        0xFFFE4A0A  /* �^�C�}�A�E�g�v�b�g�}�X�^�C�l�[�u�����W�X�^S */
#define  MTU2S_TOCR1S       0xFFFE4A0E  /* �^�C�}�A�E�g�v�b�g�R���g���[�����W�X�^1S */
#define  MTU2S_TOCR2S       0xFFFE4A0F  /* �^�C�}�A�E�g�v�b�g�R���g���[�����W�X�^2S */
#define  MTU2S_TGCRS        0xFFFE4A0D  /* �^�C�}�Q�[�g�R���g���[�����W�X�^S */
#define  MTU2S_TCDRS        0xFFFE4A14  /* �^�C�}�����f�[�^���W�X�^S */
#define  MTU2S_TDDRS        0xFFFE4A16  /* �^�C�}�f�b�h�^�C���f�[�^���W�X�^S */
#define  MTU2S_TCNTSS       0xFFFE4A20  /* �^�C�}�T�u�J�E���^S */
#define  MTU2S_TCBRS        0xFFFE4A22  /* �^�C�}�����o�b�t�@���W�X�^S */
#define  MTU2S_TITCRS       0xFFFE4A30  /* �^�C�}���荞�݊Ԉ����ݒ背�W�X�^S */
#define  MTU2S_TITCNTS      0xFFFE4A31  /* �^�C�}���荞�݊Ԉ����񐔃J�E���^S */
#define  MTU2S_TBTERS       0xFFFE4A32  /* �^�C�}�o�b�t�@�]���ݒ背�W�X�^S */
#define  MTU2S_TDERS        0xFFFE4A34  /* �^�C�}�f�b�h�^�C���C�l�[�u�����W�X�^S */
#define  MTU2S_TSYCRS       0xFFFE4A50  /* �^�C�}�V���N���N���A���W�X�^S */
#define  MTU2S_TWCRS        0xFFFE4A60  /* �^�C�}�g�`�R���g���[�����W�X�^S */
#define  MTU2S_TOLBRS       0xFFFE4A36  /* �^�C�}�A�E�g�v�b�g���x���o�b�t�@���W�X�^S */
#define  POE2_ICSR1         0xFFFE5000  /* ���̓��x���R���g���[���^�X�e�[�^�X���W�X�^1 */
#define  POE2_OCSR1         0xFFFE5002  /* �o�̓��x���R���g���[���^�X�e�[�^�X���W�X�^1 */
#define  POE2_ICSR2         0xFFFE5004  /* ���̓��x���R���g���[���^�X�e�[�^�X���W�X�^2 */
#define  POE2_OCSR2         0xFFFE5006  /* �o�̓��x���R���g���[���^�X�e�[�^�X���W�X�^2 */
#define  POE2_ICSR3         0xFFFE5008  /* ���̓��x���R���g���[���^�X�e�[�^�X���W�X�^3 */
#define  POE2_SPOER         0xFFFE500A  /* �\�t�g�E�F�A�|�[�g�A�E�g�v�b�g�C�l�[�u�����W�X�^ */
#define  POE2_POECR1        0xFFFE500B  /* �|�[�g�A�E�g�v�b�g�C�l�[�u���R���g���[�����W�X�^1 */
#define  POE2_POECR2        0xFFFE500C  /* �|�[�g�A�E�g�v�b�g�C�l�[�u���R���g���[�����W�X�^2 */
#define  POE2_POECR3        0xFFFE500E  /* �|�[�g�A�E�g�v�b�g�C�l�[�u���R���g���[�����W�X�^3 */
#define  CMT_CMSTR          0xFFFEC000  /* �R���y�A�}�b�`�^�C�}�X�^�[�g���W�X�^ */
#define  CMT_CMCSR_0        0xFFFEC002  /* �R���y�A�}�b�`�^�C�}�R���g���[���^�X�e�[�^�X���W�X�^_0 */
#define  CMT_CMCNT_0        0xFFFEC004  /* �R���y�A�}�b�`�J�E���^_0 */
#define  CMT_CMCOR_0        0xFFFEC006  /* �R���y�A�}�b�`�R���X�^���g���W�X�^_0 */
#define  CMT_CMCSR_1        0xFFFEC008  /* �R���y�A�}�b�`�^�C�}�R���g���[���^�X�e�[�^�X���W�X�^_1 */
#define  CMT_CMCNT_1        0xFFFEC00A  /* �R���y�A�}�b�`�J�E���^_1 */
#define  CMT_CMCOR_1        0xFFFEC00C  /* �R���y�A�}�b�`�R���X�^���g���W�X�^_1 */
#define  WDT_WTCSR          0xFFFE0000  /* �E�H�b�`�h�b�O�^�C�}�R���g���[���^�X�e�[�^�X���W�X�^ */
#define  WDT_WTCNT          0xFFFE0002  /* �E�H�b�`�h�b�O�^�C�}�J�E���^ */
#define  WDT_WRCSR          0xFFFE0004  /* �E�H�b�`�h�b�O���Z�b�g�R���g���[���^�X�e�[�^�X���W�X�^ */
#define  SCI_SCSMR_0        0xFFFF8000  /* �V���A�����[�h���W�X�^_0 */
#define  SCI_SCBRR_0        0xFFFF8002  /* �r�b�g���[�g���W�X�^_0 */
#define  SCI_SCSCR_0        0xFFFF8004  /* �V���A���R���g���[�����W�X�^_0 */
#define  SCI_SCTDR_0        0xFFFF8006  /* �g�����X�~�b�g�f�[�^���W�X�^_0 */
#define  SCI_SCSSR_0        0xFFFF8008  /* �V���A���X�e�[�^�X���W�X�^_0 */
#define  SCI_SCRDR_0        0xFFFF800A  /* ���V�[�u�f�[�^���W�X�^_0 */
#define  SCI_SCSDCR_0       0xFFFF800C  /* �V���A���f�B���N�V�����R���g���[�����W�X�^_0 */
#define  SCI_SCSPTR_0       0xFFFF800E  /* �V���A���|�[�g���W�X�^_0 */
#define  SCI_SPMR_0         0xFFFF8014  /* �T���v�����O���[�h���W�X�^_0 */
#define  SCI_SCSMR_1        0xFFFF8800  /* �V���A�����[�h���W�X�^_1 */
#define  SCI_SCBRR_1        0xFFFF8802  /* �r�b�g���[�g���W�X�^_1 */
#define  SCI_SCSCR_1        0xFFFF8804  /* �V���A���R���g���[�����W�X�^_1 */
#define  SCI_SCTDR_1        0xFFFF8806  /* �g�����X�~�b�g�f�[�^���W�X�^_1 */
#define  SCI_SCSSR_1        0xFFFF8808  /* �V���A���X�e�[�^�X���W�X�^_1 */
#define  SCI_SCRDR_1        0xFFFF880A  /* ���V�[�u�f�[�^���W�X�^_1 */
#define  SCI_SCSDCR_1       0xFFFF880C  /* �V���A���f�B���N�V�����R���g���[�����W�X�^_1 */
#define  SCI_SCSPTR_1       0xFFFF880E  /* �V���A���|�[�g���W�X�^_1 */
#define  SCI_SPMR_1         0xFFFF8814  /* �T���v�����O���[�h���W�X�^_1 */
#define  SCI_SCSMR_2        0xFFFF9000  /* �V���A�����[�h���W�X�^_2 */
#define  SCI_SCBRR_2        0xFFFF9002  /* �r�b�g���[�g���W�X�^_2 */
#define  SCI_SCSCR_2        0xFFFF9004  /* �V���A���R���g���[�����W�X�^_2 */
#define  SCI_SCTDR_2        0xFFFF9006  /* �g�����X�~�b�g�f�[�^���W�X�^_2 */
#define  SCI_SCSSR_2        0xFFFF9008  /* �V���A���X�e�[�^�X���W�X�^_2 */
#define  SCI_SCRDR_2        0xFFFF900A  /* ���V�[�u�f�[�^���W�X�^_2 */
#define  SCI_SCSDCR_2       0xFFFF900C  /* �V���A���f�B���N�V�����R���g���[�����W�X�^_2 */
#define  SCI_SCSPTR_2       0xFFFF900E  /* �V���A���|�[�g���W�X�^_2 */
#define  SCI_SPMR_2         0xFFFF9014  /* �T���v�����O���[�h���W�X�^_2 */
#define  SCIF_SCSMR_3       0xFFFE9800  /* �V���A�����[�h���W�X�^_3 */
#define  SCIF_SCBRR_3       0xFFFE9804  /* �r�b�g���[�g���W�X�^_3 */
#define  SCIF_SCSCR_3       0xFFFE9808  /* �V���A���R���g���[�����W�X�^_3 */
#define  SCIF_SCFTDR_3      0xFFFE980C  /* �g�����X�~�b�gFIFO�f�[�^���W�X�^_3 */
#define  SCIF_SCFSR_3       0xFFFE9810  /* �V���A���X�e�[�^�X���W�X�^_3 */
#define  SCIF_SCFRDR_3      0xFFFE9814  /* ���V�[�uFIFO�f�[�^���W�X�^_3 */
#define  SCIF_SCFCR_3       0xFFFE9818  /* FIFO�R���g���[�����W�X�^_3 */
#define  SCIF_SCFDR_3       0xFFFE981C  /* FIFO�f�[�^�J�E���g�Z�b�g���W�X�^_3 */
#define  SCIF_SCSPTR_3      0xFFFE9820  /* �V���A���|�[�g���W�X�^_3 */
#define  SCIF_SCLSR_3       0xFFFE9824  /* ���C���X�e�[�^�X���W�X�^_3 */
#define  SCIF_SCSEMR_3      0xFFFE9900  /* �V���A���g�����[�h���W�X�^ */
#define  RSPI_SPCR          0xFFFFB000  /* RSPI���䃌�W�X�^ */
#define  RSPI_SSLP          0xFFFFB001  /* RSPI�X���[�u�Z���N�g�ɐ����W�X�^ */
#define  RSPI_SPPCR         0xFFFFB002  /* RSPI�[�q���䃌�W�X�^ */
#define  RSPI_SPSR          0xFFFFB003  /* RSPI�X�e�[�^�X���W�X�^ */
#define  RSPI_SPDR          0xFFFFB004  /* RSPI�f�[�^���W�X�^ */
#define  RSPI_SPSCR         0xFFFFB008  /* RSPI�V�[�P���X���䃌�W�X�^ */
#define  RSPI_SPSSR         0xFFFFB009  /* RSPI�V�[�P���X�X�e�[�^�X���W�X�^ */
#define  RSPI_SPBR          0xFFFFB00A  /* RSPI�r�b�g���[�g���W�X�^ */
#define  RSPI_SPDCR         0xFFFFB00B  /* RSPI�f�[�^�R���g���[�����W�X�^ */
#define  RSPI_SPCKD         0xFFFFB00C  /* RSPI�N���b�N�x�����W�X�^ */
#define  RSPI_SSLND         0xFFFFB00D  /* RSPI�X���[�u�Z���N�g�l�Q�[�g�x�����W�X�^ */
#define  RSPI_SPND          0xFFFFB00E  /* RSPI���A�N�Z�X�x�����W�X�^ */
#define  RSPI_SPCMD0        0xFFFFB010  /* RSPI�R�}���h���W�X�^0 */
#define  RSPI_SPCMD1        0xFFFFB012  /* RSPI�R�}���h���W�X�^1 */
#define  RSPI_SPCMD2        0xFFFFB014  /* RSPI�R�}���h���W�X�^2 */
#define  RSPI_SPCMD3        0xFFFFB016  /* RSPI�R�}���h���W�X�^3 */
#define  ADC_ADCR_0         0xFFFFE800  /* A/D�R���g���[�����W�X�^_0 */
#define  ADC_ADSR_0         0xFFFFE802  /* A/D�X�e�[�^�X���W�X�^_0 */
#define  ADC_ADSTRGR_0      0xFFFFE81C  /* A/D�J�n�g���K�I�����W�X�^_0 */
#define  ADC_ADANSR_0       0xFFFFE820  /* A/D�A�i���O���̓`���l���I�����W�X�^_0 */
#define  ADC_ADBYPSCR_0     0xFFFFE830  /* A/D�o�C�p�X�R���g���[�����W�X�^_0 */
#define  ADC_ADDR0          0xFFFFE840  /* A/D�f�[�^���W�X�^0 */
#define  ADC_ADDR1          0xFFFFE842  /* A/D�f�[�^���W�X�^1 */
#define  ADC_ADDR2          0xFFFFE844  /* A/D�f�[�^���W�X�^2 */
#define  ADC_ADDR3          0xFFFFE846  /* A/D�f�[�^���W�X�^3 */
#define  ADC_ADCR_1         0xFFFFEC00  /* A/D�R���g���[�����W�X�^_1 */
#define  ADC_ADSR_1         0xFFFFEC02  /* A/D�X�e�[�^�X���W�X�^_1 */
#define  ADC_ADSTRGR_1      0xFFFFEC1C  /* A/D�J�n�g���K�I�����W�X�^_1 */
#define  ADC_ADANSR_1       0xFFFFEC20  /* A/D�A�i���O���̓`���l���I�����W�X�^_1 */
#define  ADC_ADBYPSCR_1     0xFFFFEC30  /* A/D�o�C�p�X�R���g���[�����W�X�^_1 */
#define  ADC_ADDR4          0xFFFFEC40  /* A/D�f�[�^���W�X�^4 */
#define  ADC_ADDR5          0xFFFFEC42  /* A/D�f�[�^���W�X�^5 */
#define  ADC_ADDR6          0xFFFFEC44  /* A/D�f�[�^���W�X�^6 */
#define  ADC_ADDR7          0xFFFFEC46  /* A/D�f�[�^���W�X�^7 */
#define  ADC_ADCR_2         0xFFFFEE00  /* A/D�R���g���[�����W�X�^_2 */
#define  ADC_ADSR_2         0xFFFFEE02  /* A/D�X�e�[�^�X���W�X�^_2 */
#define  ADC_ADSTRGR_2      0xFFFFEE1C  /* A/D�J�n�g���K�I�����W�X�^_2 */
#define  ADC_ADANSR_2       0xFFFFEE20  /* A/D�A�i���O���̓`���l���I�����W�X�^_2 */
#define  ADC_ADBYPSCR_2     0xFFFFEE30  /* A/D�o�C�p�X�R���g���[�����W�X�^_2 */
#define  ADC_ADTSR_0        0xFFFFE930  /* A/D�g���K�Z���N�g���W�X�^_0 */
#define  ADC_ADTSR_1        0xFFFFED30  /* A/D�g���K�Z���N�g���W�X�^_1 */
#define  ADC_ADTSR_2        0xFFFFEF30  /* A/D�g���K�Z���N�g���W�X�^_2 */
#define  ADC_ADDR8          0xFFFFEE40  /* A/D�f�[�^���W�X�^8 */
#define  ADC_ADDR9          0xFFFFEE42  /* A/D�f�[�^���W�X�^9 */
#define  ADC_ADDR10         0xFFFFEE44  /* A/D�f�[�^���W�X�^10 */
#define  ADC_ADDR11         0xFFFFEE46  /* A/D�f�[�^���W�X�^11 */
#define  ADC_ADDR12         0xFFFFEE48  /* A/D�f�[�^���W�X�^12 */
#define  ADC_ADDR13         0xFFFFEE4A  /* A/D�f�[�^���W�X�^13 */
#define  ADC_ADDR14         0xFFFFEE4C  /* A/D�f�[�^���W�X�^14 */
#define  ADC_ADDR15         0xFFFFEE4E  /* A/D�f�[�^���W�X�^15 */
#define  ADC_ADDR0GR0_0     0xFFFFE932  /* A/D�O���[�v0�f�[�^0���W�X�^_0 */
#define  ADC_ADDR0GR0_1     0xFFFFED32  /* A/D�O���[�v0�f�[�^0���W�X�^_1 */
#define  ADC_ADDR0GR0_2     0xFFFFEF32  /* A/D�O���[�v0�f�[�^0���W�X�^_2 */
#define  ADC_ADDR2GR1_0     0xFFFFE934  /* A/D�O���[�v1�f�[�^2���W�X�^_0 */
#define  ADC_ADDR2GR1_1     0xFFFFED34  /* A/D�O���[�v1�f�[�^2���W�X�^_1 */
#define  ADC_ADDR2GR1_2     0xFFFFEF34  /* A/D�O���[�v1�f�[�^2���W�X�^_2ADDR2GR1_2 */
#define  PFC_PAIORH         0xFFFE3804  /* �|�[�gA�EIO���W�X�^H */
#define  PFC_PAIORL         0xFFFE3806  /* �|�[�gA�EIO���W�X�^L */
#define  PFC_PACRH1         0xFFFE380E  /* �|�[�gA�R���g���[�����W�X�^H1 */
#define  PFC_PACRL4         0xFFFE3810  /* �|�[�gA�R���g���[�����W�X�^L4 */
#define  PFC_PACRL3         0xFFFE3812  /* �|�[�gA�R���g���[�����W�X�^L3 */
#define  PFC_PACRL2         0xFFFE3814  /* �|�[�gA�R���g���[�����W�X�^L2 */
#define  PFC_PACRL1         0xFFFE3816  /* �|�[�gA�R���g���[�����W�X�^L1 */
#define  PFC_PAPCRH         0xFFFE3828  /* �|�[�gA�v���A�b�vMOS�R���g���[�����W�X�^H */
#define  PFC_PAPCRL         0xFFFE382A  /* �|�[�gA�v���A�b�vMOS�R���g���[�����W�X�^L */
#define  PFC_PBIORH         0xFFFE3884  /* �|�[�gB�EIO���W�X�^H */
#define  PFC_PBIORL         0xFFFE3886  /* �|�[�gB�EIO���W�X�^L */
#define  PFC_PBCRH2         0xFFFE388C  /* �|�[�gB�R���g���[�����W�X�^H2 */
#define  PFC_PBCRH1         0xFFFE388E  /* �|�[�gB�R���g���[�����W�X�^H1 */
#define  PFC_PBCRL2         0xFFFE3894  /* �|�[�gB�R���g���[�����W�X�^L2 */
#define  PFC_PBCRL1         0xFFFE3896  /* �|�[�gB�R���g���[�����W�X�^L1 */
#define  PFC_PBPCRH         0xFFFE38A8  /* �|�[�gB�v���A�b�vMOS�R���g���[�����W�X�^H */
#define  PFC_PBPCRL         0xFFFE38AA  /* �|�[�gB�v���A�b�vMOS�R���g���[�����W�X�^L */
#define  PFC_PCIORL         0xFFFE3906  /* �|�[�gC�EIO���W�X�^L */
#define  PFC_PCCRL4         0xFFFE3910  /* �|�[�gC�R���g���[�����W�X�^L4 */
#define  PFC_PCCRL3         0xFFFE3912  /* �|�[�gC�R���g���[�����W�X�^L3 */
#define  PFC_PCCRL2         0xFFFE3914  /* �|�[�gC�R���g���[�����W�X�^L2 */
#define  PFC_PCCRL1         0xFFFE3916  /* �|�[�gC�R���g���[�����W�X�^L1 */
#define  PFC_PCPCRL         0xFFFE392A  /* �|�[�gC�v���A�b�vMOS�R���g���[�����W�X�^L */
#define  PFC_PDIORL         0xFFFE3986  /* �|�[�gD�EIO���W�X�^L */
#define  PFC_PDCRL4         0xFFFE3990  /* �|�[�gD�R���g���[�����W�X�^L4 */
#define  PFC_PDCRL3         0xFFFE3992  /* �|�[�gD�R���g���[�����W�X�^L3 */
#define  PFC_PDCRL2         0xFFFE3994  /* �|�[�gD�R���g���[�����W�X�^L2 */
#define  PFC_PDCRL1         0xFFFE3996  /* �|�[�gD�R���g���[�����W�X�^L1 */
#define  PFC_PDPCRL         0xFFFE39AA  /* �|�[�gD�v���A�b�vMOS�R���g���[�����W�X�^L */
#define  PFC_PEIORL         0xFFFE3A06  /* �|�[�gE�EIO���W�X�^L */
#define  PFC_PECRL4         0xFFFE3A10  /* �|�[�gE�R���g���[�����W�X�^L4 */
#define  PFC_PECRL3         0xFFFE3A12  /* �|�[�gE�R���g���[�����W�X�^L3 */
#define  PFC_PECRL2         0xFFFE3A14  /* �|�[�gE�R���g���[�����W�X�^L2 */
#define  PFC_PECRL1         0xFFFE3A16  /* �|�[�gE�R���g���[�����W�X�^L1 */
#define  PFC_HCPCR          0xFFFE3A20  /* ��d���|�[�g�R���g���[�����W�X�^ */
#define  PFC_PEPCRL         0xFFFE3A2A  /* �|�[�gE�v���A�b�vMOS�R���g���[�����W�X�^L */
#define  PFC_PDACKCR        0xFFFE3A2C  /* DACK�o�̓^�C�~���O�R���g���[�����W�X�^ */

#define  PADRH              0xFFFE3800  /* �|�[�gA�f�[�^���W�X�^H */
#define  PADRL              0xFFFE3802  /* �|�[�gA�f�[�^���W�X�^L */
#define  PAPRH              0xFFFE381C  /* �|�[�gA�|�[�g���W�X�^H */
#define  PAPRL              0xFFFE381E  /* �|�[�gA�|�[�g���W�X�^L */
#define  PBDRH              0xFFFE3880  /* �|�[�gB�f�[�^���W�X�^H */
#define  PBDRL              0xFFFE3882  /* �|�[�gB�f�[�^���W�X�^L */
#define  PBPRH              0xFFFE389C  /* �|�[�gB�|�[�g���W�X�^H */
#define  PBPRL              0xFFFE389E  /* �|�[�gB�|�[�g���W�X�^L */
#define  PCDRL              0xFFFE3902  /* �|�[�gC�f�[�^���W�X�^L */
#define  PCPRL              0xFFFE391E  /* �|�[�gC�|�[�g���W�X�^L */
#define  PDDRL              0xFFFE3982  /* �|�[�gD�f�[�^���W�X�^L */
#define  PDPRL              0xFFFE399E  /* �|�[�gD�|�[�g���W�X�^L */
#define  PEDRL              0xFFFE3A02  /* �|�[�gE�f�[�^���W�X�^L */
#define  PEPRL              0xFFFE3A1E  /* �|�[�gE�|�[�g���W�X�^L */
#define  PFDRL              0xFFFE3A82  /* �|�[�gF�f�[�^���W�X�^L */
#define  FPMON_              0xFFFFA800  /* �t���b�V���[�q���j�^���W�X�^ */
#define  FMODR_              0xFFFFA802  /* �t���b�V�����[�h���W�X�^ */
#define  FASTAT_             0xFFFFA810  /* �t���b�V���A�N�Z�X�X�e�[�^�X���W�X�^ */
#define  FAEINT_             0xFFFFA811  /* �t���b�V���A�N�Z�X�G���[���荞�݋����W�X�^ */
#define  ROMMAT_             0xFFFFA820  /* ROM�}�b�g�I�����W�X�^ */
#define  FCURAME_            0xFFFFA854  /* FCURAM�C�l�[�u�����W�X�^ */
#define  FSTATR0_            0xFFFFA900  /* �t���b�V���X�e�[�^�X���W�X�^0 */
#define  FSTATR1_            0xFFFFA901  /* �t���b�V���X�e�[�^�X���W�X�^1 */
#define  FENTRYR_            0xFFFFA902  /* �t���b�V��P/E���[�h�G���g�����W�X�^ */
#define  FPROTR_             0xFFFFA904  /* �t���b�V���v���e�N�g���W�X�^ */
#define  FRESETR_            0xFFFFA906  /* �t���b�V�����Z�b�g���W�X�^ */
#define  FCMDR_              0xFFFFA90A  /* FCU�R�}���h���W�X�^ */
#define  FCPSR_              0xFFFFA918  /* FCU�����؂�ւ����W�X�^ */
#define  EEPBCCNT_           0xFFFFA91A  /* FLD�u�����N�`�F�b�N���䃌�W�X�^ */
#define  FPESTAT_            0xFFFFA91C  /* �t���b�V��P/E�X�e�[�^�X���W�X�^ */
#define  EEPBCSTAT_          0xFFFFA91E  /* FLD�u�����N�`�F�b�N�X�e�[�^�X���W�X�^ */
#define  PCKAR_              0xFFFFA938  /* ���ӃN���b�N�ʒm���W�X�^ */
#define  EEPRE0_             0xFFFFA840  /* FLD�ǂݏo�������W�X�^0 */
#define  EEPRE1_             0xFFFFA842  /* FLD�ǂݏo�������W�X�^1 */
#define  EEPWE0_             0xFFFFA850  /* FLD�������݁^���������W�X�^0 */
#define  EEPWE1_             0xFFFFA852  /* FLD�������݁^���������W�X�^1 */
#define  RCCR_               0xFFFC1400  /* ROM�L���b�V�����䃌�W�X�^ */
#define  STBCR              0xFFFE0014  /* �X�^���o�C�R���g���[�����W�X�^ */
#define  STBCR2             0xFFFE0018  /* �X�^���o�C�R���g���[�����W�X�^2 */
#define  SYSCR1             0xFFFE0402  /* �V�X�e���R���g���[�����W�X�^1 */
#define  SYSCR2             0xFFFE0404  /* �V�X�e���R���g���[�����W�X�^2 */
#define  STBCR3             0xFFFE0408  /* �X�^���o�C�R���g���[�����W�X�^3 */
#define  STBCR4             0xFFFE040C  /* �X�^���o�C�R���g���[�����W�X�^4 */
#define  STBCR5             0xFFFE0418  /* �X�^���o�C�R���g���[�����W�X�^5 */
#define  STBCR6             0xFFFE041C  /* �X�^���o�C�R���g���[�����W�X�^6 */
#define  SDIR_               0xFFFE2000  /* �C���g���N�V�������W�X�^ */


/* �����݃x�N�^�ԍ� */
#define  INT_NMI                  11             /* NMI */
#define  INT_UBC                  12             /* ���[�U�u���[�N */
#define  INT_H-UDI                14             /* H-UDI �u���[�N */
#define  INT_IRQ0                 64             /* �O���[�q IRQ0  */
#define  INT_IRQ1                 65             /* �O���[�q IRQ1  */
#define  INT_IRQ2                 66             /* �O���[�q IRQ2  */
#define  INT_IRQ3                 67             /* �O���[�q IRQ3  */
#define  INT_IRQ4                 68             /* �O���[�q IRQ4  */
#define  INT_IRQ5                 69             /* �O���[�q IRQ5  */
#define  INT_IRQ6                 70             /* �O���[�q IRQ6  */
#define  INT_FIFE                 91             /* �O���[�q IRQ7  */
#define  INT_ADC_ADI0             92
#define  INT_ADC_ADI1             96
#define  INT_ADC_ADI2             100
#define  INT_RCAN_ERS_0           104
#define  INT_RCAN_OVR_0           105
#define  INT_RCAN_RM              106
#define  INT_RCAN_SLE_0           107
#define  INT_DMAC_DEI0            108
#define  INT_DMAC_HEI0            109
#define  INT_DMAC_DEI1            112
#define  INT_DMAC_HEI1            113
#define  INT_DMAC_DEI2            116
#define  INT_DMAC_HEI2            117
#define  INT_DMAC_DEI3            120
#define  INT_DMAC_HEI3            121
#define  INT_DMAC_DEI4            124
#define  INT_DMAC_HEI4            125
#define  INT_DMAC_DEI5            128
#define  INT_DMAC_HEI5            129
#define  INT_DMAC_DEI6            132
#define  INT_DMAC_HEI6            133
#define  INT_DMAC_DEI7            136
#define  INT_DMAC_HEI7            137
#define  INT_CMI0                 140
#define  INT_CMI1                 144
#define  INT_WDT_ITI              152
#define  INT_MTU2_TGIA_0          156
#define  INT_MTU2_TGIB_0          157
#define  INT_MTU2_TGIC_0          158
#define  INT_MTU2_TGID_0          159
#define  INT_MTU2_TCIV_0          160
#define  INT_MTU2_TGIE_0          161
#define  INT_MTU2_TGIF_0          162
#define  INT_MTU2_TGIA_1          164
#define  INT_MTU2_TGIB_1          165
#define  INT_MTU2_TCIV_1          168
#define  INT_MTU2_TCIU_1          169
#define  INT_MTU2_TGIA_2          172
#define  INT_MTU2_TGIB_2          173
#define  INT_MTU2_TCIV_2          176
#define  INT_MTU2_TCIU_2          177
#define  INT_MTU2_TGIA_3          180
#define  INT_MTU2_TGIB_3          181
#define  INT_MTU2_TGIC_3          182
#define  INT_MTU2_TGID_3          183
#define  INT_MTU2_TCIV_3          184
#define  INT_MTU2_TGIA_4          188
#define  INT_MTU2_TGIB_4          189
#define  INT_MTU2_TGIC_4          190
#define  INT_MTU2_TGID_4          191
#define  INT_MTU2_TCIV_4          192
#define  INT_MTU2_TGIU_5          196
#define  INT_MTU2_TGIV_5          197
#define  INT_MTU2_TGIW_5          198
#define  INT_POE2_OEI1            200
#define  INT_POE2_OEI2            201
#define  INT_MTU2S_TGIA_3S        204
#define  INT_MTU2S_TGIB_3S        205
#define  INT_MTU2S_TGIC_3S        206
#define  INT_MTU2S_TGID_3S        207
#define  INT_MTU2S_TCIV_3S        208
#define  INT_MTU2S_TGIA_4S        212
#define  INT_MTU2S_TGIB_4S        213
#define  INT_MTU2S_TGIC_4S        214
#define  INT_MTU2S_TGID_4S        215
#define  INT_MTU2S_TCIV_4S        216
#define  INT_MTU2S_TGIU_5S        220
#define  INT_MTU2S_TGIV_5S        221
#define  INT_MTU2S_TGIW_5S        222
#define  INT_POE2_OEI3            224
#define  INT_RSPI_SPEI            233
#define  INT_RSPI_SPRI            234
#define  INT_RSPI_SPTI            235
#define  INT_ERI0                 240
#define  INT_RXI0                 241
#define  INT_TXI0                 242
#define  INT_TEI0                 243
#define  INT_ERI1                 244
#define  INT_RXI1                 245
#define  INT_TXI1                 246
#define  INT_TEI1                 247
#define  INT_ERI2                 248
#define  INT_RXI2                 249
#define  INT_TXI2                 250
#define  INT_TEI2                 251
#define  INT_BRI3                 252
#define  INT_ERI3                 253
#define  INT_RXI3                 254
#define  INT_TXI3                 255

#endif /* SH7237_H */
