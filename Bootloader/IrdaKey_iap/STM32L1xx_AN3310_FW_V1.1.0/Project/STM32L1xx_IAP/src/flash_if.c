/**
  ******************************************************************************
  * @file    STM32L1xx_IAP/src/flash_if.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/** @addtogroup STM32L1xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  FLASH_Unlock();

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t flashaddress;
  
  flashaddress = StartSector;
  
  //while (flashaddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
    if (FLASH_ErasePage(flashaddress) == FLASH_COMPLETE)
    {
		return (0);
		//flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      /* Error occurred while page erase */
      return (1);
    }
  }
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  FLASH_Status status = FLASH_BUSY;
  uint32_t* malPointer = (uint32_t *)Data;
  uint32_t memBuffer[32]; /* Temporary buffer holding data that will be written in a half-page space */
  uint32_t* mempBuffer = memBuffer;
  uint32_t i = 0;

  while (malPointer < (uint32_t*)(Data + DataLength))
  {
    /* Fill with the received buffer */
    while (mempBuffer < (memBuffer + 32))
    {
      /* If there are still data available in the received buffer */
      if (malPointer < ((uint32_t *)Data + DataLength))
      {
        *(uint32_t *)(mempBuffer++) = *(uint32_t *)(malPointer++);
      }
      else /* no more data available in the received buffer: fill remaining with dummy 0 */
      {
        *(uint32_t *)(mempBuffer++) = 0;
      }
    }

    /* Write the buffer to the memory */
    status = FLASH_ProgramHalfPage(*FlashAddress, (uint32_t *)(memBuffer));
  
    if (status != FLASH_COMPLETE)
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }

    /* Check if flash content matches memBuffer */
    for (i = 0; i < 32; i++)
    {
      if ((*(uint32_t*)(*(uint32_t*)FlashAddress + 4 * i)) != memBuffer[i])
      {
        /* flash content doesn't match memBuffer */
        return(2);
      }
    }

    /* Increment the memory pointer */
    *FlashAddress += 128;

    /* Reinitialize the intermediate buffer pointer */
    mempBuffer = memBuffer;
  }

  return (0);
}

/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  *         2: Flash memory is not write protected
  */
uint32_t FLASH_If_DisableWriteProtection(void)
{
  FLASH_Status status = FLASH_BUSY;

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);  
  
  /* Test if user memory is write protected */
  if (FLASH_If_GetWriteProtectionStatus() != 0x00)
  {
    /* Unlock the Option Bytes */  
    FLASH_OB_Unlock();

    /* Disable the write protection of user application pages */ 
    status = FLASH_If_WriteProtectionConfig();
    if (status == FLASH_COMPLETE)
    {
      /* Write Protection successfully disabled */
      return (0);
    }
    else
    {
      /* Error: Flash write unprotection failed */
      return (1);
    }
  }
  else
  {
     /* Flash memory is not write protected */
     return(2);
  }
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval If the sector is write-protected, the corresponding bit in returned
  *         value is set.
  *         If the sector isn't write-protected, the corresponding bit in returned
  *         value is reset.
  *         e.g. if only sector 3 is write-protected, returned value is 0x00000008
  */
