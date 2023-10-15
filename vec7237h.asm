;****************************************************************************
;* SHCベクタテーブル設定（SH7237)                                           *
;*                                                                          *
;*  File name : vec7237h.asm                                                *
;*                                                                          *
;*                                                                          *
;*                                                                          *
;****************************************************************************

;* スタックポインタ初期値の指定は、STACK_ENDセクションで行っています。
;* リンカにて、STACK_ENDセクションを定義してください。
;* STACK_ENDセクションの開始アドレスが、SP初期値となります。
;  START VECT/00000000,P,C,D/00000600,R,B,STACK/0C000400,STACK_END/0D000000

        .section STACK_END,DATA     ; スタック領域最終アドレス+1
_INIT_STACK: .define "(STARTOF STACK_END)"

;* 外部参照

        .import __INIT              ; Cの初期設定ルーチン
        .import _ini_cpu            ; CPU内蔵周辺の初期化

;* リセットベクタ

        ; SDRAMへコードをローディングする場合、VECTセクションをSDRAM先頭に定義。
        ; ROM上のコードをそのまま動作させる場合、0番地に定義。

        .section VECT,CODE
        .export _RST_VECT
_RST_VECT:
        ; パワーオンリセット
        .data.l _RESET - (STARTOF VECT) ; PC初期値
        .data.l _INIT_STACK             ; SP初期値
        ; マニュアルリセット
        .data.l _RESET - (STARTOF VECT) ; PC初期値
        .data.l _INIT_STACK             ; SP初期値

;* 割込みベクタテーブル

        ; 割込みベクタを直接ROMに定義する場合は、アセンブル時にROMVECマクロを
        ; 定義し、割込みハンドラのシンボルをここに書き加えてください。
        ; def_inh や cre_isr システムコールを用いる場合、この定義は不要です。

      .aifdef ROMVEC
        .import _inthdr             ; 周期タイマ割込みハンドラ
        .import _int_sio0_er        ; シリアルチャネル0 受信エラー割込みハンドラ
        .import _int_sio0_rx        ; シリアルチャネル0 受信割込みハンドラ
        .import _int_irq4           ; シリアルチャネル0 受信割込みハンドラ
        .import _int_sio1_er        ; シリアルチャネル1 受信エラー割込みハンドラ
        .import _int_sio1_rx        ; シリアルチャネル1 受信割込みハンドラ
        .import _int_sio2_er        ; シリアルチャネル2 受信エラー割込みハンドラ
        .import _int_sio2_rx        ; シリアルチャネル2 受信割込みハンドラ
