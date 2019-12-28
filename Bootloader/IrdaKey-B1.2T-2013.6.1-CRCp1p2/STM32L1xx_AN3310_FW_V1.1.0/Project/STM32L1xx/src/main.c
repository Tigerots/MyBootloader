
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
u8 WorkKeyData[16];//������Կ

u8 HardVersion[8]="B1.1";//Ӳ���汾
/*
B1.0: ��������: 1. ����ͷ�����ƶ�,���ڰ�װ����ʱ����Ӱ��
				2. �Ƿ�ȡ��������,ѹ��0.3V�Ƚϴ�,��Flash�Ƽ�������ѹΪ2.7-3.6V
B1.1: �޸İ���Ϊ����,�Լ�С��������
B1.2: �������������л��ѹ���
*/

u8 SoftVersion[8]="V1016";//����汾
/*
V1016: �޸���ָ֤��,����д��֤,�ſ����key���ڵ�CRC,�޸Ĵ����ļ���ʱ�����
V1015: дkeyָ������CRC����У��(ͬʱ��Ҫ������Ʊ���Ͷ�̬��)
V1014: �޸�ɾ������,ʹɾ����Ŀռ���������� 
V1013: ����ʱ���ж�������ԭ����ʵʱ���޸�Ϊ��ʱ������ֵ,ʹ���������ʱ��׼ȷ
V1010-V1012:���г�ʼ��ϵͳ,֤�鱻�ƻ�������,���ɿ��Ź���������������
V1012: �޸����ڲ����ļ�����-1���µ�֤������
V1011: ����Flash��ʽ���ȴ�ʱ��,�м�ι��,�ϸ��ж϶������ļ��ĳ���
V1010: ���̸�ʽ��ʱ��,�޸��ļ��������ֵ��ķ��ش���
V1009: ��ʽ��������ι��
V1008: ���ӿ��Ź�, ��BOR
//---------------------------------------------------
1001-t1: ���԰汾1
1001-t2: �޸��豸ID��ȡ����
1001-t3: �޸Ķ������ļ���ȡ��һ���ֽڵ�����, д��¼�ļ����ڼ�¼����,��0
1001-t4: ����ʱ��У׼
1001-t5: ����������,ֱ�ӷ���OK��,����CRC����(��ʱ����),KDFֻ��Կ��key���Զ�ȡ
1001-t6: �޸�key��֤����,����ʱ���͵���Ϣ�ù�����Կ����
1001-t7: �޸İ�������,�ߵ͵�ƽ�޸�
1001-t8: ����PVD����,��key��Ϣ�ṹ����
1001-t9: ����,δ����
1002-t1: ����,δ����
1002-t2: ����һ�ζ�ȡ������¼
1002-t3: �޸�һ��д������¼�����ƴ���
1002-t4: ���Ӷ�ʱ��TIM2, ���ݴ���200ms�����Ӧ��������,ʱ�ӵ���������,=134
1002-t5: ����key��ɨ��ȥ��ʱ��
1002-t6: �����key̧��,����״̬ͬ�����Խ���ͨ�Ų���
*/
u8 SysBusyFlag;//ϵͳæµ��־,æµʱ��ִ�а�������
u32 rand1=0,rand2=0;



//�ж��Ƿ�������
u8 Is_LeapYear(u16 year)
{
	if( (year%4==0)&& (year%100!=0)||(year%400==0))
		return 1;
	else
		return 0;
}
//��ȡ��ǰ��
u8 YFlag=0;
u8 MFlag=0;
u8 DFlag=0;
u16 Yeartmp;
//u32 SysSecond;
//u8 Second;
u8 GetCurrSecond(void)
{
	RTC_TimeShow();//��ȡ��ǰʱ��
	
	SysCurrTime[5] = RTC_TimeStructure.RTC_Seconds;
	SysCurrTime[4] = RTC_TimeStructure.RTC_Minutes;
	SysCurrTime[3] = RTC_TimeStructure.RTC_Hours;
	SysCurrTime[2] = RTC_DateStructure.RTC_Date;
	SysCurrTime[1] = RTC_DateStructure.RTC_Month;
	SysCurrTime[0] = RTC_DateStructure.RTC_Year;
	
	return RTC_TimeStructure.RTC_Seconds;
}
//����ʱ��
extern void RTC_TimeRegulate(void);
void SetCurrSecond(void)
{
	RTC_TimeRegulate(); 
}