uint32_t FLASH_If_GetWriteProtectionStatus(void)
{
#ifdef STM32L1XX_MD 
  return(FLASH_OB_GetWRP() & FLASH_PROTECTED_SECTORS);
#elif defined STM32L1XX_HD
  uint32_t WRP_Status = 0, WRP1_Status = 0, WRP2_Status = 0, sectornumber = 0;
  
  sectornumber=FLASH_SECTOR_NUMBER;
  /* Test on the start address in Bank1 */
  if (FLASH_START_ADDRESS <= 0x0802FF00)
  {
    /* Test on the user application to be programmed in Bank1 */
    if (APPLICATION_ADDRESS <= 0x0802FF00)
    {
      WRP2_Status = FLASH_OB_GetWRP2();
      
      if(sectornumber < 32)
      {
        WRP_Status = FLASH_OB_GetWRP() & ((uint32_t)~((1<<sectornumber) - 1));
        WRP1_Status = FLASH_OB_GetWRP1();
      }
      else
      {
        WRP1_Status = (FLASH_OB_GetWRP1() & ((uint32_t)~((1<<(sectornumber-32)) - 1)));
      }
    }
    else if (APPLICATION_ADDRESS >= 0x08030000) /* Test on the user application to be programmed in Bank2 */
    {
      sectornumber = (uint32_t)((APPLICATION_ADDRESS - 0x08030000)>>12);
      if (sectornumber < 16)
      {
        WRP1_Status = FLASH_OB_GetWRP1() & (uint32_t)~((1 << (sectornumber +16) - 1));
        WRP2_Status = FLASH_OB_GetWRP2();
      }
      else
      {
        WRP2_Status = FLASH_OB_GetWRP2() & (uint32_t)~((1 << (sectornumber-16) - 1));
      }
    }
  }
  else if (FLASH_START_ADDRESS >= 0x08030000) /* Test on the start address in Bank2 */
  {  
    if (APPLICATION_ADDRESS <= 0x0802FF00)  /* Test on the user application to be programmed in Bank1 */
    {
      sectornumber = (uint32_t)(APPLICATION_ADDRESS - 0x08000000)>>12;
      if(sectornumber < 32)
      {
        WRP_Status = FLASH_OB_GetWRP() & (uint32_t)~((1<<sectornumber - 1));
        WRP1_Status = FLASH_OB_GetWRP1()&0x0000FFFF;
      }
      else
      {
        WRP1_Status = (FLASH_OB_GetWRP1() & (uint32_t)~((1 << (sectornumber-32) - 1)))&0x0000FFFF;    
      }
    }
    else  /* Test on the user application to be programmed in Bank2 */
    {
      if(sectornumber < 16)
      {
        WRP1_Status = FLASH_OB_GetWRP1() & ((uint32_t)~(1 << (sectornumber + 16) - 1));
        WRP2_Status = FLASH_OB_GetWRP2();
      }
      else
      {
        WRP2_Status = (FLASH_OB_GetWRP2() & (uint32_t)~(1 <<( sectornumber - 16) - 1));
      }      
    }
  }
  if ((WRP_Status!=0)||(WRP1_Status!=0)||(WRP2_Status!=0))
    return 1;
  else
    return 0;

#endif   
}

/**
  * @brief  Disable the write protection status of user flash area.
  * @param  None
  * @retval If the sector is write-protected, the corresponding bit in returned
  *         value is set.
  *         If the sector isn't write-protected, the corresponding bit in returned
  *         value is reset.
  *         e.g. if only sector 3 is write-protected, returned value is 0x00000008
  */
