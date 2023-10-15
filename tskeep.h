/**************************************************************************************************
 EEPROM�A�N�Z�X����

 �X�V��     �X�V��      �X�V���e
 ---------- ----------- ---------------------------------------------------------------------------
 2014.06.23 K.Hidaka    �E�V�K�쐬

**************************************************************************************************/
#ifndef _EEPROM_H_
#define _EEPROM_H_

/*=================================================================================================
  �}�N����`
=================================================================================================*/
#define PAGEMAX							128
#define SPI_TIMOUT						500
#define EEPROM_ADR_END					0xffff





// �R�}���h
#define CMD_WRITE_ENABLE                0x06
#define CMD_WRITE_DISABLE               0x04
#define CMD_READ_STATUS                 0x05
#define CMD_WRITE_STATUS                0x01
#define CMD_READ_ARRAY                  0x03
#define CMD_WRITE_ARRAY                 0x02

// �X�e�[�^�X
#define STS_MASK_BUSY                   0x01                        // 1:�������ݓ��쒆


// �y�[�W�X�e�[�^�X
#define SPI_SSL1		1
#define SPI_SSL2		2


// EEPROM�̃R�}���h
#pragma pack 1
typedef struct {
    B                                cmd;                        // �R�}���h
    B                                adrh;                       // �A�h���X(��ʂW�r�b�g)
    B                                adrl;                       // �A�h���X(���ʂW�r�b�g)
} tEEPCMD;
#pragma unpack

// SPI����M��Ɨp�f�[�^�o�b�t�@
#pragma pack 1
typedef union {
    B                                buff[PAGEMAX+sizeof(tEEPCMD)];
    struct {
        tEEPCMD                         cmd;                        // EEPROM�R�}���h
        B                            data[PAGEMAX];     // �y�[�W�f�[�^
//        tPAGE                           page;                       // �y�[�W�t�H�[�}�b�g
    } format;
} tEEPBUF;
#pragma unpack

/*=================================================================================================
  �֐��v���g�^�C�v�錾
=================================================================================================*/
extern TASK  TskEep( void );
extern ER	eep_GetAllInfo     ( UB *getdat  );
extern ER	eep_GetUserInfo    ( UB *getdat  );
extern ER	eep_GetSpeedData   ( UB *getdat  );
extern ER	eep_GetSpeedPattern( UB *getdat  );
extern ER	eep_GetPositionInfo( UB *getdat  );
extern ER	eep_GetProtocolData( UB *getdat,  UB index   );
extern ER	eep_GetProtocol    ( UB *getdat  );
extern ER	eep_GetOptionInfo  ( UB *getdat  );
extern void Rcvset_spddat( const char* str );
//extern void Rcvset_spdptn( const char* str );


#endif /* _EEPROM_H_ */
