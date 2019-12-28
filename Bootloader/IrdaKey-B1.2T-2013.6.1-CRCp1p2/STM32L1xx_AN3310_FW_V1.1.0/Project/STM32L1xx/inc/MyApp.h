
#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif 

#include "stm32l152d_eval.h"

//#define __USECRC


#define ENCRYPT 0  //���� 
#define DECRYPT 1  //����


// ����USART1������

__MYAPP u32 TimeCnt;

#define Rxd1BufLen 150
__MYAPP uint8_t   Rxd1Buf[Rxd1BufLen];            
__MYAPP uint16_t  Rx1Head;
__MYAPP uint16_t  Rx1Tail;

__MYAPP u8 cnt1,cnt2,cnt3;
__MYAPP void delay20ms(void);

//__MYAPP u8 SysBusyFlag;//ϵͳæ��־
__MYAPP u8 KeyTaskFlag;//�а�������,��δ�������
__MYAPP u8 BatteryVol;//��ص�ѹ
__MYAPP u8 IrdaBuf[150];
//__MYAPP u8 ReturnBuf[64];


__MYAPP RTC_InitTypeDef   RTC_InitStructure;
__MYAPP RTC_TimeTypeDef   RTC_TimeStructure;
__MYAPP RTC_DateTypeDef	  RTC_DateStructure;
//__MYAPP RTC_AlarmTypeDef  RTC_AlarmStructure;



__MYAPP void delayms(void);
__MYAPP u8 ScanKey1(void);
__MYAPP void IrdaSendKey(u8 *ukeyid, u8 ukeyvalue, u8 *urandom);
__MYAPP void IrdaAsk(u8 *dat, u8 DatCnt, u16 ErrCode);
//MYAPP u8 ScanKey2(void);

__MYAPP void myRTC_Init(void);
__MYAPP void IAP_Init(void);
__MYAPP void InterruptConfig(void);
__MYAPP void GPIOInit(void);
__MYAPP void RTC_AlarmShow(void);
__MYAPP void RTC_TimeShow(void);
__MYAPP void System_Reset(void);











//*********�궨��************************************
//���¶���������־
#define UPD_START_FLAG			1			//��ʼ������־
#define UPD_NORMAL_FLAG			2			//����������־
#define UPD_END_FLAG			3			//����������־


#define CMD_Update_Program	0xB5			//�̼�����

		#define CMD_UPD_FILE_SIZE_ERROR	0x14	//�����ļ���С����
		#define CMD_UPD_FILE_ERROR		0x15	//�����ļ���ʽ����
		#define CMD_UPD_CHECKSUM_ERROR	0x16	//�����ļ�У��Ͳ���
		#define UPD_FLAG_ERROR			0x17	//������־����
		#define UPD_LENGTH_ERROR		0x18	//�������ݳ��ȴ���
		#define FILE_WRITE_ERROR		0x19		//д�ļ�����


//#define FLASH_OK			CMD_OK
//#define FLASH_ERROR			1

//#define CMD_OK						0x9000
//#define CMD_FAIL					1
//#define CMD_TIMEOUT					1
#define UNSUPPORT_ERROR				0x02			//��֧�ָ�����
#define VERIFY_SUM_ERROR			0x05		//����ʹ���
#define LENGTH_VERIFY_SUM_ERROR		0x06		//����У��ʹ���
#define CMD_PARAM_ERROR				0x07		//���������ʽ����

#define UPDATE_FILE_SIZE_ERROR		0x14		//�����ļ���С����
#define UPDATE_FILE_FORMAT_ERROR 	0x15		//�����ļ���ʽ������Ч�����ļ�
#define UPDATE_FILE_WRITE_ERROR		0x16		//�����ļ�д����
#define UPDATE_FILE_SUM_ERROR		0x18		//�����ļ�У��ʹ���



__MYAPP void Update_Program_Handle(u8 * UpDateBuffer);


