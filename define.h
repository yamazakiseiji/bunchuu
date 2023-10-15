/******************************************************************************
* �萔��`                                                                    *
*                                                                             *
* File name : define.h                                                        *
*                                                                             *
* Copyright (c) 2018 NICHIRYO Co., Ltd.                                       *
*                                                                             *
******************************************************************************/
#ifndef _DEFEINE_H_
#define _DEFEINE_H_






//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �����t���R���p�C����`

#define EEP_RWSIZE_TYPE		1			// 0:1byte RW, 1:8byte RW
#define LOGSEND				1			// 


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// ���ʒ�`

// �Z�b�g�E�N���A
#define SET					1
#define CLR					0

#define ON					1
#define OFF					0

#define OK					0
#define NG					1


// ���[�^�쓮�����
#define DEFDAT				0xff
#define REQIDL				0x00		// 
#define REQRDY				0x01		// ����O����
#define REQACT				0x02		// ������s
#define REQEND				0xEE		// ����I��

// 
#define NO_DATA				0xff


// BIT���
#define BIT0				0x0001
#define BIT1				0x0002
#define BIT2				0x0004
#define BIT3				0x0008
#define BIT4				0x0010
#define BIT5				0x0020
#define BIT6				0x0040
#define BIT7				0x0080
#define BIT8				0x0100
#define BIT9				0x0200
#define BIT10				0x0400
#define BIT11				0x0800
#define BIT12				0x1000
#define BIT13				0x2000
#define BIT14				0x4000
#define BIT15				0x8000


// ���s��������
#define E_OK				0           // ���s����
#define E_SUC				0           // ���s����                         = PSTAT_OK����
#define E_NON				6           // �����Ȃ�

#define N_HEX				16
#define N_DEC				10

#define N_SIG				1
#define N_UNSIG				0

// Timeout
#define DLY_TMOUT			-1

#define PARAM0				0	// ��O�p�����[�^
#define PARAM1				1	// ��P�p�����[�^
#define PARAM2				2	// ��Q�p�����[�^
#define PARAM3				3	// ��R�p�����[�^
#define PARAM4				4	// ��S�p�����[�^
#define PARAM5				5	// ��T�p�����[�^
#define PARAM6				6	// ��U�p�����[�^
#define PARAM7				7	// ��V�p�����[�^
#define PARAM8				8	// ��W�p�����[�^
#define PARAM9				9	// ��X�p�����[�^
#define PARAM10				10	// ��P�O�p�����[�^
#define PARAM11				11	// ��P�P�p�����[�^
#define PARAM12				12	// ��P�Q�p�����[�^
#define PARAM13				13	// ��P�R�p�����[�^
#define PARAM14				14	// ��P�S�p�����[�^
#define PARAM15				15	// ��P�T�p�����[�^
#define PARAM16				16	// ��P�U�p�����[�^
#define PARAM17				17	// ��P�V�p�����[�^
#define PARAM18				18	// ��P�W�p�����[�^
#define PARAM19				19	// ��P�X�p�����[�^
#define PARAM20				20	// ��Q�O�p�����[�^
#define PARAM21				21	// ��Q�P�p�����[�^
#define PARAM22				22	// ��Q�Q�p�����[�^
#define PARAM23				23	// ��Q�R�p�����[�^
#define PARAM24				24	// ��Q�S�p�����[�^
#define PARAM25				25	// ��Q�T�p�����[�^
#define PARAM26				26	// ��Q�U�p�����[�^
#define PARAM27				27	// ��Q�V�p�����[�^
#define PARAM28				28	// ��Q�W�p�����[�^
#define PARAM29				29	// ��Q�X�p�����[�^


// IO�f�[�^��
#define INDAT_CNTRL_MAX		8
#define OTDAT_CNTRL_MAX		8

// �v���X�E�}�C�i�X
#define MINUS				1
#define PLUS				0

// �g�^�k
#define HIGH				1
#define LOW					0


#define POMPVALUE_BASE 1000
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �^�X�N�E�n���h���ݒ�

//*************************************
// �^�X�N�h�c
//*************************************
#define ID_TSK_RX0          1           // ����o�b�ʐM��M�^�X�N
#define ID_TSK_TX0          2           // ����o�b�ʐM���M�^�X�N
#define ID_TSK_RXEX         3           // �ʐM��M�^�X�N
#define ID_TSK_TXEX         4           // �ʐM���M�^�X�N
#define ID_TSK_CMD          5           // �R�}���h���s�^�X�N
#define ID_TSK_EEP          6           // EEPROM���s�^�X�N
#define ID_TSK_IO           7           // ���͊Ď��^�X�N
#define ID_TSK_MOTWAIT      8           // ���[�^����O�f�B���C�^�X�N
#define ID_TSK_MOTACT       9           // ���[�^���쐧��^�X�N
#define ID_TSK_MOTEND      10           // ���[�^����I���Ď��^�X�N
#define ID_TSK_INT         11           // INT�R�}���h�Ď��p�^�X�N
#define ID_TSK_LED         12           // 
#define ID_TSK_PUNIT       13
#define ID_TSK_TMPCNT      14
#define TSK_NUM            14           // �^�X�N��

//*************************************
// �n���h���h�c
//*************************************
#define ID_HDR_CYC          1           // �����N���n���h��
#define HDR_NUM             2           // �n���h���� @+1

//*************************************
// ���荞�݃T�[�r�X���[�`��
//*************************************
#define ID_ISR_RX0          1
#define ID_ISR_ER0          2
#define ID_ISR_RX2          3
#define ID_ISR_ER2          4

//*************************************
// �f�[�^�L���[
//*************************************
#define ID_DTQ_RXD          1           // SIO0 ��M�f�[�^�L���[
#define ID_DTQ_RXEX         2           // SIO�g�� ��M�f�[�^�L���[
#define DTQ_NUM             3           // 

//*************************************
// ���[��BOX
//*************************************
#define ID_MBX_RX0          1           // 
#define ID_MBX_TX0          2           // SIO0 ���M�f�[�^
#define ID_MBX_RXEX         3           // 
#define ID_MBX_TXEX         4           // SIO�g�� ���M�f�[�^
#define ID_MBX_CMD          5           // COM  �R�}���h
#define ID_MBX_EEP          6           // 
#define ID_MBX_IO           7           // �h�n�Ď�
#define ID_MBX_MOTWAIT      8           // 
#define ID_MBX_MOTACT       9           // ���[�^����
#define ID_MBX_MOTEND      10           // 
#define ID_MBX_INT         11           // INT�R�}���h�Ď�
#define ID_MBX_LED         12           // 
#define ID_MBX_PUNIT       13           // 
#define ID_MBX_TMPCNT      14           // 
#define MBX_NUM      	   14           // 

