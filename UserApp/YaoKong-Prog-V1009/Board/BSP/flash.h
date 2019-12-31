/******************** (C) COPYRIGHT 2007 HuaYuan ********************
* File Name          : flash.h
* Author             : Han yongliang
* Date First Issued  : 05/11/2008
* Description        : Header for flash.c file.
********************************************************************************
* History:
* 05/11/2008: V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _FLASH_H
#define _FLASH_H

#define  AT26DF_FLASH_ID        0x1F440000

#define SPI_FLASH_PageSize      256
#define HZK_TOTAL_PAGE			418			//汉字库占用页数

#define FLASH_HZK_ADDR			0x400		//汉字库起始地址，共112K
#define FLASH_PIC_ADDR			0x5C0		//屏保图片起始地址,共16K
#define FLASH_UPD_FLAG_ADDR		0x600		//升级文件标志地址
#define FLASH_UPD_START_ADDR	0x602		//升级文件存放起始地址,共123.5K
#define FLASH_UPD_END_ADDR		0x7EF		//升级文件存放结束地址


#define FLASH_USER_START_ADDR	0x7F0		//用户数据区起始地址,单位为页
#define FLASH_USER_END_ADDR		0x7F2		//用户数据区结束地址,单位为页
#define FLASH_DATA_PWD_ADDR		0x7F3		//数据密钥地址存储地址
#define FLASH_CARD_PWD_ADDR		0x7F4		//IC卡密钥地址存储地址



#define WRSR       				0x01  /* Write Status Register instruction */ 
#define READ       				0x03  /* Read from Memory instruction In Low Frequncy*/
#define RDSR       				0x05  /* Read Status Register instruction  */
#define RDID       				0x9F  /* Read identification */
#define	PAGE_ERASE				0x81  //页擦除
#define WRITE      				0x02  /* Write to Memory instruction */
#define PAGE_WRITE				0x11  //页编程
#define PAGE_WRITE_AUTO_ERASE	0x82  //页编程自动擦除
#define BE_2K      				0x50  // Block 2K Erase instruction 
#define BE_4K      				0x20  // Block 4K Erase instruction 
#define WIP_Flag   				0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 				0xA5


/* Exported types ------------------------------------------------------------*/

extern unsigned char HzDot_Data_Buffer[28];	//每个汉字点阵占28字节
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void Flash_GPIO_Configuration(void);

void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite);
void SPI_FLASH_BufferWrite(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite);
void SPI_FLASH_BufferRead(unsigned char* pBuffer, unsigned int ReadAddr, unsigned short NumByteToRead);
extern void SPI_FLASH_PageErase(unsigned int PageAddr);
extern void SPI_FLASH_BufferWrite512(unsigned char* pBuffer, unsigned int PageAddr);
extern void SPI_FLASH_BufferRead512(unsigned char* pBuffer, unsigned int PageAddr);
unsigned int SPI_FLASH_ReadID(void);
unsigned char Read_Data_PassWord(unsigned char *pt);
unsigned char Read_Card_PassWord(unsigned char *pt);
unsigned char Check_PassWord_Is_OK(void);
	      void Write_Data_PassWord(unsigned char *pt);
		  void Write_Card_PassWord(unsigned char  *pt);
		  void Restore_Factory(void);
unsigned char User_Read_Data(int addr,unsigned char  *pt,unsigned int  n);
unsigned char User_Write_Data(int addr,unsigned char  *pt,unsigned int  n);

/*----- Low layer function -----*/
void Delay (int cnt);
unsigned char SPI_FLASH_ReadByte(void);
void SPI_FLASH_SendByte(unsigned char byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* _FLASH_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
