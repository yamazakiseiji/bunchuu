/**************************************************************************************************
 EEPROMアクセス制御

 更新日     更新者      更新内容
 ---------- ----------- ---------------------------------------------------------------------------
 2014.06.23 K.Hidaka    ・新規作成

**************************************************************************************************/
#ifndef _EEPROM_H_
#define _EEPROM_H_

/*=================================================================================================
  マクロ定義
=================================================================================================*/
#define PAGEMAX							128
#define SPI_TIMOUT						500
#define EEPROM_ADR_END					0xffff





// コマンド
#define CMD_WRITE_ENABLE                0x06
#define CMD_WRITE_DISABLE               0x04
#define CMD_READ_STATUS                 0x05
#define CMD_WRITE_STATUS                0x01
#define CMD_READ_ARRAY                  0x03
#define CMD_WRITE_ARRAY                 0x02

// ステータス
#define STS_MASK_BUSY                   0x01                        // 1:書き込み動作中


// ページステータス
#define SPI_SSL1		1
#define SPI_SSL2		2


// EEPROMのコマンド
#pragma pack 1
typedef struct {
    B                                cmd;                        // コマンド
    B                                adrh;                       // アドレス(上位８ビット)
    B                                adrl;                       // アドレス(下位８ビット)
} tEEPCMD;
#pragma unpack

// SPI送受信作業用データバッファ
#pragma pack 1
typedef union {
    B                                buff[PAGEMAX+sizeof(tEEPCMD)];
    struct {
        tEEPCMD                         cmd;                        // EEPROMコマンド
        B                            data[PAGEMAX];     // ページデータ
//        tPAGE                           page;                       // ページフォーマット
    } format;
} tEEPBUF;
#pragma unpack

/*=================================================================================================
  関数プロトタイプ宣言
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
