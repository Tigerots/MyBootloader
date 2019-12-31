/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : usb_desc.h
* Author             : everbright6666@126.com
* Version            : V1.0
* Date               : 3/30/2009
* Description        : Descriptor Header for USB Reader
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21
#define SIZ_HID_DESC                   0x09
#define OFF_HID_DESC                   0x12

#define SIZ_DEVICE_DESC                18
#define SIZ_CONFIG_DESC                41
#define SIZ_REPORT_DESC                39
#define SIZ_STRING_LANGID              4
#define SIZ_STRING_VENDOR              64
#define SIZ_STRING_PRODUCT             44
#define SIZ_STRING_SERIAL              26

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const u8 DeviceDescriptor[SIZ_DEVICE_DESC];
extern const u8 ConfigDescriptor[SIZ_CONFIG_DESC];
extern const u8 ReportDescriptor[SIZ_REPORT_DESC];
extern const u8 StringLangID[SIZ_STRING_LANGID];
extern const u8 StringVendor[SIZ_STRING_VENDOR];
extern const u8 StringProduct[SIZ_STRING_PRODUCT];
extern const u8 StringSerial[SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
