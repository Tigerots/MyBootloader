
/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "MyApp.h"
#include "AES.h"


#ifdef USE_STM32L152_EVAL
 #include "stm32l152_eval.h"
#elif defined USE_STM32L152D_EVAL
 #include "stm32l152d_eval.h"
#endif

extern u8 KeyTaskFlag;
extern u8 ScanKey1(void);
extern void IrdaSendKey(u8 *ukeyid, u8 ukeyvalue, u8 *urandom);

u8 KeyValue, HandleP1, HandleP2;
u32 LastOperSecond = 0, CurrSecond = 0;
u8 SysCurrTime[6] = {13,1,19,0,0,0};
u16 ShutDownTime;

u32 LastBusySecond = 0;

/* Private function prototypes -----------------------------------------------*/
extern void IAP_Init(void);

extern void delayms(void);
extern u8 IrdaBuf[150];
u8 WorkKeyData[16];//工作密钥

u8 HardVersion[8]="B1.1";//硬件版本
/*
B1.0: 遗留问题: 1. 红外头向内移动,现在安装壳体时会受影响
				2. 是否取消二极管,压降0.3V比较大,而Flash推荐工作电压为2.7-3.6V
B1.1: 修改按键为上拉,以减小待机电流
B1.2: 三个按键均具有唤醒功能
*/

u8 SoftVersion[8]="V1016";//软件版本
/*
V1016: 修改认证指令,增加写认证,放开检测key存在的CRC,修改创建文件的时间更新
V1015: 写key指令增加CRC数据校验(同时需要升级彩票机和动态库)
V1014: 修改删除机制,使删除后的空间可以再利用 
V1013: 待机时间判断依据有原来的实时钟修改为定时器计数值,使进入待机的时间准确
V1010-V1012:都有初始化系统,证书被破坏的问题,怀疑看门狗及待机检测的问题
V1012: 修改由于查找文件返回-1导致的证书问题
V1011: 增加Flash格式化等待时间,中间喂狗,严格判断二进制文件的长度
V1010: 缩短格式化时间,修改文件超过最大值后的返回处理
V1009: 格式化过程中喂狗
V1008: 增加看门狗, 打开BOR
//---------------------------------------------------
1001-t1: 测试版本1
1001-t2: 修改设备ID读取错误
1001-t3: 修改二进制文件读取少一个字节的问题, 写记录文件少于记录长度,则补0
1001-t4: 增加时钟校准
1001-t5: 待机启动后,直接发送OK键,增加CRC函数(暂时屏蔽),KDF只有钥匙key可以读取
1001-t6: 修改key认证流程,按键时发送的信息用工作密钥加密
1001-t7: 修改按键控制,高低电平修改
1001-t8: 增加PVD功能,有key信息结构传输
1001-t9: 测试,未发行
1002-t1: 测试,未发行
1002-t2: 增加一次读取多条记录
1002-t3: 修改一次写多条记录的限制错误
1002-t4: 增加定时器TIM2, 数据处理200ms后才响应按键操作,时钟调快两个数,=134
1002-t5: 缩短key的扫描去抖时间
1002-t6: 不检测key抬起,按下状态同样可以进行通信操作
*/
u8 SysBusyFlag;//系统忙碌标志,忙碌时不执行按键操作
u32 rand1=0,rand2=0;



//判断是否是闰年
u8 Is_LeapYear(u16 year)
{
	if( (year%4==0)&& (year%100!=0)||(year%400==0))
		return 1;
	else
		return 0;
}
//获取当前秒
u8 YFlag=0;
u8 MFlag=0;
u8 DFlag=0;
u16 Yeartmp;
//u32 SysSecond;
//u8 Second;
u8 GetCurrSecond(void)
{
	RTC_TimeShow();//获取当前时间
	
	SysCurrTime[5] = RTC_TimeStructure.RTC_Seconds;
	SysCurrTime[4] = RTC_TimeStructure.RTC_Minutes;
	SysCurrTime[3] = RTC_TimeStructure.RTC_Hours;
	SysCurrTime[2] = RTC_DateStructure.RTC_Date;
	SysCurrTime[1] = RTC_DateStructure.RTC_Month;
	SysCurrTime[0] = RTC_DateStructure.RTC_Year;
	
	return RTC_TimeStructure.RTC_Seconds;
}
//设置时钟
extern void RTC_TimeRegulate(void);
void SetCurrSecond(void)
{
	RTC_TimeRegulate(); 
}