//*************************************
// �������v�[��
//*************************************
//#define ID_MPF_RS0          1           // ->SIO0
#define ID_MPF_TX0          2           // SIO0 ���M�f�[�^
#define ID_MPF_TXEX         3           // ->SIO2
#define ID_MPF_CMD          4           // 
#define ID_MPF_EEP          5           // 
#define ID_MPF_IO           6           // 
#define ID_MPF_MOTWAIT      7           // 
#define ID_MPF_MOTACT       8           // 
#define ID_MPF_MOTEND       9           // 
#define ID_MPF_INT          10          // 
#define ID_MPF_LED          11          // 
#define ID_MPF_PUNIT        12          // 
#define ID_MPF_TMPCNT       13          // 
#define MPF_NUM     	    13          // �v�[����

//*************************************
// �Z�}�t�H
//*************************************
#define ID_SEM_INIT0		1 			// C-INIT0
#define ID_SEM_PAUSE		2 			// C-PAUSE@
#define ID_SEM_HDMOV		3 			// C-HDMOV
#define ID_SEM_TPGET		4 			// C-TPGET
#define ID_SEM_TPRLS		5 			// C-TPRLS
#define ID_SEM_DROPN		6 			// C-DROPN
#define ID_SEM_SKSET		7 			// C-SKSET
#define ID_SEM_SUCK0		8 			// C-SUCK0
#define ID_SEM_DTSET		9 			// C-DTSET
#define ID_SEM_DOT00		10			// C-DOT00
#define ID_SEM_MXSET		11			// C-MXSET
#define ID_SEM_MIX00		12			// C-MIX00
#define ID_SEM_THMOV		13			// C-THMOV
#define ID_SEM_CPOPN		14			// C-CPOPN
#define ID_SEM_CPCLS		15			// C-CPCLS
#define ID_SEM_BCDSP		16			// C-BCDSP
#define ID_SEM_TURN1		17			// C-TURN1
#define ID_SEM_BZCNT		18			// C-BZCNT
#define ID_SEM_LEDON		19			// C-LEDON
#define ID_SEM_MEMRW		20			// C-MEMRW
#define SEM_NUM       (ID_SEM_MEMRW+1)  // �Z�}�t�H��

//*************************************
// �A���[���n���h��
//*************************************
#define ID_ALM_R0DAT        1           // SIO0 �f�[�^��M�^�C���A�E�g����
#define ID_ALM_R0RSP        2           // SIO0 ���X�|���X��M�^�C���A�E�g����
#define ID_ALM_R0ACK        3           // 
#define ID_ALM_R2DAT        4           // SIO2 �f�[�^��M�^�C���A�E�g����
#define ID_ALM_R2RSP        5           // SIO2 ���X�|���X��M�^�C���A�E�g����
#define ID_ALM_R2ACK        6           // 
#define ID_ALM_CPOPN        7           // 
#define ID_ALM_BARRD        8           // 
#define ALM_NUM     	    9           // �A���[����
							
//*************************************
// ���b�Z�[�W�h�c
//*************************************

// RX0 TASK -> CMD TASK

// ��{�R�}���h
                                        /*==========================================================*/
                                        /* No. Command ���e                                         */
                                        /*==========================================================*/
#define	ID_MSG_HELLO		0           /*  HELLO   �N���ʒm                                      */
#define	ID_MSG_LOG			1           /*  reserve                                               */
#define	ID_MSG_VER			2           /*  VER     �o�[�W�����m�F                                */
#define	ID_MSG_ALLIN		3           /*  ALLIN   �SI/O����                                     */
#define	ID_MSG_BIN			4           /*  BIN     I/O����                                       */
#define	ID_MSG_BOUT			5           /*  BOUT    I/O�o��                                       */
#define	ID_MSG_CINIT		6           /*  CINIT   ���̓J�E���^�̐ݒ�                            */
#define	ID_MSG_COUNT		7           /*  COUNT   ���̓J�E���^�̓ǂݏo��                        */
#define	ID_MSG_INT			8           /*  INT     ���荞�ݐݒ�                                  */
#define	ID_MSG_ABSMV		9           /*  ABSMV   �p���X���[�^�ړ�(��Έʒu)                    */
#define	ID_MSG_ABORT		10          /*  ABORT   �p���X���[�^����~                            */
#define	ID_MSG_ENCODER		11          /*  ENCODER �G���R�[�_�̐ݒ�                              */
#define	ID_MSG_LIMIT		12          /*  LIMIT   �p���X���[�^���_���A                          */
#define	ID_MSG_MAP			13          /*  MAP     �Q���̃p���X���[�^�ړ�                        */
#define	ID_MSG_PDELAY		14          /*  PDELAY  �p���X���[�^�̃p���[����҂����Ԑݒ�          */
#define	ID_MSG_POWER		15          /*  POWER   �p���X���[�^�̗㎥����                        */
#define	ID_MSG_PSET			16          /*  PSET    �p���X���[�^�̃J�E���^���Z�b�g                */
#define	ID_MSG_RELMV		17          /*  RELMV   �p���X���[�^�ړ�(���Έʒu)                    */
#define	ID_MSG_SET			18          /*  SET     �p���X���[�^���x�ݒ�                          */
#define	ID_MSG_STATUS		19          /*  STATUS  �p���X���[�^��ԓǂݏo��                      */
#define	ID_MSG_STOP			20          /*  STOP    �p���X���[�^������~                          */
#define	ID_MSG_BZCNT		21          /*  BZCNT                                                 */
#define	ID_MSG_EEPREAD		22          /*  EEPREAD   */
#define	ID_MSG_EEPWRIT		23          /*  EEPWRIT   */
#define	ID_MSG_INTRPT		24          /*  INTERRUPT                                             */
#define	ID_MSG_TMPSET_HS	26          /*                                                        */
#define	ID_MSG_TMPGET_HS	27          /*                                                        */
                                        /*                                                        */
#define	ID_MSG_VALVEINI_HS	29          /*                                                        */
#define	ID_MSG_PRESSINI_HS	30          /*                                                        */
#define	ID_MSG_PRESSREN_HS	31          /*                                                        */
#define	ID_MSG_PRESSTAN_HS	32          /*                                                        */
#define	ID_MSG_PRESSCAN_HS	33          /*                                                        */
#define	ID_MSG_VALVEOUT_HS	34          /*                                                        */
#define	ID_MSG_PRESSURE_HS	35          /*                                                        */
#define	ID_MSG_IMPPDAT		36          /*                                                        */
#define	ID_MSG_IMPSPED		37          /*                                                        */
#define	ID_MSG_IMPPOSI		38          /*                                                        */
#define	ID_MSG_IMPOPTN		39          /*                                                        */
#define	ID_MSG_EXPRUNL		40          /*                                                        */
#define	ID_MSG_EXPCOML		41          /*                                                        */
#define	ID_MSG_EXPPOSI		42          /*                                                        */
#define	ID_MSG_EXPOPTN		43          /*                                                        */
#define	ID_MSG_EXPCLOG		44          /*                                                        */





