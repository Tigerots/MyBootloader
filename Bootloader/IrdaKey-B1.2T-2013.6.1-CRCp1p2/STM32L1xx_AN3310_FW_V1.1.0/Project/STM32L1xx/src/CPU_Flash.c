/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : Flash.c
* Author             : Hanyongliang
* Date First Issued  : 04/03/2009
* Description        : This file provides all the SPI Flash(AT26DF041) firmware functions.
********************************************************************************
* History:
* 05/07/2009
*******************************************************************************/
//#include <includes.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x_lib.h"
#include "bsp.h"
#include "hw_config.h"
#include "cpu_flash.h"
#include "stm32f10x_flash.h"
#include "WatchDogApp.h"

#define FLASH_BUFFER_SIZE	0x400
/* Private typedef -----------------------------------------------------------*/
u32 BlockNbr = 0, UserMemoryMask = 0;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
u32 FlashDestination; // Flash user program offset
u8  Flash_Buffer[FLASH_BUFFER_SIZE];	//2K�ֽڻ���


u32 Update_CheckSum;	//���������
u32 UpdateFW_Address;	//�����̼���ַ
u32 UpdateFW_Rec_Size;	//�Ѿ����յ������ļ���С
u32 UpdateFW_Total_Sector;	//�����̼�������������

FunctionalState Write_IC_Card_Enable	=	DISABLE;

extern u8 USB_OutBuffer[USB_REC_BUFFER_SIZE];
extern u8 Update_Buffer[FLASH_UPD_SEND_SIZE];
extern void USB_CMD_Respond(u8 cmd,u8 Result);


/*******************************************************************************
* Function Name  : FLASH_DisableWriteProtectionPages
* Description    : Disable the write protection of desired pages
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
  u32 useroptionbyte = 0, WRPR = 0;
  u16 var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
  FLASH_Status status = FLASH_BUSY;

  WRPR = FLASH_GetWriteProtectionOptionByte();

  /* Test if user memory is write protected */
  if ((WRPR & UserMemoryMask) != UserMemoryMask)
  {
    useroptionbyte = FLASH_GetUserOptionByte();

    UserMemoryMask |= WRPR;

    status = FLASH_EraseOptionBytes();

    if (UserMemoryMask != 0xFFFFFFFF)
    {
      status = FLASH_EnableWriteProtection((u32)~UserMemoryMask);
    }

    /* Test if user Option Bytes are programmed */
    if ((useroptionbyte & 0x07) != 0x07)
    { /* Restore user Option Bytes */
      if ((useroptionbyte & 0x01) == 0x0)
      {
        var1 = OB_IWDG_HW;
      }
      if ((useroptionbyte & 0x02) == 0x0)
      {
        var2 = OB_STOP_RST;
      }
      if ((useroptionbyte & 0x04) == 0x0)
      {
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);
    }

    if (status == FLASH_COMPLETE)
    {
      /* Enable WWDG clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
      /* Generate a system Reset to re-load the new option bytes: enable WWDG and set
        counter value to 0x4F, as T6 bit is cleared this will generate a WWDG reset */
      WWDG_Enable(0x4F);
    }
  }
}
/*******************************************************************************
* Function Name  : FLASH_PagesMask
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
u32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;
}
/*******************************************************************************
* Function Name  : User_Erase_Update_Data
* Description    : �û�ɾ����������
* Input          : None
* Output		 : None
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void User_Erase_Update_Data(void)
{
	//��������Flash
	FLASH_Unlock();

	//����Ҫд�ĵ�ַ��������
	BlockNbr = (UPDATE_DATA_ADDR - 0x08000000) >> 12;
	//ʹ��STM3210BоƬʱ��Eϵ�м��㷽����ͬ
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//ȡ��д����
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages
	//Flash��дĿ���ַ
	FlashDestination = UPDATE_DATA_ADDR;

	//������ռ��ҳ����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(UPDATE_DATE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
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
	for (j = 0;(j < n) && (FlashDestination <  UPDATE_DATA_ADDR + UPDATE_DATE_SIZE);j += 4)
	{
		/* Program the data received into STM32F10x Flash */
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		if (*(u32*)FlashDestination != *(u32*)RamSource)
		{
			return FLASH_ERROR;
		}
		FlashDestination += 4;
		RamSource += 4;
	}
