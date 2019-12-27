/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : Flash.c
* Author             : Hanyongliang
* Date First Issued  : 06/01/2008
* Description        : This file provides all the SPI Flash(AT26DF041) firmware functions.
********************************************************************************
* History:
* 05/07/2008
*******************************************************************************/
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_wwdg.h>
#include <string.h>
#include <stdio.h>
#include <flash.h>


/* Private typedef -----------------------------------------------------------*/
#define GPIOC_FLASH_CS	  GPIO_Pin_4	 
#define GPIOE_FLASH_SCK	  GPIO_Pin_2	
#define GPIOE_FLASH_MOSI  GPIO_Pin_12	
#define GPIOC_FLASH_MISO  GPIO_Pin_5	

#define GPIOx_CS	  	GPIOB	 // GPIOC Pin 14
#define GPIOx_SCK	  	GPIOD	 // GPIOB Pin 5 
#define GPIOx_MOSI  	GPIOC	 // GPIOB Pin 0
#define GPIOx_MISO  	GPIOB	 // GPIOC Pin 15


#define SPI_Flash_Write_Enable()  SPI_FLASH_SendByte(SPI_FLASH_WREN)
#define SPI_Flash_Write_Disable() SPI_FLASH_SendByte(SPI_FLASH_WRDIS)

/* Private typedef -----------------------------------------------------------*/
u8 mybuffer[0x200];			//读取的数据放在这里
u32 BlockNbr = 0, UserMemoryMask = 0;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
u32 FlashDestination = ApplicationAddress; // Flash user program offset
u32 UpdateFW_Offset_Addr;	//固件升级偏移地址

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/
void Delay (int cnt)
{
  while (cnt--);
}

