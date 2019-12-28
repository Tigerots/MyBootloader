
#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif 

#include "stm32l152d_eval.h"

//#define __USECRC


#define ENCRYPT 0  //加密 
#define DECRYPT 1  //解密


// 定义USART1缓冲区

__MYAPP u32 TimeCnt;

#define Rxd1BufLen 150
__MYAPP uint8_t   Rxd1Buf[Rxd1BufLen];            
__MYAPP uint16_t  Rx1Head;
__MYAPP uint16_t  Rx1Tail;

__MYAPP u8 cnt1,cnt2,cnt3;
__MYAPP void delay20ms(void);

//__MYAPP u8 SysBusyFlag;//系统忙标志
__MYAPP u8 KeyTaskFlag;//有按键按下,但未处理完成
__MYAPP u8 BatteryVol;//电池电压
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











//*********宏定义************************************
//以下定义升级标志
#define UPD_START_FLAG			1			//开始升级标志
#define UPD_NORMAL_FLAG			2			//正在升级标志
#define UPD_END_FLAG			3			//结束升级标志


#define CMD_Update_Program	0xB5			//固件升级

		#define CMD_UPD_FILE_SIZE_ERROR	0x14	//升级文件大小有误
		#define CMD_UPD_FILE_ERROR		0x15	//升级文件格式错误
		#define CMD_UPD_CHECKSUM_ERROR	0x16	//升级文件校验和不对
		#define UPD_FLAG_ERROR			0x17	//升级标志错误
		#define UPD_LENGTH_ERROR		0x18	//升级数据长度错误
		#define FILE_WRITE_ERROR		0x19		//写文件错误


//#define FLASH_OK			CMD_OK
//#define FLASH_ERROR			1

//#define CMD_OK						0x9000
//#define CMD_FAIL					1
//#define CMD_TIMEOUT					1
#define UNSUPPORT_ERROR				0x02			//不支持该命令
#define VERIFY_SUM_ERROR			0x05		//检验和错误
#define LENGTH_VERIFY_SUM_ERROR		0x06		//长度校验和错误
#define CMD_PARAM_ERROR				0x07		//命令参数格式错误

#define UPDATE_FILE_SIZE_ERROR		0x14		//升级文件大小错误
#define UPDATE_FILE_FORMAT_ERROR 	0x15		//升级文件格式错误，无效升级文件
#define UPDATE_FILE_WRITE_ERROR		0x16		//升级文件写错误
#define UPDATE_FILE_SUM_ERROR		0x18		//升级文件校验和错误



__MYAPP void Update_Program_Handle(u8 * UpDateBuffer);