FLASH_Status FLASH_If_WriteProtectionConfig(void)
{
  FLASH_Status state = FLASH_COMPLETE;
#ifdef STM32L1XX_MD 
  
  /* Disable the write protection of user application pages */
  state = FLASH_OB_WRPConfig(FLASH_PROTECTED_SECTORS, DISABLE); 
#elif defined STM32L1XX_HD
  FLASH_Status WRPstatus = FLASH_COMPLETE, WRP1status = FLASH_COMPLETE, WRP2status = FLASH_COMPLETE ;
  uint32_t sectornumber = 0;
  
  sectornumber=FLASH_SECTOR_NUMBER;
  
  /* Test on the start address on Bank1 */
  if (FLASH_START_ADDRESS <= 0x0802FF00)
  {
    if (sectornumber < 32)
    {
      WRPstatus = FLASH_OB_WRPConfig(((uint32_t)~((1 << sectornumber) - 1)), DISABLE);
      WRP1status = FLASH_OB_WRP1Config(OB_WRP1_AllPages, DISABLE);
      WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, DISABLE);
    }
    else if ((sectornumber >= 32)&& (sectornumber < 64))
    {
      WRP1status = FLASH_OB_WRP1Config(((uint32_t)~((1 << (sectornumber - 32)) - 1)), DISABLE);
      WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, DISABLE);
    }
    else 
      WRP2status = FLASH_OB_WRP2Config(((uint32_t)~((1 << (sectornumber - 64)) - 1)), DISABLE);
  }
  else  if (FLASH_START_ADDRESS >= 0x08030000)/* Test on the start address on Bank2 */
  {
    if( APPLICATION_ADDRESS <=0x0802FF00)/* User application in BANK1 */
    {
      sectornumber = (uint32_t)((APPLICATION_ADDRESS-0x08000000)>>12);
      if(sectornumber < 32)
      {
        WRPstatus = FLASH_OB_WRPConfig((uint32_t)~((1 << sectornumber) - 1), DISABLE);
        WRP1status = FLASH_OB_WRP1Config(0x0000FFFF,DISABLE);
      }
      else
      { 
        WRP1status = FLASH_OB_WRP1Config((uint32_t)~((1 << (sectornumber-32)) - 1), DISABLE);
      }
    }
    else /* User application Bank2 */
    {
      if(sectornumber < 16)
      {
        WRP1status = FLASH_OB_WRP1Config((uint32_t)~((1 << (sectornumber+16)) - 1), DISABLE); 
        WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, DISABLE);
      }
      else
      {
        WRP2status = FLASH_OB_WRP2Config((uint32_t)~((1 << (sectornumber -16 )) - 1), DISABLE);
      }
    }

    if ((WRPstatus!=FLASH_COMPLETE)||(WRP1status!=FLASH_COMPLETE)||(WRP2status!=FLASH_COMPLETE))
      state = FLASH_ERROR_WRP;
  }
#endif   
  return state;
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/


/* Private typedef -----------------------------------------------------------*/
u8 mybuffer[PAGE_SIZE];			//读取的数据放在这里
u32 BlockNbr = 0, UserMemoryMask = 0;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
u32 FlashDestination = ApplicationAddress; // Flash user program offset
u32 UpdateFW_Offset_Addr;	//固件升级偏移地址