;5/11        .import _int_sio3_er        ; シリアルチャネル3 受信エラー割込みハンドラ
;5/11        .import _int_sio3_rx        ; シリアルチャネル3 受信割込みハンドラ

        .data.l -1                  ;   4: 一般不当命令
        .data.l -1                  ;   5: Reserved
        .data.l -1                  ;   6: スロット不当命令
        .data.l -1                  ;   7: Reserved
        .data.l -1                  ;   8: Reserved
        .data.l -1                  ;   9: CPUアドレスエラー
        .data.l -1                  ;  10: DMACアドレスエラー
        .data.l -1                  ;  11: NMI
        .data.l -1                  ;  12: UBC
        .data.l -1                  ;  13: Reserved
        .data.l -1                  ;  14: HUDI
        .data.l -1                  ;  15: バンクオーバフロー
        .data.l -1                  ;  16: バンクアンダフロー
        .data.l -1                  ;  17: 整数除算例外 (0除算)
        .data.l -1                  ;  18: 整数除算例外 (オーバフロー)
        .data.l -1                  ;  19: Reserved
        .data.l -1                  ;  20: Reserved
        .data.l -1                  ;  21: Reserved
        .data.l -1                  ;  22: Reserved
        .data.l -1                  ;  23: Reserved
        .data.l -1                  ;  24: Reserved
        .data.l -1                  ;  25: Reserved
        .data.l -1                  ;  26: Reserved
        .data.l -1                  ;  27: Reserved
        .data.l -1                  ;  28: Reserved
        .data.l -1                  ;  29: Reserved
        .data.l -1                  ;  30: Reserved
        .data.l -1                  ;  31: Reserved
        .data.l -1                  ;  32:
        .data.l -1                  ;  33:
        .data.l -1                  ;  34:
        .data.l -1                  ;  35:
        .data.l -1                  ;  36:
        .data.l -1                  ;  37:
        .data.l -1                  ;  38:
        .data.l -1                  ;  39:
        .data.l -1                  ;  40:
        .data.l -1                  ;  41:
        .data.l -1                  ;  42:
        .data.l -1                  ;  43:
        .data.l -1                  ;  44:
        .data.l -1                  ;  45:
        .data.l -1                  ;  46:
        .data.l -1                  ;  47:
        .data.l -1                  ;  48:
        .data.l -1                  ;  49:
        .data.l -1                  ;  50:
        .data.l -1                  ;  51:
        .data.l -1                  ;  52:
        .data.l -1                  ;  53:
        .data.l -1                  ;  54:
        .data.l -1                  ;  55:
        .data.l -1                  ;  56:
        .data.l -1                  ;  57:
        .data.l -1                  ;  58:
        .data.l -1                  ;  59:
        .data.l -1                  ;  60:
        .data.l -1                  ;  61:
        .data.l -1                  ;  62:
        .data.l -1                  ;  63:
        .data.l -1                  ;  64: IRQ0
        .data.l -1                  ;  65: IRQ1
        .data.l -1                  ;  66: IRQ2
        .data.l -1                  ;  67: IRQ3
        .data.l _int_irq4           ;  68: IRQ4
        .data.l -1                  ;  69: IRQ5
        .data.l -1                  ;  70: IRQ6
        .data.l -1                  ;  71: IRQ7
        .data.l -1                  ;  72:
        .data.l -1                  ;  73:
        .data.l -1                  ;  74:
        .data.l -1                  ;  75:
        .data.l -1                  ;  76:
        .data.l -1                  ;  77:
        .data.l -1                  ;  78:
        .data.l -1                  ;  79:
        .data.l -1                  ;  80:
        .data.l -1                  ;  81:
        .data.l -1                  ;  82:
        .data.l -1                  ;  83:
        .data.l -1                  ;  84:
        .data.l -1                  ;  85:
        .data.l -1                  ;  86:
        .data.l -1                  ;  87:
        .data.l -1                  ;  88:
        .data.l -1                  ;  89:
        .data.l -1                  ;  90:
        .data.l -1                  ;  91:
        .data.l -1                  ;  92: ADI0
        .data.l -1                  ;  93:
        .data.l -1                  ;  94:
        .data.l -1                  ;  95:
        .data.l -1                  ;  96: ADI1
        .data.l -1                  ;  97:
        .data.l -1                  ;  98:
        .data.l -1                  ;  99:
        .data.l -1                  ; 100: ADI2
        .data.l -1                  ; 101:
        .data.l -1                  ; 102:
        .data.l -1                  ; 103:
        .data.l -1                  ; 104: ERS_0
        .data.l -1                  ; 105: OVR_0
        .data.l -1                  ; 106: RM0_0、RM1_0
        .data.l -1                  ; 107: SLE_0
        .data.l -1                  ; 108: DEI0
        .data.l -1                  ; 109: HEI0
        .data.l -1                  ; 110:
        .data.l -1                  ; 111:
        .data.l -1                  ; 112: DEI1
        .data.l -1                  ; 113: HEI1
        .data.l -1                  ; 114:
        .data.l -1                  ; 115:
        .data.l -1                  ; 116: DEI2
        .data.l -1                  ; 117: HEI2
        .data.l -1                  ; 118:
        .data.l -1                  ; 119:
        .data.l -1                  ; 120: DEI3
        .data.l -1                  ; 121: HEI3
        .data.l -1                  ; 122:
        .data.l -1                  ; 123:
        .data.l -1                  ; 124: DEI4
        .data.l -1                  ; 125: HEI4
        .data.l -1                  ; 126:
        .data.l -1                  ; 127:
        .data.l -1                  ; 128: DEI5
        .data.l -1                  ; 129: HEI5
        .data.l -1                  ; 130:
        .data.l -1                  ; 131:
        .data.l -1                  ; 132: DEI6
        .data.l -1                  ; 133: HEI6
        .data.l -1                  ; 134:
        .data.l -1                  ; 135:
        .data.l -1                  ; 136: DEI7
        .data.l -1                  ; 137: HEI7
        .data.l -1                  ; 138:
        .data.l -1                  ; 139:
        .data.l _inthdr             ; 140: CMI0
        .data.l -1                  ; 141:
        .data.l -1                  ; 142:
        .data.l -1                  ; 143:
        .data.l -1                  ; 144: CMI1
        .data.l -1                  ; 145:
        .data.l -1                  ; 146:
        .data.l -1                  ; 147:
        .data.l -1                  ; 148: CMI
        .data.l -1                  ; 149:
        .data.l -1                  ; 150:
        .data.l -1                  ; 151:
        .data.l -1                  ; 152: ITI
        .data.l -1                  ; 153:
        .data.l -1                  ; 154: USBRXI
        .data.l -1                  ; 155: USBTXI
        .data.l -1                  ; 156: TGI0A
        .data.l -1                  ; 157: TGI0B
        .data.l -1                  ; 158: TGI0C
        .data.l -1                  ; 159: TGI0D
        .data.l -1                  ; 160: TCI0V
        .data.l -1                  ; 161: TGI0E
        .data.l -1                  ; 162: TGI0F
        .data.l -1                  ; 163:
        .data.l -1                  ; 164: TGI1A
        .data.l -1                  ; 165: TGI1B
        .data.l -1                  ; 166:
        .data.l -1                  ; 167:
        .data.l -1                  ; 168: TCI1V
        .data.l -1                  ; 169: TCI1U
        .data.l -1                  ; 170:
        .data.l -1                  ; 171:
        .data.l -1                  ; 172: TGI2A
        .data.l -1                  ; 173: TGI2B
        .data.l -1                  ; 174:
        .data.l -1                  ; 175:
        .data.l -1                  ; 176: TCI2V
        .data.l -1                  ; 177: TCI2U
        .data.l -1                  ; 178:
        .data.l -1                  ; 179:
        .data.l -1                  ; 180: TGI3A
        .data.l -1                  ; 181: TGI3B
        .data.l -1                  ; 182: TGI3C
        .data.l -1                  ; 183: TGI3D
        .data.l -1                  ; 184: TCI3V
        .data.l -1                  ; 185:
        .data.l -1                  ; 186:
        .data.l -1                  ; 187:
        .data.l -1                  ; 188: TGI4A
        .data.l -1                  ; 189: TGI4B
        .data.l -1                  ; 190: TGI4C
        .data.l -1                  ; 191: TGI4D
        .data.l -1                  ; 192: TCI4V
        .data.l -1                  ; 193:
        .data.l -1                  ; 194:
        .data.l -1                  ; 195:
        .data.l -1                  ; 196: TGI5U
        .data.l -1                  ; 197: TGI5V
        .data.l -1                  ; 198: TGI5W
        .data.l -1                  ; 199:
        .data.l -1                  ; 200: OEI1
        .data.l -1                  ; 201: OEI2
        .data.l -1                  ; 202:
        .data.l -1                  ; 203:
        .data.l -1                  ; 204: TGI3A
        .data.l -1                  ; 205: TGI3B
        .data.l -1                  ; 206: TGI3C
        .data.l -1                  ; 207: TGI3D
        .data.l -1                  ; 208: TCI3V
        .data.l -1                  ; 209:
        .data.l -1                  ; 210:
        .data.l -1                  ; 211:
        .data.l -1                  ; 212: TGI4A
        .data.l -1                  ; 213: TGI4B
        .data.l -1                  ; 214: TGI4C
        .data.l -1                  ; 215: TGI4D
        .data.l -1                  ; 216: TCI4V
        .data.l -1                  ; 217:
        .data.l -1                  ; 218:
        .data.l -1                  ; 219:
        .data.l -1                  ; 220: TGI5U
        .data.l -1                  ; 221: TGI5V
        .data.l -1                  ; 222: TGI5W
        .data.l -1                  ; 223:
        .data.l -1                  ; 224: OEI3
        .data.l -1                  ; 225:
        .data.l -1                  ; 226: USI0
        .data.l -1                  ; 227: USI1
        .data.l -1                  ; 228: STPI
        .data.l -1                  ; 229: NAKI
        .data.l -1                  ; 230: RXI
        .data.l -1                  ; 231: TXI
        .data.l -1                  ; 232: TEI
        .data.l -1                  ; 233: SSERI
        .data.l -1                  ; 234: SSRXI
        .data.l -1                  ; 235: SSTXI
        .data.l -1                  ; 236: ERI4
        .data.l -1                  ; 237: RXI4
        .data.l -1                  ; 238: TXI4
        .data.l -1                  ; 239: TEI4
        .data.l _int_sio0_er        ; 240: ERI0
        .data.l _int_sio0_rx        ; 241: RXI0
        .data.l -1                  ; 242: TXI0
        .data.l -1                  ; 243: TEI0
        .data.l _int_sio1_er        ; 244: ERI1
        .data.l _int_sio1_rx        ; 245: RXI1
        .data.l -1                  ; 246: TXI1
        .data.l -1                  ; 247: TEI1
        .data.l _int_sio2_er        ; 248: ERI2
        .data.l _int_sio2_rx        ; 249: RXI2
        .data.l -1                  ; 250: TXI2
        .data.l -1                  ; 251: TEI2
        .data.l -1                  ; 252: BRI3
        .data.l -1                  ; 253: ERI3
        .data.l -1                  ; 254: RXI3