//RSA AES CRC
/*******************************************************************************
* Function Name  : Get_Unique_Device_ID
* Description    : 得到设备唯一ID号
				   96位的独特ID位于地址 0x1FFFF7E8 ~ 0x1FFFF7F4 的系统存储区 
				   由ST在工厂中写入(用户不能修改) 用户可以以字节、半字、或字的方式单独读取其间的任一地址
* Input          : *ID_Buffer  ID号存储缓存
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
void Get_Unique_Device_ID(u8 *Buffer)
{
	u8 i;
	u16 tmp;
	u32 FlashDestination; // Flash user program offset
	u8 ID_Buffer[12];

	//Flash上IC虚拟目标地址
	FlashDestination = 0x1FF80050;
	//密钥占4字节
	for(i=0;i<12;i++)
	{
		ID_Buffer[i]= *(u8*)FlashDestination;
		FlashDestination++;
	}
	tmp = QuickCRC16((unsigned char * )&ID_Buffer[0], 6);
	Buffer[0] = tmp & 0xff;
	Buffer[1] = tmp>>8 & 0xff;
	tmp = QuickCRC16((u8 *)&ID_Buffer[6], 6);
	Buffer[2] = tmp & 0xff;
	Buffer[3] = tmp>>8 & 0xff;
}
void StandBySet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);//使能定时器2
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, DISABLE); 
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, DISABLE);//禁止看门狗
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;	// 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


extern u32 TimeCnt;
u32 GetTick(void)
{
	return TimeCnt;
}

s16 IrdaProtocolHandle(u8 *ProData, u8 Cnt)
{
	u16 ret=0x9000;
	u8 ulen=0;
	u8 RandData[8];//随机数

	u16 tCrc1=0,tCrc2=0;
	
	if(ProData[0] == 0)//CLA
	{
		if((ProData[1] != 0xB5) && (ProData[1] != 0x0A))//升级数据不进行CRC校验
		{
			#ifdef __USECRC
			{
				tCrc1 = QuickCRC16((u8 *)&ProData[0], ProData[4]+5);//CRC校验
				tCrc2 = ProData[5+ProData[4]] | ProData[6+ProData[4]]<<8;
				if(tCrc1 != tCrc2)
				{//crc错误, 直接返回
					IrdaAsk((u8 *)0, 0, CrcErr);//成功
					return -1;
				}
			}
			#endif
		}
		if(1)
		{
			if(1)
			{
				switch(ProData[1])//INS
				{
					case 0x01://创建二进制文件
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							GetCurrSecond();
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key程序存储区, FIDH=00,FIDL=01~03
							{
								//不需要创建?
								ret = CreatBinFileEeprom(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]));
								IrdaAsk((u8 *)0, 0, ret);//成功
							}
							else
							{
								ret = CreatBinFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]));
							}
							IrdaAsk((u8 *)0, 0, ret);
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					case 0x02://创建定长记录文件
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							GetCurrSecond();
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key程序存储区, FIDH=00,FIDL=01~03
							{//不能创建 FID=1,2,3的记录
								IrdaAsk((u8 *)0, 0, 0x0107);//成功
							}
							else
							{
								ret = CreatRecordFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), ProData[9]);
								IrdaAsk((u8 *)0, 0, ret);//返回错误类型,发送至原设备
							}
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					case 0x03://写二进制文件
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key程序存储区, FIDH=00,FIDL=01~03
							{
								ret = WriteKeySelfFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), (ProData[9]), (u8 *)&ProData[10]);
								ReadKeySelfFile(0xff, 0xff, 0, 0, (u8 *)ret);
							}
							else
							{
								ret = WriteBinFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), (ProData[9]), (u8 *)&ProData[10]);
							}
							IrdaAsk((u8 *)0, 0, ret);
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					case 0x04://读二进制文件
						ulen = ProData[9];
						if(ulen > 128)
						{
							IrdaAsk((u8 *)0, 0, FileLenErr);
						}
						else
						{
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key程序存储区, FIDH=00,FIDL=01~03
							{
								ret = ReadKeySelfFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), (ProData[9]), (u8 *)&IrdaBuf[0]);
							}
							else
							{
								ret = ReadBinFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), (ProData[9]), (u8 *)&IrdaBuf[0]);
							}
							if(ret == CmdOK)
							{
								IrdaAsk((u8 *)&IrdaBuf[0], ulen, 0x9000);
							}
							else
								IrdaAsk((u8 *)0, 0, ret);
						}
					break; 
					case 0x05://写记录(改为追加记录)
						if( ((ProData[2]==HandleP1)&&(ProData[3]==HandleP2)) 
						  ||((ProData[2]==0)&&(ProData[3]==0)))//P1,P2
						{
							ret = WriteRecordFile(ProData[5], ProData[6], 0xFF, ProData[7], (u8 *)&ProData[8]);
							IrdaAsk((u8 *)0, 0, ret);
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					case 0x06://读记录
						ret = ReadRecordFile(ProData[5], ProData[6], ProData[7], ProData[8], (u8 *)&IrdaBuf[0]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x07://查找记录
						ret = FindRecord(ProData[5], ProData[6], ProData[7], ProData[8], (u8 *)&ProData[9]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth+2, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x08://删除记录
						ret = DeleRecord(ProData[5], ProData[6], ProData[7], ProData[8]);
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x09://删除文件
						if((ProData[5] == 0) && (ProData[6]<=0x03))//key程序存储区, FIDH=00,FIDL=01~03
						{
							ret = DeleteSelfFile(ProData[5], ProData[6]);
						}
						else
						{
							ret = DeleteFile(ProData[5], ProData[6]);
						}
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x0A://读ID
						BatteryVol = PWR_GetFlagStatus(PWR_FLAG_PVDO);
						delay20ms();
						memcpy((u8 *)&IrdaBuf[0], SysKeyID, 4);
						memcpy((u8 *)&IrdaBuf[4], HardVersion, 8);
						memcpy((u8 *)&IrdaBuf[12], SoftVersion, 8);
						IrdaBuf[20] = BatteryVol;
						memset((u8 *)&IrdaBuf[21], 0, 3);
						IrdaAsk((u8 *)&IrdaBuf, 24, 0x9000);
					break;
					case 0x0B://读随机数
						rand1=rand();
						rand2=rand();
						memcpy((void *)RandData, (u8 *)&rand1, 4);
						memcpy((void *)&RandData[4], (u8 *)&rand2, 4);
						IrdaAsk((u8 *)&RandData[0], 8, 0x9000);
					break;
					case 0x0C://写卡认证
						memcpy(WorkKeyData, (u8 *)&ProData[5], 16);
						if(KeyTaskFlag == 1)
						{//发送按键后的写认证,重发按键
							KeyTaskFlag = 0;
							IrdaSendKey(SysKeyID, KeyValue, (u8 *)WorkKeyData);
						}
						else
						{
							IrdaAsk((u8 *)&IrdaBuf[0], 0, 0x9000);
						}
					break;
					case 0x16://写卡认证
						HandleP1 = rand(); IrdaBuf[0]=HandleP1;
						HandleP2 = rand(); IrdaBuf[1]=HandleP2;
						IrdaAsk((u8 *)&IrdaBuf[0], 2, 0x9000);
					break;
					case 0x0D://清空记录
						ClearRecord(ProData[5], ProData[6]);
						IrdaAsk((u8 *)0, 0, 0x9000);
					break;
					case 0x0F://读文件属性
						ret = ReadFileInfo(ProData[5], ProData[6]);
						IrdaAsk((u8 *)&stFileAttribute.FileType, sizeof(stFileAttributeType), ret);
					break;
					case 0x10://读时钟
						GetCurrSecond();
						IrdaAsk((u8 *)SysCurrTime, 6, 0x9000);
					break;
					case 0x11://写时钟
						memcpy((u8 *)SysCurrTime, (u8 *)&ProData[5], 6);
						SetCurrSecond();
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x12://设置待机时间
						if((ProData[5] | ProData[6]<<8) < 5)
						{
							IrdaAsk((u8 *)0, 0, 0x01FF);
						}
						else
						{
							ShutDownTime = ProData[5] | ProData[6]<<8;
							IrdaAsk((u8 *)0, 0, 0x9000);
						}
					break;
					case 0x13://AES验证
						Get_Key_Aes_Data(2);
						Data_AES(ENCRYPT, (u8 *)&ProData[5], 16);//AES解密
					
						//memcpy((u8 *)&IrdaBuf[0], (u8 *)&ProData[5], 16); 
						IrdaAsk((u8 *)&ProData[5], 16, 0x9000);
					break;
					
					case 0x14://读多条记录
						ulen = ProData[9];//条数
						ret = ReadRecordFileN(ProData[5], ProData[6], ProData[7], ProData[8], ProData[9], (u8 *)&IrdaBuf[0]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth*ulen, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x15://改为追加多条记录
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							ret = WriteRecordFileN(ProData[5], ProData[6], ProData[7], ProData[8], (u8 *)&ProData[9]);
							IrdaAsk((u8 *)0, 0, ret);
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					
					case 0xB5://写升级文件
						Update_Program_Handle(ProData);
					break;
					case 0xB6://格式化内存,Flash,但保留内存前16字节的标志位
						FormatSystem();
						IrdaAsk((u8 *)0, 0, 0x9000);
					break;
					case 0xB7://读电压
						IrdaAsk((u8 *)0, 0, ret);
					break;
					default:
						SysBusyFlag = 0;
						ret = OtherErr;//INS错误,归结为其他错误
						IrdaAsk((u8 *)0, 0, ret);
					break;
				}	
				LastOperSecond = GetTick();
				LastBusySecond = GetTick();//获取当前计数值,忙碌开始时间
				IWDG_ReloadCounter(); 				
				SysBusyFlag = 1;
				cnt1 = 0;cnt2 = 0;cnt3 = 0;
			}
			else
			{
				SysBusyFlag = 0;
				ret = OtherErr;//协议长度错误,归结为其他错误
				IrdaAsk((u8 *)0, 0, ret);
			}
		}
	}			
	return ret;
}

void IWDGInit(void)
{
	// Enable write access to IWDG_PR and IWDG_RLR registers 
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	// IWDG counter clock: LSI/32 
	//IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetPrescaler(IWDG_Prescaler_256);

	// 
	//内部频率 LSI(=40K)
	//看门狗频率 LSI/256 = 40/256(约150 Hz, 6.5ms)
	//喂狗时间   值(最大0x0fff)*6.5
	//		   如: 256*6.5=1.6秒
	//			   1000*6.5=6.5秒
	//
	IWDG_SetReload(1000);

	//Reload IWDG counter
	IWDG_ReloadCounter();

	//Enable IWDG (the LSI oscillator will be enabled by hardware) 
	IWDG_Enable();
}


//u16 flashid;
extern void TIM2_Configuration(void);

u32 CurrTick;
int main(void)
{ 
	s16 ret;
	u16 IWDGFlag;
	HandleP1=0;
	HandleP2=0;
	
	RCC_DeInit();
	RCC_LSICmd(ENABLE);//使能内部时钟
	
	
    myRTC_Init();
	InterruptConfig();
	
	GPIOInit();
	IAP_Init();
	
	Flash_GPIO_Configuration();
	Check_Flash_Module_Is_Normal();
	SPI_FLASH_Wakeup();
	Get_Unique_Device_ID(SysKeyID);
	
	ReadKeySelfFile(0xff, 0xff, 0, 0, (u8 *)0);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	IWDGFlag = RTC_ReadBackupRegister(RTC_BKP_DR1);
	if (IWDGFlag  != 0xaa55)
	{
		IWDGInit();
	}
	else
	{
		StandBySet();
		RTC_WriteBackupRegister(RTC_BKP_DR1, 0x0);
		SPI_FLASH_DeepSleep();
		KeyTaskFlag = 0;
		PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE); /* Enable WKUP pin 1 */
		PWR_EnterSTANDBYMode();//待机
	}
	TIM2_Configuration();
	
	rand1=rand();
	rand2=rand();
	ShutDownTime = 20;
	LastOperSecond = GetTick();
	CurrSecond = LastOperSecond;
	memset(IrdaBuf, 0, 100);
	
	SysBusyFlag = 0;
	KeyValue=3;
	KeyTaskFlag = 1;
	IrdaSendKey(SysKeyID, KeyValue, (u8 *)WorkKeyData);//唤醒时直接发送OK按键
	
	delay20ms();	
	
	while (1)
	{
		IWDG_ReloadCounter(); 
		CurrTick = GetTick();//获取当前计数值
		if(((CurrTick-LastBusySecond)>=50) && (SysBusyFlag != 0))
		{
			SysBusyFlag = 0;
		}
		if(SysBusyFlag == 0)
		{
			KeyValue=ScanKey1();
			if(KeyValue!=0)
			{
				KeyTaskFlag = 1;
				IrdaSendKey(SysKeyID, KeyValue, (u8 *)WorkKeyData);
				LastOperSecond = GetTick();
			}
		}
		ret = IrdaGetData();
		if(ret != -1)
		{//接收到返回,说明未认证通过
			LastOperSecond = GetTick();
			IWDG_ReloadCounter(); 
			IrdaProtocolHandle(IrdaBuf, ret);
			Rx1Tail=Rx1Head;
		}
		CurrTick = GetTick();//获取当前计数值
		if( (abs(CurrTick-LastOperSecond))>=(ShutDownTime*100) )
		{
			RTC_WriteBackupRegister(RTC_BKP_DR1, 0xaa55);
			System_Reset();//复位系统
		}
	}
}