/*******************************************************************************
* Function Name  : Read_Update_Finish_Flag
* Description    : 读完成标志
* Input          : None
* Return         : 标志值
* 影 响		 	 ：None
*******************************************************************************/
unsigned int Read_Update_Finish_Flag(void)
{
	u8 i;
	u8 Flash_Buffer[0x10];
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
	//FLASH_Lock();
	iTemp	=	Flash_Buffer[3];
	iTemp	=	(iTemp<<8)|Flash_Buffer[2];
	iTemp	=	(iTemp<<8)|Flash_Buffer[1];
	iTemp	=	(iTemp<<8)|Flash_Buffer[0];
   
	return iTemp;
}
/*******************************************************************************
* Function Name  : FLASH_PagesMask
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
u32 FLASH_PagesMask(u32 Size)
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
* Function Name  : Write_Update_Finish_Flag
* Description    : 写升级完成标志
* Input          : None
* Return         : None
* 影 响		 	 ：None
*******************************************************************************/
void Write_Update_Finish_Flag(void)
{
	u8 Flash_Buffer[0x20];
//	u32 j;

	//解锁程序Flash
	//FLASH_Unlock();
	FLASH_If_Init();

	// 根据地址计算出块号
	BlockNbr = (UPDATE_FLAG_ADDR - 0x08000000) >> 12;
	// 使用STM3210B芯片时，E系列计算方法不同
	UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	//取消写保护
	FLASH_If_DisableWriteProtection();
	//if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 //	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	//更新要修改的数据
	Flash_Buffer[0]		=(UPDATE_FLAG_FINISHED&0xff);
  	Flash_Buffer[1]		=((UPDATE_FLAG_FINISHED>>8)&0xff);
	Flash_Buffer[2]		=((UPDATE_FLAG_FINISHED>>16)&0xff);
	Flash_Buffer[3]		=((UPDATE_FLAG_FINISHED>>24)&0xff);

  	//Flash烧写目标地址
	FlashDestination = UPDATE_FLAG_ADDR;
	
	//擦除所占用页内容
	FLASH_If_Erase(FlashDestination);
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
//  NbrOfPage = FLASH_PagesMask(PAGE_SIZE);
    // Erase the FLASH pages 
    //for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
	//	FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
	RamSource = (u32)(&Flash_Buffer[0]);
	if (FLASH_If_Write(&FlashDestination, (uint32_t*)RamSource, (uint16_t) UPDATE_FLAG_SIZE/4)  == 0)
	{
		return;
	}
	/*
	for (j = 0;(j < UPDATE_FLAG_SIZE) && (FlashDestination <  UPDATE_FLAG_ADDR + UPDATE_FLAG_SIZE);j += 4)
	{
		// Program the data received into STM32F10x Flash 
		FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
		FlashDestination += 4;
		RamSource += 4;
	}
	*/
	//上锁Flash
	//FLASH_Lock();
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
	u32 Flash_Dest;
	//地址超范围时，返回起始地址超范围错误
	if(addr>=UPDATE_DATE_SIZE)		
		return 5;
	//起始地址和长度之和超范围时
	len	=	addr+N;
	if(len>=UPDATE_DATE_SIZE)
		return 6;

	//Flash上IC虚拟目标地址
	Flash_Dest = UPDATE_DATA_ADDR+addr;
	//先读出所占用页的内容
	for(i=0;i<N;i++)
	{
		*pt= *(u8*)Flash_Dest;
		pt++;
		Flash_Dest ++;
	}
	return FLASH_OK;
}
/*******************************************************************************
* Function Name  : Flash_Update_Program
* Description    : 更新存储在程序存储器中的程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_Update_Program(void)
{
	u32  i;

	//解锁程序Flash
	//FLASH_Unlock();
	FLASH_If_Init();

	//要烧写的目标地址
	FlashDestination = ApplicationAddress;
	// Get the number of block (4 or 2 pages) from where the user program will be loaded 
	BlockNbr = (FlashDestination - 0x08000000) >> 12;
	// Compute the mask to test if the Flash memory, where the user program will be loaded, is write protected 
	#ifdef USE_STM3210B_EVAL 	// USE_STM3210B_EVAL
		UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
	#else					 	// USE_STM3210E_EVAL 
		if (BlockNbr < 62)
		{
			UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
		}
		else
		{
			UserMemoryMask = ((u32)0x80000000);
		}
	#endif 
	//取消写保护
	FLASH_If_DisableWriteProtection();
	//if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
		//FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

	FlashDestination = ApplicationAddress; // Flash user program offset

    /* Erase the needed pages where the user application will be loaded */
    /* Define the number of page to be erased */
	/* Clear All pending flags */
	//FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(UPDATE_DATE_SIZE);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		//FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
		FLASH_If_Erase(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
//	NbrOfPage = UPDATE_DATE_SIZE/PAGE_SIZE;
	UpdateFW_Offset_Addr=0;	//固件升级程序偏移地址
	for(i=0;i< NbrOfPage;i++)
	{
		//读升级程序的信息  		
		User_Read_Update_Data(UpdateFW_Offset_Addr, mybuffer, PAGE_SIZE);
		RamSource = (u32)(&mybuffer[0]);
		if (FLASH_If_Write(&FlashDestination, (uint32_t*)RamSource, (uint16_t) FLASH_PAGE_SIZE/4)  == 0)
		{
			UpdateFW_Offset_Addr	+=	PAGE_SIZE;
		}
		else
		{
			return ;
		}
			
		/*
		for (j = 0;(j < PAGE_SIZE) && (FlashDestination <  ApplicationAddress + UPDATE_DATE_SIZE);j += 4)
		{
			FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
			if (*(u32*)FlashDestination != *(u32*)RamSource)
			{
				return ;
			}
			FlashDestination += 4;
			RamSource += 4;
		}*/
		//UpdateFW_Offset_Addr	+=	PAGE_SIZE;
	 }
	//上锁Flash
	//FLASH_Lock();
}






























