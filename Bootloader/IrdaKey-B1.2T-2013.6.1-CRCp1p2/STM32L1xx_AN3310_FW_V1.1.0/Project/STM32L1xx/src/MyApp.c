#define MYAPP

#include    <string.h>
#include 	"MyApp.h"
#include 	"common.h"
#include	"SpiFlash.h"
#include 	"flash_if.h"
#include 	"crc.h"
#include 	"aes.h"
#include "stm32l1xx_tim.h"

uint32_t AsynchPrediv=0, SynchPrediv=0;

void delayms(void)
{
	u16 i,j;
	for(i=0; i<100; i++)
	{
		for(j=0; j<200; j++)
		{}
	}
}

void delay20ms(void)
{
	u16 i,j;
	for(i=0; i<1; i++)
	{
		for(j=0; j<200; j++)
		{}
	}
}
#define KeyEffect	1//��PCBʹ��1, (>=V1.2)
//#define NewPCB



//����ɨ�����
u8 ScanKey1(void)
{
	u8 key1,key2,key3;
	
#ifndef NewPCB
	key1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
	if(key1 == KeyEffect)
	{
		cnt1++;
		if(cnt1 > 100)
		{
			return 3;
		}
	}
	else
	{
		cnt1 = 0;
	}
#else
	key1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
	if(key1 == KeyEffect)
	{
		cnt1++;
		if(cnt1 > 100)
		{
			return 3;
		}
	}
	else
	{
		cnt1 = 0;
	}
#endif
	key2 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7); 
	if(key2 == KeyEffect)
	{
		cnt2++;
		if(cnt2 > 100)
		{
			return 2;
		}
	}
	else
	{
		cnt2 = 0;
	}
	key3 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
	if(key3 == KeyEffect)
	{
		cnt3++;
		if(cnt3 > 100)
		{
			return 1;
		}
	}
	else
	{
		cnt3 = 0;
	}

	return 0;
}

/*********************************************************
��������: 	ͨ�����ⷢ�ͼ�ֵ
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

**********************************************************/ 
void IrdaSendKey(u8 *ukeyid, u8 ukeyvalue, u8 *urandom)
{
	u8 Buffer[24];

	Buffer[0] = 0xFF;//CLA
	Buffer[1] = 0x01;//INS
	Buffer[2] = 0x00;//P1
	Buffer[3] = 0x00;//P2
	Buffer[4] = 0x0D;//P3=13�ֽ�
	Buffer[5] = ukeyid[0];//keyid
	Buffer[6] = ukeyid[1];//keyid
	Buffer[7] = ukeyid[2];//keyid
	Buffer[8] = ukeyid[3];//keyid
	Buffer[9] = ukeyvalue;//keyvalue
	memcpy((u8 *)&Buffer[10], urandom, 8);
	memset((u8 *)&Buffer[18], 0, 3);
	
	Get_Key_Aes_Data(2);
	Data_AES(ENCRYPT, (u8 *)&Buffer[5], 16);//AES����
	
	 
	Buffer[21] = 0x90;
	Buffer[22] = 0x00;
	MyPrintf1(Buffer, 23);
	
}