/*******************************************************************************
* Function Name  : Fram_GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_GPIO_Configuration(void)
{
GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

  /* Configure SPI2 pins: CS ,SCK and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIOC_FLASH_CS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOx_CS, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIOE_FLASH_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOx_SCK, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIOE_FLASH_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOx_MOSI, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIOC_FLASH_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//输入上拉
  GPIO_Init(GPIOx_MISO, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOx_CS, GPIOC_FLASH_CS);
  GPIO_SetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
  GPIO_SetBits(GPIOx_MOSI, GPIOE_FLASH_MOSI);
}

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
* Function Name  : FLASH_CS
* Description    : FLASH存储器片选 控制
* Input          : NewState:DISABLE or ENABLE
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_CS(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		GPIO_ResetBits(GPIOx_CS, GPIOC_FLASH_CS);
	}
	else
	{
		GPIO_SetBits(GPIOx_CS, GPIOC_FLASH_CS);
		GPIO_SetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
		GPIO_SetBits(GPIOx_MOSI, GPIOE_FLASH_MOSI);
	}
}
/*******************************************************************************
* Function Name  : SPI_Flash_ReadByte
* Description    : Receive one Byte From FLASH controller 
* Input          : None
* Output         : None
* Return         : Data Readed
*******************************************************************************/
static unsigned char SPI_Flash_ReadByte (void)
{
unsigned char i=0;	
unsigned char data=0;


	for(i=0;i<8;i++)
	{
	 data	= (data<<1) ;
	 GPIO_ResetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
	 if   (GPIO_ReadInputDataBit(GPIOx_MISO, GPIOC_FLASH_MISO))  
         	data   |=0x01;  
	 GPIO_SetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
	}
	return data;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Write one Byte to FLASH controller 
* Input          : Data
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_FLASH_SendByte (unsigned char data_out)
{
unsigned char i;

  for (i=0; i<8; i++)                 
    {
	    GPIO_ResetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
		if ((data_out&0x80)==0)            // start from MSB to LSB
	      GPIO_ResetBits(GPIOx_MOSI, GPIOE_FLASH_MOSI);
	    else
	      GPIO_SetBits(GPIOx_MOSI, GPIOE_FLASH_MOSI);
	    
	 	GPIO_SetBits(GPIOx_SCK, GPIOE_FLASH_SCK);
	    data_out<<=1;                      // shift to next output bit, MSB will be send
    }
}
/*******************************************************************************
* Function Name  : SPI_Flash_Status
* Description    : SPI Flash读状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 SPI_Flash_Status(void)
{
u8 ret;

  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_RDSTAT);
  ret	=	SPI_Flash_ReadByte();
  FLASH_CS(DISABLE);	               // chip select disabled
  return ret; 
}
/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : SPI Flash等待空闲的到来 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Wait_Busy(int timeout) 
{
  while ((timeout--)>0)                
    {
   		Delay(150);
    	if ((SPI_Flash_Status()&0x1)==0)   // cti status
      		break;  
    }
}
/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
unsigned int SPI_FLASH_ReadID(void)
{
unsigned int Temp = 0;


  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_MIDDID);
  SPI_FLASH_SendByte(0);
  SPI_FLASH_SendByte(0);
  SPI_FLASH_SendByte(0);
  Temp	=	SPI_Flash_ReadByte();
  Temp	= (Temp<<8)|SPI_Flash_ReadByte();

  FLASH_CS(DISABLE);	               // chip select disabled
  return (Temp);
}


/*******************************************************************************
* Function Name  : SPI_FLASH_Read_JID
* Description    : Reads FLASH JEDEC ID-Byte2, MemType-Byte1, MemSize-Byte0
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
unsigned int SPI_FLASH_Read_JID(void)
{
unsigned int Temp = 0;


  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_JEDECID);
  Temp	=	SPI_Flash_ReadByte();
  Temp	= (Temp<<8)|SPI_Flash_ReadByte();
  Temp	= (Temp<<8)|SPI_Flash_ReadByte();
  FLASH_CS(DISABLE);

  return (Temp);
}
/*******************************************************************************
* Function Name  : SPI_Flash_Read_Block
* Description    : 读SPI Flash块
* Input          : 起始地址:start_address,要读的尺寸:blk_size
* Output         : pBuffer:读出的数据指针
* Return         : None
*******************************************************************************/
void SPI_Flash_Read_Block(unsigned char * pBuffer,int start_address, int blk_size)
{
u8 spi_do_buffer[4]; 
int i;



	FLASH_CS(ENABLE);                         // chip select enabled

	spi_do_buffer[0]=SPI_FLASH_RDDATA;   // put command code to output buffer
	spi_do_buffer[1]=(start_address>>16)&0xFF; // put A23:16 to output buffer
	spi_do_buffer[2]=(start_address>>8)&0xFF; // put A15:8 to output buffer
	spi_do_buffer[3]=(start_address>>0)&0xFF; // put A7:0 to output buffer
	
	SPI_FLASH_SendByte(spi_do_buffer[0]);
	SPI_FLASH_SendByte(spi_do_buffer[1]);
	SPI_FLASH_SendByte(spi_do_buffer[2]);
	SPI_FLASH_SendByte(spi_do_buffer[3]);
	
	for (i=0; i<blk_size; i++)           // pres vsechny Byty bloku
	{
	  *pBuffer	=	SPI_Flash_ReadByte();
	  pBuffer++;
	}
	FLASH_CS(DISABLE);


}

/*******************************************************************************
* Function Name  : SPI_FLASH_BlockWrite
* Description    : 该函数不能写超过256字节的数据，否则多余的字节将被摒弃
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - address : FLASH's internal Page address to write to.
*                  - blk_size : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BlockWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size)
{
u8 spi_do_buffer[4]; 
int i;
 	
	

	FLASH_CS(ENABLE);                         // chip select enabled
	SPI_Flash_Write_Enable();        
	FLASH_CS(DISABLE);

	FLASH_CS(ENABLE);                         // chip select enabled
    spi_do_buffer[0]=SPI_FLASH_PAGEPGM; // put command code to output buffer
    spi_do_buffer[1]=(address>>16)&0xFF;// put A23:16 to output buffer
    spi_do_buffer[2]=(address>>8)&0xFF; // put A15:8 to output buffer
    spi_do_buffer[3]=(address>>0)&0xFF; // put A7:0 to output buffer
	SPI_FLASH_SendByte(spi_do_buffer[0]);
	SPI_FLASH_SendByte(spi_do_buffer[1]);
	SPI_FLASH_SendByte(spi_do_buffer[2]);
	SPI_FLASH_SendByte(spi_do_buffer[3]);
	for (i=0; i<blk_size; i++)           // pres vsechny Byty bloku
	{
	  SPI_FLASH_SendByte(*pBuffer);
	  pBuffer++;
	}
	FLASH_CS(DISABLE);

	SPI_Flash_Wait_Busy(1000);
	FLASH_CS(ENABLE);
	SPI_Flash_Write_Disable();
	FLASH_CS(DISABLE);
}
/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : SPI FLash写，可以连续写，但不要超过512个
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - address : FLASH's internal address to write to.
*                  - blk_size : number of bytes to write to the FLASH,
* Output         : None
* Return         : 成功，失败
*******************************************************************************/
void SPI_FLASH_BufferWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size)
{
u16 size_one,size_two;
u32 addr1,addr2;

	addr1	=	address%SPI_FLASH_PageSize;
	addr2	=	addr1+blk_size;
	size_two=	addr2-SPI_FLASH_PageSize;
	size_one=	blk_size-size_two;
	if(addr2>SPI_FLASH_PageSize)
	//跨页，要写两次
	{
		SPI_FLASH_BlockWrite(pBuffer,address, size_one);	//写第一页,pBuffer,已经在该函数内修改
		addr2=address+size_one;
		pBuffer=pBuffer+size_one;
		SPI_FLASH_BlockWrite(pBuffer,addr2, size_two);	//写第一页
	}
	else
	{
		SPI_FLASH_BlockWrite(pBuffer,address, blk_size);
	}

}
/*******************************************************************************
* Function Name  : SPI_Flash_Unlock
* Description    : SPI FLash解锁
* Input          : None
* Output         : None
* Return         : 成功，失败
*******************************************************************************/
u8 SPI_Flash_Unlock(void)
{
u8 status;

  FLASH_CS(ENABLE);                         // chip select enabled
  SPI_Flash_Write_Enable();
  FLASH_CS(DISABLE); 
  
  status=SPI_Flash_Status();         
  if ((status&0x2)==0)              
    return FAIL;
  
  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_WRSTAT);
  SPI_FLASH_SendByte(0);
  FLASH_CS(DISABLE); 

  SPI_Flash_Wait_Busy(5000);
  status=SPI_Flash_Status();        
  if ((status&0xBC)==0)                // pokud jsou protect bity nulove
    return OK;