#define	ID_MSG_CBZCNT		50          /*  C-BZCNT                                               */
#define	ID_MSG_CLEDON		51          /*  C-LEDON                                               */

#define	ID_MSG_BASE_CMD_MAX	45          /* ��{�R�}���h����                                       */


//Response
#define	ID_MSG_CMD			100         /* �R�}���h���X�|���X                                       */
#define	ID_MSG_ACK			101         /* �`�b�j                                                   */
#define	ID_MSG_NAK			102         /* �m�`�j                                                   */
#define	ID_MSG_CMD_BAR		103         /* �R�}���h���X�|���X                                       */
#define	ID_MSG_RUNLOGERR	104          /*  */

// etc
#define	ID_MSG_VALVEINI	150          /*  */
#define	ID_MSG_PRESSINI	151          /*  */
#define	ID_MSG_PRESSREN	152          /*  */
#define	ID_MSG_PRESSTAN	153          /*  */
#define	ID_MSG_PRESSCAN	154          /*  */
#define	ID_MSG_VALVEOUT	155          /*  */
#define	ID_MSG_PRESSURE	156          /*  */
#define	ID_MSG_TMPSET	157          /*  */
#define	ID_MSG_TMPGET	158          /*  */
#define	ID_MSG_TMPGETR	159          /*  */



// etc
#define	ID_MSG_TIMWAIT		200         /* �v�`�h�s                                                 */
#define	ID_MSG_EEPWRIT_STS	201         /* �f�o�C�X��ԏ����݁i�����s�v�j                           */



//CMD TASK->COUNT TASK



//  -> MOT TASK
#define ID_MSG_CNT_START	0
#define ID_MSG_CNT_STOP		1


//*************************************
// �^�b�`�p�l���h�c
//*************************************
#if 0
enum TPID_KIND
{
	TPID_FWVER = 0x01,
	TPID_OPTIF = 0x02,
	TPID_LGIN1 = 0x03,
	TPID_LGIN2 = 0x04,
	TPID_INITA = 0x05
};
#endif
#if 1
#define	ID_TPL_FWVER		0x01        /* FW�o�[�W�������                                         */
#define	ID_TPL_OPTIF		0x02        /* �I�v�V�������                                           */
#define	ID_TPL_LGIN1		0x03        /* ���O�C���F��1                                            */
#define	ID_TPL_LGIN2		0x04        /* ���O�C���F��2                                            */
#define	ID_TPL_INITA		0x05        /* �������w��                                               */
#define	ID_TPL_TITLE		0x06        /* �v���g�R���^�C�g���v��                                   */
#define	ID_TPL_PREXT		0x07        /* �v���g�R���L��                                           */
#define	ID_TPL_USRAL		0x08        /* ���[�U�[���S�擾                                       */
#define	ID_TPL_INF01		0x11        /* ���[�U�[1���                                            */
#define	ID_TPL_INF02		0x12        /* ���[�U�[2���                                            */
#define	ID_TPL_INF03		0x13        /* ���[�U�[3���                                            */
#define	ID_TPL_INF04		0x14        /* ���[�U�[4���                                            */
#define	ID_TPL_INF05		0x15        /* ���[�U�[5���                                            */
#define	ID_TPL_INF06		0x16        /* ���[�U�[6���                                            */
#define	ID_TPL_INF07		0x17        /* ���[�U�[7���                                            */
#define	ID_TPL_INF08		0x18        /* ���[�U�[8���                                            */
#define	ID_TPL_INF09		0x19        /* ���[�U�[9���                                            */
#define	ID_TPL_INF10		0x1A        /* ���[�U�[10���                                           */
#define	ID_TPL_REC01		0x21        /* ���[�U�[1�o�^                                            */
#define	ID_TPL_REC02		0x22        /* ���[�U�[2�o�^                                            */
#define	ID_TPL_REC03		0x23        /* ���[�U�[3�o�^                                            */
#define	ID_TPL_REC04		0x24        /* ���[�U�[4�o�^                                            */
#define	ID_TPL_REC05		0x25        /* ���[�U�[5�o�^                                            */
#define	ID_TPL_REC06		0x26        /* ���[�U�[6�o�^                                            */
#define	ID_TPL_REC07		0x27        /* ���[�U�[7�o�^                                            */
#define	ID_TPL_REC08		0x28        /* ���[�U�[8�o�^                                            */
#define	ID_TPL_REC09		0x29        /* ���[�U�[9�o�^                                            */
#define	ID_TPL_REC10		0x2A        /* ���[�U�[10�o�^                                           */
#define	ID_TPL_CHG01		0x31        /* ���[�U�[1�ύX                                            */
#define	ID_TPL_CHG02		0x32        /* ���[�U�[2�ύX                                            */
#define	ID_TPL_CHG03		0x33        /* ���[�U�[3�ύX                                            */
#define	ID_TPL_CHG04		0x34        /* ���[�U�[4�ύX                                            */
#define	ID_TPL_CHG05		0x35        /* ���[�U�[5�ύX                                            */
#define	ID_TPL_CHG06		0x36        /* ���[�U�[6�ύX                                            */
#define	ID_TPL_CHG07		0x37        /* ���[�U�[7�ύX                                            */
#define	ID_TPL_CHG08		0x38        /* ���[�U�[8�ύX                                            */
#define	ID_TPL_CHG09		0x39        /* ���[�U�[9�ύX                                            */
#define	ID_TPL_CHG10		0x3A        /* ���[�U�[10�ύX                                           */
#define	ID_TPL_DEL01		0x41        /* ���[�U�[1�폜                                            */
#define	ID_TPL_DEL02		0x42        /* ���[�U�[2�폜                                            */
#define	ID_TPL_DEL03		0x43        /* ���[�U�[3�폜                                            */
#define	ID_TPL_DEL04		0x44        /* ���[�U�[4�폜                                            */
#define	ID_TPL_DEL05		0x45        /* ���[�U�[5�폜                                            */
#define	ID_TPL_DEL06		0x46        /* ���[�U�[6�폜                                            */
#define	ID_TPL_DEL07		0x47        /* ���[�U�[7�폜                                            */
#define	ID_TPL_DEL08		0x48        /* ���[�U�[8�폜                                            */
#define	ID_TPL_DEL09		0x49        /* ���[�U�[9�폜                                            */
#define	ID_TPL_DEL10		0x4A        /* ���[�U�[10�폜                                           */
#define	ID_TPL_FWPOL		0x50        /* FW��ԁi�|�[�����O�j                                     */
#define	ID_TPL_PRTCL		0x51        /* �v���g�R�����s                                           */
#define	ID_TPL_RESTA		0x52        /* ���X�^�[�g���s                                           */
#define	ID_TPL_PAUSE		0x53        /* �ꎞ��~                                                 */
#define	ID_TPL_RESUM		0x54        /* �ĊJ                                                     */
#define	ID_TPL_ABORT		0x55        /* ���f                                                     */
#define	ID_TPL_OPTCD		0x56        /* �I�v�V�����o�[�R�[�h���s                                 */
#define	ID_TPL_SMBAR		0x57        /* �T���v���`���[�uQR�R�[�h                                 */
#define	ID_TPL_SMSGL		0x58        /* �T���v���`���[�u�L���m�F�i�P�j                           */
#define	ID_TPL_SMALL		0x59        /* �T���v���`���[�u�L���m�F�i�S�j                           */
#define	ID_TPL_CLQRC		0x5A        /* ����`���[�uQR�R�[�h                                     */
#define	ID_TPL_CLSGL		0x5B        /* ����`���[�u�L���i�P�j                                   */
#define	ID_TPL_CLALL		0x5C        /* ����`���[�u�L���i�S�j                                   */
#define	ID_TPL_CLLNE		0x5D        /* ����`���[�u���[��QR�R�[�h                               */
#define	ID_TPL_LDLMP		0x60        /* LED                                                      */
#define	ID_TPL_UVLMP		0x61        /* UV�_��                                                   */
#define	ID_TPL_PTLMP		0x62        /* �p�g�����v                                               */
#define	ID_TPL_POWOF		0x63        /* �d��OFF                                                  */
#define	ID_TPL_EXRUN		0x6B        /* �^�]�ɗ�v��                                             */
#define	ID_TPL_PRINF		0x6E        /* �v���g�R�����v��                                       */