return FLASH_OK;
}
/*********************************************************************  
�� �� ��: User_Read_Update_Data()
��    ��: ����������
˵    ��: 
��    ��: 
ȫ�ֱ���:
��ڲ���: addr:��ַ;
  		    *pt:�洢�������ݻ����ָ��;
		     N:Ҫ��ȡ�ĸ���;
���ڲ�����ָ���ڲ�RAM��ָ�� *pt
�� �� ֵ: FLASH_OK:�ɹ�;other:ʧ��
*********************************************************************/
u8 User_Read_Update_Data(u16 addr,u8 *pt,u16 N)
{
int len;
int i;
	
	//��ַ����Χʱ��������ʼ��ַ����Χ����
	if(addr>=UPDATE_DATE_SIZE)		
		return FLASH_ERROR;
	//��ʼ��ַ�ͳ���֮�ͳ���Χʱ
	len	=	addr+N;
	if(len>=UPDATE_DATE_SIZE)
		return FLASH_ERROR;

	//��������Flash
	FLASH_Unlock();
	
	//Flash��IC����Ŀ���ַ
	FlashDestination = UPDATE_DATA_ADDR+addr;
	//�ȶ�����ռ��ҳ������
	for(i=0;i<N;i++)
   	{
		*pt= *(u8*)FlashDestination;
		pt++;
		FlashDestination ++;
   	}
	//����Flash
	FLASH_Lock();
	
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

	u32 j;
	
	//��������Flash
	FLASH_Unlock();

	// ���ݵ�ַ��������
	BlockNbr = (UPDATE_FLAG_ADDR - 0x08000000) >> 12;
	// ʹ��STM3210BоƬʱ��Eϵ�м��㷽����ͬ
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//ȡ��д����
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	//����Ҫ�޸ĵ�����
	Flash_Buffer[0]		=(UPDATE_FLAG_GO&0xff);
  	Flash_Buffer[1]		=((UPDATE_FLAG_GO>>8)&0xff);
	Flash_Buffer[2]		=((UPDATE_FLAG_GO>>16)&0xff);
	Flash_Buffer[3]		=((UPDATE_FLAG_GO>>24)&0xff);

  	//Flash��дĿ���ַ
	FlashDestination = UPDATE_FLAG_ADDR;
	
	//������ռ��ҳ����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//��д����ʼ
	RamSource = (u32)(&Flash_Buffer[0]);
	for (j = 0;(j < UPDATE_FLAG_SIZE) && (FlashDestination <  UPDATE_FLAG_ADDR + UPDATE_FLAG_SIZE);j += 4)
	{
		/* Program the data received into STM32F10x Flash */
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	//����Flash
	FLASH_Lock();
}
/*******************************************************************************
* Function Name  : Read_Update_Finish_Flag
* Description    : ����ɱ�־
* Input          : None
* Return         : ��־ֵ
* Ӱ ��		 	 ��None
*******************************************************************************/
u32 Read_Update_Finish_Flag(void)
{
	u8 i;
	u32 iTemp;
	//��������Flash
	FLASH_Unlock();
	//Flash��IC����Ŀ���ַ
	FlashDestination = UPDATE_FLAG_ADDR;
	//��־ռ4�ֽ�
	for(i=0;i<4;i++)
	{
		Flash_Buffer[i]= *(u8*)FlashDestination;
		FlashDestination ++;
	}
	//����Flash
	FLASH_Lock();
		iTemp	=	Flash_Buffer[3];
		iTemp	=	(iTemp<<8)|Flash_Buffer[2];
		iTemp	=	(iTemp<<8)|Flash_Buffer[1];
		iTemp	=	(iTemp<<8)|Flash_Buffer[0];
	return iTemp;
}
/*******************************************************************************
* Function Name  : Write_Software_Version_Flag
* Description    : д����汾��־
* Input          : None
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void Write_Software_Version_Flag(void)
{

	u32 j;
	
	//��������Flash
	FLASH_Unlock();

	// ���ݵ�ַ��������
	BlockNbr = (SOFTWARE_VERSION_ADDR - 0x08000000) >> 12;
	// ʹ��STM3210BоƬʱ��Eϵ�м��㷽����ͬ
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//ȡ��д����
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	//����Ҫ�޸ĵ�����
	Flash_Buffer[0]		=(SOFTWARE_VERSION&0xff);
  	Flash_Buffer[1]		=((SOFTWARE_VERSION>>8)&0xff);
	Flash_Buffer[2]		=((SOFTWARE_VERSION>>16)&0xff);
	Flash_Buffer[3]		=((SOFTWARE_VERSION>>24)&0xff);

	Flash_Buffer[4]		=	SysInitFlag;//��ʼ����־
  	Flash_Buffer[5]		=	SysWorkMode;//����ģʽ
	Flash_Buffer[6]		=	AlarmMode;//����ģʽ
	Flash_Buffer[7]		=	0;//��������

  	Get_Unique_Device_ID(&Flash_Buffer[0x10]);

	//Flash��дĿ���ַ
	FlashDestination = SOFTWARE_VERSION_ADDR;
	
	//������ռ��ҳ����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//��д����ʼ
	RamSource = (u32)(&Flash_Buffer[0]);
	for (j = 0;(j < SOFTWARE_VERSION_SIZE) && (FlashDestination <  SOFTWARE_VERSION_ADDR + SOFTWARE_VERSION_SIZE);j += 4)
	{
		/* Program the data received into STM32F10x Flash */
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	//����Flash
	FLASH_Lock();
}
/*******************************************************************************
* Function Name  : Read_Software_Version_Flag
* Description    : ����ɱ�־
* Input          : None
* Return         : �ɹ���ʧ��
* Ӱ ��		 	 ��None
*******************************************************************************/
u32 Read_Software_Version_Flag(void)
{
	u8 i;
	u8 Device_ID[12];
	
	//��������Flash
	FLASH_Unlock();
	//Flash��IC����Ŀ���ַ
	FlashDestination = SOFTWARE_VERSION_ADDR;
	//��־ռ4�ֽ�
	for(i=0;i<SOFTWARE_VERSION_SIZE;i++)
	   {
			Flash_Buffer[i]= *(u8*)FlashDestination;
			FlashDestination ++;
	   }
	//����Flash
	FLASH_Lock();

	System_Software_Version	=	Flash_Buffer[3];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[2];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[1];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[0];
	if(System_Software_Version == SOFTWARE_VERSION)
	{
		//�ж��豸ID�Ƿ���ȷ
		Get_Unique_Device_ID(Device_ID);
		for(i=0;i<12;i++)
		{
			if(Device_ID[i]!=Flash_Buffer[0x10+i])
			{
				return CMD_FAIL;
			}
		}
	}
	SysInitFlag		=	Flash_Buffer[4];//��ʼ����־
	SysWorkMode		=	Flash_Buffer[5];//����ģʽ
	AlarmMode	    =	Flash_Buffer[6];//������ѹ
	//SysCurrentLevel	=	Flash_Buffer[7];//��������
	return CMD_OK;
}
/*******************************************************************************
* Function Name  : Get_Unique_Device_ID
* Description    : �õ��豸ΨһID��
				   96λ�Ķ���IDλ�ڵ�ַ 0x1FFFF7E8 ~ 0x1FFFF7F4 ��ϵͳ�洢�� 
				   ��ST�ڹ�����д��(�û������޸�) �û��������ֽڡ����֡����ֵķ�ʽ������ȡ������һ��ַ
* Input          : *ID_Buffer  ID�Ŵ洢����
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void Get_Unique_Device_ID(u8 *ID_Buffer)
{
	u8 i;

	//Flash��IC����Ŀ���ַ
	FlashDestination = 0x1FFFF7E8;
	//��Կռ4�ֽ�
	for(i=0;i<12;i++)
	{
		*ID_Buffer= *(u8*)FlashDestination;
		ID_Buffer++;
		FlashDestination ++;
	}
}
/*******************************************************************************
* Function Name  : Flash__Protect
* Description    : Flash����
* Input          : None
* Return         : None
* Ӱ ��		 	 ��None
*******************************************************************************/
void Flash__Protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET) 
	{ 
		FLASH_Unlock();//This MUST done first 
		FLASH_ReadOutProtection(ENABLE) ; 
		
		NVIC_GenerateSystemReset(); 
	}
}

