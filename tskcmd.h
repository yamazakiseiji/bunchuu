/******************************************************************************
* �R�}���h���s�^�X�N                                                          *
*                                                                             *
* File name : tskcmd.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _CMDTASK_H_
#define _CMDTASK_H_

extern TASK TskCmd( void );
extern void AlmComR0Ack( VP_INT exinf );										// �R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��
extern void AlmCom2RspTmo( VP_INT exinf );										// �R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��
extern void AlmComR3Rsp( VP_INT exinf );										// �R�}���h���N�G�X�g��M�Ď��p�A���[���n���h��


// ����I���ʒm(��TskCmd)
//extern void CmdEndResponse	(
//						ID txkid,
//						UB bcid,
//						UB msgid,
//						UB inf1,
//						UB inf2,
//						ST_CNTLDATA* cntl
//					);
extern void CmdEndResponse	( 
						ID txkid, 
						UB procid, 
						UB bcid, 
						UB msgid, 
						UB inf1, 
						UB inf2, 
						ST_CNTLDATA* cntl
					);

extern void CmdSendResponse( ST_CNTLDATA* cntl );								// �R�}���h�����̑��M
extern void SendResponse( ST_CNTLDATA* cntl );									// ���X�|���X���M

extern void CmdMakeResponsePsns( UB msgid,
						const char* cmdchr, 
						INT result,         
						unsigned short d1,  
						unsigned short d2,  
						unsigned short d3,  
						unsigned short d4,  
						unsigned short d5,  
						unsigned short d6,  
						unsigned short d7,  
						unsigned short d8,  
						unsigned short d9,  
						unsigned short d10, 
						unsigned short d11, 
						unsigned short d12, 
						unsigned short d13 );
// �����f�[�^�쐬
extern void CmdMakeResponse( ST_CNTLDATA* cntl,
							UB anum,
							const char* p1,
							INT p2,
							INT p3,
							INT p4,
							INT p5,
							INT p6,
							INT p7,
							INT p8,
							INT p9,
							INT p10,
							INT p11,
							INT p12
						);

// �����f�[�^�쐬
extern void CmdMakeResponseX( ST_CNTLDATA* cntl,
							UB anum,
							const char* cmd,
							INT err,
							INT p2,
							INT p3,
							INT p4,
							INT p5,
							INT p6,
							INT p7,
							INT p8
						);
extern void CmdMakeResponseInt( ST_CNTLDATA* cntl,UB anum,const char* p1,INT p2,INT p3 );					// �����f�[�^�쐬(Interrupt�p)
extern void CmdMakeResponseCount(ST_CNTLDATA* cntl,UB anum,const char* p1,INT p2,INT p3,UINT p4 );			// �����f�[�^�쐬(���̓J�E���^�ǂݏo���p)
extern void CmdMakeResponseStr( ST_CNTLDATA* cntl,UB anum,const char* cmd,INT err,char* str2,char* str3 );	// 
extern BOOL GetPowerSts(UB pm);
extern void DebugProcStep( INT step );
extern INT  GetDebugProcStep();
extern void DebugProcStepInit();

extern void CmdSendResponse2( ST_CNTLDATA* cntl );
extern void SendResponse2( ST_CNTLDATA* cntl );

//extern void GetPsnsData( ST_PSNS *pdat );
extern INT GetPsnsData( unsigned char *pdat );

// extern void LOGOUT(ER result, INT par1, INT par2);
extern void CmdMakeResponseSns();
extern void RpMakeSend( UB parno, INT pardat );
extern void SetRcvData( unsigned char *buf, unsigned int len );
extern UB MentePcSendbuf[3400];


#endif  // _CMDTASK_H_