#endif


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// ���荞�݌n�ݒ�

//*************************************
// ���荞�ݗD�揇��
//*************************************
#define INT_PRI_SCI0        7           // SCI0
#define INT_PRI_SCI1        7           // SCI1
#define INT_PRI_SCI2        7           // SCI2
#define INT_PRI_SCI3        7           // SCI3
#define INT_PRI_IRQ4        6           // IRQ4

//*************************************
// ���荞�݋֎~�}�X�N
//*************************************
#define CPU_INT_DISABLE     9

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �V���A���ʐM

//*************************************
// �ʐM�|�[�g�ԍ�
//*************************************
#define SIO_PC              0          // �o�b�ʐM�|�[�g�̔ԍ�
#define SIO_EX1             1          // 
#define SIO_EX2             2          // 

//*************************************
// �|�[�g�I�[�v���^�N���[�Y
//*************************************
#define SIO_CLOSE           0
#define SIO_OPEN            1

//*************************************
// ����R�[�h
//*************************************
#define ACK                 0x06
#define NAK                 0x15
#define STX                 0x02
#define ETX                 0x03
#define CRCHR               0x0d
#define LFCHR               0x0a

//*************************************
// �R�}���h�f�[�^��o�b�t�@�T�C�Y
//*************************************
#define CMD_BUFSIZ          60			// �R�}���h��M�o�b�t�@�T�C�Y
#define COM_BUFSIZ          128			// �ʐM�f�[�^��M�o�b�t�@�T�C�Y

//*************************************
// �p�����[�^�ő吔
//*************************************
//#define MAX_ARGS            36
#define MAX_ARGS            100

//*************************************
// ��M�f�[�^�^�C�v
//*************************************
#define COM_MTYP_CHR        0          // �����M�F�L�����N�^
#define COM_MTYP_ORN        1          // �G���[�F�I�[�o�[�����G���[
#define COM_MTYP_FER        2          // �G���[�F�t���[�~���O�G���[
#define COM_MTYP_PER        3          // �G���[�F�p���e�B�G���[
#define COM_MTYP_RTMO       4          // �G���[�F��M�^�C���A�E�g
#define COM_MTYP_STMO       5          // �G���[�F���M�^�C���A�E�g
#define COM_MTYP_ERR        6          // �G���[�F���̑��G���[
#define COM_MTYP_BDSTART    10          // 
#define COM_MTYP_BDEND      11          // 

//*************************************
// �o�b�ʐM�^�C���A�E�g
//*************************************
#if DEBUG_SIM_MODE == 2 
	#define TMOUT_RCV       (3000/1) // ��M�^�C���A�E�g
	#define	TMOUT_ACK		(3000/1)
#else
	#define TMOUT_RCV       (3000/1) // ��M�^�C���A�E�g
	#define	TMOUT_ACK		(3000/1)
#endif

//*************************************
// ���u�ʐM���X�|���X�^�C���A�E�g
//*************************************
#define TMOUT_RSP       (3000/1) // ��M�^�C���A�E�g
#define TMOUT_BAR       (90000/1) // ��M�^�C���A�E�g


//*************************************
// ���X�|���X���
//*************************************


//*****************************************
// �X�e�[�^�X
//*****************************************
#define STS00				0x00	// S00 ������
#define STS01				0x01	// S01 
#define STS02				0x02	// S02 
#define STS03				0x03	// S03 
#define STS04				0x04	// S04 
#define STS05				0x05	// S05 
#define STS10				0x10	// S10 
#define STS20				0x20	// S20 
#define STS30				0x30	// S30 


//*************************************
// �o�b�ʐM�Ǘ��p
//*************************************
#define	RCVCON				0x01
#define	RCVCMD 				0x10	// �R�}���h��M
#define	RCVACK 				0x11	// �`�b�j��M
#define	RCVNAK 				0x12	// �m�`�j��M
#define	RCVEND				0x18
#define	BCCERR 				0x83	// �G���[
#define	TMOERR 				0x84	// �G���[
#define	RCVERR 				0x85	// �G���[

#define RTY_OVER			3

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �R�}���h�^�X�N

// INT�R�}���h
#define	TRG_NM				0						// N    ����
#define	TRG_HD				1						// HD  �FHigh���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
#define	TRG_HP				2       				// H+  �FHigh���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
#define	TRG_HM				3       				// H-  �FHigh���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
#define	TRG_LD				4       				// LD  �FLow���͏�Ԃ̊ԁACW/CCW���������Ɉꎞ��~
#define	TRG_LP				5       				// L+  �FLow���͏�Ԃ̊ԁACW�����݈̂ꎞ��~
#define	TRG_LM				6       				// L-  �FLow���͏�Ԃ̊ԁACCW�����݈̂ꎞ��~
#define	TRG_UD				7       				// UD  �F�����オ��G�b�W���o���ACW/CCW���������Ɉꎞ��~
#define	TRG_UP				8       				// U+  �F�����オ��G�b�W���o���ACW�����݈̂ꎞ��~
#define	TRG_UM				9       				// U-  �F�����オ��G�b�W���o���ACCW�����݈̂ꎞ��~
#define	TRG_DD				10      				// DD  �F����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
#define	TRG_DP				11      				// D+  �F����������G�b�W���o���ACW�����݈̂ꎞ��~
#define	TRG_DM				12      				// D-  �F����������G�b�W���o���ACCW�����݈̂ꎞ��~
#define	TRG_ED				13      				// ED  �F�����オ��^����������G�b�W���o���ACW/CCW���������Ɉꎞ��~
#define	TRG_EP				14      				// E+  �F�����オ��^����������G�b�W���o���ACW�����݈̂ꎞ��~
#define	TRG_EM				15      				// E-  �F�����オ��^����������G�b�W���o���ACCW�����݈̂ꎞ��~