/*******************************************************************************
* Function Name  : CMD_UpdateFW_Handle
* Description    : �����̼�
* Input          : None
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/
/*void CMD_UpdateFW_Handle(void)
{
	u8 upd_flag;	//������־
	u16 Data_len,i;	//���ݳ���
	u32 device,chip,app_size;
	u32 iTemp;
//	char buffer[10];

	upd_flag	= USB_OutBuffer[8];
    Data_len	= USB_OutBuffer[10];
	Data_len	= (Data_len<<8)|USB_OutBuffer[9];
	if(upd_flag==UPD_START_FLAG)	//������ʼ
	{
		//��鳤���Ƿ���ȷ
		if(Data_len!=FLASH_UPD_SEND_SIZE)
		{
			USB_CMD_Respond(CMD_UpdateFW, UPD_LENGTH_ERROR);
			return;
		}
		device  = (USB_OutBuffer[14] <<24)|(USB_OutBuffer[13] <<16)|(USB_OutBuffer[12] <<8)|(USB_OutBuffer[11]);
		chip	= (USB_OutBuffer[22]<<24)|(USB_OutBuffer[21]<<16)|(USB_OutBuffer[20] <<8)|(USB_OutBuffer[19]);
		app_size= (USB_OutBuffer[30]<<24)|(USB_OutBuffer[29]<<16)|(USB_OutBuffer[28]<<8)|(USB_OutBuffer[27]);
		//У���豸�����Ƿ���ȷ,��Ҫ����PC���Ե��豸����
		if(device!=APP_DEVICE_TYPE_HEAD)
		{
			USB_CMD_Respond(CMD_UpdateFW,CMD_UPD_FILE_ERROR);
			return ;
		}
		//���оƬ�ͺ��Ƿ�����
		iTemp = chip & APP_CHIP_TYPE_MASK;
		if(iTemp>0)
		{
				USB_CMD_Respond(CMD_UpdateFW,CMD_UPD_FILE_ERROR);
				return ;
		}
		//�������С�Ƿ���������
		if((app_size<APP_MIN_SIZE)||(app_size>APP_MAX_SIZE))
		{
				USB_CMD_Respond(CMD_UpdateFW, CMD_UPD_FILE_ERROR);
				return ;
		}
		//��ʾ�����õ�

		for(i=0;i<20;i++)
			Update_Buffer[i]=USB_OutBuffer[11+i];
		//���ļ�����
		Update_CheckSum=0;
		for(i=0; i<FLASH_UPD_SEND_SIZE-20; i+=4)
		{
			iTemp	=(USB_OutBuffer[i+34]<<24)|(USB_OutBuffer[i+33]<<16)|(USB_OutBuffer[i+32]<<8)|(USB_OutBuffer[31+i]);	
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+23] = (iTemp>>24)&0xff;
			Update_Buffer[i+22] = (iTemp>>16)&0xff;
			Update_Buffer[i+21] = (iTemp>>8)&0xff;
			Update_Buffer[i+20]	  = (iTemp)&0xff;
		}
		USB_CMD_Respond(CMD_UpdateFW, CMD_OK);
		//���������ļ���������
		//iTemp	=	app_size/PAGE_SIZE+1;
		//for(i=0; i<iTemp; i=i+16)
		{
			User_Erase_Update_Data();
			//FLASH_ErasePage(i+UPDATE_DATA_ADDR/PAGE_SIZE);	//--??
			//SPI_FLASH_Erase_4K(i+FLASH_UPD_START_ADDR);
		}

		UpdateFW_Address	=  UPDATE_DATA_ADDR;//�����ļ���ŵ�ַ0x8009400
//		SPI_FLASH_BufferWrite((u8 *)Update_Buffer, UpdateFW_Address, FLASH_UPD_SEND_SIZE);
		User_Write_Update_Data(UpdateFW_Address, (u8 *)Update_Buffer,  FLASH_UPD_SEND_SIZE);
		UpdateFW_Rec_Size	=	FLASH_UPD_SEND_SIZE;
		UpdateFW_Address	=	UpdateFW_Address +	FLASH_UPD_SEND_SIZE;
		  //��ʾ����
//			sprintf(buffer,"%d\0", UpdateFW_Rec_Size);
//			GUI_SetFont(&GUI_FontHZ_SimSun_24);
//			GUI_SetColor(GUI_DARKBLUE);
//			GUI_DispStringAt(buffer,(SMALL_BMP_START_X+30),(SMALL_BMP_START_Y+40));

	}
	else if(upd_flag==UPD_NORMAL_FLAG)//��������״̬
	{
		//��鳤���Ƿ���ȷ
		if(Data_len!=FLASH_UPD_SEND_SIZE)
		{
			USB_CMD_Respond(CMD_UpdateFW,UPD_LENGTH_ERROR);
			return;
		}
		//���ļ�����
		for(i=0;i<FLASH_UPD_SEND_SIZE;i+=4)
		{
			iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);	
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]   = (iTemp)&0xff;
		}
		 //��ʾ����
		UpdateFW_Rec_Size	+=	FLASH_UPD_SEND_SIZE;
//			sprintf(buffer,"%d ", UpdateFW_Rec_Size);
//			GUI_SetFont(&GUI_FontHZ_SimSun_24);
//			GUI_SetColor(GUI_DARKBLUE);
//			GUI_DispStringAt(buffer,(SMALL_BMP_START_X+30),(SMALL_BMP_START_Y+40));
		//--SPI_FLASH_BufferWrite((u8 *)Update_Buffer, UpdateFW_Address, FLASH_UPD_SEND_SIZE);
		User_Write_Update_Data(UpdateFW_Address, (u8 *)Update_Buffer,  FLASH_UPD_SEND_SIZE);
		UpdateFW_Address	=	UpdateFW_Address + FLASH_UPD_SEND_SIZE;
		USB_CMD_Respond(CMD_UpdateFW, CMD_OK);
	}
	else if(upd_flag==UPD_END_FLAG)//��������״̬
	{
		app_size=Data_len-4;
		//���ļ�����
		for(i=0;i<app_size;i+=4)
		{
			iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);	
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]	  = (iTemp)&0xff;
		}
		//�ж��ļ�У����Ƿ���ȷ,��ȥ��
		iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);
		if(iTemp!= Update_CheckSum)
		{
			USB_CMD_Respond(CMD_UpdateFW,CMD_UPD_CHECKSUM_ERROR);
			return;
		}
		//��д���һ����������
		//--SPI_FLASH_BufferWrite((u8 *)Update_Buffer,UpdateFW_Address,FLASH_UPD_SEND_SIZE);
		User_Write_Update_Data(UpdateFW_Address, (u8 *)Update_Buffer,  FLASH_UPD_SEND_SIZE);
		UpdateFW_Address	=	UpdateFW_Address + FLASH_UPD_SEND_SIZE;
		//������Ӧ
		USB_CMD_Respond(CMD_UpdateFW,CMD_OK);
		UpdateFW_Rec_Size	+=	Data_len;
		 //��ʾ����
//			sprintf(buffer,"%d ", UpdateFW_Rec_Size);
//			GUI_SetFont(&GUI_FontHZ_SimSun_24);
//			GUI_SetColor(GUI_DARKBLUE);
//			GUI_DispStringAt(buffer,(SMALL_BMP_START_X+30),(SMALL_BMP_START_Y+40));
//
//			GUI_SetColor(GUI_BLUE);
//			GUI_DispStringAt("Finished!",(SMALL_BMP_START_X+30),(SMALL_BMP_START_Y+70));
		
		//����������־,��������
		Write_Update_Finish_Flag();
		//--FLASH_ErasePage(UPDATE_FLAG_ADDR);
		//--Update_Buffer[0]=UPDATE_FLAG_GO&0xff;
		//--Update_Buffer[1]=(UPDATE_FLAG_GO>>8)&0xff;
		//--Update_Buffer[2]=(UPDATE_FLAG_GO>>16)&0xff;
		//--Update_Buffer[3]=(UPDATE_FLAG_GO>>24)&0xff;
		//--SPI_FLASH_PageWrite((u8 *)Update_Buffer, UPDATE_FLAG_ADDR, 4);
		//--FLASH_ProgramWord(UPDATE_FLAG_ADDR, UPDATE_FLAG_GO);
		
//			OSTimeDlyHMSM(0,0,1,0);
//			System_What_Doing_Now	=	SYSTEM_IDLE;
		//��λ�豸
		System_Reset();//��λϵͳ
	}
	else
	{
		 USB_CMD_Respond(CMD_UpdateFW,UPD_FLAG_ERROR);
	}
}
*/

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
void Update_Program_Handle(void)
{
	u8 upd_flag;	//������־
	u16 Data_len,i;	//���ݳ���
	u32 device,chip,app_size;
	u32 iTemp;

   	Data_len	=		USB_OutBuffer[10];
	Data_len	=		(Data_len<<8)|USB_OutBuffer[9];
	upd_flag 	=  		USB_OutBuffer[8];
   	if(upd_flag==UPD_START_FLAG)	//������ʼ
	{
		UpdateFW_Offset_Addr=0;
		//��鳤���Ƿ���ȷ
		if(Data_len!=FLASH_UPD_SEND_SIZE)
		{
			USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
			return;
		}
		device  = (USB_OutBuffer[14] <<24)|(USB_OutBuffer[13] <<16)|(USB_OutBuffer[12] <<8)|(USB_OutBuffer[11]);
		chip	= (USB_OutBuffer[22]<<24)|(USB_OutBuffer[21]<<16)|(USB_OutBuffer[20] <<8)|(USB_OutBuffer[19]);
		app_size= (USB_OutBuffer[30]<<24)|(USB_OutBuffer[29]<<16)|(USB_OutBuffer[28]<<8)|(USB_OutBuffer[27]);
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
			iTemp	=(USB_OutBuffer[i+34]<<24)|(USB_OutBuffer[i+33]<<16)|(USB_OutBuffer[i+32]<<8)|(USB_OutBuffer[31+i]);
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
			USB_CMD_Respond(CMD_Update_Program,CMD_OK);
			//����LED��ʾ����
			UpdateFW_Total_Addr	=	app_size;
			//LED_Display_Cache_Down	=	(UpdateFW_Offset_Addr*100)/UpdateFW_Total_Addr;
//					memset(LED_Display_Cache, 0, 16);
//					Display_Number_Roll(2, LED_Display_Cache_Down);
//					TM_MDis(0, 16, &LED_Display_Cache[0]);
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
			iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);
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
			//������ʾ����
			//LED_Display_Cache_Down	=	(UpdateFW_Offset_Addr*100)/UpdateFW_Total_Addr;
//					memset(LED_Display_Cache, 0, 16);
//					Display_Number_Roll(2, LED_Display_Cache_Down);
//					TM_MDis(0,16, &LED_Display_Cache[0]);

			USB_CMD_Respond(CMD_Update_Program,CMD_OK);
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
			iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);	
			Update_CheckSum += iTemp;	//У���Ϊ����ǰ�ĺ�
			iTemp  -=8;
			Update_Buffer[i+3] = (iTemp>>24)&0xff;
			Update_Buffer[i+2] = (iTemp>>16)&0xff;
			Update_Buffer[i+1] = (iTemp>>8)&0xff;
			Update_Buffer[i]	  = (iTemp)&0xff;
		}
		//�ж��ļ�У����Ƿ���ȷ,��ȥ��
		/*
		iTemp	=(USB_OutBuffer[i+14]<<24)|(USB_OutBuffer[i+13]<<16)|(USB_OutBuffer[i+12]<<8)|(USB_OutBuffer[11+i]);
		if(iTemp!= Update_CheckSum)
		{
			USB_CMD_Respond(CMD_Update_Program,UPDATE_FILE_SUM_ERROR);
			LED_Display_Cache_Down=99;
			return;
		}
		*/
		//��д���һ����������
		if(User_Write_Update_Data(UpdateFW_Offset_Addr,Update_Buffer,Data_len)==FLASH_OK)
		{
			UpdateFW_Offset_Addr	+=Data_len;
			USB_CMD_Respond(CMD_Update_Program,CMD_OK);
		}
		else
		{
			USB_CMD_Respond(CMD_Update_Program,FLASH_ERROR);
		}
		//����������־
		Write_Update_Finish_Flag();
		//������Ӧ
		USB_CMD_Respond(CMD_Update_Program,CMD_OK);
		Delay(10000);	//��ʱһ�£�������λ���յ���Ӧ
		//��λ�豸

		System_Reset();//��λϵͳ
	}
	else
	{
		 USB_CMD_Respond(CMD_Update_Program,CMD_PARAM_ERROR);
	}
}





