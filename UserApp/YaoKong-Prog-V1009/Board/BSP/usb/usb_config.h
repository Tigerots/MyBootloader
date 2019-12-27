/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/*����ע����豸,��������Ӧ����������ʱ�õ����¶���*/
#define ID_VENDOR_H			0x04		//���̴��Ÿ��ֽ�
#define ID_VENDOR_L			0x83		//���̴��ŵ��ֽ�
#define ID_PRODUCT_H		0x12		//��Ʒ���Ÿ��ֽ�
#define ID_PRODUCT_L		0xC0		//��Ʒ���ŵ��ֽ�


/* Exported functions ------------------------------------------------------- */
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Cable_Config (FunctionalState NewState);
void USB_Reader_SendMsg(u8 buffer[],u16 len);

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