//�ړ���MIN,MAX
#define MVMIN				-2147483648
#define MVMAX				2147483647

// PSET MIN,MAX
#define PSMIN				-32767
#define PSMAX				32767

// SPEED MIN,MAX
#define SPMIN				0
#define SPMAX				100000

#define BCCTRL_MAX			19


// LIMIT�R�}���h
// �ҋ@���A���
#define GENTEN_POS			0						// ���_���A����
#define NUKEDA_POS			1						// ���_�����o������


// ABSMV,RELMV,LIMIT����
#define REQ_STAUD			0
#define REQ_STAFH			1
#define REQ_STAFL			2


// 
#define DIS					0						// ��t�֎~
#define ENA					1						// ��t����
#define CHK					2						// ���[�^����m�F
#define BSY					3						// ���[�^�r�W�[
#define IGN					4						// �����i�K��O�j



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �^�C�}

#define	INTERVAL_1M			1						// �C���^�[�o��1msec
#define	INTERVAL_10M		10						// �C���^�[�o��10msec
#define	INTERVAL_100M		100						// �C���^�[�o��100msec
#define	INTERVAL_1S			1000					// �C���^�[�o��1sec
#define	DLY_1M				1						// 1msec
#define	DLY_4M				4						// 4msec
#define	DLY_10M				10						// 10msec
#define	DLY_50M				50						// 50msec
#define	DLY_100M			100						// 100msec
#define	WAITTIM				60						// 60��msec
#define	TIM_AIR_VAl			200						// ��C�يĎ�2sec(10msec�P��)



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �d�d�o�q�n�l����

// tskeep.h�Q��
#if ( EEP_RWSIZE_TYPE == 0 )
#define EEP_RWSIZE			1
#else
#define EEP_RWSIZE			120
#endif



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// ���[�V�����l�b�g
#define SYS_RTY_OVER		10






//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// ���s����

#define	MNET_CMD_RTY_MAX	120	
#define	READ_RTY_MAX		3
#define	WRITE_RTY_MAX		3




//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// �G���[


// �G���[�R�[�h
// 000
#define	E_SUCCESS		0x00000000		// 0000 ����I��
#define	E_IDLING		0x00000000		// 0000 �A�C�h�����O�I��
#define	E_LIQSUCEND		0x00000001		// 0001 �t�z������
#define	E_LIQDETECT		0x00000002		// 0002 �t�ʌ��m����
#define	E_PROTCOLSTOP	0x00000003		// 0003 �v���g�R�����f��
#define	E_PROTCOLPAUSE	0x00000004		// 0004 �v���g�R���ꎞ��~��
#define	E_MIDDLEEND		0x00000080		// 0080 ����r���I��
//                     �`0x000000BF		// 00BF ����r���I��
#define	E_CONTINUE		0x000000F1		// 00F1 �������E�����p�����i���t�@�[�������ł̂ݗ��p�j
#define	E_IGNORE		0x000000F1		// 00F2 ����

#define	E_PRES_FAIL		0x00000020		// �������s
#define	E_PRES_LEAK		0x00000021		// �t����
#define	E_RECV_NONE		0x00000030		// ����M
#define	E_SEND_FAIL		0x00000031		// ���M���s
#define	E_RECV_FAIL		0x00000032		// ��M���s
#define	E_TEMP_FAIL		0x000000FF		// ���x�f�[�^�擾���s





// 100
#define	E_BUSY			0x00000100		// �r�W�[�擪
#define E_BSY_HEAD_X1	0x00000101		// 0101 ����X���r�W�[
#define E_BSY_HEAD_Y1	0x00000102		// 0102 ����Y���r�W�[
#define E_BSY_HEAD_Z1	0x00000103		// 0103 ����Z1���r�W�[
#define E_BSY_HEAD_Z2	0x00000104		// 0104 ����Z2���r�W�[
#define E_BSY_HEAD_P1	0x00000105		// 0105 �|���v1���r�W�[
#define E_BSY_HEAD_P2	0x00000106		// 0106 �|���v2���r�W�[
#define E_BSY_HEAD_PT	0x00000107		// 0107 �s�b�`���r�W�[
#define E_BSY_KAIS_Z1	0x00000108		// 0108 �J��Z���r�W�[
#define E_BSY_KAIS_R1	0x00000109		// 0109 �J��R1���r�W�[
#define E_BSY_KAIS_R2	0x0000010A		// 010A �J��R2���r�W�[
#define E_BSY_EXT_BAR	0x0000010B		// 010B �˂��o���_���r�W�[
#define E_BSY_HANS_X1	0x0000010C		// 010C ����X���r�W�[
#define E_BSY_HANS_Y1	0x0000010D		// 010D ����Y���r�W�[
#define E_BSY_HANS_Z1	0x0000010E		// 010E ����Z���r�W�[
#define E_BSY_HANS_R1	0x0000010F		// 010F ����R���r�W�[
#define E_BSY_TURNTBL	0x00000110		// 0110 �e�[�u�����r�W�[
#define E_BSY_BARMOV1	0x00000111		// 0111 �o�[�R�[�h1�����r�W�[
#define E_BSY_BARMOV2	0x00000112		// 0112 �o�[�R�[�h2�����r�W�[
										// 0113 �\��
										// �`   �\��
										// 013F �\��
#define	E_BSY_CMDDIS	0x00000140		// 0140 �r��


// 200
#define	E_CUR_ERR		0x00000200		// 0200 �㎥���s
#define E_CUR_HEAD_X1	0x00000201		// ����X���㎥���s
#define E_CUR_HEAD_Y1	0x00000202		// ����Y���㎥���s
#define E_CUR_HEAD_Z1	0x00000203		// ����Z1���㎥���s
#define E_CUR_HEAD_Z2	0x00000204		// ����Z2���㎥���s
#define E_CUR_HEAD_P1	0x00000205		// �|���v1���㎥���s
#define E_CUR_HEAD_P2	0x00000206		// �|���v2���㎥���s
#define E_CUR_HEAD_PT	0x00000207		// �s�b�`���㎥���s
#define E_CUR_KAIS_Z1	0x00000208		// �J��Z���㎥���s
#define E_CUR_KAIS_R1	0x00000209		// �J��R1���㎥���s
#define E_CUR_KAIS_R2	0x0000020A		// �J��R2���㎥���s
#define E_CUR_EXT_BAR	0x0000020B		// �˂��o���_���㎥���s
#define E_CUR_HANS_X1	0x0000020C		// ����X���㎥���s
#define E_CUR_HANS_Y1	0x0000020D		// ����Y���㎥���s
#define E_CUR_HANS_Z1	0x0000020E		// ����Z���㎥���s
#define E_CUR_HANS_R1	0x0000020F		// ����R���㎥���s
#define E_CUR_TURNTBL	0x00000210		// �e�[�u�����㎥���s
#define E_CUR_BARMOV1	0x00000211		// �o�[�R�[�h1�����㎥���s
#define E_CUR_BARMOV2	0x00000212		// �o�[�R�[�h2�����㎥���s
										// 0213 �\��
										// �`   �\��
										// 023F �\��

