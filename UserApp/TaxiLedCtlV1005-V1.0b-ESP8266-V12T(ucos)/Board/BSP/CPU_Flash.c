/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : Flash.c
* Author             : Hanyongliang
* Date First Issued  : 04/03/2009
* Description        : This file provides all the SPI Flash(AT26DF041) firmware functions.
********************************************************************************
* History:
* 05/07/2009
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "stm32f10x_lib.h"
#include "bsp.h"
#include "cpu_flash.h"
#include "stm32f10x_flash.h"


#define FLASH_OK			CMD_OK
#define FLASH_ERROR			1
#define CMD_OK				0
#define CMD_FAIL			1
#define CMD_TIMEOUT			1


#define FLASH_BUFFER_SIZE	0x400
/* Private typedef -----------------------------------------------------------*/
u32 BlockNbr = 0, UserMemoryMask = 0;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
u32 FlashDestination; 
u8  Flash_Buffer[FLASH_BUFFER_SIZE];	//2K字节缓存


u32 Update_CheckSum;	//升级检验和
u32 UpdateFW_Address;	//升级固件地址
u32 UpdateFW_Rec_Size;	//已经接收的升级文件大小
u32 UpdateFW_Total_Sector;	//升级固件所用总扇区数

FunctionalState Write_IC_Card_Enable	=	DISABLE;



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
* Description    : 用户删除升级数据
* Input          : None
* Output		 : None
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
void User_Erase_Update_Data(void)
{
	//解锁程序Flash
	FLASH_Unlock();

	//根据要写的地址计算出块号
	BlockNbr = (UPDATE_DATA_ADDR - 0x08000000) >> 12;
	//使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages
	//Flash烧写目标地址
	FlashDestination = UPDATE_DATA_ADDR;

	//擦除所占用页内容
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(UPDATE_DATE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
}

void my_erase_pages(u32 addr, u32  size)
{
	//解锁程序Flash
	FLASH_Unlock();

	//根据要写的地址计算出块号
	BlockNbr = (addr - 0x08000000) >> 12;
	//使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages
	//Flash烧写目标地址
	FlashDestination = addr;

	//擦除所占用页内容
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(size);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
}
/*******************************************************************************
* Function Name  : User_Write_Update_Data
* Description    : 用户写升级数据
* Input          :  addr:要写入的地址
*		   			  n:要写入的个数
* Output		 :*pt: 存储写入数据缓存的指针;
* Return         : FLASH_OK:成功;other：失败
* 影 响		 	 ：None
*******************************************************************************/
unsigned char User_Write_Update_Data(int addr, unsigned char  *pt, unsigned int  n)
{

	u32  iTemp;
	u32  j;

//	//地址超范围时，返回起始地址超范围错误,	
//	if(addr>=UPDATE_DATE_SIZE)		return FLASH_ERROR;
//	//起始地址和长度之和超范围时
//	iTemp	=	addr+n;
//	if(iTemp>=UPDATE_DATE_SIZE)	return FLASH_ERROR;	

  	//Flash烧写目标地址
  	FlashDestination = addr;
	
	//烧写程序开始
	RamSource = (u32)(&pt[0]);
	for (j = 0;(j < n) && (FlashDestination <  addr+n); j += 4)
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
函 数 名: User_Read_Update_Data()
功    能: 读升级数据
说    明: 
调    用: 
全局变量:
入口参数: addr:地址;
  		    *pt:存储读出数据缓存的指针;
		     N:要读取的个数;
出口参数：指向内部RAM的指针 *pt
返 回 值: FLASH_OK:成功;other:失败
*********************************************************************/
u8 User_Read_Update_Data(u16 addr,u8 *pt,u16 N)
{
int len;
int i;
	
	//地址超范围时，返回起始地址超范围错误
	if(addr>=UPDATE_DATE_SIZE)		
		return FLASH_ERROR;
	//起始地址和长度之和超范围时
	len	=	addr+N;
	if(len>=UPDATE_DATE_SIZE)
		return FLASH_ERROR;

	//解锁程序Flash
	FLASH_Unlock();
	
	//Flash上IC虚拟目标地址
	FlashDestination = UPDATE_DATA_ADDR+addr;
	//先读出所占用页的内容
	for(i=0;i<N;i++)
   	{
		*pt= *(u8*)FlashDestination;
		pt++;
		FlashDestination ++;
   	}
	//上锁Flash
	FLASH_Lock();
	
	return FLASH_OK;
}
/*******************************************************************************
* Function Name  : Write_Update_Finish_Flag
* Description    : 写升级完成标志
* Input          : None
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
/*void Write_Update_Finish_Flag(void)
{

	u32 j;
	
	//解锁程序Flash
	FLASH_Unlock();

	// 根据地址计算出块号
	BlockNbr = (UPDATE_FLAG_ADDR - 0x08000000) >> 12;
	// 使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	//更新要修改的数据
	Flash_Buffer[0]		=(UPDATE_FLAG_GO&0xff);
  	Flash_Buffer[1]		=((UPDATE_FLAG_GO>>8)&0xff);
	Flash_Buffer[2]		=((UPDATE_FLAG_GO>>16)&0xff);
	Flash_Buffer[3]		=((UPDATE_FLAG_GO>>24)&0xff);

  	//Flash烧写目标地址
	FlashDestination = UPDATE_FLAG_ADDR;
	
	//擦除所占用页内容
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
	RamSource = (u32)(&Flash_Buffer[0]);
	for (j = 0;(j < UPDATE_FLAG_SIZE) && (FlashDestination <  UPDATE_FLAG_ADDR + UPDATE_FLAG_SIZE);j += 4)
	{
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	//上锁Flash
	FLASH_Lock();
}*/
/*******************************************************************************
* Function Name  : Read_Update_Finish_Flag
* Description    : 读完成标志
* Input          : None
* Return         : 标志值
* 影 响		 	 ：None
*******************************************************************************/
/*u32 Read_Update_Finish_Flag(void)
{
	u8 i;
	u32 iTemp;
	//解锁程序Flash
	FLASH_Unlock();
	//Flash上IC虚拟目标地址
	FlashDestination = UPDATE_FLAG_ADDR;
	//标志占4字节
	for(i=0;i<4;i++)
	{
		Flash_Buffer[i]= *(u8*)FlashDestination;
		FlashDestination ++;
	}
	//上锁Flash
	FLASH_Lock();
		iTemp	=	Flash_Buffer[3];
		iTemp	=	(iTemp<<8)|Flash_Buffer[2];
		iTemp	=	(iTemp<<8)|Flash_Buffer[1];
		iTemp	=	(iTemp<<8)|Flash_Buffer[0];
	return iTemp;
}*/
/*******************************************************************************
* Function Name  : Write_Software_Version_Flag
* Description    : 写软件版本标志
* Input          : None
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
/*
void Write_Software_Version_Flag(void)
{

	u32 j;
	
	//解锁程序Flash
	FLASH_Unlock();

	// 根据地址计算出块号
	BlockNbr = (SOFTWARE_VERSION_ADDR - 0x08000000) >> 12;
	// 使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	//更新要修改的数据
	Flash_Buffer[0]		=(SOFTWARE_VERSION&0xff);
  	Flash_Buffer[1]		=((SOFTWARE_VERSION>>8)&0xff);
	Flash_Buffer[2]		=((SOFTWARE_VERSION>>16)&0xff);
	Flash_Buffer[3]		=((SOFTWARE_VERSION>>24)&0xff);

//	Flash_Buffer[4]		=	SysInitFlag;//初始化标志
 // 	Flash_Buffer[5]		=	SysWorkMode;//工作模式
//	Flash_Buffer[6]		=	AlarmMode;//报警模式
	Flash_Buffer[7]		=	0;//驱动电流

  	Get_Unique_Device_ID(&Flash_Buffer[0x10]);

	//Flash烧写目标地址
	FlashDestination = SOFTWARE_VERSION_ADDR;
	
	//擦除所占用页内容
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    // Erase the FLASH pages 
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
	RamSource = (u32)(&Flash_Buffer[0]);
	for (j = 0;(j < SOFTWARE_VERSION_SIZE) && (FlashDestination <  SOFTWARE_VERSION_ADDR + SOFTWARE_VERSION_SIZE);j += 4)
	{
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	//上锁Flash
	FLASH_Lock();
}*/
/*******************************************************************************
* Function Name  : Read_Software_Version_Flag
* Description    : 读完成标志
* Input          : None
* Return         : 成功，失败
* 影 响		 	 ：None
*******************************************************************************/
/*u32 System_Software_Version;
u32 Read_Software_Version_Flag(void)
{
	u8 i;
	u8 Device_ID[12];
	
	//解锁程序Flash
	FLASH_Unlock();
	//Flash上IC虚拟目标地址
	FlashDestination = SOFTWARE_VERSION_ADDR;
	//标志占4字节
	for(i=0;i<SOFTWARE_VERSION_SIZE;i++)
	{
		Flash_Buffer[i]= *(u8*)FlashDestination;
		FlashDestination ++;
	}
	//上锁Flash
	FLASH_Lock();

	System_Software_Version	=	Flash_Buffer[3];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[2];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[1];
	System_Software_Version	=	(System_Software_Version<<8)|Flash_Buffer[0];
	if(System_Software_Version == SOFTWARE_VERSION)
	{
		//判断设备ID是否正确
		Get_Unique_Device_ID(Device_ID);
		for(i=0;i<12;i++)
		{
			if(Device_ID[i]!=Flash_Buffer[0x10+i])
			{
				return CMD_FAIL;
			}
		}
	}
//	SysInitFlag		=	Flash_Buffer[4];//初始化标志
//	SysWorkMode		=	Flash_Buffer[5];//工作模式
//	AlarmMode	    =	Flash_Buffer[6];//驱动电压
	//SysCurrentLevel	=	Flash_Buffer[7];//驱动电流
	return CMD_OK;
}*/



void Write2Flash(u32 Eaddr, u8 *Edat, u16 Elen)
{
	u32 j;
	u32 ObjAddr;
	
	ObjAddr = Eaddr+FlashBassAddr;
	
	//解锁程序Flash
	FLASH_Unlock();

	// 根据地址计算出块号
	BlockNbr = (ObjAddr - 0x08000000) >> 12;
	// 使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	

	//Flash烧写目标地址
	FlashDestination = ObjAddr;
	
	//擦除所占用页内容
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
	RamSource = (u32)(&Edat[0]);
	for (j = 0; j<Elen; j += 4)
	{
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	//上锁Flash
	FLASH_Lock();
	
}

void ReadFromFlash(u32 Eaddr, u8 *Edat, u16 Elen)
{
	u8 i;
	
	//解锁程序Flash
	FLASH_Unlock();
	//Flash上IC虚拟目标地址
	FlashDestination = Eaddr+FlashBassAddr;
	//标志占4字节
	for(i=0; i<Elen; i++)
	{
		Edat[i]= *(u8*)FlashDestination;
		FlashDestination ++;
	}
	//上锁Flash
	FLASH_Lock();
}
/*******************************************************************************
* Function Name  : Get_Unique_Device_ID
* Description    : 得到设备唯一ID号
				   96位的独特ID位于地址 0x1FFFF7E8 ~ 0x1FFFF7F4 的系统存储区 
				   由ST在工厂中写入(用户不能修改) 用户可以以字节、半字、或字的方式单独读取其间的任一地址
* Input          : *ID_Buffer  ID号存储缓存
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
void Get_Unique_Device_ID(u8 *ID_Buffer)
{
	u8 i;

	//Flash上IC虚拟目标地址
	FlashDestination = 0x1FFFF7E8;
	//密钥占4字节
	for(i=0;i<12;i++)
	{
		*ID_Buffer= *(u8*)FlashDestination;
		ID_Buffer++;
		FlashDestination ++;
	}
}
/*******************************************************************************
* Function Name  : Flash__Protect
* Description    : Flash加密
* Input          : None
* Return         : None
* 影 响		 	 ：None
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





