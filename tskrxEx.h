/******************************************************************************
* �V���A���ʐM�g���p ��M�^�X�N�w�b�_                                         *
*                                                                             *
* File name : tskrx2.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _TSKRX2_H_
#define _TSKRX2_H_


// �v���g�^�C�v
extern TASK TskRxEx( void );

extern ER   sio2_init( INT act ) ;												// �V���A���|�[�g�Q������
extern void sio2_set( volatile struct st_sci* comm );							// �V���A���Q�ʐM�ݒ�
extern void AlmComR2Dat( VP_INT exinf );										// �R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��
extern BOOL GetPortActive( UB pno );											// �|�[�g��Ԏ擾



#endif  // _TSKRX2_H_