// 300
#define	E_DTY_ERR		0x00000300		// 0300 �E��
#define E_DTY_HEAD_X1	0x00000301		// ����X���E��
#define E_DTY_HEAD_Y1	0x00000302		// ����Y���E��
#define E_DTY_HEAD_Z1	0x00000303		// ����Z1���E��
#define E_DTY_HEAD_Z2	0x00000304		// ����Z2���E��
#define E_DTY_HEAD_P1	0x00000305		// �|���v1���E��
#define E_DTY_HEAD_P2	0x00000306		// �|���v2���E��
#define E_DTY_HEAD_PT	0x00000307		// �s�b�`���E��
#define E_DTY_KAIS_Z1	0x00000308		// �J��Z���E��
#define E_DTY_KAIS_R1	0x00000309		// �J��R1���E��
#define E_DTY_KAIS_R2	0x0000030A		// �J��R2���E��
#define E_DTY_EXT_BAR	0x0000030B		// �˂��o���_���E��
#define E_DTY_HANS_X1	0x0000030C		// ����X���E��
#define E_DTY_HANS_Y1	0x0000030D		// ����Y���E��
#define E_DTY_HANS_Z1	0x0000030E		// ����Z���E��
#define E_DTY_HANS_R1	0x0000030F		// ����R���E��
#define E_DTY_TURNTBL	0x00000310		// �e�[�u�����E��
#define E_DTY_BARMOV1	0x00000311		// �o�[�R�[�h1�����E��
#define E_DTY_BARMOV2	0x00000312		// �o�[�R�[�h2�����E��


// 340
#define	E_LMT_ERR		0x00000340		// 0340 ���~�b�g
#define E_LMT_HEAD_X1	0x00000341		// ����X���E��
#define E_LMT_HEAD_Y1	0x00000342		// ����Y���E��
#define E_LMT_HEAD_Z1	0x00000343		// ����Z1���E��
#define E_LMT_HEAD_Z2	0x00000344		// ����Z2���E��
#define E_LMT_HEAD_P1	0x00000345		// �|���v1���E��
#define E_LMT_HEAD_P2	0x00000346		// �|���v2���E��
#define E_LMT_HEAD_PT	0x00000347		// �s�b�`���E��
#define E_LMT_KAIS_Z1	0x00000348		// �J��Z���E��
#define E_LMT_KAIS_R1	0x00000349		// �J��R1���E��
#define E_LMT_KAIS_R2	0x0000034A		// �J��R2���E��
#define E_LMT_EXT_BAR	0x0000034B		// �˂��o���_���E��
#define E_LMT_HANS_X1	0x0000034C		// ����X���E��
#define E_LMT_HANS_Y1	0x0000034D		// ����Y���E��
#define E_LMT_HANS_Z1	0x0000034E		// ����Z���E��
#define E_LMT_HANS_R1	0x0000034F		// ����R���E��
#define E_LMT_TURNTBL	0x00000350		// �e�[�u�����E��
#define E_LMT_BARMOV1	0x00000351		// �o�[�R�[�h1�����E��
#define E_LMT_BARMOV2	0x00000352		// �o�[�R�[�h2�����E��


// 400
#define	E_CON_ERR		0x00000400		// �ڑ��ُ�
#define E_CON_HEAD_Y0	0x00000401		// ����Y���ڑ��ُ�
#define E_CON_HEAD_X1	0x00000401		// ����X���ڑ��ُ�
#define E_CON_HEAD_Y1	0x00000402		// ����Y���ڑ��ُ�
#define E_CON_HEAD_Z1	0x00000403		// ����Z1���ڑ��ُ�
#define E_CON_HEAD_Z2	0x00000404		// ����Z2���ڑ��ُ�
#define E_CON_HEAD_P1	0x00000405		// �|���v1���ڑ��ُ�
#define E_CON_HEAD_P2	0x00000406		// �|���v2���ڑ��ُ�
#define E_CON_HEAD_PT	0x00000407		// �s�b�`���ڑ��ُ�
#define E_CON_KAIS_Z1	0x00000408		// �J��Z���ڑ��ُ�
#define E_CON_KAIS_R1	0x00000409		// �J��R1���ڑ��ُ�
#define E_CON_KAIS_R2	0x0000040A		// �J��R2���ڑ��ُ�
#define E_CON_EXT_BAR	0x0000040B		// �˂��o���_���ڑ��ُ�
#define E_CON_HANS_X1	0x0000040C		// ����X���ڑ��ُ�
#define E_CON_HANS_Y1	0x0000040D		// ����Y���ڑ��ُ�
#define E_CON_HANS_Z1	0x0000040E		// ����Z���ڑ��ُ�
#define E_CON_HANS_R1	0x0000040F		// ����R���ڑ��ُ�
#define E_CON_TURNTBL	0x00000410		// �e�[�u�����ڑ��ُ�
#define E_CON_BARMOV1	0x00000411		// �o�[�R�[�h1�����ڑ��ُ�
#define E_CON_BARMOV2	0x00000412		// �o�[�R�[�h2�����ڑ��ُ�
										// 0413 �\��
										// �`   �\��
										// 043F �\��
#define	E_CON_SYSCOM	0x00000440		// 0440 �V�X�e���ʐM���s
#define	E_CON_IO_COM	0x00000441		// 0441 IO�ʐM���s


// 500
#define	E_MNT_ACCESS	0x00000500		// 0500�`05FF MNET���W�X�^�A�N�Z�X�ُ�

// 600
#define	E_COM_CTRLPC	0x00000600		// 0600 ����o�b�ʐM�ڑ��ُ�
#define	E_COM_BARCOD	0x00000602		// 0602 �o�[�R�[�h�ʐM�ڑ��ُ�
#define	E_COM_P_OPEN	0x00000604		// 0604 �|�[�g�I�[�v���ς�
#define	E_COM_P_NONE	0x00000605		// 0605 �|�[�g���I�[�v��
#define	E_COM_BCCERR	0x00000606		// 0606 BCC�s��v
#define	E_COM_CMDDEF	0x00000610		// 0610 �R�}���h����`
#define	E_COM_SPITMO	0x00000621		// 0621 SPI�ʐM�^�C���A�E�g
#define	E_COM_SPIOVR	0x00000622		// 0621 SPI�ʐM�I�[�o�[����
// 700
#define	E_PAR_SHORTG	0x00000700		// 07xx �p�����[�^�ߕs��
// 800
#define	E_PAR_OUTRNG	0x00000800		// 08xx �p�����[�^�͈͊O
// 900
#define E_BAR_READNG 	0x00000900 		// 0900 �o�[�R�[�h�Ǎ����s �ǎ�萔�s��v
#define E_BAR_RACKNG 	0x00000901 		// 0901 �o�[�R�[�h�Ǎ����s ���b�N�m�f 
#define E_BAR_TIMOUT 	0x00000902 		// 0902 �o�[�R�[�h�Ǎ����s �^�C���A�E�g
#define E_BAR_POS1NG 	0x00000911 		// 0911 �o�[�R�[�h�Ǎ����s �|�W�V���������s�� 