;5/11        .data.l _int_sio3_er        ; 253: ERI3
;5/11        .data.l _int_sio3_rx        ; 254: RXI3
        .data.l -1                  ; 255: TXI3
      .aendi

;* リセット処理

        .export _RESET
        .org H'400
_RESET:
        ; リスタートを考慮して割込みマスクレベルを最高に
        stc     sr,r0
        or      #H'F0,r0
        ldc     r0,sr

        mov.l   ini_cpu,r2
        jsr     @r2
        nop

        ; リスタートを考慮してスタックポインタ初期化
        mov.l   INIT_STACK,sp

        ; Cの初期化ルーチンへ
        mov.l   INIT,r2
        jmp     @r2
        nop

;* 参照アドレス/データ値

        .align  4
INIT_STACK:     .data.l _INIT_STACK ; スタック領域終わりのアドレス
INIT:           .data.l __INIT
ini_cpu:        .data.l _ini_cpu

;* 各セクションのアドレス

        .section P,CODE,ALIGN=4
        .section D,DATA,ALIGN=4     ; 初期化データ領域(ROM)
        .section R,DATA,ALIGN=4     ; 初期化データ領域(RAM)
        .section B,DATA,ALIGN=4     ; 未初期化データ領域(RAM)
        .section STACK,DATA,ALIGN=4 ; スタック領域(RAM)
        .res.w  1
        .section C,DATA,ALIGN=4     ; 定数領域(ROM)
        .align  4
