/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : spi_Flash.c
* Author             : Hanyongliang
* Date First Issued  : 03/23/2009
* Description        : This file provides all the SPI Flash(AT26DF041) firmware functions.
********************************************************************************
* History:
* 03/30/2009
*******************************************************************************/
#include "stm32l152d_eval.h"
#include <SpiFlash.h>
unsigned char SysUpDataFlag;

/* Private typedef -----------------------------------------------------------*/
#define PIN_FLASH_CS	  GPIO_Pin_9	 
#define PIN_FLASH_SCK	  GPIO_Pin_4	
#define PIN_FLASH_MOSI  GPIO_Pin_5	
#define PIN_FLASH_MISO  GPIO_Pin_8	

#define PORT_FLASH_CS	   GPIOB	
#define PORT_FLASH_SCK	 GPIOB
#define PORT_FLASH_MOSI  GPIOB
#define PORT_FLASH_MISO  GPIOB



#define SPI_Flash_Write_Enable()  SPI_FLASH_SendByte(SPI_FLASH_WREN)
#define SPI_Flash_Write_Disable() SPI_FLASH_SendByte(SPI_FLASH_WRDIS)


//OS_EVENT *pSemFlash = NULL;	/* Flash����Ȩ���ź��� */

void Delay(u32 nCount)
{
	u32 index = 0; 
	for(index = (0xFF * nCount); index != 0; index--)
	{
	}
}
/*******************************************************************************
* Function Name  : Flash_GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure SPI2 pins: CS ,SCK and MOSI */
	GPIO_InitStructure.GPIO_Pin = PIN_FLASH_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_FLASH_CS, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = PIN_FLASH_MOSI;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(PORT_FLASH_MOSI, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_FLASH_SCK;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(PORT_FLASH_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  PIN_FLASH_MISO;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	//��������
	GPIO_Init(PORT_FLASH_MISO, &GPIO_InitStructure);

	GPIO_SetBits(PORT_FLASH_CS, PIN_FLASH_CS);
	GPIO_SetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
	GPIO_SetBits(PORT_FLASH_MOSI, PIN_FLASH_MOSI);
	GPIO_SetBits(PORT_FLASH_MOSI, PIN_FLASH_MISO);


}
/*******************************************************************************
* Function Name  : Flash_Start
* Description    : �����ϵͳ�������Flash�洢����Ȩ��
* Input          : None
* Output         : None
* Return         : None                                                       
*******************************************************************************/
void Flash_Start(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);//ʹ�ܶ�ʱ��2
}
/*******************************************************************************
* Function Name  : Flash_End
* Description    : �ͷŷ���Flash�洢����Ȩ��
* Input          : None
* Output         : None
* Return         : None                                                       
*******************************************************************************/
void Flash_End(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2
}
/*******************************************************************************
* Function Name  : Flash_CS
* Description    : Spi Flash Chip Select
* Input          : NewState:DISABLE or ENABLE
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_CS(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		GPIO_ResetBits(PORT_FLASH_CS, PIN_FLASH_CS);
	}
	else
	{
		GPIO_SetBits(PORT_FLASH_CS, PIN_FLASH_CS);
		GPIO_SetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
		GPIO_SetBits(PORT_FLASH_MOSI, PIN_FLASH_MOSI);
	}
}
/*******************************************************************************
* Function Name  : SPI_Flash_ReadByte
* Description    : Receive one Byte From FLASH controller 
* Input          : None
* Output         : None
* Return         : Data Readed
*******************************************************************************/
unsigned char SPI_Flash_ReadByte (void)
{
unsigned char i=0;	
unsigned char data=0;


	for(i=0;i<8;i++)
	{
	 data	= (data<<1) ;
	 GPIO_ResetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
	 if   (GPIO_ReadInputDataBit(PORT_FLASH_MISO, PIN_FLASH_MISO))  
         	data   |=0x01;  
	 GPIO_SetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
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
void SPI_FLASH_SendByte (unsigned char data_out)
{
unsigned char i;

  for (i=0; i<8; i++)                 
    {
	    GPIO_ResetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
		if ((data_out&0x80)==0)            // start from MSB to LSB
	      GPIO_ResetBits(PORT_FLASH_MOSI, PIN_FLASH_MOSI);
	    else
	      GPIO_SetBits(PORT_FLASH_MOSI, PIN_FLASH_MOSI);
	    
	 	GPIO_SetBits(PORT_FLASH_SCK, PIN_FLASH_SCK);
	    data_out<<=1;                      // shift to next output bit, MSB will be send
    }

}
/*******************************************************************************
* Function Name  : SPI_Flash_Status
* Description    : SPI Flash��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char SPI_Flash_Status(void)
{
unsigned char ret;

  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_RDSTAT);
  ret	=	SPI_Flash_ReadByte();
  FLASH_CS(DISABLE);	               // chip select disabled
  return ret; 
}
/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : SPI Flash�ȴ����еĵ��� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Wait_Busy(int timeout) 
{
  while ((timeout--)>0)                
  {
	IWDG_ReloadCounter(); 
	Delay(80);
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
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
extern void delayms(void);
unsigned int SPI_FLASH_DeepSleep(void)
{
	unsigned int Temp = 0;

	FLASH_CS(ENABLE);                    // chip select enabled
	SPI_FLASH_SendByte(SPI_FLASH_PWRDWN);//B9
	delayms();//Delay(10);
	FLASH_CS(DISABLE);	               // chip select disabled
	return (Temp);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
unsigned int SPI_FLASH_Wakeup(void)
{
	unsigned int Temp = 0;

	FLASH_CS(ENABLE);                    // chip select enabled
	SPI_FLASH_SendByte(SPI_FLASH_RELPWRDWN);//AB
	delayms();//Delay(10);
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
* Description    : ��SPI Flash��
* Input          : ��ʼ��ַ:start_address,Ҫ���ĳߴ�:blk_size
* Output         : pBuffer:����������ָ��
* Return         : None
*******************************************************************************/
unsigned char spi_do_buffer[4];
void SPI_Flash_Read_Block(unsigned char * pBuffer,int start_address, int blk_size)
{
 
int i;
Flash_Start();	

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

Flash_End();
}

/*******************************************************************************
* Function Name  : SPI_Flash_Read_Block
* Description    : ��SPI Flash��
* Input          : ��ʼ��ַ:start_address,Ҫ���ĳߴ�:blk_size
* Output         : pBuffer:����������ָ��
* Return         : None
*******************************************************************************/
void SPI_Flash_BufferRead(unsigned char * pBuffer,int start_address, int blk_size)
{
unsigned char spi_do_buffer[4]={0}; 
int i;

Flash_Start();	

	FLASH_CS(ENABLE);                         // chip select enabled

	spi_do_buffer[0]=SPI_FLASH_FASTRD;   // put command code to output buffer
	spi_do_buffer[1]=(start_address>>16)&0xFF; // put A23:16 to output buffer
	spi_do_buffer[2]=(start_address>>8)&0xFF; // put A15:8 to output buffer
	spi_do_buffer[3]=(start_address>>0)&0xFF; // put A7:0 to output buffer
	
	SPI_FLASH_SendByte(spi_do_buffer[0]);
	SPI_FLASH_SendByte(spi_do_buffer[1]);
	SPI_FLASH_SendByte(spi_do_buffer[2]);
	SPI_FLASH_SendByte(spi_do_buffer[3]);

	SPI_Flash_ReadByte();//��һ�ֽ�dummy(�ٵ�),2012,srw
	for (i=0; i<blk_size; i++)           // pres vsechny Byty bloku
	{
	  *pBuffer	=	SPI_Flash_ReadByte();
	  pBuffer++;
	}
	FLASH_CS(DISABLE);

Flash_End();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BlockWrite
* Description    : �ú�������д����256�ֽڵ����ݣ����������ֽڽ�������
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
unsigned char spi_do_buffer[4]={0}; 
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
* Description    : SPI FLashд����������д������Ҫ����512��
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - address : FLASH's internal address to write to.
*                  - blk_size : number of bytes to write to the FLASH,
* Output         : None
* Return         : �ɹ���ʧ��
*******************************************************************************/
void SPI_FLASH_BufferWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size)
{
unsigned short int size_one,size_two;
u32 addr1,addr2;

Flash_Start();	

	addr1	=	address%SPI_FLASH_PageSize;
	addr2	=	addr1+blk_size;
	size_two=	addr2-SPI_FLASH_PageSize;
	size_one=	blk_size-size_two;
	if(addr2>SPI_FLASH_PageSize)
	//��ҳ��Ҫд����
	{
		SPI_FLASH_BlockWrite(pBuffer,address, size_one);	//д��һҳ,pBuffer,�Ѿ��ڸú������޸�
		addr2=address+size_one;
		pBuffer=pBuffer+size_one;
		SPI_FLASH_BlockWrite(pBuffer,addr2, size_two);	//д��һҳ
	}
	else
	{
		SPI_FLASH_BlockWrite(pBuffer,address, blk_size);
	}
Flash_End();	
}
/*******************************************************************************
* Function Name  : SPI_Flash_Unlock
* Description    : SPI FLash����
* Input          : None
* Output         : None
* Return         : �ɹ���ʧ��
*******************************************************************************/
unsigned char SPI_Flash_Unlock(void)
{
unsigned char status;

  FLASH_CS(ENABLE);                         // chip select enabled
  SPI_Flash_Write_Enable();
  FLASH_CS(DISABLE); 
  
  status=SPI_Flash_Status();         
  if ((status&0x2)==0)              
    return CMD_FAIL;
  
  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_FLASH_SendByte(SPI_FLASH_WRSTAT);
  SPI_FLASH_SendByte(0);
  FLASH_CS(DISABLE); 

  SPI_Flash_Wait_Busy(100);
  status=SPI_Flash_Status();        
  if ((status&0xBC)==0)                // pokud jsou protect bity nulove
    return CMD_OK;

return CMD_FAIL;
}
/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : SPI FLash��������оƬ
* Input          : None
* Output         : None
* Return         : �ɹ���ʧ��
*******************************************************************************/
unsigned char SPI_Flash_Erase_Chip(void)
{
unsigned char status;

  FLASH_CS(ENABLE);                    // chip select enabled
  SPI_Flash_Write_Enable();            // napred povol zapis, nuluje se automaticky po uspesnem smazani
  FLASH_CS(DISABLE);                         // ������ָ��󣬱�����CSΪ�߲ſ�ʼִ��

  status=SPI_Flash_Status();           // zkontroluj status registr
  if ((status&0x2)==0)                 // pokud se WE nenahodil
    return CMD_FAIL; 
   
  FLASH_CS(ENABLE);                         // chip select enabled  
  SPI_FLASH_SendByte(SPI_FLASH_CHIPERASE);   // spust mazani cele FlashROM
  FLASH_CS(DISABLE);                         // ������ָ��󣬱�����CSΪ�߲ſ�ʼִ��
  
  IWDG_ReloadCounter();
  Delay(80);//��ʱ1��
  IWDG_ReloadCounter();
  Delay(80);//��ʱ1��
  IWDG_ReloadCounter();
  Delay(80);//��ʱ1��
  IWDG_ReloadCounter();
  SPI_Flash_Wait_Busy(5000);  
//  IWDG_ReloadCounter();
  
  return CMD_OK;
}
/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : SPI FLash����
* Input          : None
* Output         : None
* Return         : �ɹ���ʧ��
*******************************************************************************/
unsigned char SPI_Flash_Erase_Sector(int sector)
{
unsigned char status;
unsigned char spi_do_buffer[4]={0}; 

Flash_Start();	
 	
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
	IWDG_ReloadCounter();
	Delay(80);//��ʱ1��
	IWDG_ReloadCounter();
	Delay(80);//��ʱ1��
	IWDG_ReloadCounter();
  	SPI_Flash_Wait_Busy(5000);
	status=SPI_Flash_Status();           // zkontroluj status registr
	if ((status&0x2)==0)                 // pokud se WE nenahodil
	{
		Flash_End();	
		return CMD_OK; 
	}

Flash_End();	
return CMD_FAIL;
}

/*******************************************************************************
* Function Name  : Check_Flash_Module_Is_Normal
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
unsigned char Check_Flash_Module_Is_Normal(void)
{
u32 flashid;

	SPI_Flash_Unlock();
	flashid=SPI_FLASH_ReadID();
	if((flashid==0)||(flashid==0xffff))
		return CMD_FAIL;

return CMD_OK;	
}