// A00
#define	E_TIP_NTHING	0x00000A00		// 0A00 �`�b�v�擾���s
#define	E_TIP_EXISTS	0x00000A01		// 0A01 �`�b�v�L��
#define	E_TIP_DPFALL	0x00000A02		// 0A02 �`�b�v�E��
#define	E_TIP_GETING	0x00000A03		// 0A03 �`�b�v�擾�ς�
//#define	E_TUB_PICARI	0x00000A08		// 0A08 (�s�b�N�A�b�v)�`���[�u�L��
//#define	E_TUB_PICNAS	0x00000A09		// 0A09 (�s�b�N�A�b�v)�`���[�u����
//#define	E_TUB_RLSNAS	0x00000A0A		// 0A0A (�����[�X)�`���[�u����
//#define	E_TUB_RLSARI	0x00000A0B		// 0A0B (�����[�X)�`���[�u�L��
//#define	E_TUB_DRPOUT	0x00000A0C		// 0A0C �`���[�u�E��
//#define	E_TUB_HNDSNS	0x00000A0D		// 0A0D �`���[�u�����n���h�Z���T�ُ�
//#define	E_CAP_TUBNAS	0x00000A10		// 0A10 �J���`���[�u����
//#define	E_CAP_TUBLUP	0x00000A11		// 0A11 �J���`���[�u�����グ
//#define	E_CAP_SENSOR	0x00000A12		// 0A12 �J���Z���T�ُ�
//#define	E_CAP_DSPNON	0x00000A13		// 0A13 �p���L���b�v����
//#define	E_CAP_FAILUR	0x00000A18		// 0A18 �J�����s


#define	E_LIQ_FAILUR	0x00000A20		// 0A20 �t�ʌ��m���s�i �����o �j
#define	E_LIQ_JAMING	0x00000A21		// 0A21 �l�܂茟�m
#define	E_LIQ_SHORTG	0x00000A22		// 0A22 �t�ʕs��( �A���� )
#define	E_NZL_COLLIS	0x00000A30		// 0A30 �m�Y���Փ�
#define	E_DRP_OPENNG	0x00000A31		// 0A31 �h���b�v�L���b�`��OPEN�G���[
#define	E_DRP_CLSENG	0x00000A32		// 0A32 �h���b�v�L���b�`��CLOSE�G���[
// B00
#define	E_AIR_EMPTY		0x00000B00		// 0B00 �G�A�G���v�e�B
// B10
#define	E_EEP_ERR		0x00000B10		// 0B00 EEPROM�G���[
#define	E_EEP_WRTERR	0x00000B10		// 0B00 EEPROM�����݃G���[
#define	E_EEP_REDERR	0x00000B11		// 0B01 EEPROM�Ǎ��݃G���[
// F000
#define	E_SYS_MLTACT	0x0000F000		// F000 �}���`�^�X�N�V�X�e������ُ�
#define	E_SYS_TSKHDL	0x0000F001		// F001 �^�X�N�E�n���h�������ُ�
#define	E_SYS_MEMPOL	0x0000F002		// F002 �������v�[�������ُ�
#define	E_SYS_MAILQU	0x0000F003		// F003 ���[���E�L���[�����ُ�
#define	E_SYS_SEMAPH	0x0000F004		// F004 �Z�}�t�H�����ُ�
#define	E_SYS_TSKSTA	0x0000F005		// F005 �^�X�N�N���ُ�
#define	E_SYS_CYCHDL	0x0000F006		// F006 �����n���h���N���ُ�
#define	E_SYS_DATERR	0x0000F007		// F007 �f�[�^�����ُ�
#define	E_SYS_MSGCOM	0x0000F008		// F008 ���b�Z�[�W�ʒm�ُ�
#define	E_SYS_POLGET	0x0000F009		// F009 �������v�[���擾���s
#define	E_SYS_POLREF	0x0000F00A		// F00A �������v�[��������s
#define	E_SYS_FNCCAL	0x0000F00B		// F00B �t�@���N�V�����R�[���ُ�


//--------------------------------------------------------------------------------------------------
// �k�d�c�\��
#define	LED_ADDR		0x0C00F020			// LED�A�h���X



//--------------------------------------------------------------------------------------------------
// �V�r�d�f�\��

#define	SEG_CLR			0x00				/* �V�r�d�f�N���A */
#define	ALL_BLINK		0xFF				/* �V�r�d�f�S���� */
#define	SEG7_ADDR		0x0C00F028			// 7SEG�A�h���X


//--------------------------------------------------------------------------------------------------
// ���u�萔
#define MOTOR_CW        1
#define MOTOR_CCW       2

#define	MTMIN			0x00000000			// ���[�^�[���蓖�čŏ��l(���[�^�O)
#define	MTMAX			0xffffffff			// ���[�^�[���蓖�čő�l(���[�^�R�P��)


//--------------------------------------------------------------------------------------------------
// ����










//--------------------------------------------------------------------------------------------------
// �R�}���h��




// [C-TPGET] --------------------------
// �`�b�v�L��
#define TP1_PRE			0x01
#define TP2_PRE			0x02
#define TPW_PRE			( TP1_PRE | TP2_PRE )

#define TP1_AFT			0x10
#define TP2_AFT			0x20
#define TPW_AFT			( TP1_AFT | TP2_AFT )

#define TPN_TARGET		0x00
#define TP1_TARGET		0x01
#define TP2_TARGET		0x02
#define TPW_TARGET		( TP1_TARGET | TP2_TARGET )


// [C-BCDSP] --------------------------
// ����w��
#define BCD_QR			0		// 0:QR
#define BCD_39			1		// 1:code39
#define BCD_IT			2		// 2:ITF
#define BCD_12			3		// 3:code128
#define BCD_JN			4		// 4:JAN/EAN/UPC

// ���
#define RACK_ID			0		// 0:���b�N�h�c
#define POSI_ID			1		// 1:�|�W�V�����h�c
#define TUBE_ID			2		// 2:�`���[�u�h�c

#define RACK_LEN		3		
#define POSI_LEN		4		
#define TUBE_LEN8		8		
#define TUBE_LEN9		9		
#define TUBE_LEN10		10		
#define TUBE_LEN11		11		
#define TUBE_LEN12		12		
#define TUBE_LEN13		13		
#define TUBE_LEN14		14		
#define TUBE_LEN15		15		
#define TUBE_LEN16		16		