__P_BGN     .data.l (STARTOF P)
__P_END     .data.l (STARTOF P)+(SIZEOF P)
__C_BGN     .data.l (STARTOF C)
__C_END     .data.l (STARTOF C)+(SIZEOF C)
__D_ROM     .data.l (STARTOF D)
__D_BGN     .data.l (STARTOF R)
__D_END     .data.l (STARTOF R)+(SIZEOF R)
__B_BGN     .data.l (STARTOF B)
__B_END     .data.l (STARTOF B)+(SIZEOF B)
__VECT_BGN  .data.l (STARTOF VECT)
        .export __P_BGN
        .export __P_END
        .export __C_BGN
        .export __C_END
        .export __D_ROM
        .export __D_BGN
        .export __D_END
        .export __B_BGN
        .export __B_END
        .export __VECT_BGN

; [ROM上のコードで実行する場合]
;
; ROM先頭┌──────────┐
;        │ 割込みベクタ(VECT) │
;        ├──────────┤
;        │   プログラム領域   │
;        │        (P)         │
;        ├──────────┤
;        │      定数領域      │
;        │        (C)         │
;        ├──────────┤
;        │  初期化データ領域  │──┐
;        │        (D)         │    │_INITSCT() でコピー
;        ├──────────┤    ｜
;         ～                  ～     ｜
; RAM先頭┌──────────┐    ｜
;        │  初期化データ領域  │←─┘
;        │        (R)         │
;        ├──────────┤
;        │ 未初期化データ領域 │←─ _INITSCT()で0クリア
;        │        (B)         │
;        ├──────────┤
;        │    スタック領域    │
;        │      (STACK)       │
;         ～                  ～
;SP初期値└──────────┘STACK_END

        .end