/*********************************************************
��������: 	ͨ�����ⷢ�ͼ�ֵ
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

**********************************************************/ 
void IrdaAsk(u8 *dat, u8 DatCnt, u16 ErrCode)
{
	u8 Buffer[140];
//	u16 tCrc1=0;
	
	if(DatCnt != 0)
	{
		memcpy(Buffer, dat, DatCnt);
		#ifdef __USECRCSEND
			tCrc1 = QuickCRC16(dat, DatCnt);//CRCУ��
			Buffer[DatCnt+0] = (u8)(tCrc1&0xff);
			Buffer[DatCnt+1] = (u8)((tCrc1>>8)&0xff);
			
			Buffer[DatCnt+2] = (u8)((ErrCode>>8)&0xff);
			Buffer[DatCnt+3] = (u8)(ErrCode&0xff);
			
			MyPrintf1(Buffer, DatCnt+4);
		#else
			Buffer[DatCnt+0] = (u8)((ErrCode>>8)&0xff);
			Buffer[DatCnt+1] = (u8)(ErrCode&0xff);
			
			MyPrintf1(Buffer, DatCnt+2);
		#endif
	}
	else
	{
		Buffer[0] = (u8)((ErrCode>>8)&0xff);
		Buffer[1] = (u8)(ErrCode&0xff);
		MyPrintf1(Buffer, 2);
	}
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
extern u8 SysCurrTime[6];
void RTC_TimeShow(void)
{
  /* Get the current Time */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_AlarmShow(void)
{
  /* Get the current Alarm */
//  RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  //printf("\n\r  The current alarm is :  %0.2d:%0.2d:%0.2d \n\r", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}


void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);
    
/* The RTC Clock may varies due to LSI frequency dispersion. */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
	RTC_DateStructure.RTC_Year = SysCurrTime[0];
	RTC_DateStructure.RTC_Month = SysCurrTime[1];
	RTC_DateStructure.RTC_Date = SysCurrTime[2];
	RTC_TimeStructure.RTC_Hours = SysCurrTime[3];
	RTC_TimeStructure.RTC_Minutes = SysCurrTime[4];
	RTC_TimeStructure.RTC_Seconds = SysCurrTime[5];
	
	if(SysCurrTime[3]>=12)
	{
		RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
	}
	else
	{
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	}
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
	{
		
	} 
	else
	{
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x0120);
		RTC_TimeShow();
	}
}

void GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
#ifndef NewPCB
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;//key1
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#else
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;//key1, 2013.4.7�޸�
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;//key2
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;//key3
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;//SD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

void InterruptConfig(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	
	#ifdef  VECT_TAB_RAM  
		/* Set the Vector Table base location at 0x20000000 */ 
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
		/* Set the Vector Table base location at 0x08000000 */ 
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
	#endif 

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*  
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	  
	EXTI_DeInit();
	EXTI_StructInit( &EXTI_InitStructure );
	
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);*/
	

	PWR_PVDLevelConfig(PWR_PVDLevel_2V6);
	PWR_PVDCmd(ENABLE);
	
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
}


void IAP_Init(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
}

uint32_t second;
#define SyncTime 0x131 //0x133 //��ֵԽС, ʱ�����Խ��, 2013.3.31, ����ΰȷ��
void myRTC_Init(void)
{	
	RTC_DeInit();	
	
//	LsiFreq = GetLSIFrequency();
	
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv	= SyncTime;  //(LsiFreq/128) - 1;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	//RTC_TimeShow();
	second = RTC_ReadBackupRegister(RTC_BKP_DR0);
	if (second  != 0x0120)
	{  
		
		/* RTC configuration  */
		RTC_Config();

		/* Configure the RTC data register and RTC prescaler */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv	= SyncTime;  //(LsiFreq/128) - 1;//0xff,ԽСԽ��
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

		/* Check on RTC init */
		second = RTC_Init(&RTC_InitStructure);
		if (second == ERROR)
		{
		//     printf("\n\r/!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
		}

		/* Configure the time register */
		RTC_TimeRegulate(); 
	}
	else
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		//      printf("\r\n Power On Reset occurred....\n\r");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
		//     printf("\r\n External Reset occurred....\n\r");
		}
		/* Enable the PWR clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		/* Allow access to RTC */
		PWR_RTCAccessCmd(ENABLE);
		/* Wait for RTC APB registers synchronisation */
		RTC_WaitForSynchro();
	}
	/* Clear WakeUp flag */
	PWR_ClearFlag(PWR_FLAG_WU);

	/* Check if the StandBy flag is set */
	if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
	{
		/* Clear StandBy flag */
		PWR_ClearFlag(PWR_FLAG_SB);
		RTC_WaitForSynchro();
	} 
	//RTC_CoarseCalibConfig(RTC_CalibSign_Negative, 120);
	//RTC_CoarseCalibCmd(ENABLE);
	//RTC_CoarseCalibConfig(RTC_CalibSign_Negative, 120);

}