// [C-BZCNT] --------------------------
// ���ײē_�������
#define PATOF			0x00	// 0:����
#define PATON			0x01	// 1:�_��
#define PATBL1			0x02	// 2:�_��(700msec)
#define PATBL2			0x03	// 3:�_��(500msec)
#define PATBL3			0x04	// 4:�_��(300msec)
#define PATCN			0x05	// 5:�p��(�����p��)

// �޻ް�������
#define BUZOF			0x00	//  0:����
#define BUZ01			0x01	//  1:�s�[	
#define BUZ02			0x02	//  2:�s�[�b�s�[�b(ON:1000msec,OFF:1000msec)	
#define BUZ03			0x03	//  3:�s�[�b�s�[�b(ON:300 msec,OFF:700 msec)	
#define BUZ04			0x04	//  4:�s�[�b�s�[�b(ON:400 msec,OFF:600 msec)	
#define BUZ05			0x05	//  5:�s�[�b�s�[�b(ON:500 msec,OFF:500 msec)	
#define BUZ06			0x06	//  6:�s�[�b�s�[�b(ON:600 msec,OFF:400 msec)	
#define BUZ07			0x07	//  7:�s�[�b�s�[�b(ON:700 msec,OFF:300 msec)	
#define BUZ08			0x08	//  8:�s�[�b�s�[�b(ON:800 msec,OFF:200 msec)	
#define BUZ09			0x09	//  9:�s�s�b�s�s�b(ON:200 msec,OFF:300 msec)	
#define BUZ0A			0x0A	// 10:�s�[�b�s�s�b	
#define BUZCN			0x0B	// 11:�p��(�����p��)


// ���ײĐF�����
#define C_GRN			0		// 0:��
#define C_YEL			1		// 1:��
#define C_RED			2		// 2:��
#define C_CLR			0xff	// ����


// [C-LEDON] --------------------------
// LED�_�������
#define LEDOF			0x00	// 0:����
#define LEDON			0x01	// 1:�_��
#define LEDBL			0x02	// 2:�_��

// Z����w��
#define ZERPOS			0		// ����PZ���h�a�����ʒu�ړ�
#define SUCPOS			1		// ����PZ���h�a�u�z���v�ʒu�ړ�     Zpos1 Z1
#define DOTPOS			2		// ����PZ���h�a�u�f�o�v�ʒu�ړ�     Zpos2 Z1
#define BLWPOS			3		// �u���[�A�E�g�G�A�����ړ�         Zpos3 Z1
#define SUCSTA			4		// ����PZ���h�a�u�z���v�J�n�����ړ� Zpos4 Z1
#define SUCEND			5		// ����PZ���h�a�u�z���v�I�������ړ� Zpos5 Z2
#define DOTSTA			6		// ����PZ���h�a�u�f�o�v�I�������ړ� Zpos4 Z3
#define DOTEND			7		// ����PZ���h�a�u�f�o�v�J�n�����ړ� Zpos5 Z1
#define TIPPOS			8		// �`�b�v�^�b�`�����ړ�             Touchz1 Z1
#define CUTPOS			9		// �t�؂�z�������ړ�               Cutz1 Z1






// �^�b�`�p�l���ʐM --------------------------

#define TP_ID_PROC_MAX	0x70	// 




// EEPROM ID --------------------------
#define EEPROM_SIZE_MAX		128


#define EEPROM_PAGE0_ADR	0x0000		// (   0 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE1_ADR	0x0080		// (   1 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE2_ADR	0x0100		// (   2 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE3_ADR	0x0180		// (   3 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE4_ADR	0x0200		// (   4 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE5_ADR	0x0280		// (   5 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE6_ADR	0x0300		// (   6 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE7_ADR	0x0380		// (   7 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE8_ADR	0x0400		// (   8 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE9_ADR	0x0480		// (   9 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE10_ADR	0x0500		// (  10 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE90_ADR	0x2D00		// (  90 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE91_ADR	0x2D80		// (  91 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE92_ADR	0x2E00		// (  92 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE93_ADR	0x2E80		// (  93 * EEPROM_SIZE_MAX )
#define EEPROM_PAGE94_ADR	0x2F00		// (  94 * EEPROM_SIZE_MAX )

#define EEPROM_PAGE100_ADR	0x3200		// ( 100 * EEPROM_SIZE_MAX )// �v���g�R���A�h���X�J�n�y�[�W�A�h���X
#define EEPROM_PAGE480_ADR	0xf000		// ( 480 * EEPROM_SIZE_MAX )// �v���g�R���A�h���X�ŏI�y�[�W�A�h���X

#define EEPROM_ALL_INFO		0x00		// Page0�`Page1,Page2�`Page5,Page6�`Page7
#define EEPROM_USER_INFO	0x01		// Page0�`Page1
#define EEPROM_SPED_PTRN	0x02		// Page2�`Page6
//#define EEPROM_SPED_SETT	0x04		// Page7
#define EEPROM_POSI_INFO	0x04		// Page8�`


#define EEPROM_PROT_INFO	0x10		// Page100�`




// �v���g�R�� --------------------------

#define	SRAM_START_ADDRESS			0x0C000000
#define PROTCOL_MAX		80
#define NOZZLE_MAX		12
#define PRESSMP_MAX		10
#define PRESSMP_WAIT	20
#define POMPAFT_WAIT	5
#define TEMP_CHK_WAIT	500
#define NEXTSTEP_WAIT	1


#define P_MSK		0x0F
#define S_MSK		0xD0
#define P_STA		0x00
#define P_NON		0x00
#define P_TGT		0x01
#define P_TDS		0x02
#define P_OPN		0x03
#define P_PRS		0x04
#define P_WAI		0x05
#define P_MIX		0x06
#define P_SUC		0x07
#define P_PUT		0x08
#define P_PR1		0x0A
#define P_PR2		0x0B
#define P_INC		0x0C
#define P_MOV		0x0D

#define STRIP_LY	0x00
#define STRIP_EB	0x01
#define STRIP_R1	0x02
#define STRIP_R2	0x03
#define STRIP_R3	0x04
#define STRIP_R4	0x05
#define STRIP_R5	0x06
#define STRIP_R6	0x07
#define STRIP_SP	0x08
#define STRIP_L1	0x09
#define STRIP_S1	0x0A
#define STRIP_S2	0x0B

#define ER_BUSY    		0x00000001
#define ER_MOTOR    	0x00000002
#define ER_TIPGETFAILE	0x00000004
#define ER_TIPDSPFAILE	0x00000008
#define ER_LIQERR		0x00000010
#define ER_LIQTUMARI	0x00000020
#define ER_BARFAILE		0x00000040

#define PROCESS_LYSI	0x01
#define PROCESS_BILD	0x02
#define PROCESS_WASH	0x03
#define PROCESS_ELUT	0x04

#endif  // _DEFEINE_H_