return FAIL;
}
/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : SPI FLash擦除整个芯片
* Input          : None
* Output         : None
* Return         : 成功，失败
*******************************************************************************/
/*
u8 SPI_Flash_Erase_Chip(void)
{
u8 status;

  FLASH_CS(ENABLE);                         // chip select enabled
  SPI_Flash_Write_Enable();            // napred povol zapis, nuluje se automaticky po uspesnem smazani
  FLASH_CS(DISABLE);                         // 发送完指令后，必须是CS为高才开始执行

  status=SPI_Flash_Status();           // zkontroluj status registr
  if ((status&0x2)==0)                 // pokud se WE nenahodil
    return FAIL; 
   
  FLASH_CS(ENABLE);                         // chip select enabled  
  SPI_FLASH_SendByte(SPI_FLASH_CHIPERASE);   // spust mazani cele FlashROM
  FLASH_CS(DISABLE);                         // 发送完指令后，必须是CS为高才开始执行
  Delay(10000000);				//延时1秒
  SPI_Flash_Wait_Busy(5000); 
  
  return OK;
}
*/
/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : SPI FLash擦除
* Input          : None
* Output         : None
* Return         : 成功，失败
*******************************************************************************/
u8 SPI_Flash_Erase_Sector(int sector)
{
u8 status;
u8 spi_do_buffer[4]; 

    FLASH_CS(ENABLE);                         // chip select enabled
    SPI_Flash_Write_Enable();        
	FLASH_CS(DISABLE);               

    spi_do_buffer[0]=SPI_FLASH_SECERASE; // put command code to output buffer
    spi_do_buffer[1]=(sector>>16)&0xFF;// put A23:16 to output buffer
    spi_do_buffer[2]=(sector>>8)&0xFF; // put A15:8 to output buffer
    spi_do_buffer[3]=(sector)&0xFF; // put A7:0 to output buffer
	FLASH_CS(ENABLE);                         // chip select enabled
	SPI_FLASH_SendByte(spi_do_buffer[0]);
	SPI_FLASH_SendByte(spi_do_buffer[1]);
	SPI_FLASH_SendByte(spi_do_buffer[2]);
	SPI_FLASH_SendByte(spi_do_buffer[3]);
	FLASH_CS(DISABLE);               
	Delay(1000000); 		//删除一扇区，大约要150ms
  	SPI_Flash_Wait_Busy(5000); 
	status=SPI_Flash_Status();           // zkontroluj status registr
	if ((status&0x2)==0)                 // pokud se WE nenahodil
	{
		return OK; 
	}
return FAIL;
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

	SPI_Flash_Erase_Sector(FLASH_UPD_FLAG_ADDR);
	mybuffer[0]=UPDATE_FLAG_FINISHED&0xff;
	mybuffer[1]=(UPDATE_FLAG_FINISHED>>8)&0xff;
	mybuffer[2]=(UPDATE_FLAG_FINISHED>>16)&0xff;
	mybuffer[3]=(UPDATE_FLAG_FINISHED>>24)&0xff;
	SPI_FLASH_BlockWrite(mybuffer,FLASH_UPD_FLAG_ADDR,4);
}
/*******************************************************************************
* Function Name  : User_Read_Config_Info
* Description    : 用户读配置信息数据
* Input          : None
* Output		 :*config_info: 存储读出配置信息缓存的指针;
* Return         : FLASH_OK:成功;other：失败
* 影 响		 	 ：None
*******************************************************************************/
unsigned char User_Read_Config_Info(struct_Config_info *config_info)
{
unsigned int i;
unsigned char mybuffer[48];
unsigned char check;

	
	SPI_Flash_Read_Block(mybuffer,FLASH_CONFIG_INFO_ADDR,sizeof(struct_Config_info)+1);

	check=0;
	for(i=0;i<sizeof(struct_Config_info);i++)
	{
		   check	+=	mybuffer[i];
	}
	if(check==mybuffer[i])
	{
		
		memcpy(config_info,mybuffer,sizeof(struct_Config_info));
		return FLASH_OK;	
	}
	else
	{	//错误时，赋值全0
		for(i=0;i<sizeof(struct_Config_info);i++)
		{
			   mybuffer[i]=0;
		}
		memcpy(config_info,mybuffer,sizeof(struct_Config_info));
	}
return FLASH_ERROR;	
}
/*******************************************************************************
* Function Name  : Read_Update_Finish_Flag
* Description    : 读完成标志
* Input          : None
* Return         : 标志值
* 影 响		 	 ：None
*******************************************************************************/
unsigned int Read_Update_Finish_Flag(void)
{
unsigned int  iTemp;

	SPI_Flash_Read_Block(mybuffer,FLASH_UPD_FLAG_ADDR,4);
	iTemp	= mybuffer[3];
	iTemp	= (iTemp<<8)|mybuffer[2];
	iTemp	= (iTemp<<8)|mybuffer[1];
	iTemp	= (iTemp<<8)|mybuffer[0];

return iTemp;
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
u32  device,chip,app_size;
u32  i,j;
u32 addr;

 // Get the number of block (4 or 2 pages) from where the user program will be loaded 
  FlashDestination = ApplicationAddress;
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
	if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
 	 	FLASH_DisableWriteProtectionPages();	// Disable the write protection of desired pages

  FlashDestination = ApplicationAddress; // Flash user program offset
  //读升级程序的信息  
  SPI_Flash_Read_Block(mybuffer,FLASH_UPD_START_ADDR,SPI_FLASH_PageSize);
  device  = (mybuffer[3] <<24)|(mybuffer[2] <<16)|(mybuffer[1] <<8)|(mybuffer[0]);
  chip	  = (mybuffer[11]<<24)|(mybuffer[10]<<16)|(mybuffer[9] <<8)|(mybuffer[8]);
  //version = (mybuffer[15]<<24)|(mybuffer[14]<<16)|(mybuffer[13]<<8)|(mybuffer[12]);
  app_size= (mybuffer[19]<<24)|(mybuffer[18]<<16)|(mybuffer[17]<<8)|(mybuffer[16]);

	//检查文件头是否正确
	if(device!=APP_DEVICE_TYPE_HEAD)
		{
			//LCD_PutString(4,40,"File Format Error!",COLOR_RED,COLOR_BLACK);
			return;
		}
	//检查芯片型号是否满足
	i = chip & APP_CHIP_TYPE_MASK;
	if(i>0)
	{
			return;
	}
	//检查程序大小是否满足条件
	if((app_size<APP_MIN_SIZE)||(app_size>APP_MAX_SIZE))
	{
			return;
	}
    /* Erase the needed pages where the user application will be loaded */
    /* Define the number of page to be erased */
	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    NbrOfPage = FLASH_PagesMask(app_size);
    /* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
    }
	//烧写程序开始
	RamSource = (u32)(&mybuffer[20]);
	for (j = 20;(j < (SPI_FLASH_PageSize)) && (FlashDestination <  ApplicationAddress + app_size);j += 4)
                  {
                    /* Program the data received into STM32F10x Flash */
                   FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
                    if (*(u32*)FlashDestination != *(u32*)RamSource)
                    {
                      /* End session */
                      return ;
	
                    }
                    FlashDestination += 4;
                    RamSource += 4;
                  }
	//烧写其它页
	NbrOfPage = app_size/SPI_FLASH_PageSize+2;
	chip = 0;
	for(i=1;i< NbrOfPage;i++)
	{
		addr	=	FLASH_UPD_START_ADDR+(i*SPI_FLASH_PageSize);
		SPI_Flash_Read_Block(mybuffer,addr,SPI_FLASH_PageSize);
		RamSource = (u32)(&mybuffer[0]);
		for (j = 0;(j < SPI_FLASH_PageSize) && (FlashDestination <  ApplicationAddress + app_size);j += 4)
	                  {
	                    /* Program the data received into STM32F10x Flash */
	                   FLASHStatus = FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
	                    if (*(u32*)FlashDestination != *(u32*)RamSource)
	                    {
	                      return ;
		
	                    }
	                    FlashDestination += 4;
	                    RamSource += 4;
	                  }
		 device = i*100/NbrOfPage;
		 if(chip!=device)
		 {
		  chip = device;
		  mybuffer[0]=chip/10+0x30;
		  mybuffer[1]=chip%10+0x30;
		  mybuffer[2]=0;
		 }
	 }
}