void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* TIM11 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2
	
	/* Time base configuration *///=1000ʱԼ20ms��ʱ
	TIM_TimeBaseStructure.TIM_Period = 1000; // �����Զ����ؼĴ���ֵΪ���ֵ,=10000ʱԼ��ʱ250ms
	TIM_TimeBaseStructure.TIM_Prescaler = 38;  // �Զ���Ԥ��Ƶ,LSIΪ26-56K ����ֵ38K
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ����ģʽ
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	
	/* ����жϱ�־λ */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	/*Ԥװ�ؼĴ��������ݱ��������͵�Ӱ�ӼĴ��� */
	TIM_ARRPreloadConfig(TIM2, DISABLE);
	/* ���ж�   ����ж� */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  
	/* TIM2 enable counter */
 	TIM_Cmd(TIM2, ENABLE);
}




/*******************************************************************************
* Function Name  : USB_CMD_Respond
* Description    : USB������Ӧ
* Input          : cmd:������
*				   Result:���������ɹ���ʧ��	
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/
void USB_CMD_Respond(u8 cmd,u16 Result)
{
	IrdaAsk((u8 *)0, 0, Result);
}

/*******************************************************************************
* Function Name  : FeedWtchDog
* Description    : ι��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void System_Reset(void)
{
 /* Enable WWDG clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
      /* Generate a system Reset to re-load the new option bytes: enable WWDG and set
        counter value to 0x4F, as T6 bit is cleared this will generate a WWDG reset */
      WWDG_Enable(0x4F);
	while(1);
}
/*******************************************************************************
* Function Name  : User_Erase_Update_Data
* Description    : �û�ɾ����������
* Input          : None
* Output		 : None
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
u32 BlockNbr = 0, UserMemoryMask = 0;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
u32 FlashDestination = ApplicationAddress; // Flash user program offset
u32 UpdateFW_Offset_Addr;	//�̼�����ƫ�Ƶ�ַ
void User_Erase_Update_Data(void)
{
	//��������Flash
	//FLASH_Unlock();
	FLASH_If_Init();

	
	//����Ҫд�ĵ�ַ��������
	BlockNbr = (UPDATE_DATA_ADDR - 0x08000000) >> 12;
	//ʹ��STM3210BоƬʱ��Eϵ�м��㷽����ͬ
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//ȡ��д����
	FLASH_If_DisableWriteProtection();
	//Flash��дĿ���ַ
	FlashDestination = UPDATE_DATA_ADDR;

	NbrOfPage = FLASH_PagesMask(UPDATE_DATE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		//FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
		FLASH_If_Erase(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
}
/*******************************************************************************
* Function Name  : User_Write_Update_Data
* Description    : �û�д��������
* Input          :  addr:Ҫд��ĵ�ַ
*		   			  n:Ҫд��ĸ���
* Output		 :*pt: �洢д�����ݻ����ָ��;
* Return         : FLASH_OK:�ɹ�;other��ʧ��
* Ӱ ��		 	 ��None
*******************************************************************************/
unsigned char User_Write_Update_Data(int addr, unsigned char  *pt, unsigned int  n)
{

	u32  iTemp;
	u32  j;

	//��ַ����Χʱ��������ʼ��ַ����Χ����,	
	if(addr>=UPDATE_DATE_SIZE)		return FLASH_ERROR;
	//��ʼ��ַ�ͳ���֮�ͳ���Χʱ
	iTemp	=	addr+n;
	if(iTemp>=UPDATE_DATE_SIZE)	return FLASH_ERROR;	

  	//Flash��дĿ���ַ
  	FlashDestination = UPDATE_DATA_ADDR+addr;
	
	//��д����ʼ
	RamSource = (u32)(&pt[0]);
	FLASH_If_Init();
	//ȡ��д����
	FLASH_If_DisableWriteProtection();
	for (j = 0;(j < n) && (FlashDestination <  UPDATE_DATA_ADDR + UPDATE_DATE_SIZE);j += 4)
	{
		//FLASH_FastProgramWord(&FlashDestination, (uint32_t*)RamSource)
		/* Program the data received into STM32F10x Flash */
		FLASHStatus = FLASH_FastProgramWord(FlashDestination, *(u32*)RamSource);
		if (*(u32*)FlashDestination != *(u32*)RamSource)
		{
			return FLASH_ERROR;
		}
		FlashDestination += 4;
		RamSource += 4;
	}
	return FLASH_OK;
}