//RSA AES CRC
/*******************************************************************************
* Function Name  : Get_Unique_Device_ID
* Description    : �õ��豸ΨһID��
				   96λ�Ķ���IDλ�ڵ�ַ 0x1FFFF7E8 ~ 0x1FFFF7F4 ��ϵͳ�洢�� 
				   ��ST�ڹ�����д��(�û������޸�) �û��������ֽڡ����֡����ֵķ�ʽ������ȡ������һ��ַ
* Input          : *ID_Buffer  ID�Ŵ洢����
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void Get_Unique_Device_ID(u8 *Buffer)
{
	u8 i;
	u16 tmp;
	u32 FlashDestination; // Flash user program offset
	u8 ID_Buffer[12];

	//Flash��IC����Ŀ���ַ
	FlashDestination = 0x1FF80050;
	//��Կռ4�ֽ�
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);//ʹ�ܶ�ʱ��2
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, DISABLE); 
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, DISABLE);//��ֹ���Ź�
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;	// 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// �����������
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
	u8 RandData[8];//�����

	u16 tCrc1=0,tCrc2=0;
	
	if(ProData[0] == 0)//CLA
	{
		if((ProData[1] != 0xB5) && (ProData[1] != 0x0A))//�������ݲ�����CRCУ��
		{
			#ifdef __USECRC
			{
				tCrc1 = QuickCRC16((u8 *)&ProData[0], ProData[4]+5);//CRCУ��
				tCrc2 = ProData[5+ProData[4]] | ProData[6+ProData[4]]<<8;
				if(tCrc1 != tCrc2)
				{//crc����, ֱ�ӷ���
					IrdaAsk((u8 *)0, 0, CrcErr);//�ɹ�
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
					case 0x01://�����������ļ�
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							GetCurrSecond();
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key����洢��, FIDH=00,FIDL=01~03
							{
								//����Ҫ����?
								ret = CreatBinFileEeprom(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]));
								IrdaAsk((u8 *)0, 0, ret);//�ɹ�
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
					case 0x02://����������¼�ļ�
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							GetCurrSecond();
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key����洢��, FIDH=00,FIDL=01~03
							{//���ܴ��� FID=1,2,3�ļ�¼
								IrdaAsk((u8 *)0, 0, 0x0107);//�ɹ�
							}
							else
							{
								ret = CreatRecordFile(ProData[5], ProData[6], (ProData[7]<<8|ProData[8]), ProData[9]);
								IrdaAsk((u8 *)0, 0, ret);//���ش�������,������ԭ�豸
							}
						}
						else
						{
							ret = P1P2Err;
							IrdaAsk((u8 *)0, 0, ret);
						}
					break;
					case 0x03://д�������ļ�
						if((ProData[2]==HandleP1)&&(ProData[3]==HandleP2))//P1,P2
						{
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key����洢��, FIDH=00,FIDL=01~03
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
					case 0x04://���������ļ�
						ulen = ProData[9];
						if(ulen > 128)
						{
							IrdaAsk((u8 *)0, 0, FileLenErr);
						}
						else
						{
							if((ProData[5] == 0) && (ProData[6]<=0x03))//key����洢��, FIDH=00,FIDL=01~03
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
					case 0x05://д��¼(��Ϊ׷�Ӽ�¼)
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
					case 0x06://����¼
						ret = ReadRecordFile(ProData[5], ProData[6], ProData[7], ProData[8], (u8 *)&IrdaBuf[0]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x07://���Ҽ�¼
						ret = FindRecord(ProData[5], ProData[6], ProData[7], ProData[8], (u8 *)&ProData[9]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth+2, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x08://ɾ����¼
						ret = DeleRecord(ProData[5], ProData[6], ProData[7], ProData[8]);
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x09://ɾ���ļ�
						if((ProData[5] == 0) && (ProData[6]<=0x03))//key����洢��, FIDH=00,FIDL=01~03
						{
							ret = DeleteSelfFile(ProData[5], ProData[6]);
						}
						else
						{
							ret = DeleteFile(ProData[5], ProData[6]);
						}
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x0A://��ID
						BatteryVol = PWR_GetFlagStatus(PWR_FLAG_PVDO);
						delay20ms();
						memcpy((u8 *)&IrdaBuf[0], SysKeyID, 4);
						memcpy((u8 *)&IrdaBuf[4], HardVersion, 8);
						memcpy((u8 *)&IrdaBuf[12], SoftVersion, 8);
						IrdaBuf[20] = BatteryVol;
						memset((u8 *)&IrdaBuf[21], 0, 3);
						IrdaAsk((u8 *)&IrdaBuf, 24, 0x9000);
					break;
					case 0x0B://�������
						rand1=rand();
						rand2=rand();
						memcpy((void *)RandData, (u8 *)&rand1, 4);
						memcpy((void *)&RandData[4], (u8 *)&rand2, 4);
						IrdaAsk((u8 *)&RandData[0], 8, 0x9000);
					break;
					case 0x0C://д����֤
						memcpy(WorkKeyData, (u8 *)&ProData[5], 16);
						if(KeyTaskFlag == 1)
						{//���Ͱ������д��֤,�ط�����
							KeyTaskFlag = 0;
							IrdaSendKey(SysKeyID, KeyValue, (u8 *)WorkKeyData);
						}
						else
						{
							IrdaAsk((u8 *)&IrdaBuf[0], 0, 0x9000);
						}
					break;
					case 0x16://д����֤
						HandleP1 = rand(); IrdaBuf[0]=HandleP1;
						HandleP2 = rand(); IrdaBuf[1]=HandleP2;
						IrdaAsk((u8 *)&IrdaBuf[0], 2, 0x9000);
					break;
					case 0x0D://��ռ�¼
						ClearRecord(ProData[5], ProData[6]);
						IrdaAsk((u8 *)0, 0, 0x9000);
					break;
					case 0x0F://���ļ�����
						ret = ReadFileInfo(ProData[5], ProData[6]);
						IrdaAsk((u8 *)&stFileAttribute.FileType, sizeof(stFileAttributeType), ret);
					break;
					case 0x10://��ʱ��
						GetCurrSecond();
						IrdaAsk((u8 *)SysCurrTime, 6, 0x9000);
					break;
					case 0x11://дʱ��
						memcpy((u8 *)SysCurrTime, (u8 *)&ProData[5], 6);
						SetCurrSecond();
						IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x12://���ô���ʱ��
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
					case 0x13://AES��֤
						Get_Key_Aes_Data(2);
						Data_AES(ENCRYPT, (u8 *)&ProData[5], 16);//AES����
					
						//memcpy((u8 *)&IrdaBuf[0], (u8 *)&ProData[5], 16); 
						IrdaAsk((u8 *)&ProData[5], 16, 0x9000);
					break;
					
					case 0x14://��������¼
						ulen = ProData[9];//����
						ret = ReadRecordFileN(ProData[5], ProData[6], ProData[7], ProData[8], ProData[9], (u8 *)&IrdaBuf[0]);
						if(ret == CmdOK)
						{
							IrdaAsk((u8 *)&IrdaBuf[0], RecordFileAttributes.RecordLenth*ulen, 0x9000);
						}
						else
							IrdaAsk((u8 *)0, 0, ret);
					break;
					case 0x15://��Ϊ׷�Ӷ�����¼
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
					
					case 0xB5://д�����ļ�
						Update_Program_Handle(ProData);
					break;
					case 0xB6://��ʽ���ڴ�,Flash,�������ڴ�ǰ16�ֽڵı�־λ
						FormatSystem();
						IrdaAsk((u8 *)0, 0, 0x9000);
					break;
					case 0xB7://����ѹ
						IrdaAsk((u8 *)0, 0, ret);
					break;
					default:
						SysBusyFlag = 0;
						ret = OtherErr;//INS����,���Ϊ��������
						IrdaAsk((u8 *)0, 0, ret);
					break;
				}	
				LastOperSecond = GetTick();
				LastBusySecond = GetTick();//��ȡ��ǰ����ֵ,æµ��ʼʱ��
				IWDG_ReloadCounter(); 				
				SysBusyFlag = 1;
				cnt1 = 0;cnt2 = 0;cnt3 = 0;
			}
			else
			{
				SysBusyFlag = 0;
				ret = OtherErr;//Э�鳤�ȴ���,���Ϊ��������
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
	//�ڲ�Ƶ�� LSI(=40K)
	//���Ź�Ƶ�� LSI/256 = 40/256(Լ150 Hz, 6.5ms)
	//ι��ʱ��   ֵ(���0x0fff)*6.5
	//		   ��: 256*6.5=1.6��
	//			   1000*6.5=6.5��
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
	RCC_LSICmd(ENABLE);//ʹ���ڲ�ʱ��
	
	
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
		PWR_EnterSTANDBYMode();//����
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
	IrdaSendKey(SysKeyID, KeyValue, (u8 *)WorkKeyData);//����ʱֱ�ӷ���OK����
	
	delay20ms();	
	
	while (1)
	{
		IWDG_ReloadCounter(); 
		CurrTick = GetTick();//��ȡ��ǰ����ֵ
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
		{//���յ�����,˵��δ��֤ͨ��
			LastOperSecond = GetTick();
			IWDG_ReloadCounter(); 
			IrdaProtocolHandle(IrdaBuf, ret);
			Rx1Tail=Rx1Head;
		}
		CurrTick = GetTick();//��ȡ��ǰ����ֵ
		if( (abs(CurrTick-LastOperSecond))>=(ShutDownTime*100) )
		{
			RTC_WriteBackupRegister(RTC_BKP_DR1, 0xaa55);
			System_Reset();//��λϵͳ
		}
	}
}


