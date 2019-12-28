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

#ifndef _SPIFLASH_H
#define _SPIFLASH_H

#define FLASH_OK	0
#define FLASH_ERROR	1

#define CMD_OK		0
#define CMD_ERROR	1
#define CMD_FAIL	1

#define  AT26DF_FLASH_ID        0x1F440000
#define  EN25T80_FLASH_ID       0x1C13
#define  EN25T16_FLASH_ID       0x1C14
#define  W25X16_FLASH_ID		0xC813


#define SPI_FLASH_PageSize      256
#define SPI_FLASH_SectorSize    0x1000			//扇区大小4K


//***************** SPI FlashROM command codes ******************************
// 25Xxx FlashROM description:
// SPI mode 0: SCK idle state = low, DIO sampled on rising edge, DO clocked on falling edge, MSB first
// 512kB memory, total 8 blocks per 64 kB, 1 block = 16 sectors per 4 kB, 1 sector = 16 pages per 256B
#define SPI_FLASH_WREN         0x06    // povol zapis
#define SPI_FLASH_WRDIS        0x04    // zakaz zapis
#define SPI_FLASH_RDSTAT       0x05    // cti status registr; S7:0 - bit7=stat reg protect, bit5=top/bot write protect, bit4:2=block protect bits, bit1=WE latch, bit0=busy
#define SPI_FLASH_WRSTAT       0x01    // zapis status registr; S7:0
#define SPI_FLASH_RDDATA       0x03    // cti data; A23:16, A15:A8, A7:0, D7:0, next DB till the end
#define SPI_FLASH_FASTRD       0x0B    // rychle cteni; A23:16, A15:A8, A7:0, dummyB, D7:0, next DB till the end
#define SPI_FLASH_FASTRD2      0x3B    // rychle cteni 2 linkami DO a DIO; A23:16, A15:A8, A7:0, dummyB, D7:0 bit interleaved, next DB till the end
#define SPI_FLASH_PAGEPGM      0x02    // programovani stranky; A23:16, A15:A8, A7:0, D7:0, next DB till the end
#define SPI_FLASH_BLKERASE     0xD8    // smazani bloku (64kB pro 25Xxx); A23:16, A15:A8, A7:0
#define SPI_FLASH_SECERASE     0x20    // smazani sektoru (4kB pro 25Xxx); A23:16, A15:A8, A7:0
#define SPI_FLASH_CHIPERASE    0xC7    // smazani cele FlashROM
#define SPI_FLASH_PWRDWN       0xB9    // 深度睡眠模式
#define SPI_FLASH_RELPWRDWN    0xAB    // 深度睡眠唤醒模式
#define SPI_FLASH_MIDDID       0x90    // ID vyrobce a zarizeni; dummyB, dummyB, 0, M7:0 (EFh=Winbond), ID7:0 (10h=X10, 11h=X20, 12h=X40, 13h=X80)
#define SPI_FLASH_JEDECID      0x9F    // JEDEC ID vyrobce; M7:0, ID15:8, ID7:0  (3011h=X10, 3012h=X20, 3013h=X40, 3014h=X80)

#define SPI_FLASH_JID_ATMEL    0x1F    // Atmel
#define SPI_FLASH_JID_STM      0x20    // ST Microelectronic
#define SPI_FLASH_JID_SST      0xBF    // SST (Silicon Storage Technology)
#define SPI_FLASH_JID_MACRONIX 0xC2    // Macronix
#define SPI_FLASH_JID_WINBOND  0xEF    // Winbond

#define SPI_FLASH_PAGE_SIZE    256     // velikost stranky, max. jednotka pro zapis
#define SPI_FLASH_SECTOR_SIZE  4096    // velikost sektoru, min. jednotka pro mazani
#define SPI_FLASH_BLOCK_SIZE   65536   // velikost bloku, vetsi jednotka pro mazani




/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void Flash_GPIO_Configuration(void);
void SPI_Flash_Read_Block(unsigned char * pBuffer,int start_address, int blk_size);
void SPI_FLASH_BlockWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size);
void SPI_FLASH_BufferWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size);
void SPI_Flash_BufferRead(unsigned char * pBuffer,int start_address, int blk_size);
unsigned char SPI_Flash_Erase_Sector(int sector);
unsigned char SPI_Flash_Erase_Chip(void);
unsigned int SPI_FLASH_ReadID(void);


unsigned int SPI_FLASH_DeepSleep(void);
unsigned int SPI_FLASH_Wakeup(void);
void Restore_Factory(void);

void Check_Device_Status(void);
unsigned char Check_Flash_Module_Is_Normal(void);


#endif /* _FLASH_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