/*******************************************************************************
* Function Name  : Write_Update_Finish_Flag
* Description    : д������ɱ�־
* Input          : None
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void Write_Update_Finish_Flag(void)
{
	u8 Flash_Buffer[0x20]={0};
	u32 j;

	//��������Flash
	FLASH_If_Init();

	// ���ݵ�ַ��������
	BlockNbr = (UPDATE_FLAG_ADDR - 0x08000000) >> 12;
	// ʹ��STM3210BоƬʱ��Eϵ�м��㷽����ͬ
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//ȡ��д����
	FLASH_If_DisableWriteProtection();

	//����Ҫ�޸ĵ�����
	Flash_Buffer[0]		=(UPDATE_FLAG_GO&0xff);
  	Flash_Buffer[1]		=((UPDATE_FLAG_GO>>8)&0xff);
	Flash_Buffer[2]		=((UPDATE_FLAG_GO>>16)&0xff);
	Flash_Buffer[3]		=((UPDATE_FLAG_GO>>24)&0xff);
//	Flash_Buffer[4]
//	Flash_Buffer[5]

  	//Flash��дĿ���ַ
	FlashDestination = UPDATE_FLAG_ADDR;
	
	//������ռ��ҳ����
	FLASH_If_Erase(FlashDestination);
	RamSource = (u32)(&Flash_Buffer[0]);
	//��д����ʼ
	FLASH_If_Init();
	//ȡ��д����
	FLASH_If_DisableWriteProtection();
	for (j = 0; (j < UPDATE_FLAG_SIZE) && (FlashDestination <  UPDATE_FLAG_ADDR+UPDATE_FLAG_SIZE); j+=4)
	{
		FLASHStatus = FLASH_FastProgramWord(FlashDestination, *(u32*)RamSource);
		if (*(u32*)FlashDestination != *(u32*)RamSource)
		{
			return;
		}
		FlashDestination += 4;
		RamSource += 4;
	}
	return;
}
/*******************************************************************************
* Function Name  : Update_Program_Handle
* Description    : ����������
* Input          : None
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/
u32 UpdateFW_Offset_Addr;
u32 UpdateFW_Total_Addr;
u8 LED_Display_Cache_Down;

//u8 UpDateBuffer[64];


u32 Update_CheckSum;	//���������
u32 UpdateFW_Address;	//�����̼���ַ
u32 UpdateFW_Rec_Size;	//�Ѿ����յ������ļ���С
u32 UpdateFW_Total_Sector;	//�����̼�������������

#define ProOffset	6  //��ͬЭ��λ��ƫ��
void Update_Program_Handle(u8 * UpDateBuffer)
{
	u8 upd_flag;	//������־
	u16 Data_len,i;	//���ݳ���
	u32 chip,app_size;
	u32 device;
	u32 iTemp;
	u8 Update_Buffer[128];

   	Data_len	=		UpDateBuffer[4];//���128�ֽ�
	upd_flag 	=  		UpDateBuffer[3];
   	if(upd_flag==UPD_START_FLAG)	//������ʼ
	{
		UpdateFW_Offset_Addr=0;
		//��鳤���Ƿ���ȷ
		if(Data_len!=FLASH_UPD_SEND_SIZE)
		{
			USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
			return;
		}
		device  = (UpDateBuffer[14-ProOffset] <<24)|(UpDateBuffer[13-ProOffset] <<16)|(UpDateBuffer[12-ProOffset] <<8)|(UpDateBuffer[11-ProOffset]);
		chip	= (UpDateBuffer[22-ProOffset]<<24)|(UpDateBuffer[21-ProOffset]<<16)|(UpDateBuffer[20-ProOffset] <<8)|(UpDateBuffer[19-ProOffset]);
		app_size= (UpDateBuffer[30-ProOffset]<<24)|(UpDateBuffer[29-ProOffset]<<16)|(UpDateBuffer[28-ProOffset]<<8)|(UpDateBuffer[27-ProOffset]);
		UpdateFW_Total_Addr	=	app_size;
		//У���豸�����Ƿ���ȷ
		if(device!=APP_DEVICE_TYPE_HEAD)
		{
			USB_CMD_Respond(CMD_Update_Program,UPDATE_FILE_FORMAT_ERROR);
			return ;
		}
		//���оƬ�ͺ��Ƿ�����
		iTemp = chip & APP_CHIP_TYPE_MASK;
		if(iTemp>0)
		{
			USB_CMD_Respond(CMD_Update_Program,UPDATE_FILE_FORMAT_ERROR);
			return ;
		}
		//�������С�Ƿ���������
		if((app_size<APP_MIN_SIZE)||(app_size>APP_MAX_SIZE))
		{
			USB_CMD_Respond(CMD_Update_Program,UPDATE_FILE_SIZE_ERROR);
			return ;
		}
		//���ļ�����
		Update_CheckSum = 0;
		for(i=0;i<FLASH_UPD_SEND_SIZE-20;i+=4)
		{
			iTemp	=(UpDateBuffer[i+34-ProOffset]<<24)|(UpDateBuffer[i+33-ProOffset]<<16)|(UpDateBuffer[i+32-ProOffset]<<8)|(UpDateBuffer[31-ProOffset+i]);
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]	  = (iTemp)&0xff;
		}
		User_Erase_Update_Data();
		if(User_Write_Update_Data(UpdateFW_Offset_Addr,Update_Buffer,Data_len-20)==FLASH_OK)
		{
			UpdateFW_Offset_Addr	=Data_len-20;
			USB_CMD_Respond(CMD_Update_Program, 0x9000);
		}
		else
		{
			USB_CMD_Respond(CMD_Update_Program,FLASH_ERROR);
		}

	}
	else if(upd_flag==UPD_NORMAL_FLAG)//��������״̬
	{
		//��鳤���Ƿ���ȷ
		if(Data_len!=FLASH_UPD_SEND_SIZE)
		{
			USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
			return;
		}
		//���ļ�����
		for(i=0;i<FLASH_UPD_SEND_SIZE;i+=4)
		{
			iTemp	=(UpDateBuffer[i+14-ProOffset]<<24)|(UpDateBuffer[i+13-ProOffset]<<16)|(UpDateBuffer[i+12-ProOffset]<<8)|(UpDateBuffer[11-ProOffset+i]);
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]	  = (iTemp)&0xff;
		}
		if(User_Write_Update_Data(UpdateFW_Offset_Addr,Update_Buffer,FLASH_UPD_SEND_SIZE)==FLASH_OK)
		{
			UpdateFW_Offset_Addr	+=Data_len;

			USB_CMD_Respond(CMD_Update_Program,0x9000);
		}
		else
		{
			USB_CMD_Respond(CMD_Update_Program,FLASH_ERROR);
		}

	}
	else if(upd_flag==UPD_END_FLAG)//��������״̬
	{
		//�ж������ĳ����Ƿ���ȷ
		Data_len	=	UpdateFW_Total_Addr-UpdateFW_Offset_Addr;	//ʣ���ֽ���
		if(Data_len>(FLASH_UPD_SEND_SIZE+20))
		{	
			USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
			return;
		}
		//���ļ�����
		for(i=0;i<Data_len;i+=4)
		{
			iTemp	=(UpDateBuffer[i+14-ProOffset]<<24)|(UpDateBuffer[i+13-ProOffset]<<16)|(UpDateBuffer[i+12-ProOffset]<<8)|(UpDateBuffer[11-ProOffset+i]);	
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]	  = (iTemp)&0xff;
		}
		//��д���һ����������
		if(User_Write_Update_Data(UpdateFW_Offset_Addr,Update_Buffer,Data_len)==FLASH_OK)
		{
			UpdateFW_Offset_Addr	+=Data_len;
			USB_CMD_Respond(CMD_Update_Program,0x9000);
		}
		else
		{
			USB_CMD_Respond(CMD_Update_Program,FLASH_ERROR);
		}
		//����������־
		Write_Update_Finish_Flag();
		//������Ӧ
		USB_CMD_Respond(CMD_Update_Program, 0x9000);
		delayms();	//��ʱһ�£�������λ���յ���Ӧ
		//��λ�豸

		System_Reset();//��λϵͳ
	}
	else
	{
		 USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
	}
}


