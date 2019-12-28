/**
  ******************************************************************************
  * @file    STM32L1xx_IAP/inc/flash_if.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   This file provides all the headers of the flash_if functions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define ABS_RETURN(x,y)         (x < y) ? (y-x) : (x-y)


/* define the address from where user application will be loaded,
   the application address should be a start sector address */
#define APPLICATION_ADDRESS   (uint32_t)0x08002000

/* define the address from where IAP will be loaded, 0x08000000:BANK1 or 
   0x08030000:BANK2 */
#define FLASH_START_ADDRESS   (uint32_t)0x08000000

/* Get the number of sectors from where the user program will be loaded */
#define FLASH_SECTOR_NUMBER  ((uint32_t)(ABS_RETURN(APPLICATION_ADDRESS,FLASH_START_ADDRESS))>>12)

#ifdef STM32L1XX_MD 
  #define USER_FLASH_LAST_PAGE_ADDRESS  0x0801FF00
  #define USER_FLASH_END_ADDRESS        0x0801FFFF
  /* Compute the mask to test if the Flash memory, where the user program will be
  loaded, is write protected */
  #define  FLASH_PROTECTED_SECTORS   ((uint32_t)~((1 << FLASH_SECTOR_NUMBER) - 1))
#elif defined STM32L1XX_HD
  #define USER_FLASH_LAST_PAGE_ADDRESS  0x0805FF00
  /* define the address from where user application will be loaded,
   the application address should be a start sector address */
  #define USER_FLASH_END_ADDRESS        0x0805FFFF
#else
 #error "Please select first the STM32 device to be used (in stm32l1xx.h)"    
#endif 
/* define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint16_t DataLength);
uint32_t FLASH_If_DisableWriteProtection(void);
uint32_t FLASH_If_GetWriteProtectionStatus(void);
FLASH_Status FLASH_If_WriteProtectionConfig(void);


//����
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define ApplicationAddress    		0x8002000		//Ӧ�ó���Flash����ʼ��ַ

//����Flash����ʱ�õ��ĺ궨��
#define UPDATE_FLAG_ADDR			0x8009000	//������־��
#define UPDATE_FLAG_SIZE			0x10		//������־��Сʵ��Ϊ4�ֽ�

#define UPDATE_DATA_ADDR			0x8009400	//��������������
#define UPDATE_DATE_SIZE			0x6C00		//�����������ߴ�26K����

#define UPDATE_FLAG_GO				0x01		//(Ӧ��)ȥ������־
#define UPDATE_FLAG_FINISHED		0xAA55		//������ɱ�־

//���ݴ���ʱ�õ��ĺ궨��
#define FLASH_UPD_SEND_SIZE			128			//ÿ��������ֽ���,ӦΪ4�����������������
#define APP_DEVICE_TYPE_HEAD		0xEF0F0002	//Ӧ�ó��������豸���ͣ�ң��key
#define APP_CHIP_TYPE_MASK			0xFFFE0000	//Ӧ�ó����ʺϵ�оƬ�ͺ�����
#define APP_MIN_SIZE				0x2000		//Ӧ�ó�����С�ߴ�8K
#define APP_MAX_SIZE				0x6C00		//Ӧ�ó������ߴ�26K

#define FLASH_PAGE_SIZE               	  (0x100)//256 Bytes
#define PAGE_SIZE                         (0x100)//256
#define FLASH_SIZE                        (0x10000) //64K 

#define OK		0x00
#define FAIL	0x01
#define FLASH_OK	0
#define FLASH_ERROR	1


/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
extern unsigned char User_Read_Update_Data(u16 addr,u8 *pt,u16 N);
extern unsigned char User_Write_Update_Data(int addr,unsigned char  *pt,unsigned int  n);
extern void 			User_Erase_Update_Data(void);
extern void 			Write_Update_Finish_Flag(void);
extern unsigned int	Read_Update_Finish_Flag(void);
void Flash_Update_Program(void);
u32 FLASH_PagesMask(u32 Size);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  /* __FLASH_IF_H */

/*******************(C)COPYRIGHT 2012 STMicroelectronics *****END OF FILE******/
